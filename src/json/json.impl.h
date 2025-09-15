#ifndef JSON_IMPL_H
#define JSON_IMPL_H

#include "json.h"
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <algorithm>
#include <fstream>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

// =============================================================================
// JSON VALUE CLASS IMPLEMENTATION
// =============================================================================

// Constructors
LUMOS_INLINE Json::Json() : value_(nullptr) {}

LUMOS_INLINE Json::Json(std::nullptr_t) : value_(nullptr) {}

LUMOS_INLINE Json::Json(bool value) : value_(value) {}

LUMOS_INLINE Json::Json(int value) : value_(static_cast<double>(value)) {}

LUMOS_INLINE Json::Json(double value) : value_(value) {}

LUMOS_INLINE Json::Json(const char *value) : value_(std::string(value)) {}

LUMOS_INLINE Json::Json(const std::string &value) : value_(value) {}

LUMOS_INLINE Json::Json(const JsonArray &value) : value_(value) {}

LUMOS_INLINE Json::Json(const JsonObject &value) : value_(value) {}

LUMOS_INLINE Json::Json(std::initializer_list<Json> init_list) : value_(JsonArray(init_list.begin(), init_list.end())) {}

LUMOS_INLINE Json::Json(std::initializer_list<std::pair<std::string, Json>> init_list) : value_(JsonObject(init_list.begin(), init_list.end())) {}

// Static factory methods
LUMOS_INLINE Json Json::FromFile(const std::string &filename)
{
    return parseJsonFromFile(filename);
}

// Type checking
LUMOS_INLINE JsonType Json::getType() const
{
    return std::visit([](const auto &value) -> JsonType
                      {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return JsonType::Null;
        } else if constexpr (std::is_same_v<T, bool>) {
            return JsonType::Boolean;
        } else if constexpr (std::is_same_v<T, double>) {
            return JsonType::Number;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return JsonType::String;
        } else if constexpr (std::is_same_v<T, JsonArray>) {
            return JsonType::Array;
        } else if constexpr (std::is_same_v<T, JsonObject>) {
            return JsonType::Object;
        } }, value_);
}

LUMOS_INLINE bool Json::isNull() const { return getType() == JsonType::Null; }
LUMOS_INLINE bool Json::isBool() const { return getType() == JsonType::Boolean; }
LUMOS_INLINE bool Json::isNumber() const { return getType() == JsonType::Number; }
LUMOS_INLINE bool Json::isString() const { return getType() == JsonType::String; }
LUMOS_INLINE bool Json::isArray() const { return getType() == JsonType::Array; }
LUMOS_INLINE bool Json::isObject() const { return getType() == JsonType::Object; }

// Value access
LUMOS_INLINE bool Json::asBool() const
{
    if (!isBool())
    {
        throw std::runtime_error("Json is not a boolean");
    }
    return std::get<bool>(value_);
}

LUMOS_INLINE double Json::asNumber() const
{
    if (!isNumber())
    {
        throw std::runtime_error("Json is not a number");
    }
    return std::get<double>(value_);
}

LUMOS_INLINE const std::string &Json::asString() const
{
    if (!isString())
    {
        throw std::runtime_error("Json is not a string");
    }
    return std::get<std::string>(value_);
}

LUMOS_INLINE const JsonArray &Json::asArray() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }
    return std::get<JsonArray>(value_);
}

LUMOS_INLINE const JsonObject &Json::asObject() const
{
    if (!isObject())
    {
        throw std::runtime_error("Json is not an object");
    }
    return std::get<JsonObject>(value_);
}

LUMOS_INLINE JsonArray &Json::asArray()
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }
    return std::get<JsonArray>(value_);
}

LUMOS_INLINE JsonObject &Json::asObject()
{
    if (!isObject())
    {
        throw std::runtime_error("Json is not an object");
    }
    return std::get<JsonObject>(value_);
}

