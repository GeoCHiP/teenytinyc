#include <cctype>
#include <iostream>
#include <unordered_map>

#include "Lexer.hpp"

static const std::unordered_map<TokenType, const char *> s_TokenTypeNames = {
    {TokenType::Eof, "end of file"},
    {TokenType::Newline, "newline"},
    {TokenType::Number, "number"},
    {TokenType::Ident, "ident"},
    {TokenType::String, "string"},
    // Keywords.
    {TokenType::Label, "LABEL"},
    {TokenType::Goto, "GOTO"},
    {TokenType::Print, "PRINT"},
    {TokenType::Input, "INPUT"},
    {TokenType::Let, "LET"},
    {TokenType::If, "IF"},
    {TokenType::Then, "THEN"},
    {TokenType::Endif, "ENDIF"},
    {TokenType::While, "WHILE"},
    {TokenType::Repeat, "REPEAT"},
    {TokenType::Endwhile, "ENDWHILE"},
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

std::optional<TokenType> Lexer::CheckIfKeyword(const std::string &str) {
    for (const auto &[tokenType, tokenName] : s_TokenTypeNames) {
        if (tokenName == str) {
            return tokenType;
        }
    }
    return std::nullopt;
}

Lexer::Lexer(const std::string &source) : m_Source(source) {
    if (m_Source.back() != '\n') {
        m_Source += '\n';
    }
    m_CurrentChar = m_Source[m_CurrentPosition];
}

Lexer::Lexer(std::string &&source) : m_Source(std::move(source)) {
    if (m_Source.back() != '\n') {
        m_Source += '\n';
    }
    m_CurrentChar = m_Source[m_CurrentPosition];
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

    Token token{std::string{m_CurrentChar}, TokenType::Eof};

    if (std::isdigit(m_CurrentChar)) {
        size_t startPos = m_CurrentPosition;
        while (std::isdigit(m_CurrentChar)) {
            NextChar();
        }
        if (m_CurrentChar == '.') {
            NextChar();
            if (!std::isdigit(m_CurrentChar)) {
                std::cerr << "Illegal character in number.\n";
                return std::nullopt;
            }
            while (std::isdigit(m_CurrentChar)) {
                NextChar();
            }
        }
        token.Kind = TokenType::Number;
        token.Value = m_Source.substr(startPos, m_CurrentPosition - startPos);
        return token;
    }

    if (std::isalpha(m_CurrentChar)) {
        size_t startPos = m_CurrentPosition;
        NextChar();
        while (std::isalnum(m_CurrentChar)) {
            NextChar();
        }
        token.Value = m_Source.substr(startPos, m_CurrentPosition - startPos);
        std::optional<TokenType> keyword = CheckIfKeyword(token.Value);
        token.Kind = keyword.value_or(TokenType::Ident);
        return token;
    }

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
            std::cerr << "Expected !=, got !" << Peek() << '\n';
            return std::nullopt;
        }
        break;
    case '"': {
        NextChar();
        size_t startPos = m_CurrentPosition;
        while (m_CurrentChar != '"') {
            if (m_CurrentChar == '\r' || m_CurrentChar == '\n' ||
                m_CurrentChar == '\t' || m_CurrentChar == '\\' ||
                m_CurrentChar == '%') {
                std::cerr << "Illegal character in string.\n";
                return std::nullopt;
            }
            NextChar();
        }
        token.Kind = TokenType::String;
        token.Value = m_Source.substr(startPos, m_CurrentPosition - startPos);
        break;
    }
    case '\n':
        token.Kind = TokenType::Newline;
        break;
    case '\0':
        token.Kind = TokenType::Eof;
        break;
    default:
        std::cerr << "Unknown token: " << m_CurrentChar << '\n';
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
