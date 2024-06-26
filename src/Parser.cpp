#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Lexer.hpp"
#include "Parser.hpp"

Parser::Parser(Lexer &lexer) : m_Lexer{lexer} {
    NextToken();
    NextToken();
}

bool Parser::CheckToken(TokenType kind) noexcept {
    return kind == m_CurrentToken.Kind;
}

bool Parser::CheckPeek(TokenType kind) noexcept {
    return kind == m_PeekToken.Kind;
}

void Parser::Match(TokenType kind) {
    if (!CheckToken(kind)) {
        std::stringstream ss;
        ss << "Expected " << TokenTypeToString(kind) << ", got"
           << TokenTypeToString(m_CurrentToken.Kind);
        throw std::runtime_error(ss.str());
    }
    NextToken();
}

void Parser::NextToken() noexcept {
    std::optional<Token> token = m_Lexer.GetToken();
    m_CurrentToken = m_PeekToken;
    m_PeekToken = token.value_or(Token{"", TokenType::Eof});
}

void Parser::Program() {
    std::cerr << "PROGRAM\n";

    while (CheckToken(TokenType::Newline)) {
        NextToken();
    }

    while (!CheckToken(TokenType::Eof)) {
        Statement();
    }

    for (const std::string &label : m_LabelsGotoed) {
        if (m_LabelsDeclared.find(label) == m_LabelsDeclared.end()) {
            std::stringstream ss;
            ss << "Attempting to GOTO to undeclared label: " << label;
            throw std::runtime_error(ss.str());
        }
    }
}

void Parser::Statement() {
    if (CheckToken(TokenType::Print)) {
        std::cerr << "STATEMENT-PRINT\n";
        NextToken();
        if (CheckToken(TokenType::String)) {
            // do something with m_CurrentToken
            NextToken();
        } else {
            Expression();
        }
    } else if (CheckToken(TokenType::If)) {
        std::cerr << "STATEMENT-IF\n";
        NextToken();
        Comparison();

        Match(TokenType::Then);
        Newline();

        while (!CheckToken(TokenType::Endif)) {
            Statement();
        }

        Match(TokenType::Endif);
    } else if (CheckToken(TokenType::While)) {
        std::cerr << "STATEMENT-WHILE\n";
        NextToken();
        Comparison();

        Match(TokenType::Repeat);
        Newline();

        while (!CheckToken(TokenType::Endwhile)) {
            Statement();
        }

        Match(TokenType::Endwhile);
    } else if (CheckToken(TokenType::Label)) {
        std::cerr << "STATEMENT-LABEL\n";
        NextToken();

        if (m_LabelsDeclared.find(m_CurrentToken.Value) !=
            m_LabelsDeclared.end()) {
            std::stringstream ss;
            ss << "Label already exists: " << m_CurrentToken.Value;
            throw std::runtime_error(ss.str());
        }

        m_LabelsDeclared.insert(m_CurrentToken.Value);
        Match(TokenType::Ident);
    } else if (CheckToken(TokenType::Goto)) {
        std::cerr << "STATEMENT-GOTO\n";
        NextToken();
        m_LabelsGotoed.insert(m_CurrentToken.Value);
        Match(TokenType::Ident);
    } else if (CheckToken(TokenType::Let)) {
        std::cerr << "STATEMENT-LET\n";
        NextToken();

        m_Symbols.insert(m_CurrentToken.Value);

        Match(TokenType::Ident);
        Match(TokenType::Eq);
        Expression();
    } else if (CheckToken(TokenType::Input)) {
        std::cerr << "STATEMENT-INPUT\n";
        NextToken();

        m_Symbols.insert(m_CurrentToken.Value);

        Match(TokenType::Ident);
    } else {
        std::stringstream ss;
        ss << "Invalid statement at " << m_CurrentToken.Value << " ("
           << TokenTypeToString(m_CurrentToken.Kind) << ")";
        throw std::runtime_error(ss.str());
    }
    Newline();
}

void Parser::Expression() {
    std::cerr << "EXPRESSION\n";

    Term();
    while (CheckToken(TokenType::Plus) || CheckToken(TokenType::Minus)) {
        NextToken();
        Term();
    }
}

void Parser::Term() {
    std::cerr << "TERM\n";

    Unary();
    while (CheckToken(TokenType::Asterisk) || CheckToken(TokenType::Slash)) {
        NextToken();
        Unary();
    }
}

void Parser::Unary() {
    std::cerr << "UNARY\n";

    if (CheckToken(TokenType::Plus) || CheckToken(TokenType::Minus)) {
        NextToken();
    }

    Primary();
}

void Parser::Primary() {
    std::cerr << "PRIMARY (" << m_CurrentToken.Value << ")\n";

    if (CheckToken(TokenType::Number)) {
        NextToken();
    } else if (CheckToken(TokenType::Ident)) {
        if (m_Symbols.find(m_CurrentToken.Value) == m_Symbols.end()) {
            std::stringstream ss;
            ss << "Referencing variable before assignment: "
               << m_CurrentToken.Value;
            throw std::runtime_error(ss.str());
        }
        NextToken();
    } else {
        std::stringstream ss;
        ss << "Unexpected token at " << m_CurrentToken.Value;
        throw std::runtime_error(ss.str());
    }
}

void Parser::Newline() noexcept {
    std::cerr << "NEWLINE\n";
    Match(TokenType::Newline);
    while (CheckToken(TokenType::Newline)) {
        NextToken();
    }
}

void Parser::Comparison() {
    std::cerr << "COMPARISON\n";

    Expression();
    if (IsComparisonOperator()) {
        NextToken();
        Expression();
    } else {
        std::stringstream ss;
        ss << "Expected comparison operator at: " << m_CurrentToken.Value
           << " (" << TokenTypeToString(m_CurrentToken.Kind) << ")";
        throw std::runtime_error(ss.str());
    }

    while (IsComparisonOperator()) {
        NextToken();
        Expression();
    }
}

bool Parser::IsComparisonOperator() noexcept {
    return CheckToken(TokenType::Eqeq) || CheckToken(TokenType::Noteq) ||
           CheckToken(TokenType::Gt) || CheckToken(TokenType::Gteq) ||
           CheckToken(TokenType::Lt) || CheckToken(TokenType::Lteq);
}