// Templated value access
template <typename T>
LUMOS_INLINE T Json::as() const
{
    if constexpr (std::is_same_v<T, bool>)
    {
        return asBool();
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        return asString();
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return asNumber();
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        return static_cast<float>(std::get<double>(value_));
    }
    else if constexpr (std::is_same_v<T, int8_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < std::numeric_limits<int8_t>::min() || val > std::numeric_limits<int8_t>::max())
        {
            throw std::out_of_range("Value out of range for int8_t");
        }
        return static_cast<int8_t>(val);
    }
    else if constexpr (std::is_same_v<T, int16_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < std::numeric_limits<int16_t>::min() || val > std::numeric_limits<int16_t>::max())
        {
            throw std::out_of_range("Value out of range for int16_t");
        }
        return static_cast<int16_t>(val);
    }
    else if constexpr (std::is_same_v<T, int32_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < std::numeric_limits<int32_t>::min() || val > std::numeric_limits<int32_t>::max())
        {
            throw std::out_of_range("Value out of range for int32_t");
        }
        return static_cast<int32_t>(val);
    }
    else if constexpr (std::is_same_v<T, int64_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < std::numeric_limits<int64_t>::min() || val > std::numeric_limits<int64_t>::max())
        {
            throw std::out_of_range("Value out of range for int64_t");
        }
        return static_cast<int64_t>(val);
    }
    else if constexpr (std::is_same_v<T, uint8_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < 0 || val > std::numeric_limits<uint8_t>::max())
        {
            throw std::out_of_range("Value out of range for uint8_t");
        }
        return static_cast<uint8_t>(val);
    }
    else if constexpr (std::is_same_v<T, uint16_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < 0 || val > std::numeric_limits<uint16_t>::max())
        {
            throw std::out_of_range("Value out of range for uint16_t");
        }
        return static_cast<uint16_t>(val);
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < 0 || val > std::numeric_limits<uint32_t>::max())
        {
            throw std::out_of_range("Value out of range for uint32_t");
        }
        return static_cast<uint32_t>(val);
    }
    else if constexpr (std::is_same_v<T, uint64_t>)
    {
        if (!isNumber())
        {
            throw std::runtime_error("Json is not a number");
        }
        double val = std::get<double>(value_);
        if (val < 0 || val > std::numeric_limits<uint64_t>::max())
        {
            throw std::out_of_range("Value out of range for uint64_t");
        }
        return static_cast<uint64_t>(val);
    }
    else
    {
        static_assert(std::is_same_v<T, bool> ||
                          std::is_same_v<T, std::string> ||
                          std::is_same_v<T, float> ||
                          std::is_same_v<T, double> ||
                          std::is_same_v<T, int8_t> ||
                          std::is_same_v<T, int16_t> ||
                          std::is_same_v<T, int32_t> ||
                          std::is_same_v<T, int64_t> ||
                          std::is_same_v<T, uint8_t> ||
                          std::is_same_v<T, uint16_t> ||
                          std::is_same_v<T, uint32_t> ||
                          std::is_same_v<T, uint64_t>,
                      "Unsupported type for Json::as<T>()");
    }
}

// Vector specialization - handle vector types in the main template
template <>
LUMOS_INLINE std::vector<bool> Json::as<std::vector<bool>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<bool> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<bool>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<std::string> Json::as<std::vector<std::string>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<std::string> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<std::string>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<float> Json::as<std::vector<float>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<float> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<float>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<double> Json::as<std::vector<double>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<double> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<double>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<int8_t> Json::as<std::vector<int8_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<int8_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<int8_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<int16_t> Json::as<std::vector<int16_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<int16_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<int16_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<int32_t> Json::as<std::vector<int32_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<int32_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<int32_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<int64_t> Json::as<std::vector<int64_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<int64_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<int64_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<uint8_t> Json::as<std::vector<uint8_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<uint8_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<uint8_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<uint16_t> Json::as<std::vector<uint16_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<uint16_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<uint16_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<uint32_t> Json::as<std::vector<uint32_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<uint32_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<uint32_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<uint64_t> Json::as<std::vector<uint64_t>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<uint64_t> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element.as<uint64_t>());
    }

    return result;
}

template <>
LUMOS_INLINE std::vector<Json> Json::as<std::vector<Json>>() const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }

    const auto &json_array = asArray();
    std::vector<Json> result;
    result.reserve(json_array.size());

    for (const auto &element : json_array)
    {
        result.push_back(element);
    }

    return result;
}

