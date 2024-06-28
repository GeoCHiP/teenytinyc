#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Parser.hpp"

Parser::Parser(Lexer &lexer, Emitter &emitter)
    : m_lexer{lexer}, m_emitter(emitter) {
    next_token();
    next_token();
}

bool Parser::check_token(TokenType kind) noexcept {
    return kind == m_current_token.kind;
}

bool Parser::CheckPeek(TokenType kind) noexcept {
    return kind == m_peek_token.kind;
}

void Parser::match(TokenType kind) {
    if (!check_token(kind)) {
        std::stringstream ss;
        ss << "Expected " << token_type_to_string(kind) << ", got"
           << token_type_to_string(m_current_token.kind);
        throw std::runtime_error(ss.str());
    }
    next_token();
}

void Parser::next_token() noexcept {
    std::optional<Token> token = m_lexer.get_token();
    m_current_token = m_peek_token;
    m_peek_token = token.value_or(Token{
        .value = "",
        .kind = TokenType::Eof,
    });
}

void Parser::program() {
    m_emitter.header_line("#include <stdio.h>");
    m_emitter.header_line("int main() {");

    while (check_token(TokenType::Newline)) {
        next_token();
    }

    while (!check_token(TokenType::Eof)) {
        statement();
    }

    m_emitter.emit_line("return 0;");
    m_emitter.emit_line("}");

    for (const std::string &label : m_labels_gotoed) {
        if (m_labels_declared.find(label) == m_labels_declared.end()) {
            std::stringstream ss;
            ss << "Attempting to GOTO to undeclared label: " << label;
            throw std::runtime_error(ss.str());
        }
    }
}

void Parser::statement() {
    if (check_token(TokenType::Print)) {
        next_token();
        if (check_token(TokenType::String)) {
            m_emitter.emit("printf(\"");
            m_emitter.emit(m_current_token.value);
            m_emitter.emit_line("\\n\");");
            next_token();
        } else {
            m_emitter.emit("printf(\"%.2f\\n\", (float)(");
            expression();
            m_emitter.emit_line("));");
        }
    } else if (check_token(TokenType::If)) {
        next_token();

        m_emitter.emit("if (");
        comparison();

        match(TokenType::Then);
        newline();

        m_emitter.emit_line(") {");

        while (!check_token(TokenType::Endif)) {
            statement();
        }

        match(TokenType::Endif);

        m_emitter.emit_line("}");
    } else if (check_token(TokenType::While)) {
        next_token();

        m_emitter.emit("while (");
        comparison();

        match(TokenType::Repeat);
        newline();
        m_emitter.emit_line(") {");

        while (!check_token(TokenType::Endwhile)) {
            statement();
        }

        match(TokenType::Endwhile);
        m_emitter.emit_line("}");
    } else if (check_token(TokenType::Label)) {
        next_token();

        if (m_labels_declared.find(m_current_token.value) !=
            m_labels_declared.end()) {
            std::stringstream ss;
            ss << "Label already exists: " << m_current_token.value;
            throw std::runtime_error(ss.str());
        }
        m_labels_declared.insert(m_current_token.value);

        m_emitter.emit(m_current_token.value);
        m_emitter.emit_line(":");

        match(TokenType::Ident);
    } else if (check_token(TokenType::Goto)) {
        next_token();
        m_labels_gotoed.insert(m_current_token.value);

        m_emitter.emit("goto ");
        m_emitter.emit(m_current_token.value);
        m_emitter.emit_line(";");

        match(TokenType::Ident);
    } else if (check_token(TokenType::Let)) {
        next_token();

        if (m_symbols.find(m_current_token.value) == m_symbols.end()) {
            m_symbols.insert(m_current_token.value);
            m_emitter.header("float ");
            m_emitter.header(m_current_token.value);
            m_emitter.header_line(";");
        }

        m_emitter.emit(m_current_token.value);
        m_emitter.emit(" = ");

        match(TokenType::Ident);
        match(TokenType::Eq);

        expression();

        m_emitter.emit_line(";");
    } else if (check_token(TokenType::Input)) {
        next_token();

        if (m_symbols.find(m_current_token.value) == m_symbols.end()) {
            m_symbols.insert(m_current_token.value);
            m_emitter.header("float ");
            m_emitter.header(m_current_token.value);
            m_emitter.header_line(";");
        }

        m_emitter.emit("if (0 == scanf(\"%f\", &");
        m_emitter.emit(m_current_token.value);
        m_emitter.emit_line(")) {");

        m_emitter.emit(m_current_token.value);
        m_emitter.emit_line(" = 0;");
        m_emitter.emit_line("scanf(\"%*s\");");
        m_emitter.emit_line("}");

        match(TokenType::Ident);
    } else {
        std::stringstream ss;
        ss << "Invalid statement at " << m_current_token.value << " ("
           << token_type_to_string(m_current_token.kind) << ")";
        throw std::runtime_error(ss.str());
    }
    newline();
}

void Parser::expression() {
    term();
    while (check_token(TokenType::Plus) || check_token(TokenType::Minus)) {
        m_emitter.emit(m_current_token.value);
        next_token();
        term();
    }
}

void Parser::term() {
    unary();
    while (check_token(TokenType::Asterisk) || check_token(TokenType::Slash)) {
        m_emitter.emit(m_current_token.value);
        next_token();
        unary();
    }
}

void Parser::unary() {
    if (check_token(TokenType::Plus) || check_token(TokenType::Minus)) {
        m_emitter.emit(m_current_token.value);
        next_token();
    }

    primary();
}

void Parser::primary() {
    if (check_token(TokenType::Number)) {
        m_emitter.emit(m_current_token.value);
        next_token();
    } else if (check_token(TokenType::Ident)) {
        if (m_symbols.find(m_current_token.value) == m_symbols.end()) {
            std::stringstream ss;
            ss << "Referencing variable before assignment: "
               << m_current_token.value;
            throw std::runtime_error(ss.str());
        }
        m_emitter.emit(m_current_token.value);
        next_token();
    } else {
        std::stringstream ss;
        ss << "Unexpected token at " << m_current_token.value;
        throw std::runtime_error(ss.str());
    }
}

void Parser::newline() noexcept {
    match(TokenType::Newline);
    while (check_token(TokenType::Newline)) {
        next_token();
    }
}

void Parser::comparison() {
    expression();
    if (is_comparison_operator()) {
        m_emitter.emit(m_current_token.value);
        next_token();
        expression();
    } else {
        std::stringstream ss;
        ss << "Expected comparison operator at: " << m_current_token.value
           << " (" << token_type_to_string(m_current_token.kind) << ")";
        throw std::runtime_error(ss.str());
    }

    while (is_comparison_operator()) {
        m_emitter.emit(m_current_token.value);
        next_token();
        expression();
    }
}

bool Parser::is_comparison_operator() noexcept {
    return check_token(TokenType::Eqeq) || check_token(TokenType::Noteq) ||
           check_token(TokenType::Gt) || check_token(TokenType::Gteq) ||
           check_token(TokenType::Lt) || check_token(TokenType::Lteq);
}
