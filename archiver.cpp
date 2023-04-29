#include "ConsoleReader.h"
#include "Decode.h"
#include "Encode.h"
#include <iostream>
#include <vector>

int IncorrectInput() {
    std::cout << "Incorrect input. Type \"archiver -h\" to get help.";
    return 0;
}

int GetHelp() {
    std::cout << "Available commands:\n\n";
    std::cout << "archiver -c *archive_name* file1 [file2 ...] -- archive files *file1 [file2 ...]* into "
        "*archive_name*.\n";
    std::cout << "archiver -d *archive_name* -- unzip files from *archive_name* into current directory.\n";
    std::cout << "archiver -h -- get help.\n";
    return 0;
}

int EncodeFiles(std::vector<std::string>& files) {
    Encoder encoder(files[0]);
    for (size_t i = 1; i < files.size(); ++i) {
        bool is_last_file = (i == files.size() - 1);
        try {
            encoder.AddFile(files[i], is_last_file);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << ": " << files[i] << "\n";
            encoder.Close();
            return 111;
        }
    }
    encoder.Close();
    return 0;
}

int DecodeFile(std::string_view file) {
    try {
        Decoder decoder(file);
        decoder.Decode();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 111;
    }
    return 0;
}

int main(int argc, char** argv) {
    ConsoleArguments args = ReadConsole(argc, argv);
    switch (args.flag) {
    case INCORRECT_INPUT:
        return IncorrectInput();
    case GET_HELP:
        return GetHelp();
    case ENCODE_FILES:
        return EncodeFiles(args.files);
    case DECODE_FILE:
        return DecodeFile(args.files[0]);
    }
}