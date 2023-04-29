#pragma once

#include <string>
#include <vector>

enum Flags { GET_HELP, ENCODE_FILES, DECODE_FILE, INCORRECT_INPUT };

struct ConsoleArguments {
    Flags flag;
    std::vector<std::string> files;
};

ConsoleArguments ReadConsole(size_t argc, char** argv);