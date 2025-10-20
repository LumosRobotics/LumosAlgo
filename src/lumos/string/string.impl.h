#ifndef STRING_IMPL_H
#define STRING_IMPL_H

#include "lumos/string.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <type_traits>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

LUMOS_INLINE bool contains(const std::string &str, const std::string &substring)
{
    if (substring.empty())
    {
        return true;
    }

    return str.find(substring) != std::string::npos;
}

LUMOS_INLINE std::string replace(const std::string &input_str, const std::string &old_substr, const std::string &new_substr)
{
    if (old_substr.empty())
    {
        return input_str;
    }

    std::string result = input_str;
    size_t pos = 0;

    while ((pos = result.find(old_substr, pos)) != std::string::npos)
    {
        result.replace(pos, old_substr.length(), new_substr);
        pos += new_substr.length();
    }

    return result;
}

LUMOS_INLINE std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> result;

    if (delimiter.empty())
    {
        result.push_back(str);
        return result;
    }

    size_t start = 0;
    size_t pos = 0;

    while ((pos = str.find(delimiter, start)) != std::string::npos)
    {
        result.push_back(str.substr(start, pos - start));
        start = pos + delimiter.length();
    }

    result.push_back(str.substr(start));

    return result;
}

// =============================================================================
// BASIC STRING OPERATIONS
// =============================================================================

LUMOS_INLINE std::string trim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
    {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

LUMOS_INLINE std::string ltrim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
    {
        return "";
    }
    return str.substr(start);
}

LUMOS_INLINE std::string rtrim(const std::string &str)
{
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    if (end == std::string::npos)
    {
        return "";
    }
    return str.substr(0, end + 1);
}