// Array operations
LUMOS_INLINE size_t Json::size() const
{
    if (isArray())
    {
        return asArray().size();
    }
    else if (isObject())
    {
        return asObject().size();
    }
    else if (isNull())
    {
        return 0;
    }
    throw std::runtime_error("Json does not support size()");
}

LUMOS_INLINE Json &Json::operator[](size_t index)
{
    if (!isArray())
    {
        value_ = JsonArray{};
    }
    auto &arr = std::get<JsonArray>(value_);
    if (index >= arr.size())
    {
        arr.resize(index + 1);
    }
    return arr[index];
}

LUMOS_INLINE const Json &Json::operator[](size_t index) const
{
    if (!isArray())
    {
        throw std::runtime_error("Json is not an array");
    }
    const auto &arr = std::get<JsonArray>(value_);
    if (index >= arr.size())
    {
        throw std::out_of_range("Array index out of range");
    }
    return arr[index];
}

LUMOS_INLINE void Json::push_back(const Json &value)
{
    if (!isArray())
    {
        value_ = JsonArray{};
    }
    std::get<JsonArray>(value_).push_back(value);
}

// Object operations
LUMOS_INLINE Json &Json::operator[](const std::string &key)
{
    if (!isObject())
    {
        value_ = JsonObject{};
    }
    return std::get<JsonObject>(value_)[key];
}

LUMOS_INLINE const Json &Json::operator[](const std::string &key) const
{
    if (!isObject())
    {
        throw std::runtime_error("Json is not an object");
    }
    const auto &obj = std::get<JsonObject>(value_);
    auto it = obj.find(key);
    if (it == obj.end())
    {
        throw std::out_of_range("Object key not found: " + key);
    }
    return it->second;
}

LUMOS_INLINE bool Json::hasKey(const std::string &key) const
{
    if (!isObject())
    {
        return false;
    }
    const auto &obj = std::get<JsonObject>(value_);
    return obj.find(key) != obj.end();
}

LUMOS_INLINE void Json::remove(const std::string &key)
{
    if (isObject())
    {
        std::get<JsonObject>(value_).erase(key);
    }
}

// Comparison operators
LUMOS_INLINE bool Json::operator==(const Json &other) const
{
    return value_ == other.value_;
}

LUMOS_INLINE bool Json::operator!=(const Json &other) const
{
    return !(*this == other);
}

// String representation
LUMOS_INLINE std::string Json::toString(bool pretty, int indent) const
{
    std::string indent_str = pretty ? std::string(indent * 2, ' ') : "";
    std::string newline = pretty ? "\n" : "";

    switch (getType())
    {
    case JsonType::Null:
        return "null";
    case JsonType::Boolean:
        return asBool() ? "true" : "false";
    case JsonType::Number:
    {
        double num = asNumber();
        if (num == static_cast<int>(num))
        {
            return std::to_string(static_cast<int>(num));
        }
        return std::to_string(num);
    }
    case JsonType::String:
    {
        std::string result = "\"";
        for (char c : asString())
        {
            switch (c)
            {
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += c;
                break;
            }
        }
        result += "\"";
        return result;
    }
    case JsonType::Array:
    {
        const auto &arr = asArray();
        if (arr.empty())
        {
            return "[]";
        }
        std::string result = "[";
        if (pretty)
            result += newline;

        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (pretty)
                result += std::string((indent + 1) * 2, ' ');
            result += arr[i].toString(pretty, indent + 1);
            if (i < arr.size() - 1)
            {
                result += ",";
            }
            if (pretty)
                result += newline;
        }

        if (pretty)
            result += indent_str;
        result += "]";
        return result;
    }
    case JsonType::Object:
    {
        const auto &obj = asObject();
        if (obj.empty())
        {
            return "{}";
        }
        std::string result = "{";
        if (pretty)
            result += newline;

        size_t i = 0;
        for (const auto &[key, value] : obj)
        {
            if (pretty)
                result += std::string((indent + 1) * 2, ' ');
            result += "\"" + key + "\":" + (pretty ? " " : "");
            result += value.toString(pretty, indent + 1);
            if (i < obj.size() - 1)
            {
                result += ",";
            }
            if (pretty)
                result += newline;
            ++i;
        }

        if (pretty)
            result += indent_str;
        result += "}";
        return result;
    }
    }
    return "";
}

