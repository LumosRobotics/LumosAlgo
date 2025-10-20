#ifndef TEXT_READER_H
#define TEXT_READER_H

#include <string>
#include <vector>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

// Function to read the entire contents of a file as a string
// Preserves all characters including newlines and whitespace
std::string ReadFile(const std::string& file_path);

// Function to write a string to a file
// Overwrites existing file or creates new file if it doesn't exist
void WriteToFile(const std::string& text, const std::string& file_path);

// Function to write vector of strings to a file, each line followed by newline
// Overwrites existing file or creates new file if it doesn't exist
void WriteToFile(const std::vector<std::string>& lines, const std::string& file_path);

#endif // TEXT_READER_H