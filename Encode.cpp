#include <algorithm>
#include "Encode.h"
#include "PriorityQueue.h"

SingleFileEncoder::Node::Node(int32_t index, int32_t ord) {
    parent_index = index;
    minimum_ord = ord;
}

SingleFileEncoder::QueueNode::QueueNode(size_t a, size_t b, size_t c) {
    frequency = a;
    ord = b;
    index = c;
}

bool SingleFileEncoder::QueueNode::operator<(const QueueNode& other) const {
    return std::tie(frequency, ord, index) < std::tie(other.frequency, other.ord, other.index);
}

bool SingleFileEncoder::QueueNode::operator<=(const QueueNode& other) const {
    return std::tie(frequency, ord, index) <= std::tie(other.frequency, other.ord, other.index);
}

SingleFileEncoder::SingleFileEncoder(std::string_view path, BitStreamOutput& output_file) : output_file_(output_file) {
    path_ = path;
    for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
        frequency_[i] = 0;
    }
}

void SingleFileEncoder::CountFrequencies() {
    BitStreamInput file(path_);
    std::string file_name = file.GetFileName();
    for (const unsigned char c : file_name) {
        ++frequency_[c];
    }
    unsigned char c = 0;
    while (file >> c) {
        ++frequency_[c];
    }
    frequency_[FILENAME_END] = 1;
    frequency_[ONE_MORE_FILE] = 1;
    frequency_[ARCHIVE_END] = 1;
}

void SingleFileEncoder::BuildTrie() {
    PriorityQueue<QueueNode> q;     // Frequency, ord and index in trie_vector.
    std::vector<Node> trie_vector;  // Ord and parent index
    for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
        if (frequency_[i] > 0) {
            trie_vector.emplace_back(-1, static_cast<int32_t>(i));
            q.Push(QueueNode(frequency_[i], i, trie_vector.size() - 1));
        }
    }
    while (q.Size() > 1) {
        auto a = q.Top();
        q.Pop();
        auto b = q.Top();
        q.Pop();
        trie_vector.emplace_back(-1, -1);
        trie_vector[a.index].parent_index = trie_vector.size() - 1;
        trie_vector[b.index].parent_index = trie_vector.size() - 1;
        size_t new_frequency = a.frequency + b.frequency;
        size_t min_ord = std::min(a.ord, b.ord);
        q.Push(QueueNode(new_frequency, min_ord, trie_vector.size() - 1));
    }
    CountCodesLength(trie_vector);
}

void SingleFileEncoder::CountCodesLength(const std::vector<Node>& trie_vector) {
    for (size_t index = 0; index < trie_vector.size() && trie_vector[index].minimum_ord != -1; ++index) {
        size_t length = 0;
        size_t temp = index;
        while (trie_vector[temp].parent_index != -1) {
            ++length;
            temp = trie_vector[temp].parent_index;
        }
        code_length_[trie_vector[index].minimum_ord] = length;
    }
}

void SingleFileEncoder::BuildCanonicalCodes() {
    std::vector<std::pair<size_t, size_t>> codes_and_chars;
    for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
        if (frequency_[i] > 0) {
            codes_and_chars.emplace_back(code_length_[i], i);
        }
    }
    std::sort(codes_and_chars.begin(), codes_and_chars.end());
    size_t current_size = codes_and_chars[0].first;
    std::vector<size_t> current_code;
    canonical_codes_[codes_and_chars[0].second] = { .code_length = current_size, .code = current_code };
    canonical_order_.push_back(codes_and_chars[0].second);
    for (size_t i = 1; i < codes_and_chars.size(); ++i) {
        size_t size = codes_and_chars[i].first;
        size_t ord = codes_and_chars[i].second;
        size_t index = current_size - 1;
        while (!current_code.empty() && current_code.back() == index) {
            current_code.pop_back();
            --index;
        }
        current_code.push_back(index);
        if (size != current_size) {
            current_size = size;
        }
        canonical_codes_[ord] = { .code_length = current_size, .code = current_code };
        canonical_order_.push_back(ord);
    }
}

void SingleFileEncoder::WriteFile(const bool write_archive_end) {
    output_file_.WriteAnyBits(CODE_BITS_LENGTH, canonical_order_.size());
    size_t max_code_length = 0;
    for (const auto i : canonical_order_) {
        output_file_.WriteAnyBits(CODE_BITS_LENGTH, i);
        if (canonical_codes_[i].code_length > max_code_length) {
            max_code_length = canonical_codes_[i].code_length;
        }
    }
    std::vector<size_t> lengths_count(max_code_length, 0);
    for (const auto i : canonical_order_) {
        ++lengths_count[canonical_codes_[i].code_length - 1];
    }
    for (const auto i : lengths_count) {
        output_file_.WriteAnyBits(CODE_BITS_LENGTH, i);
    }
    BitStreamInput file(path_);
    std::string file_name = file.GetFileName();
    for (const auto c : file_name) {
        WriteChar(static_cast<size_t>(c));
    }
    WriteChar(FILENAME_END);
    unsigned char c = 0;
    while (file >> c) {
        WriteChar(c);
    }
    if (write_archive_end) {
        WriteChar(ARCHIVE_END);
        output_file_.AppendToByte();
    } else {
        WriteChar(ONE_MORE_FILE);
    }
}

void SingleFileEncoder::WriteChar(const size_t ord) {
    int last = -1;
    for (size_t j = 0; j < canonical_codes_[ord].code.size(); ++j) {
        for (size_t i = last + 1; i < canonical_codes_[ord].code[j]; ++i) {
            output_file_.WriteAnyBits(ONE_BIT, 0);
        }
        output_file_.WriteAnyBits(ONE_BIT, 1);
        last = canonical_codes_[ord].code[j];
    }

    for (size_t i = last + 1; i < canonical_codes_[ord].code_length; ++i) {
        output_file_.WriteAnyBits(ONE_BIT, 0);
    }
}

Encoder::Encoder(std::string_view path) : output_file_(path) {
}

void Encoder::AddFile(std::string_view path, const bool is_last_file = false) {
    SingleFileEncoder file(path, output_file_);
    file.CountFrequencies();
    file.BuildTrie();
    file.BuildCanonicalCodes();
    file.WriteFile(is_last_file);
}

void Encoder::Close() {
    output_file_.Close();
}

Encoder::~Encoder() {
    output_file_.Close();
}