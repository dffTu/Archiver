#include "BitStream.h"
#include "config.h"

// Byte-by-byte number input class

BitStreamInput::BitStreamInput(std::string_view path) {
    path_ = path;
    file_.open(std::string(path), std::ios_base::binary);
    if (file_.fail()) {
        throw std::runtime_error("Couldn't open file");
    }
}

size_t BitStreamInput::ReadAnyBits(size_t count) {
    size_t result = 0;
    char c = 0;
    while (count != 0) {
        if (first_unused_bit_ == 0) {
            if (file_.eof()) {
                throw std::runtime_error("Invalid file format (not enough bits to read from file)");
            }
            file_.get(c);
            last_byte_ = static_cast<size_t>(c);
            first_unused_bit_ = BITS_IN_BYTE;
        }
        result <<= 1;
        if (last_byte_ & (1 << (first_unused_bit_ - 1))) {
            ++result;
        }
        --first_unused_bit_;
        --count;
    }
    return result;
}

bool BitStreamInput::operator>>(unsigned char& x) {
    if (file_.eof()) {
        throw std::runtime_error("Invalid file format (not enough bits to read from file)");
    }
    char signed_char = static_cast<char>(x);
    file_.get(signed_char);
    x = static_cast<unsigned char>(signed_char);
    return !file_.eof();
}

void BitStreamInput::Close() {
    file_.close();
    first_unused_bit_ = 0;
    last_byte_ = 0;
}

std::string BitStreamInput::GetFileName() const {
    return path_.filename().string();
}

BitStreamInput::~BitStreamInput() {
    path_.clear();
    file_.close();
}

// Byte-by-byte number output class

BitStreamOutput::BitStreamOutput(std::string_view path) {
    path_ = path;
    file_.open(std::string(path), std::ios_base::binary);
}

void BitStreamOutput::WriteAnyBits(size_t count, const size_t x) {
    while (count != 0) {
        last_byte_ <<= 1;
        if (x & (1 << (count - 1))) {
            ++last_byte_;
        }
        ++current_size_;
        --count;
        if (current_size_ == BITS_IN_BYTE) {
            file_ << char(last_byte_);
            last_byte_ = 0;
            current_size_ = 0;
        }
    }
}

void BitStreamOutput::AppendToByte() {
    if (current_size_ != 0) {
        last_byte_ <<= (BITS_IN_BYTE - current_size_);
    }
    file_ << char(last_byte_);
    current_size_ = 0;
    last_byte_ = 0;
}

void BitStreamOutput::operator<<(const char& x) {
    file_ << x;
}

void BitStreamOutput::Close() {
    file_.close();
    current_size_ = 0;
    last_byte_ = 0;
}

BitStreamOutput::~BitStreamOutput() {
    file_.close();
}