LUMOS_INLINE std::string Json::toString(bool pretty, int indent, std::uint8_t indent_size) const
{
    std::string indent_str = pretty ? std::string(indent * indent_size, ' ') : "";
    std::string newline = pretty ? "\n" : "";

    switch (getType())
    {
    case JsonType::Null:
        return "null";
    case JsonType::Boolean:
        return asBool() ? "true" : "false";
    case JsonType::Number:
    {
        double num = asNumber();
        if (num == static_cast<int>(num))
        {
            return std::to_string(static_cast<int>(num));
        }
        return std::to_string(num);
    }
    case JsonType::String:
    {
        std::string result = "\"";
        for (char c : asString())
        {
            switch (c)
            {
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += c;
                break;
            }
        }
        result += "\"";
        return result;
    }
    case JsonType::Array:
    {
        const auto &arr = asArray();
        if (arr.empty())
        {
            return "[]";
        }
        std::string result = "[";
        if (pretty)
            result += newline;

        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (pretty)
                result += std::string((indent + 1) * indent_size, ' ');
            result += arr[i].toString(pretty, indent + 1, indent_size);
            if (i < arr.size() - 1)
            {
                result += ",";
            }
            if (pretty)
                result += newline;
        }

        if (pretty)
            result += indent_str;
        result += "]";
        return result;
    }
    case JsonType::Object:
    {
        const auto &obj = asObject();
        if (obj.empty())
        {
            return "{}";
        }
        std::string result = "{";
        if (pretty)
            result += newline;

        size_t i = 0;
        for (const auto &[key, value] : obj)
        {
            if (pretty)
                result += std::string((indent + 1) * indent_size, ' ');
            result += "\"" + key + "\":" + (pretty ? " " : "");
            result += value.toString(pretty, indent + 1, indent_size);
            if (i < obj.size() - 1)
            {
                result += ",";
            }
            if (pretty)
                result += newline;
            ++i;
        }

        if (pretty)
            result += indent_str;
        result += "}";
        return result;
    }
    }
    return "";
}

// File operations
LUMOS_INLINE void Json::ToFile(const std::string &filename, bool pretty, std::uint8_t indent) const
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << toString(pretty, 0, indent);
    if (!file)
    {
        throw std::runtime_error("Error writing to file: " + filename);
    }
}

// =============================================================================
// JSON PARSING IMPLEMENTATION
// =============================================================================

namespace detail
{
    LUMOS_INLINE void skipWhitespace(const std::string &str, size_t &pos)
    {
        while (pos < str.length() && std::isspace(str[pos]))
        {
            ++pos;
        }
    }

    LUMOS_INLINE std::string parseString(const std::string &str, size_t &pos)
    {
        if (pos >= str.length() || str[pos] != '\"')
        {
            throw std::runtime_error("Expected '\"' at position " + std::to_string(pos));
        }

        ++pos; // Skip opening quote
        std::string result;

        while (pos < str.length() && str[pos] != '\"')
        {
            if (str[pos] == '\\')
            {
                ++pos;
                if (pos >= str.length())
                {
                    throw std::runtime_error("Unexpected end of string in escape sequence");
                }

                switch (str[pos])
                {
                case '\"':
                    result += '\"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '/':
                    result += '/';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                default:
                    throw std::runtime_error("Invalid escape sequence at position " + std::to_string(pos));
                }
            }
            else
            {
                result += str[pos];
            }
            ++pos;
        }

        if (pos >= str.length())
        {
            throw std::runtime_error("Unterminated string");
        }

        ++pos; // Skip closing quote
        return result;
    }

