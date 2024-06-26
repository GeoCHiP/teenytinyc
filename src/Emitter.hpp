#pragma once

#include <string>
#include <string_view>

class Emitter {
public:
    Emitter(const std::string &outputPath);

    void Emit(std::string_view code) noexcept;

    void EmitLine(std::string_view code) noexcept;

    void Header(std::string_view code) noexcept;

    void HeaderLine(std::string_view code) noexcept;

    void WriteFile() noexcept;

private:
    std::string m_OutputPath;
    std::string m_Header;
    std::string m_Code;
};
