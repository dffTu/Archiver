#pragma once

#include <fstream>
#include <filesystem>
#include <iostream>

// Byte-by-byte number input class
class BitStreamInput {
private:
    std::filesystem::path path_;
    std::ifstream file_;
    size_t first_unused_bit_ = 0;
    size_t last_byte_ = 0;

public:
    explicit BitStreamInput(std::string_view path);

    size_t ReadAnyBits(size_t count);

    bool operator>>(unsigned char& x);

    void Close();

    std::string GetFileName() const;

    ~BitStreamInput();
};

// Byte-by-byte number output class
class BitStreamOutput {
private:
    std::filesystem::path path_;
    std::ofstream file_;
    size_t current_size_ = 0;
    size_t last_byte_ = 0;

public:
    explicit BitStreamOutput(std::string_view path);

    void WriteAnyBits(size_t count, const size_t x);

    void AppendToByte();

    void operator<<(const char& x);

    void Close();

    ~BitStreamOutput();
};