#include "ConsoleReader.h"

ConsoleArguments ReadConsole(size_t argc, char** argv) {
    ConsoleArguments result = { .flag = INCORRECT_INPUT, .files = {} };
    if (argc == 1) {
        return result;
    }
    std::string flag = *(argv + 1);
    result.files = std::vector<std::string>(argv + 2, argv + argc);
    if (flag == "-h") {
        result.flag = GET_HELP;
        return result;
    } else if (flag == "-c") {
        if (result.files.size() >= 2) {
            result.flag = ENCODE_FILES;
        }
        return result;
    } else if (flag == "-d") {
        if (result.files.size() == 1) {
            result.flag = DECODE_FILE;
        }
        return result;
    } else {
        return result;
    }
}