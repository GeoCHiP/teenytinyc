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

const char *TokenTypeToString(TokenType t);

struct Token {
    std::string Value;
    TokenType Kind;
};

class Lexer {
public:
    explicit Lexer(const std::string &source);
    explicit Lexer(std::string &&source);

    void NextChar() noexcept;

    char Peek() noexcept;

    void Abort(const std::string &message);

    void SkipWhitespace() noexcept;

    void SkipComment() noexcept;

    std::optional<Token> GetToken() noexcept;

    static std::optional<TokenType> CheckIfKeyword(const std::string &str);

private:
    std::string m_Source;
    char m_CurrentChar = '\0';
    size_t m_CurrentPosition = 0;
};
