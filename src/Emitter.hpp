#pragma once

#include <string>
#include <string_view>

class Emitter {
public:
    Emitter(const std::string &outputPath);

    void emit(std::string_view code) noexcept;

    void emit_line(std::string_view code) noexcept;

    void header(std::string_view code) noexcept;

    void header_line(std::string_view code) noexcept;

    void write_file() noexcept;

private:
    std::string m_output_path;
    std::string m_header;
    std::string m_code;
};
