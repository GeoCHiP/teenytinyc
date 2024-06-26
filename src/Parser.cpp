#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Parser.hpp"

Parser::Parser(Lexer &lexer, Emitter &emitter)
    : m_Lexer{lexer}, m_Emitter(emitter) {
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
    m_Emitter.HeaderLine("#include <stdio.h>");
    m_Emitter.HeaderLine("int main() {");

    while (CheckToken(TokenType::Newline)) {
        NextToken();
    }

    while (!CheckToken(TokenType::Eof)) {
        Statement();
    }

    m_Emitter.EmitLine("return 0;");
    m_Emitter.EmitLine("}");

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
        NextToken();
        if (CheckToken(TokenType::String)) {
            m_Emitter.Emit("printf(\"");
            m_Emitter.Emit(m_CurrentToken.Value);
            m_Emitter.EmitLine("\\n\");");
            NextToken();
        } else {
            m_Emitter.Emit("printf(\"%.2f\\n\", (float)(");
            Expression();
            m_Emitter.EmitLine("));");
        }
    } else if (CheckToken(TokenType::If)) {
        NextToken();

        m_Emitter.Emit("if (");
        Comparison();

        Match(TokenType::Then);
        Newline();

        m_Emitter.EmitLine(") {");

        while (!CheckToken(TokenType::Endif)) {
            Statement();
        }

        Match(TokenType::Endif);

        m_Emitter.EmitLine("}");
    } else if (CheckToken(TokenType::While)) {
        NextToken();

        m_Emitter.Emit("while (");
        Comparison();

        Match(TokenType::Repeat);
        Newline();
        m_Emitter.EmitLine(") {");

        while (!CheckToken(TokenType::Endwhile)) {
            Statement();
        }

        Match(TokenType::Endwhile);
        m_Emitter.EmitLine("}");
    } else if (CheckToken(TokenType::Label)) {
        NextToken();

        if (m_LabelsDeclared.find(m_CurrentToken.Value) !=
            m_LabelsDeclared.end()) {
            std::stringstream ss;
            ss << "Label already exists: " << m_CurrentToken.Value;
            throw std::runtime_error(ss.str());
        }
        m_LabelsDeclared.insert(m_CurrentToken.Value);

        m_Emitter.Emit(m_CurrentToken.Value);
        m_Emitter.EmitLine(":");

        Match(TokenType::Ident);
    } else if (CheckToken(TokenType::Goto)) {
        NextToken();
        m_LabelsGotoed.insert(m_CurrentToken.Value);

        m_Emitter.Emit("goto ");
        m_Emitter.Emit(m_CurrentToken.Value);
        m_Emitter.EmitLine(";");

        Match(TokenType::Ident);
    } else if (CheckToken(TokenType::Let)) {
        NextToken();

        if (m_Symbols.find(m_CurrentToken.Value) == m_Symbols.end()) {
            m_Symbols.insert(m_CurrentToken.Value);
            m_Emitter.Header("float ");
            m_Emitter.Header(m_CurrentToken.Value);
            m_Emitter.HeaderLine(";");
        }

        m_Emitter.Emit(m_CurrentToken.Value);
        m_Emitter.Emit(" = ");

        Match(TokenType::Ident);
        Match(TokenType::Eq);

        Expression();

        m_Emitter.EmitLine(";");
    } else if (CheckToken(TokenType::Input)) {
        NextToken();

        if (m_Symbols.find(m_CurrentToken.Value) == m_Symbols.end()) {
            m_Symbols.insert(m_CurrentToken.Value);
            m_Emitter.Header("float ");
            m_Emitter.Header(m_CurrentToken.Value);
            m_Emitter.HeaderLine(";");
        }

        m_Emitter.Emit("if (0 == scanf(\"%f\", &");
        m_Emitter.Emit(m_CurrentToken.Value);
        m_Emitter.EmitLine(")) {");

        m_Emitter.Emit(m_CurrentToken.Value);
        m_Emitter.EmitLine(" = 0;");
        m_Emitter.EmitLine("scanf(\"%*s\");");
        m_Emitter.EmitLine("}");

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
    Term();
    while (CheckToken(TokenType::Plus) || CheckToken(TokenType::Minus)) {
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
        Term();
    }
}

void Parser::Term() {
    Unary();
    while (CheckToken(TokenType::Asterisk) || CheckToken(TokenType::Slash)) {
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
        Unary();
    }
}

void Parser::Unary() {
    if (CheckToken(TokenType::Plus) || CheckToken(TokenType::Minus)) {
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
    }

    Primary();
}

void Parser::Primary() {
    if (CheckToken(TokenType::Number)) {
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
    } else if (CheckToken(TokenType::Ident)) {
        if (m_Symbols.find(m_CurrentToken.Value) == m_Symbols.end()) {
            std::stringstream ss;
            ss << "Referencing variable before assignment: "
               << m_CurrentToken.Value;
            throw std::runtime_error(ss.str());
        }
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
    } else {
        std::stringstream ss;
        ss << "Unexpected token at " << m_CurrentToken.Value;
        throw std::runtime_error(ss.str());
    }
}

void Parser::Newline() noexcept {
    Match(TokenType::Newline);
    while (CheckToken(TokenType::Newline)) {
        NextToken();
    }
}

void Parser::Comparison() {
    Expression();
    if (IsComparisonOperator()) {
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
        Expression();
    } else {
        std::stringstream ss;
        ss << "Expected comparison operator at: " << m_CurrentToken.Value
           << " (" << TokenTypeToString(m_CurrentToken.Kind) << ")";
        throw std::runtime_error(ss.str());
    }

    while (IsComparisonOperator()) {
        m_Emitter.Emit(m_CurrentToken.Value);
        NextToken();
        Expression();
    }
}

bool Parser::IsComparisonOperator() noexcept {
    return CheckToken(TokenType::Eqeq) || CheckToken(TokenType::Noteq) ||
           CheckToken(TokenType::Gt) || CheckToken(TokenType::Gteq) ||
           CheckToken(TokenType::Lt) || CheckToken(TokenType::Lteq);
}
