#ifndef JSON_H
#define JSON_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <limits>
#include <initializer_list>

// Forward declaration
class Json;

// JSON value types
enum class JsonType : std::uint8_t
{
    Null,
    Boolean,
    Number,
    String,
    Array,
    Object
};

// JSON value container
using JsonObject = std::map<std::string, Json>;
using JsonArray = std::vector<Json>;

// JSON value class
class Json
{
private:
    std::variant<
        std::nullptr_t,
        bool,
        double,
        std::string,
        JsonArray,
        JsonObject>
        value_;

public:
    // Constructors
    Json();
    Json(std::nullptr_t);
    Json(bool value);
    Json(int value);
    Json(double value);
    Json(const char *value);
    Json(const std::string &value);
    Json(const JsonArray &value);
    Json(const JsonObject &value);
    Json(std::initializer_list<Json> init_list);
    Json(std::initializer_list<std::pair<std::string, Json>> init_list);

    // Copy and move constructors
    Json(const Json &other) = default;
    Json(Json &&other) noexcept = default;

    // Assignment operators
    Json &operator=(const Json &other) = default;
    Json &operator=(Json &&other) noexcept = default;

    // Static factory methods
    static Json FromFile(const std::string &filename);

    // Type checking
    JsonType getType() const;
    bool isNull() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;

    // Value access
    bool asBool() const;
    double asNumber() const;
    const std::string &asString() const;
    const JsonArray &asArray() const;
    const JsonObject &asObject() const;

    // Mutable value access
    JsonArray &asArray();
    JsonObject &asObject();

    // Templated value access
    template <typename T>
    T as() const;

    // Array operations
    size_t size() const;
    Json &operator[](size_t index);
    const Json &operator[](size_t index) const;
    void push_back(const Json &value);

    // Object operations
    Json &operator[](const std::string &key);
    const Json &operator[](const std::string &key) const;
    bool hasKey(const std::string &key) const;
    void remove(const std::string &key);

    // Comparison operators
    bool operator==(const Json &other) const;
    bool operator!=(const Json &other) const;

    // String representation
    std::string toString(bool pretty = false, int indent = 0) const;
    std::string toString(bool pretty, int indent, std::uint8_t indent_size) const;

    // File operations
    void ToFile(const std::string &filename, bool pretty = false, std::uint8_t indent = 4) const;
};

// JSON parsing and serialization functions
Json parseJson(const std::string &json_str);
Json parseJsonFromFile(const std::string &filename);
bool writeJsonToFile(const Json &value, const std::string &filename, bool pretty = false);

// JSON validation
bool isValidJson(const std::string &json_str);

// JSON path operations
Json getValueByPath(const Json &root, const std::string &path);
bool setValueByPath(Json &root, const std::string &path, const Json &value);

// JSON manipulation utilities
Json mergeObjects(const Json &obj1, const Json &obj2);
Json deepCopy(const Json &value);
std::vector<std::string> getObjectKeys(const Json &obj);
size_t getJsonSize(const Json &value);

// Stream operators
std::ostream &operator<<(std::ostream &os, const Json &value);
std::istream &operator>>(std::istream &is, Json &value);

#endif // JSON_H