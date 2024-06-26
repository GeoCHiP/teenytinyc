#include <cctype>

#include "Lexer.hpp"

Lexer::Lexer(const std::string &source) : m_Source(source + "\n") {
    m_CurrentChar = source[m_CurrentPosition];
}

Lexer::Lexer(std::string &&source) : m_Source(std::move(source)) {
    m_Source += "\n";
    m_CurrentChar = source[m_CurrentPosition];
}

void Lexer::NextChar() noexcept {
    m_CurrentPosition += 1;
    if (m_CurrentPosition >= m_Source.size()) {
        m_CurrentChar = '\0';
    } else {
        m_CurrentChar = m_Source[m_CurrentPosition];
    }
}

char Lexer::Peek() noexcept {
    if (m_CurrentPosition + 1 >= m_Source.size()) {
        return '\0';
    }
    return m_Source[m_CurrentPosition + 1];
}

std::optional<Token> Lexer::GetToken() noexcept {
    SkipWhitespace();

    Token token{m_CurrentChar, TokenType::Eof};

    switch (m_CurrentChar) {
    case '+':
        token.Kind = TokenType::Plus;
        break;
    case '-':
        token.Kind = TokenType::Minus;
        break;
    case '*':
        token.Kind = TokenType::Asterisk;
        break;
    case '/':
        token.Kind = TokenType::Slash;
        break;
    case '\n':
        token.Kind = TokenType::Newline;
        break;
    case '\0':
        token.Kind = TokenType::Eof;
        break;
    default:
        return std::nullopt;
        break;
    }

    NextChar();
    return token;
}

void Lexer::SkipWhitespace() noexcept {
    while (std::isspace(m_CurrentChar) && m_CurrentChar != '\n') {
        NextChar();
    }
}
