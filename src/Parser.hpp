#pragma once

#include <unordered_set>

#include "Emitter.hpp"
#include "Lexer.hpp"

class Parser {
public:
    Parser(Lexer &lexer, Emitter &emitter);

    bool CheckToken(TokenType kind) noexcept;

    bool CheckPeek(TokenType kind) noexcept;

    void Match(TokenType kind);

    void NextToken() noexcept;

    void Program();

    void Statement();

    void Expression();

    void Term();

    void Unary();

    void Primary();

    void Newline() noexcept;

    void Comparison();

    bool IsComparisonOperator() noexcept;

private:
    Lexer &m_Lexer;
    Emitter &m_Emitter;
    Token m_CurrentToken;
    Token m_PeekToken;
    std::unordered_set<std::string> m_Symbols;
    std::unordered_set<std::string> m_LabelsDeclared;
    std::unordered_set<std::string> m_LabelsGotoed;
};
