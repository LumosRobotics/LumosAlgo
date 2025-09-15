#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>

// Basic string operations
bool contains(const std::string& str, const std::string& substring);
std::string replace(const std::string& input_str, const std::string& old_substr, const std::string& new_substr);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);

// Basic String Operations
std::string trim(const std::string& str);
std::string ltrim(const std::string& str);
std::string rtrim(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);
std::string toLowerCase(const std::string& str);
std::string toUpperCase(const std::string& str);
std::string reverse(const std::string& str);

// String Joining/Building
std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

template<typename... Args>
std::string join(const std::string& delimiter, const Args&... args);
std::string repeat(const std::string& str, size_t count);
std::string pad(const std::string& str, size_t total_length, char pad_char = ' ');
std::string padLeft(const std::string& str, size_t total_length, char pad_char = ' ');
std::string padRight(const std::string& str, size_t total_length, char pad_char = ' ');

// Advanced String Processing
std::string strip(const std::string& str, const std::string& chars_to_remove);
std::string replaceFirst(const std::string& input_str, const std::string& old_substr, const std::string& new_substr);
std::string replaceLast(const std::string& input_str, const std::string& old_substr, const std::string& new_substr);
size_t find(const std::string& str, const std::string& substring, size_t start_pos = 0);
size_t findLast(const std::string& str, const std::string& substring);
size_t count(const std::string& str, const std::string& substring);

// String Validation/Classification
bool isNumeric(const std::string& str);
bool isAlpha(const std::string& str);
bool isAlphaNumeric(const std::string& str);
bool isEmpty(const std::string& str);
bool isBlank(const std::string& str);
bool isValidEmail(const std::string& str);
bool isValidUrl(const std::string& str);

// String Formatting
std::string escape(const std::string& str);
std::string unescape(const std::string& str);
std::string toTitleCase(const std::string& str);

#endif // STRING_H