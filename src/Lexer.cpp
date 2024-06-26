#include <cctype>
#include <unordered_map>

#include "Lexer.hpp"

static const std::unordered_map<TokenType, const char *> s_TokenTypeNames = {
    {TokenType::Eof, "end of file"},
    {TokenType::Newline, "newline"},
    {TokenType::Number, "number"},
    {TokenType::Ident, "ident"},
    {TokenType::String, "string"},
    // Keywords.
    {TokenType::Label, "label"},
    {TokenType::Goto, "goto"},
    {TokenType::Print, "print"},
    {TokenType::Input, "input"},
    {TokenType::Let, "let"},
    {TokenType::If, "if"},
    {TokenType::Then, "then"},
    {TokenType::Endif, "endif"},
    {TokenType::While, "while"},
    {TokenType::Repeat, "repeat"},
    {TokenType::Endwhile, "endwhile"},
    // Operators.
    {TokenType::Eq, "eq"},
    {TokenType::Plus, "plus"},
    {TokenType::Minus, "minus"},
    {TokenType::Asterisk, "asterisk"},
    {TokenType::Slash, "slash"},
    {TokenType::Eqeq, "eqeq"},
    {TokenType::Noteq, "noteq"},
    {TokenType::Lt, "lt"},
    {TokenType::Lteq, "lteq"},
    {TokenType::Gt, "gt"},
    {TokenType::Gteq, "gteq"},
};

const char *TokenTypeToString(TokenType t) {
    return s_TokenTypeNames.at(t);
}

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
    SkipComment();

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
    case '=':
        if (Peek() == '=') {
            token.Value = "==";
            token.Kind = TokenType::Eqeq;
            NextChar();
        } else {
            token.Kind = TokenType::Eq;
        }
        break;
    case '>':
        if (Peek() == '=') {
            token.Value = ">=";
            token.Kind = TokenType::Gteq;
            NextChar();
        } else {
            token.Kind = TokenType::Gt;
        }
        break;
    case '<':
        if (Peek() == '=') {
            token.Value = "<=";
            token.Kind = TokenType::Lteq;
            NextChar();
        } else {
            token.Kind = TokenType::Lt;
        }
        break;
    case '!':
        if (Peek() == '=') {
            token.Value = "!=";
            token.Kind = TokenType::Noteq;
            NextChar();
        } else {
            return std::nullopt;
        }
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

void Lexer::SkipComment() noexcept {
    if (m_CurrentChar == '#') {
        while (m_CurrentChar != '\n') {
            NextChar();
        }
    }
}