LUMOS_INLINE bool startsWith(const std::string &str, const std::string &prefix)
{
    if (prefix.length() > str.length())
    {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

LUMOS_INLINE bool endsWith(const std::string &str, const std::string &suffix)
{
    if (suffix.length() > str.length())
    {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

LUMOS_INLINE std::string toLowerCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

LUMOS_INLINE std::string toUpperCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

LUMOS_INLINE std::string reverse(const std::string &str)
{
    std::string result = str;
    std::reverse(result.begin(), result.end());
    return result;
}

// =============================================================================
// STRING JOINING/BUILDING
// =============================================================================

LUMOS_INLINE std::string join(const std::vector<std::string> &strings, const std::string &delimiter)
{
    if (strings.empty())
    {
        return "";
    }

    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i)
    {
        result += delimiter + strings[i];
    }
    return result;
}

template <typename... Args>
LUMOS_INLINE std::string join(const std::string &delimiter, const Args &...args)
{
    // Compile-time check that all arguments are string-like types
    static_assert((std::is_convertible_v<Args, std::string> && ...),
                  "All arguments to join must be convertible to std::string");

    if (sizeof...(args) == 0)
    {
        return "";
    }

    std::vector<std::string> strings;
    strings.reserve(sizeof...(args));

    // Convert all arguments to strings and add to vector
    (strings.emplace_back(std::string(args)), ...);

    return join(strings, delimiter);
}

LUMOS_INLINE std::string repeat(const std::string &str, size_t count)
{
    std::string result;
    result.reserve(str.length() * count);
    for (size_t i = 0; i < count; ++i)
    {
        result += str;
    }
    return result;
}

LUMOS_INLINE std::string pad(const std::string &str, size_t total_length, char pad_char)
{
    if (str.length() >= total_length)
    {
        return str;
    }

    size_t padding_needed = total_length - str.length();
    size_t left_padding = padding_needed / 2;
    size_t right_padding = padding_needed - left_padding;

    return std::string(left_padding, pad_char) + str + std::string(right_padding, pad_char);
}

LUMOS_INLINE std::string padLeft(const std::string &str, size_t total_length, char pad_char)
{
    if (str.length() >= total_length)
    {
        return str;
    }
    return std::string(total_length - str.length(), pad_char) + str;
}

LUMOS_INLINE std::string padRight(const std::string &str, size_t total_length, char pad_char)
{
    if (str.length() >= total_length)
    {
        return str;
    }
    return str + std::string(total_length - str.length(), pad_char);
}

// =============================================================================
// ADVANCED STRING PROCESSING
// =============================================================================

LUMOS_INLINE std::string strip(const std::string &str, const std::string &chars_to_remove)
{
    std::string result;
    result.reserve(str.length());

    for (char c : str)
    {
        if (chars_to_remove.find(c) == std::string::npos)
        {
            result += c;
        }
    }
    return result;
}

LUMOS_INLINE std::string replaceFirst(const std::string &input_str, const std::string &old_substr, const std::string &new_substr)
{
    if (old_substr.empty())
    {
        return input_str;
    }

    size_t pos = input_str.find(old_substr);
    if (pos == std::string::npos)
    {
        return input_str;
    }

    std::string result = input_str;
    result.replace(pos, old_substr.length(), new_substr);
    return result;
}

LUMOS_INLINE std::string replaceLast(const std::string &input_str, const std::string &old_substr, const std::string &new_substr)
{
    if (old_substr.empty())
    {
        return input_str;
    }

    size_t pos = input_str.rfind(old_substr);
    if (pos == std::string::npos)
    {
        return input_str;
    }

    std::string result = input_str;
    result.replace(pos, old_substr.length(), new_substr);
    return result;
}

LUMOS_INLINE size_t find(const std::string &str, const std::string &substring, size_t start_pos)
{
    return str.find(substring, start_pos);
}

LUMOS_INLINE size_t findLast(const std::string &str, const std::string &substring)
{
    return str.rfind(substring);
}

LUMOS_INLINE size_t count(const std::string &str, const std::string &substring)
{
    if (substring.empty())
    {
        return 0;
    }

    size_t count = 0;
    size_t pos = 0;

    while ((pos = str.find(substring, pos)) != std::string::npos)
    {
        count++;
        pos += substring.length();
    }

    return count;
}

// =============================================================================
// STRING VALIDATION/CLASSIFICATION
// =============================================================================

LUMOS_INLINE bool isNumeric(const std::string &str)
{
    if (str.empty())
    {
        return false;
    }

    size_t start = 0;
    if (str[0] == '+' || str[0] == '-')
    {
        start = 1;
    }

    bool has_decimal = false;
    for (size_t i = start; i < str.length(); ++i)
    {
        if (str[i] == '.')
        {
            if (has_decimal)
            {
                return false; // Multiple decimal points
            }
            has_decimal = true;
        }
        else if (!std::isdigit(str[i]))
        {
            return false;
        }
    }

    return start < str.length(); // Ensure we have at least one digit
}

LUMOS_INLINE bool isAlpha(const std::string &str)
{
    if (str.empty())
    {
        return false;
    }

    return std::all_of(str.begin(), str.end(), [](char c)
                       { return std::isalpha(c); });
}

LUMOS_INLINE bool isAlphaNumeric(const std::string &str)
{
    if (str.empty())
    {
        return false;
    }

    return std::all_of(str.begin(), str.end(), [](char c)
                       { return std::isalnum(c); });
}

LUMOS_INLINE bool isEmpty(const std::string &str)
{
    return str.empty();
}

LUMOS_INLINE bool isBlank(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), [](char c)
                       { return std::isspace(c); });
}

LUMOS_INLINE bool isValidEmail(const std::string &str)
{
    const std::regex email_pattern(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(str, email_pattern);
}

LUMOS_INLINE bool isValidUrl(const std::string &str)
{
    const std::regex url_pattern(
        R"(^https?://[a-zA-Z0-9.-]+(?:\.[a-zA-Z]{2,})?(?:/[^\s]*)?$)");
    return std::regex_match(str, url_pattern);
}

// =============================================================================
// STRING FORMATTING
// =============================================================================

LUMOS_INLINE std::string escape(const std::string &str)
{
    std::string result;
    result.reserve(str.length() * 2); // Reserve extra space for escape sequences

    for (char c : str)
    {
        switch (c)
        {
        case '\n':
            result += "\\n";
            break;
        case '\t':
            result += "\\t";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\v':
            result += "\\v";
            break;
        case '\\':
            result += "\\\\";
            break;
        case '\"':
            result += "\\\"";
            break;
        case '\'':
            result += "\\'";
            break;
        default:
            result += c;
            break;
        }
    }
    return result;
}

LUMOS_INLINE std::string unescape(const std::string &str)
{
    std::string result;
    result.reserve(str.length());

    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '\\' && i + 1 < str.length())
        {
            switch (str[i + 1])
            {
            case 'n':
                result += '\n';
                i++;
                break;
            case 't':
                result += '\t';
                i++;
                break;
            case 'r':
                result += '\r';
                i++;
                break;
            case 'b':
                result += '\b';
                i++;
                break;
            case 'f':
                result += '\f';
                i++;
                break;
            case 'v':
                result += '\v';
                i++;
                break;
            case '\\':
                result += '\\';
                i++;
                break;
            case '\"':
                result += '\"';
                i++;
                break;
            case '\'':
                result += '\'';
                i++;
                break;
            default:
                result += str[i];
                break;
            }
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

LUMOS_INLINE std::string toTitleCase(const std::string &str)
{
    std::string result = str;
    bool capitalize_next = true;

    for (char &c : result)
    {
        if (std::isalpha(c))
        {
            if (capitalize_next)
            {
                c = std::toupper(c);
                capitalize_next = false;
            }
            else
            {
                c = std::tolower(c);
            }
        }
        else
        {
            capitalize_next = std::isspace(c);
        }
    }
    return result;
}

// =============================================================================
// VECTOR STRING OPERATIONS
// =============================================================================

LUMOS_INLINE std::vector<std::string> sort(const std::vector<std::string> &strings)
{
    std::vector<std::string> result = strings;
    std::sort(result.begin(), result.end());
    return result;
}

LUMOS_INLINE void sortInPlace(std::vector<std::string> &strings)
{
    std::sort(strings.begin(), strings.end());
}

LUMOS_INLINE std::vector<std::string> unique(const std::vector<std::string> &strings)
{
    std::vector<std::string> result = strings;
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

LUMOS_INLINE std::vector<std::string> padLeft(const std::vector<std::string> &strings, const size_t num_pad_instances, const std::string &pad_str)
{
    std::vector<std::string> result;
    result.reserve(strings.size());

    std::string padding = repeat(pad_str, num_pad_instances);

    for (const auto &str : strings)
    {
        result.push_back(padding + str);
    }

    return result;
}

LUMOS_INLINE std::vector<std::string> padLeftUpToTotalLength(const std::vector<std::string> &strings, const size_t total_length, const std::string &pad_str)
{
    std::vector<std::string> result;
    result.reserve(strings.size());

    for (const auto &str : strings)
    {
        if (str.length() >= total_length)
        {
            result.push_back(str);
        }
        else
        {
            size_t padding_needed = total_length - str.length();
            size_t pad_instances = (padding_needed + pad_str.length() - 1) / pad_str.length(); // Ceiling division
            std::string padding = repeat(pad_str, pad_instances);

            // Trim padding to exact length needed
            if (padding.length() > padding_needed)
            {
                padding = padding.substr(0, padding_needed);
            }

            result.push_back(padding + str);
        }
    }

    return result;
}

LUMOS_INLINE std::vector<std::string> padRight(const std::vector<std::string> &strings, const size_t num_pad_instances, const std::string &pad_str)
{
    std::vector<std::string> result;
    result.reserve(strings.size());

    std::string padding = repeat(pad_str, num_pad_instances);

    for (const auto &str : strings)
    {
        result.push_back(str + padding);
    }

    return result;
}

LUMOS_INLINE std::vector<std::string> padRightUpToTotalLength(const std::vector<std::string> &strings, const size_t total_length, const std::string &pad_str)
{
    std::vector<std::string> result;
    result.reserve(strings.size());

    for (const auto &str : strings)
    {
        if (str.length() >= total_length)
        {
            result.push_back(str);
        }
        else
        {
            size_t padding_needed = total_length - str.length();
            size_t pad_instances = (padding_needed + pad_str.length() - 1) / pad_str.length(); // Ceiling division
            std::string padding = repeat(pad_str, pad_instances);

            // Trim padding to exact length needed
            if (padding.length() > padding_needed)
            {
                padding = padding.substr(0, padding_needed);
            }

            result.push_back(str + padding);
        }
    }

    return result;
}

#endif // STRING_IMPL_H