    LUMOS_INLINE double parseNumber(const std::string &str, size_t &pos)
    {
        size_t start = pos;

        if (pos < str.length() && str[pos] == '-')
        {
            ++pos;
        }

        if (pos >= str.length() || !std::isdigit(str[pos]))
        {
            throw std::runtime_error("Invalid number format at position " + std::to_string(start));
        }

        while (pos < str.length() && std::isdigit(str[pos]))
        {
            ++pos;
        }

        if (pos < str.length() && str[pos] == '.')
        {
            ++pos;
            if (pos >= str.length() || !std::isdigit(str[pos]))
            {
                throw std::runtime_error("Invalid number format at position " + std::to_string(start));
            }
            while (pos < str.length() && std::isdigit(str[pos]))
            {
                ++pos;
            }
        }

        if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E'))
        {
            ++pos;
            if (pos < str.length() && (str[pos] == '+' || str[pos] == '-'))
            {
                ++pos;
            }
            if (pos >= str.length() || !std::isdigit(str[pos]))
            {
                throw std::runtime_error("Invalid number format at position " + std::to_string(start));
            }
            while (pos < str.length() && std::isdigit(str[pos]))
            {
                ++pos;
            }
        }

        return std::stod(str.substr(start, pos - start));
    }

    LUMOS_INLINE Json parseValue(const std::string &str, size_t &pos);

    LUMOS_INLINE JsonArray parseArray(const std::string &str, size_t &pos)
    {
        if (pos >= str.length() || str[pos] != '[')
        {
            throw std::runtime_error("Expected '[' at position " + std::to_string(pos));
        }

        ++pos; // Skip '['
        JsonArray array;

        skipWhitespace(str, pos);
        if (pos < str.length() && str[pos] == ']')
        {
            ++pos;
            return array;
        }

        while (pos < str.length())
        {
            array.push_back(parseValue(str, pos));

            skipWhitespace(str, pos);
            if (pos >= str.length())
            {
                throw std::runtime_error("Unexpected end of input in array");
            }

            if (str[pos] == ']')
            {
                ++pos;
                break;
            }
            else if (str[pos] == ',')
            {
                ++pos;
                skipWhitespace(str, pos);
            }
            else
            {
                throw std::runtime_error("Expected ',' or ']' at position " + std::to_string(pos));
            }
        }

        return array;
    }

    LUMOS_INLINE JsonObject parseObject(const std::string &str, size_t &pos)
    {
        if (pos >= str.length() || str[pos] != '{')
        {
            throw std::runtime_error("Expected '{' at position " + std::to_string(pos));
        }

        ++pos; // Skip '{'
        JsonObject object;

        skipWhitespace(str, pos);
        if (pos < str.length() && str[pos] == '}')
        {
            ++pos;
            return object;
        }

        while (pos < str.length())
        {
            skipWhitespace(str, pos);

            // Parse key
            std::string key = parseString(str, pos);

            skipWhitespace(str, pos);
            if (pos >= str.length() || str[pos] != ':')
            {
                throw std::runtime_error("Expected ':' at position " + std::to_string(pos));
            }
            ++pos; // Skip ':'

            skipWhitespace(str, pos);

            // Parse value
            object[key] = parseValue(str, pos);

            skipWhitespace(str, pos);
            if (pos >= str.length())
            {
                throw std::runtime_error("Unexpected end of input in object");
            }

            if (str[pos] == '}')
            {
                ++pos;
                break;
            }
            else if (str[pos] == ',')
            {
                ++pos;
                skipWhitespace(str, pos);
            }
            else
            {
                throw std::runtime_error("Expected ',' or '}' at position " + std::to_string(pos));
            }
        }

        return object;
    }

    LUMOS_INLINE Json parseValue(const std::string &str, size_t &pos)
    {
        skipWhitespace(str, pos);

        if (pos >= str.length())
        {
            throw std::runtime_error("Unexpected end of input");
        }

        char c = str[pos];

        if (c == 'n')
        {
            if (str.substr(pos, 4) == "null")
            {
                pos += 4;
                return Json(nullptr);
            }
        }
        else if (c == 't')
        {
            if (str.substr(pos, 4) == "true")
            {
                pos += 4;
                return Json(true);
            }
        }
        else if (c == 'f')
        {
            if (str.substr(pos, 5) == "false")
            {
                pos += 5;
                return Json(false);
            }
        }
        else if (c == '\"')
        {
            return Json(parseString(str, pos));
        }
        else if (c == '[')
        {
            return Json(parseArray(str, pos));
        }
        else if (c == '{')
        {
            return Json(parseObject(str, pos));
        }
        else if (c == '-' || std::isdigit(c))
        {
            return Json(parseNumber(str, pos));
        }

        throw std::runtime_error("Unexpected character at position " + std::to_string(pos));
    }
} // namespace detail

