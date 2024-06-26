#include <iostream>

#include "Lexer.hpp"

int main() {
    std::string source = "+- # This is a comment!\n */";
    Lexer lexer{source};

    std::optional<Token> result = lexer.GetToken();
    while (result.has_value() && result.value().Kind != TokenType::Eof) {
        std::cout << TokenTypeToString(result.value().Kind) << '\n';
        result = lexer.GetToken();
    }
}
