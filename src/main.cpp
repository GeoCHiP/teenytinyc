#include <fstream>
#include <iostream>

#include "Emitter.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

std::optional<std::string> ReadFile(const std::string &path) {
    std::ifstream stream{path};
    if (!stream) {
        return std::nullopt;
    }

    constexpr size_t readSize = 4096;
    std::string out;
    std::string buffer(readSize, '\0');

    while (stream.read(&buffer[0], readSize)) {
        out.append(buffer, 0, stream.gcount());
    }
    out.append(buffer, 0, stream.gcount());

    return out;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage:\n    " << argv[0] << " <source_file>\n";
        return 1;
    }

    std::optional<std::string> source = ReadFile(argv[1]);
    if (!source) {
        std::cerr << "Could not open file: " << argv[1] << '\n';
        return 1;
    }

    Lexer lexer{source.value()};
    Emitter emitter{"out.c"};
    Parser parser{lexer, emitter};

    try {
        parser.Program();
    } catch (const std::exception &e) {
        std::cerr << argv[1] << ": ";
        std::cerr << e.what() << '\n';
        return 1;
    }

    emitter.WriteFile();
}
