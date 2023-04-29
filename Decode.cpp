#include "Decode.h"

Decoder::Decoder(std::string_view path) : archive_file_(path) {
    path_ = path;
}

void Decoder::CheckIfNumberIsValid(const size_t& number) {
    if (number >= ALPHABET_SIZE) {
        throw std::runtime_error("Too big number in input (MAX: " + std::to_string(ALPHABET_SIZE) +
            ", THIS: " + std::to_string(number) + ")");
    }
}

void Decoder::CheckIfSymbolIsValid(const size_t& symbol) {
    if (symbol > MAX_SYMBOL) {
        throw std::runtime_error("Too big symbol in input (MAX: " + std::to_string(MAX_SYMBOL) +
            ", THIS: " + std::to_string(symbol) + ")");
    }
}

size_t Decoder::GetAlphabetSize() {
    size_t alphabet = archive_file_.ReadAnyBits(CODE_BITS_LENGTH);
    if (alphabet > ALPHABET_SIZE) {
        throw std::runtime_error("Too big alphabet size in input (MAX: " + std::to_string(ALPHABET_SIZE) +
            ", THIS: " + std::to_string(alphabet) + ")");
    }
    return alphabet;
}

void Decoder::GetAlphabetChars(const size_t& alphabet, std::vector<size_t>& chars) {
    std::vector<bool> used(ALPHABET_SIZE, false);
    for (size_t i = 0; i < alphabet; ++i) {
        size_t symbol = archive_file_.ReadAnyBits(CODE_BITS_LENGTH);
        CheckIfNumberIsValid(symbol);
        if (used[symbol]) {
            throw std::runtime_error("Two same codes in archive (CODE: " + std::to_string(symbol) + ")");
        }
        used[symbol] = true;
        chars.push_back(symbol);
    }
}

void Decoder::AddOneToCode(std::vector<bool>& code) {
    size_t sz = code.size();
    while (!code.empty() && code.back()) {
        code.pop_back();
    }
    if (code.empty()) {
        throw std::runtime_error("Wrong canonical codes in archive");
    }
    code.back() = true;
    while (code.size() < sz) {
        code.push_back(false);
    }
}

void Decoder::BuildTrie(const size_t& alphabet, const std::vector<size_t>& chars, Trie& trie) {
    size_t codes_count = 0;
    std::vector<bool> current_code;
    for (size_t code_length = 1; codes_count < alphabet; ++code_length) {
        size_t counter = archive_file_.ReadAnyBits(CODE_BITS_LENGTH);
        for (size_t i = 0; i < counter; ++i) {
            if (!current_code.empty()) {
                AddOneToCode(current_code);
            }
            while (current_code.size() < code_length) {
                current_code.push_back(false);
            }
            trie.AddToTrie(current_code, chars[codes_count + i]);
        }
        codes_count += counter;
    }
}

size_t Decoder::ReadAnotherSymbol(Trie& trie) {
    trie.ResetCurrentNode();
    while (trie.GetCurrentNodeValue() == -1) {
        size_t bit = archive_file_.ReadAnyBits(ONE_BIT);
        if (bit == 0) {
            trie.MoveLeft();
        } else {
            trie.MoveRight();
        }
    }
    size_t symbol = trie.GetCurrentNodeValue();
    CheckIfNumberIsValid(symbol);
    return symbol;
}

std::string Decoder::GetFileName(Trie& trie) {
    size_t symbol = ReadAnotherSymbol(trie);
    std::string file_name;
    while (symbol != FILENAME_END) {
        CheckIfSymbolIsValid(symbol);
        file_name.push_back(char(symbol));
        symbol = ReadAnotherSymbol(trie);
    }
    if (file_name.empty()) {
        throw std::runtime_error("Empty filename of file in archive");
    }
    return file_name;
}

bool Decoder::WriteContents(Trie& trie, std::string_view file_name) {
    BitStreamOutput output_file(file_name);
    size_t symbol = ReadAnotherSymbol(trie);
    while (symbol != ARCHIVE_END && symbol != ONE_MORE_FILE) {
        CheckIfSymbolIsValid(symbol);
        char c = symbol;
        output_file << c;
        symbol = ReadAnotherSymbol(trie);
    }
    return symbol == ONE_MORE_FILE;
}

void Decoder::Decode() {
    std::vector<size_t> chars;
    Trie trie;
    size_t alphabet_size = GetAlphabetSize();
    GetAlphabetChars(alphabet_size, chars);
    BuildTrie(alphabet_size, chars, trie);
    if (WriteContents(trie, GetFileName(trie))) {
        Decode();
    }
}