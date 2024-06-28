#pragma once

#include <optional>
#include <string>

enum class TokenType {
    Eof = -1,
    Newline = 0,
    Number = 1,
    Ident = 2,
    String = 3,
    // Keywords.
    Label = 101,
    Goto = 102,
    Print = 103,
    Input = 104,
    Let = 105,
    If = 106,
    Then = 107,
    Endif = 108,
    While = 109,
    Repeat = 110,
    Endwhile = 111,
    // Operators.
    Eq = 201,
    Plus = 202,
    Minus = 203,
    Asterisk = 204,
    Slash = 205,
    Eqeq = 206,
    Noteq = 207,
    Lt = 208,
    Lteq = 209,
    Gt = 210,
    Gteq = 211,
};

const char *token_type_to_string(TokenType t);

struct Token {
    std::string value;
    TokenType kind;
};

class Lexer {
public:
    explicit Lexer(const std::string &source);
    explicit Lexer(std::string &&source);

    void next_char() noexcept;

    char peek() noexcept;

    void abort(const std::string &message);

    void skip_whitespace() noexcept;

    void skip_comment() noexcept;

    std::optional<Token> get_token() noexcept;

    static std::optional<TokenType> check_if_keyword(const std::string &str);

private:
    std::string m_source;
    char m_current_char = '\0';
    size_t m_current_position = 0;
};
