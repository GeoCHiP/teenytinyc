#include <iostream>

#include "Lexer.hpp"

int main() {
    std::string source = "+- */";
    Lexer lexer{source};

    std::optional<Token> result = lexer.GetToken();
    while (result.has_value() && result.value().Kind != TokenType::Eof) {
        std::visit([](auto &&value) { std::cout << value << '\n'; },
                   result.value().Value);
        result = lexer.GetToken();
    }
}
