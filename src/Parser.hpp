#pragma once

#include <unordered_set>

#include "Emitter.hpp"
#include "Lexer.hpp"

class Parser {
public:
    Parser(Lexer &lexer, Emitter &emitter);

    bool check_token(TokenType kind) noexcept;

    bool CheckPeek(TokenType kind) noexcept;

    void match(TokenType kind);

    void next_token() noexcept;

    void program();

    void statement();

    void expression();

    void term();

    void unary();

    void primary();

    void newline() noexcept;

    void comparison();

    bool is_comparison_operator() noexcept;

private:
    Lexer &m_lexer;
    Emitter &m_emitter;
    Token m_current_token;
    Token m_peek_token;
    std::unordered_set<std::string> m_symbols;
    std::unordered_set<std::string> m_labels_declared;
    std::unordered_set<std::string> m_labels_gotoed;
};
