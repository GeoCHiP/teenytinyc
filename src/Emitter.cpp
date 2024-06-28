#include <fstream>

#include "Emitter.hpp"

Emitter::Emitter(const std::string &outputPath) : m_output_path(outputPath) {}

void Emitter::emit(std::string_view code) noexcept {
    m_code += code;
}

void Emitter::emit_line(std::string_view code) noexcept {
    m_code += code;
    m_code += '\n';
}

void Emitter::header(std::string_view code) noexcept {
    m_header += code;
}

void Emitter::header_line(std::string_view code) noexcept {
    m_header += code;
    m_header += '\n';
}

void Emitter::write_file() noexcept {
    std::ofstream stream{m_output_path};
    std::string result{m_header + m_code};
    stream.write(result.c_str(), result.size());
}
