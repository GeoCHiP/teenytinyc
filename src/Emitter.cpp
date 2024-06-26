#include <fstream>

#include "Emitter.hpp"

Emitter::Emitter(const std::string &outputPath) : m_OutputPath(outputPath) {}

void Emitter::Emit(std::string_view code) noexcept {
    m_Code += code;
}

void Emitter::EmitLine(std::string_view code) noexcept {
    m_Code += code;
    m_Code += '\n';
}

void Emitter::Header(std::string_view code) noexcept {
    m_Header += code;
}

void Emitter::HeaderLine(std::string_view code) noexcept {
    m_Header += code;
    m_Header += '\n';
}

void Emitter::WriteFile() noexcept {
    std::ofstream stream{m_OutputPath};
    std::string result{m_Header + m_Code};
    stream.write(result.c_str(), result.size());
}
