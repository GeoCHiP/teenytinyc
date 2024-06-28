#include <cctype>
#include <iostream>
#include <unordered_map>

#include "Lexer.hpp"

static const std::unordered_map<TokenType, const char *> s_token_type_names = {
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

const char *token_type_to_string(TokenType t) {
    return s_token_type_names.at(t);
}

std::optional<TokenType> Lexer::check_if_keyword(const std::string &str) {
    for (const auto &[token_type, token_name] : s_token_type_names) {
        if (token_name == str) {
            return token_type;
        }
    }
    return std::nullopt;
}

Lexer::Lexer(const std::string &source) : m_source(source) {
    if (m_source.back() != '\n') {
        m_source += '\n';
    }
    m_current_char = m_source[m_current_position];
}

Lexer::Lexer(std::string &&source) : m_source(std::move(source)) {
    if (m_source.back() != '\n') {
        m_source += '\n';
    }
    m_current_char = m_source[m_current_position];
}

void Lexer::next_char() noexcept {
    m_current_position += 1;
    if (m_current_position >= m_source.size()) {
        m_current_char = '\0';
    } else {
        m_current_char = m_source[m_current_position];
    }
}

char Lexer::peek() noexcept {
    if (m_current_position + 1 >= m_source.size()) {
        return '\0';
    }
    return m_source[m_current_position + 1];
}

std::optional<Token> Lexer::get_token() noexcept {
    skip_whitespace();
    skip_comment();

    Token token{
        .value = std::string{m_current_char},
        .kind = TokenType::Eof,
    };

    if (std::isdigit(m_current_char)) {
        size_t start_pos = m_current_position;
        while (std::isdigit(m_current_char)) {
            next_char();
        }
        if (m_current_char == '.') {
            next_char();
            if (!std::isdigit(m_current_char)) {
                std::cerr << "Illegal character in number.\n";
                return std::nullopt;
            }
            while (std::isdigit(m_current_char)) {
                next_char();
            }
        }
        token.kind = TokenType::Number;
        token.value =
            m_source.substr(start_pos, m_current_position - start_pos);
        return token;
    }

    if (std::isalpha(m_current_char)) {
        size_t startPos = m_current_position;
        next_char();
        while (std::isalnum(m_current_char)) {
            next_char();
        }
        token.value = m_source.substr(startPos, m_current_position - startPos);
        std::optional<TokenType> keyword = check_if_keyword(token.value);
        token.kind = keyword.value_or(TokenType::Ident);
        return token;
    }

    switch (m_current_char) {
    case '+':
        token.kind = TokenType::Plus;
        break;
    case '-':
        token.kind = TokenType::Minus;
        break;
    case '*':
        token.kind = TokenType::Asterisk;
        break;
    case '/':
        token.kind = TokenType::Slash;
        break;
    case '=':
        if (peek() == '=') {
            token.value = "==";
            token.kind = TokenType::Eqeq;
            next_char();
        } else {
            token.kind = TokenType::Eq;
        }
        break;
    case '>':
        if (peek() == '=') {
            token.value = ">=";
            token.kind = TokenType::Gteq;
            next_char();
        } else {
            token.kind = TokenType::Gt;
        }
        break;
    case '<':
        if (peek() == '=') {
            token.value = "<=";
            token.kind = TokenType::Lteq;
            next_char();
        } else {
            token.kind = TokenType::Lt;
        }
        break;
    case '!':
        if (peek() == '=') {
            token.value = "!=";
            token.kind = TokenType::Noteq;
            next_char();
        } else {
            std::cerr << "Expected !=, got !" << peek() << '\n';
            return std::nullopt;
        }
        break;
    case '"': {
        next_char();
        size_t start_pos = m_current_position;
        while (m_current_char != '"') {
            if (m_current_char == '\r' || m_current_char == '\n' ||
                m_current_char == '\t' || m_current_char == '\\' ||
                m_current_char == '%') {
                std::cerr << "Illegal character in string.\n";
                return std::nullopt;
            }
            next_char();
        }
        token.kind = TokenType::String;
        token.value =
            m_source.substr(start_pos, m_current_position - start_pos);
        break;
    }
    case '\n':
        token.kind = TokenType::Newline;
        break;
    case '\0':
        token.kind = TokenType::Eof;
        break;
    default:
        std::cerr << "Unknown token: " << m_current_char << '\n';
        return std::nullopt;
        break;
    }

    next_char();
    return token;
}

void Lexer::skip_whitespace() noexcept {
    while (std::isspace(m_current_char) && m_current_char != '\n') {
        next_char();
    }
}

void Lexer::skip_comment() noexcept {
    if (m_current_char == '#') {
        while (m_current_char != '\n') {
            next_char();
        }
    }
}
