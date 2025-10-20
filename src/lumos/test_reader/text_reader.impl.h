#ifndef TEXT_READER_IMPL_H
#define TEXT_READER_IMPL_H

#include "lumos/test_reader/text_reader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

LUMOS_INLINE std::string ReadFile(const std::string &file_path)
{
    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + file_path);
    }

    // Get file size for efficient allocation
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Reserve space for the string content
    std::string content;
    content.reserve(static_cast<size_t>(file_size));

    // Read the entire file using iterator approach
    // This preserves all characters including newlines
    content.assign((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());

    if (file.bad())
    {
        throw std::runtime_error("Error reading from file: " + file_path);
    }

    return content;
}

LUMOS_INLINE void WriteToFile(const std::string &text, const std::string &file_path)
{
    std::ofstream file(file_path, std::ios::binary | std::ios::trunc);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for writing: " + file_path);
    }

    // Write the entire string as-is
    file.write(text.c_str(), text.size());

    if (file.bad())
    {
        throw std::runtime_error("Error writing to file: " + file_path);
    }
}

LUMOS_INLINE void WriteToFile(const std::vector<std::string> &lines, const std::string &file_path)
{
    std::ofstream file(file_path, std::ios::binary | std::ios::trunc);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for writing: " + file_path);
    }

    // Write each line followed by a newline character
    for (const auto &line : lines)
    {
        file.write(line.c_str(), line.size());
        file.write("\n", 1); // Add newline after each line

        if (file.bad())
        {
            throw std::runtime_error("Error writing to file: " + file_path);
        }
    }
}

#endif // TEXT_READER_IMPL_H