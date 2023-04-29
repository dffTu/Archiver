#pragma once

#include "BitStream.h"
#include "config.h"
#include "Trie.h"

class Decoder {
private:
    std::string_view path_;
    BitStreamInput archive_file_;

public:
    Decoder() = delete;

    explicit Decoder(std::string_view path);

    static void CheckIfNumberIsValid(const size_t& number);

    static void CheckIfSymbolIsValid(const size_t& symbol);

    size_t GetAlphabetSize();

    void GetAlphabetChars(const size_t& alphabet, std::vector<size_t>& chars);

    static void AddOneToCode(std::vector<bool>& code);

    void BuildTrie(const size_t& alphabet, const std::vector<size_t>& chars, Trie& trie);

    size_t ReadAnotherSymbol(Trie& trie);

    std::string GetFileName(Trie& trie);

    bool WriteContents(Trie& trie, std::string_view file_name);

    void Decode();
};