LUMOS_INLINE Json parseJson(const std::string &json_str)
{
    size_t pos = 0;
    Json result = detail::parseValue(json_str, pos);

    detail::skipWhitespace(json_str, pos);
    if (pos < json_str.length())
    {
        throw std::runtime_error("Unexpected characters after JSON value");
    }

    return result;
}

LUMOS_INLINE Json parseJsonFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    return parseJson(content);
}

LUMOS_INLINE bool writeJsonToFile(const Json &value, const std::string &filename, bool pretty)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    file << value.toString(pretty);
    return true;
}

LUMOS_INLINE bool isValidJson(const std::string &json_str)
{
    try
    {
        parseJson(json_str);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// =============================================================================
// JSON UTILITY FUNCTIONS
// =============================================================================

LUMOS_INLINE Json getValueByPath(const Json &root, const std::string &path)
{
    if (path.empty() || path == ".")
    {
        return root;
    }

    Json current = root;
    std::string remaining = path;

    if (remaining[0] == '.')
    {
        remaining = remaining.substr(1);
    }

    while (!remaining.empty())
    {
        size_t dot_pos = remaining.find('.');
        std::string key = remaining.substr(0, dot_pos);

        if (current.isObject() && current.hasKey(key))
        {
            current = current[key];
        }
        else
        {
            return Json(nullptr);
        }

        if (dot_pos == std::string::npos)
        {
            break;
        }
        remaining = remaining.substr(dot_pos + 1);
    }

    return current;
}

LUMOS_INLINE bool setValueByPath(Json &root, const std::string &path, const Json &value)
{
    if (path.empty() || path == ".")
    {
        root = value;
        return true;
    }

    std::string remaining = path;
    if (remaining[0] == '.')
    {
        remaining = remaining.substr(1);
    }

    Json *current = &root;

    while (true)
    {
        size_t dot_pos = remaining.find('.');
        std::string key = remaining.substr(0, dot_pos);

        if (dot_pos == std::string::npos)
        {
            // Last key, set the value
            (*current)[key] = value;
            return true;
        }

        if (!current->isObject())
        {
            *current = JsonObject{};
        }

        current = &((*current)[key]);
        remaining = remaining.substr(dot_pos + 1);
    }
}

LUMOS_INLINE Json mergeObjects(const Json &obj1, const Json &obj2)
{
    if (!obj1.isObject() || !obj2.isObject())
    {
        throw std::runtime_error("Both values must be objects");
    }

    Json result = obj1;
    const auto &obj2_map = obj2.asObject();

    for (const auto &[key, value] : obj2_map)
    {
        result[key] = value;
    }

    return result;
}

LUMOS_INLINE Json deepCopy(const Json &value)
{
    return Json(value);
}

LUMOS_INLINE std::vector<std::string> getObjectKeys(const Json &obj)
{
    if (!obj.isObject())
    {
        throw std::runtime_error("Value is not an object");
    }

    std::vector<std::string> keys;
    const auto &obj_map = obj.asObject();

    for (const auto &[key, value] : obj_map)
    {
        keys.push_back(key);
    }

    return keys;
}

LUMOS_INLINE size_t getJsonSize(const Json &value)
{
    switch (value.getType())
    {
    case JsonType::Null:
    case JsonType::Boolean:
    case JsonType::Number:
    case JsonType::String:
        return 1;
    case JsonType::Array:
    case JsonType::Object:
        return value.size();
    }
    return 0;
}

// Stream operators
LUMOS_INLINE std::ostream &operator<<(std::ostream &os, const Json &value)
{
    os << value.toString();
    return os;
}

LUMOS_INLINE std::istream &operator>>(std::istream &is, Json &value)
{
    std::string json_str((std::istreambuf_iterator<char>(is)),
                         std::istreambuf_iterator<char>());
    value = parseJson(json_str);
    return is;
}

#endif // JSON_IMPL_H