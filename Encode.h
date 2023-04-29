#pragma once

#include "BitStream.h"
#include "config.h"
#include <vector>

class SingleFileEncoder {
private:
    struct Code {
        size_t code_length = 0;
        std::vector<size_t> code;
    };
    struct Node {
        int32_t parent_index = -1;
        int32_t minimum_ord = -1;
        explicit Node(int32_t index, int32_t ord);
    };
    struct QueueNode {
        size_t frequency = 0;
        size_t ord = 0;
        size_t index = 0;
        QueueNode() = default;
        explicit QueueNode(size_t a, size_t b, size_t c);
        bool operator<(const QueueNode& other) const;
        bool operator<=(const QueueNode& other) const;
    };
    size_t frequency_[ALPHABET_SIZE];
    size_t code_length_[ALPHABET_SIZE];
    Code canonical_codes_[ALPHABET_SIZE];
    std::vector<size_t> canonical_order_;
    std::string_view path_;
    BitStreamOutput& output_file_;

public:
    explicit SingleFileEncoder(std::string_view path, BitStreamOutput& output_file);

    void CountFrequencies();

    void BuildTrie();

    void CountCodesLength(const std::vector<Node>& trie_vector);

    void BuildCanonicalCodes();

    void WriteFile(const bool write_archive_end);

    void WriteChar(const size_t ord);
};

class Encoder {
private:
    BitStreamOutput output_file_;

public:
    explicit Encoder(std::string_view path);

    void AddFile(std::string_view path, const bool is_last_file);

    void Close();

    ~Encoder();
};