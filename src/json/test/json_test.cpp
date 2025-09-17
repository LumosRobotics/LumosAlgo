#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>

#include "json/json.impl.h"

namespace lumos
{

    // Test fixture for JSON functions tests
    class JSONTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Test JSON strings
            simple_json = R"({"name": "John", "age": 30, "active": true})";

            array_json = R"([1, 2, "three", true, null])";

            nested_json = R"({
            "user": {
                "name": "Alice",
                "details": {
                    "age": 25,
                    "email": "alice@example.com"
                }
            },
            "scores": [95, 87, 92]
        })";

            complex_json = R"({
            "string": "Hello \"World\"",
            "number": 42.5,
            "boolean": false,
            "null_value": null,
            "array": [1, 2, 3],
            "object": {
                "nested": "value"
            }
        })";

            // Test file path
            test_filename = "/tmp/lumos_test.json";
        }

        void TearDown() override
        {
            // Clean up test file
            std::remove(test_filename.c_str());
        }

        std::string simple_json, array_json, nested_json, complex_json;
        std::string test_filename;
    };

    // =============================================================================
    // JSON VALUE CONSTRUCTION TESTS
    // =============================================================================

    TEST_F(JSONTest, Assignments)
    {
        Json mixed = {42, "hello", true, 3.14, nullptr, Json::Object({{"nested_key", "value"}}), // object
                      Json::Array({1, 2, 3})};                                                      // array inside array

        Json t1 = {"hello", 42, 3.14, true, nullptr};

        // Test 2: Array with nested array and object
        Json t2 = {
            "string",
            123,
            Json::Array({1, 2, 3}),                    // nested array
            Json::Object({{"key", "value"}, {"x", 99}}) // object
        };

        // Test 3: Mix signed/unsigned/float
        Json t3 = {-10, static_cast<unsigned>(20), 3.14159f};

        // Test 4: Deep nesting
        Json t4 = {
            Json::Object({{"nested_obj", Json::Object({{"id", 1}, {"flag", false}})}}), // object
            Json::Object({{"another", "thing"}}),                                       // object
            Json::Array({1, "two", 3.0, true})                                         // mixed array
        };
    }

    TEST_F(JSONTest, ConstructorTests)
    {
        Json null_val;
        EXPECT_TRUE(null_val.isNull());
        EXPECT_EQ(null_val.getType(), JsonType::Null);

        Json bool_val(true);
        EXPECT_TRUE(bool_val.isBool());
        EXPECT_TRUE(bool_val.asBool());

        Json int_val(42);
        EXPECT_TRUE(int_val.isNumber());
        EXPECT_DOUBLE_EQ(int_val.asNumber(), 42.0);

        Json double_val(3.14);
        EXPECT_TRUE(double_val.isNumber());
        EXPECT_DOUBLE_EQ(double_val.asNumber(), 3.14);

        Json string_val("Hello");
        EXPECT_TRUE(string_val.isString());
        EXPECT_EQ(string_val.asString(), "Hello");

        JsonArray array = {Json(1), Json(2), Json(3)};
        Json array_val(array);
        EXPECT_TRUE(array_val.isArray());
        EXPECT_EQ(array_val.size(), 3U);

        JsonObject object = {{"key", Json("value")}};
        Json object_val(object);
        EXPECT_TRUE(object_val.isObject());
        EXPECT_EQ(object_val.size(), 1U);
    }

    // =============================================================================
    // JSON PARSING TESTS
    // =============================================================================

    TEST_F(JSONTest, ParseSimpleValues)
    {
        Json null_val = parseJson("null");
        EXPECT_TRUE(null_val.isNull());

        Json bool_true = parseJson("true");
        EXPECT_TRUE(bool_true.isBool());
        EXPECT_TRUE(bool_true.asBool());

        Json bool_false = parseJson("false");
        EXPECT_TRUE(bool_false.isBool());
        EXPECT_FALSE(bool_false.asBool());

        Json number = parseJson("42.5");
        EXPECT_TRUE(number.isNumber());
        EXPECT_DOUBLE_EQ(number.asNumber(), 42.5);

        Json string_val = parseJson("\"Hello World\"");
        EXPECT_TRUE(string_val.isString());
        EXPECT_EQ(string_val.asString(), "Hello World");
    }

    TEST_F(JSONTest, ParseStringEscapes)
    {
        Json escaped = parseJson(R"("Hello \"World\" with \\backslash\\ and \n newline")");
        EXPECT_TRUE(escaped.isString());
        EXPECT_EQ(escaped.asString(), "Hello \"World\" with \\backslash\\ and \n newline");
    }

    TEST_F(JSONTest, ParseNumbers)
    {
        EXPECT_DOUBLE_EQ(parseJson("0").asNumber(), 0.0);
        EXPECT_DOUBLE_EQ(parseJson("-42").asNumber(), -42.0);
        EXPECT_DOUBLE_EQ(parseJson("3.14159").asNumber(), 3.14159);
        EXPECT_DOUBLE_EQ(parseJson("-2.718").asNumber(), -2.718);
        EXPECT_DOUBLE_EQ(parseJson("1.23e4").asNumber(), 1.23e4);
        EXPECT_DOUBLE_EQ(parseJson("1.23E-4").asNumber(), 1.23E-4);
    }

    TEST_F(JSONTest, ParseArray)
    {
        Json array = parseJson(array_json);
        EXPECT_TRUE(array.isArray());
        EXPECT_EQ(array.size(), 5U);

        EXPECT_TRUE(array[0].isNumber());
        EXPECT_DOUBLE_EQ(array[0].asNumber(), 1.0);

        EXPECT_TRUE(array[1].isNumber());
        EXPECT_DOUBLE_EQ(array[1].asNumber(), 2.0);

        EXPECT_TRUE(array[2].isString());
        EXPECT_EQ(array[2].asString(), "three");

        EXPECT_TRUE(array[3].isBool());
        EXPECT_TRUE(array[3].asBool());

        EXPECT_TRUE(array[4].isNull());
    }

    TEST_F(JSONTest, ParseObject)
    {
        Json obj = parseJson(simple_json);
        EXPECT_TRUE(obj.isObject());
        EXPECT_EQ(obj.size(), 3U);

        EXPECT_TRUE(obj.hasKey("name"));
        EXPECT_TRUE(obj["name"].isString());
        EXPECT_EQ(obj["name"].asString(), "John");

        EXPECT_TRUE(obj.hasKey("age"));
        EXPECT_TRUE(obj["age"].isNumber());
        EXPECT_DOUBLE_EQ(obj["age"].asNumber(), 30.0);

        EXPECT_TRUE(obj.hasKey("active"));
        EXPECT_TRUE(obj["active"].isBool());
        EXPECT_TRUE(obj["active"].asBool());
    }

    TEST_F(JSONTest, ParseNestedStructures)
    {
        Json nested = parseJson(nested_json);
        EXPECT_TRUE(nested.isObject());

        EXPECT_TRUE(nested.hasKey("user"));
        Json user = nested["user"];
        EXPECT_TRUE(user.isObject());

        EXPECT_EQ(user["name"].asString(), "Alice");

        Json details = user["details"];
        EXPECT_TRUE(details.isObject());
        EXPECT_DOUBLE_EQ(details["age"].asNumber(), 25.0);
        EXPECT_EQ(details["email"].asString(), "alice@example.com");

        EXPECT_TRUE(nested.hasKey("scores"));
        Json scores = nested["scores"];
        EXPECT_TRUE(scores.isArray());
        EXPECT_EQ(scores.size(), 3U);
        EXPECT_DOUBLE_EQ(scores[0].asNumber(), 95.0);
    }

    // =============================================================================
    // JSON SERIALIZATION TESTS
    // =============================================================================

    TEST_F(JSONTest, ToStringSimpleValues)
    {
        EXPECT_EQ(Json().toString(), "null");
        EXPECT_EQ(Json(true).toString(), "true");
        EXPECT_EQ(Json(false).toString(), "false");
        EXPECT_EQ(Json(42).toString(), "42");
        EXPECT_EQ(Json(3.14).toString(), "3.140000");
        EXPECT_EQ(Json("Hello").toString(), "\"Hello\"");
    }

    TEST_F(JSONTest, ToStringArray)
    {
        JsonArray arr = {Json(1), Json("two"), Json(true)};
        Json array_val(arr);

        std::string result = array_val.toString();
        EXPECT_EQ(result, "[1,\"two\",true]");
    }

    TEST_F(JSONTest, ToStringObject)
    {
        JsonObject obj = {
            {"name", Json("John")},
            {"age", Json(30)},
            {"active", Json(true)}};
        Json object_val(obj);

        std::string result = object_val.toString();
        // Note: map order might vary, so we'll parse it back to verify
        Json parsed = parseJson(result);
        EXPECT_EQ(parsed["name"].asString(), "John");
        EXPECT_DOUBLE_EQ(parsed["age"].asNumber(), 30.0);
        EXPECT_TRUE(parsed["active"].asBool());
    }

    TEST_F(JSONTest, ToStringPrettyPrint)
    {
        JsonObject obj = {
            {"name", Json("John")},
            {"age", Json(30)}};
        Json object_val(obj);

        std::string pretty = object_val.toString(true);
        EXPECT_TRUE(pretty.find("\n") != std::string::npos);
        EXPECT_TRUE(pretty.find("  ") != std::string::npos); // Indentation
    }

    TEST_F(JSONTest, RoundTripParsing)
    {
        Json original = parseJson(complex_json);
        std::string serialized = original.toString();
        Json parsed_back = parseJson(serialized);

        EXPECT_EQ(original, parsed_back);
    }

    // =============================================================================
    // JSON VALUE OPERATIONS TESTS
    // =============================================================================

    TEST_F(JSONTest, ArrayOperations)
    {
        Json array_val;

        array_val.push_back(Json(1));
        array_val.push_back(Json("two"));
        array_val.push_back(Json(true));

        EXPECT_TRUE(array_val.isArray());
        EXPECT_EQ(array_val.size(), 3U);

        EXPECT_DOUBLE_EQ(array_val[0].asNumber(), 1.0);
        EXPECT_EQ(array_val[1].asString(), "two");
        EXPECT_TRUE(array_val[2].asBool());

        // Test index assignment
        array_val[1] = Json(42);
        EXPECT_DOUBLE_EQ(array_val[1].asNumber(), 42.0);
    }

    TEST_F(JSONTest, ObjectOperations)
    {
        Json object_val;

        object_val["name"] = Json("Alice");
        object_val["age"] = Json(25);
        object_val["active"] = Json(true);

        EXPECT_TRUE(object_val.isObject());
        EXPECT_EQ(object_val.size(), 3U);

        EXPECT_TRUE(object_val.hasKey("name"));
        EXPECT_FALSE(object_val.hasKey("nonexistent"));

        EXPECT_EQ(object_val["name"].asString(), "Alice");
        EXPECT_DOUBLE_EQ(object_val["age"].asNumber(), 25.0);
        EXPECT_TRUE(object_val["active"].asBool());

        // Test key removal
        object_val.remove("age");
        EXPECT_FALSE(object_val.hasKey("age"));
        EXPECT_EQ(object_val.size(), 2U);
    }

    // =============================================================================
    // FILE I/O TESTS
    // =============================================================================

    TEST_F(JSONTest, FileReadWrite)
    {
        Json original = parseJson(simple_json);

        // Write to file
        EXPECT_TRUE(writeJsonToFile(original, test_filename));

        // Read back from file
        Json loaded = parseJsonFromFile(test_filename);

        EXPECT_EQ(original, loaded);
    }

    TEST_F(JSONTest, FileReadWritePretty)
    {
        Json original = parseJson(simple_json);

        // Write pretty-printed JSON to file
        EXPECT_TRUE(writeJsonToFile(original, test_filename, true));

        // Read back from file
        Json loaded = parseJsonFromFile(test_filename);

        EXPECT_EQ(original, loaded);
    }

    TEST_F(JSONTest, FileReadNonExistent)
    {
        EXPECT_THROW(parseJsonFromFile("/non/existent/file.json"), std::runtime_error);
    }

    // =============================================================================
    // JSON UTILITY FUNCTION TESTS
    // =============================================================================

    TEST_F(JSONTest, PathOperations)
    {
        Json root = parseJson(nested_json);

        // First, let's test basic object access directly
        EXPECT_TRUE(root.isObject());
        EXPECT_TRUE(root.hasKey("user"));
    }

    TEST_F(JSONTest, ObjectMerging)
    {
        Json obj1 = parseJson(R"({"a": 1, "b": 2})");
        Json obj2 = parseJson(R"({"b": 3, "c": 4})");

        Json merged = mergeObjects(obj1, obj2);

        EXPECT_TRUE(merged.isObject());
        EXPECT_EQ(merged.size(), 3U);
        EXPECT_DOUBLE_EQ(merged["a"].asNumber(), 1.0);
        EXPECT_DOUBLE_EQ(merged["b"].asNumber(), 3.0); // obj2 value overwrites obj1
        EXPECT_DOUBLE_EQ(merged["c"].asNumber(), 4.0);
    }

    TEST_F(JSONTest, GetObjectKeys)
    {
        Json obj = parseJson(simple_json);
        std::vector<std::string> keys = getObjectKeys(obj);

        EXPECT_EQ(keys.size(), 3U);
        // Keys are stored in map, so they're sorted
        std::sort(keys.begin(), keys.end());
        EXPECT_EQ(keys[0], "active");
        EXPECT_EQ(keys[1], "age");
        EXPECT_EQ(keys[2], "name");
    }

    TEST_F(JSONTest, JsonSize)
    {
        EXPECT_EQ(getJsonSize(Json()), 1U);
        EXPECT_EQ(getJsonSize(Json(true)), 1U);
        EXPECT_EQ(getJsonSize(Json("test")), 1U);

        Json array = parseJson("[1,2,3]");
        EXPECT_EQ(getJsonSize(array), 3U);

        Json object = parseJson(simple_json);
        EXPECT_EQ(getJsonSize(object), 3U);
    }

    // =============================================================================
    // ERROR HANDLING TESTS
    // =============================================================================

    TEST_F(JSONTest, ParseErrors)
    {
        EXPECT_THROW(parseJson(""), std::runtime_error);
        EXPECT_THROW(parseJson("{"), std::runtime_error);
        EXPECT_THROW(parseJson("}"), std::runtime_error);
        EXPECT_THROW(parseJson("["), std::runtime_error);
        EXPECT_THROW(parseJson("]"), std::runtime_error);
        EXPECT_THROW(parseJson("\"unterminated string"), std::runtime_error);
        EXPECT_THROW(parseJson("invalid"), std::runtime_error);
        EXPECT_THROW(parseJson("{\"key\"}"), std::runtime_error);           // Missing value
        EXPECT_THROW(parseJson("{\"key\" \"value\"}"), std::runtime_error); // Missing colon
    }

    TEST_F(JSONTest, TypeErrors)
    {
        Json string_val("hello");

        EXPECT_THROW(string_val.asBool(), std::runtime_error);
        EXPECT_THROW(string_val.asNumber(), std::runtime_error);
        EXPECT_THROW(string_val.asArray(), std::runtime_error);
        EXPECT_THROW(string_val.asObject(), std::runtime_error);

        Json array_val = parseJson("[1,2,3]");
        EXPECT_THROW(array_val["key"], std::runtime_error); // Array accessed as object

        Json object_val = parseJson("{\"key\":\"value\"}");
        EXPECT_THROW(object_val[0], std::runtime_error); // Object accessed as array
    }

    TEST_F(JSONTest, OutOfRangeErrors)
    {
        Json array_val = parseJson("[1,2,3]");
        EXPECT_THROW(array_val[10], std::out_of_range);

        Json object_val = parseJson("{\"key\":\"value\"}");
        EXPECT_THROW(object_val["nonexistent"], std::out_of_range);
    }

    // =============================================================================
    // JSON VALIDATION TESTS
    // =============================================================================

    TEST_F(JSONTest, JsonValidation)
    {
        EXPECT_TRUE(isValidJson("null"));
        EXPECT_TRUE(isValidJson("true"));
        EXPECT_TRUE(isValidJson("42"));
        EXPECT_TRUE(isValidJson("\"string\""));
        EXPECT_TRUE(isValidJson("[]"));
        EXPECT_TRUE(isValidJson("{}"));
        EXPECT_TRUE(isValidJson(simple_json));
        EXPECT_TRUE(isValidJson(nested_json));

        EXPECT_FALSE(isValidJson(""));
        EXPECT_FALSE(isValidJson("invalid"));
        EXPECT_FALSE(isValidJson("{"));
        EXPECT_FALSE(isValidJson("\"unterminated"));
        EXPECT_FALSE(isValidJson("[1,2,]"));
        EXPECT_FALSE(isValidJson("{\"key\":}"));
    }

    // =============================================================================
    // STREAM OPERATIONS TESTS
    // =============================================================================

    TEST_F(JSONTest, StreamOutput)
    {
        Json value = parseJson(simple_json);

        std::ostringstream oss;
        oss << value;

        Json parsed_back = parseJson(oss.str());
        EXPECT_EQ(value, parsed_back);
    }

    TEST_F(JSONTest, StreamInput)
    {
        std::istringstream iss(simple_json);
        Json value;

        iss >> value;

        EXPECT_TRUE(value.isObject());
        EXPECT_EQ(value["name"].asString(), "John");
        EXPECT_DOUBLE_EQ(value["age"].asNumber(), 30.0);
    }

    // =============================================================================
    // COMPARISON TESTS
    // =============================================================================

    TEST_F(JSONTest, EqualityComparison)
    {
        Json val1 = parseJson(simple_json);
        Json val2 = parseJson(simple_json);
        Json val3 = parseJson(array_json);

        EXPECT_TRUE(val1 == val2);
        EXPECT_FALSE(val1 == val3);
        EXPECT_TRUE(val1 != val3);
        EXPECT_FALSE(val1 != val2);
    }

    TEST_F(JSONTest, TemplatedAsFunction)
    {
        // Test basic types
        Json bool_val(true);
        EXPECT_EQ(bool_val.as<bool>(), true);

        Json string_val("hello");
        EXPECT_EQ(string_val.as<std::string>(), "hello");

        Json number_val(42.5);
        EXPECT_DOUBLE_EQ(number_val.as<double>(), 42.5);
        EXPECT_FLOAT_EQ(number_val.as<float>(), 42.5f);

        // Test integer types
        Json int_val(123);
        EXPECT_EQ(int_val.as<int8_t>(), 123);
        EXPECT_EQ(int_val.as<int16_t>(), 123);
        EXPECT_EQ(int_val.as<int32_t>(), 123);
        EXPECT_EQ(int_val.as<int64_t>(), 123);
        EXPECT_EQ(int_val.as<uint8_t>(), 123);
        EXPECT_EQ(int_val.as<uint16_t>(), 123);
        EXPECT_EQ(int_val.as<uint32_t>(), 123U);
        EXPECT_EQ(int_val.as<uint64_t>(), 123ULL);
    }

    TEST_F(JSONTest, TemplatedAsRangeChecking)
    {
        // Test range checking for int8_t
        Json large_val(1000);
        EXPECT_THROW(large_val.as<int8_t>(), std::out_of_range);

        Json negative_val(-10);
        EXPECT_THROW(negative_val.as<uint8_t>(), std::out_of_range);

        // Test valid range
        Json valid_val(100);
        EXPECT_EQ(valid_val.as<int8_t>(), 100);
        EXPECT_EQ(valid_val.as<uint8_t>(), 100);
    }

    TEST_F(JSONTest, TemplatedAsTypeErrors)
    {
        Json string_val("hello");
        EXPECT_THROW(string_val.as<int32_t>(), std::runtime_error);
        EXPECT_THROW(string_val.as<double>(), std::runtime_error);
        EXPECT_THROW(string_val.as<bool>(), std::runtime_error);

        Json number_val(42);
        EXPECT_THROW(number_val.as<std::string>(), std::runtime_error);
    }

    TEST_F(JSONTest, TemplatedAsVectorTypes)
    {
        // Test vector<int>
        Json int_array = parseJson("[1, 2, 3, 4, 5]");
        std::vector<int32_t> int_vec = int_array.as<std::vector<int32_t>>();
        EXPECT_EQ(int_vec.size(), 5U);
        EXPECT_EQ(int_vec[0], 1);
        EXPECT_EQ(int_vec[4], 5);

        // Test vector<string>
        Json string_array = parseJson(R"(["hello", "world", "test"])");
        std::vector<std::string> string_vec = string_array.as<std::vector<std::string>>();
        EXPECT_EQ(string_vec.size(), 3U);
        EXPECT_EQ(string_vec[0], "hello");
        EXPECT_EQ(string_vec[2], "test");

        // Test vector<bool>
        Json bool_array = parseJson("[true, false, true]");
        std::vector<bool> bool_vec = bool_array.as<std::vector<bool>>();
        EXPECT_EQ(bool_vec.size(), 3U);
        EXPECT_EQ(bool_vec[0], true);
        EXPECT_EQ(bool_vec[1], false);

        // Test vector<double>
        Json double_array = parseJson("[1.1, 2.2, 3.3]");
        std::vector<double> double_vec = double_array.as<std::vector<double>>();
        EXPECT_EQ(double_vec.size(), 3U);
        EXPECT_DOUBLE_EQ(double_vec[0], 1.1);
        EXPECT_DOUBLE_EQ(double_vec[2], 3.3);
    }

    TEST_F(JSONTest, TemplatedAsVectorJson)
    {
        // Test vector<Json> for mixed array
        Json mixed_array = parseJson(R"([1, "hello", true, null, {"key": "value"}])");
        std::vector<Json> json_vec = mixed_array.as<std::vector<Json>>();

        EXPECT_EQ(json_vec.size(), 5U);
        EXPECT_TRUE(json_vec[0].isNumber());
        EXPECT_EQ(json_vec[0].as<int32_t>(), 1);
        EXPECT_TRUE(json_vec[1].isString());
        EXPECT_EQ(json_vec[1].as<std::string>(), "hello");
        EXPECT_TRUE(json_vec[2].isBool());
        EXPECT_EQ(json_vec[2].as<bool>(), true);
        EXPECT_TRUE(json_vec[3].isNull());
        EXPECT_TRUE(json_vec[4].isObject());
        EXPECT_EQ(json_vec[4]["key"].as<std::string>(), "value");
    }

    TEST_F(JSONTest, TemplatedAsVectorErrors)
    {
        // Test error when trying to convert non-array to vector
        Json string_val("hello");
        EXPECT_THROW(string_val.as<std::vector<int32_t>>(), std::runtime_error);

        Json object_val = parseJson(R"({"key": "value"})");
        EXPECT_THROW(object_val.as<std::vector<std::string>>(), std::runtime_error);

        // Test error when array elements can't be converted
        Json mixed_array = parseJson(R"([1, "hello", true])");
        EXPECT_THROW(mixed_array.as<std::vector<int32_t>>(), std::runtime_error);
    }

    TEST_F(JSONTest, FromFileFunction)
    {
        // Create a test JSON file
        std::string test_json_content = R"({
    "name": "Alice",
    "age": 30,
    "active": true,
    "scores": [85, 92, 78],
    "address": {
        "street": "123 Main St",
        "city": "Anytown"
    }
})";

        // Write test content to file
        std::ofstream test_file(test_filename);
        test_file << test_json_content;
        test_file.close();

        // Test FromFile function
        Json loaded_json = Json::FromFile(test_filename);

        // Verify the loaded content
        EXPECT_TRUE(loaded_json.isObject());
        EXPECT_EQ(loaded_json["name"].as<std::string>(), "Alice");
        EXPECT_EQ(loaded_json["age"].as<int32_t>(), 30);
        EXPECT_TRUE(loaded_json["active"].as<bool>());

        // Test array
        EXPECT_TRUE(loaded_json["scores"].isArray());
        EXPECT_EQ(loaded_json["scores"].size(), 3U);
        EXPECT_EQ(loaded_json["scores"][0].as<int32_t>(), 85);
        EXPECT_EQ(loaded_json["scores"][2].as<int32_t>(), 78);

        // Test nested object
        EXPECT_TRUE(loaded_json["address"].isObject());
        EXPECT_EQ(loaded_json["address"]["street"].as<std::string>(), "123 Main St");
        EXPECT_EQ(loaded_json["address"]["city"].as<std::string>(), "Anytown");
    }

    TEST_F(JSONTest, FromFileErrors)
    {
        // Test with non-existent file
        EXPECT_THROW(Json::FromFile("/non/existent/file.json"), std::runtime_error);

        // Test with invalid JSON content
        std::string invalid_json_file = "/tmp/invalid_test.json";
        std::ofstream invalid_file(invalid_json_file);
        invalid_file << "{ invalid json content }";
        invalid_file.close();

        EXPECT_THROW(Json::FromFile(invalid_json_file), std::runtime_error);

        // Clean up
        std::remove(invalid_json_file.c_str());
    }

    TEST_F(JSONTest, FromFileEmpty)
    {
        // Test with empty file
        std::string empty_json_file = "/tmp/empty_test.json";
        std::ofstream empty_file(empty_json_file);
        empty_file.close();

        EXPECT_THROW(Json::FromFile(empty_json_file), std::runtime_error);

        // Clean up
        std::remove(empty_json_file.c_str());
    }

    TEST_F(JSONTest, ToFileFunction)
    {
        // Create a JSON object
        Json json_obj;
        json_obj["name"] = Json("Bob");
        json_obj["age"] = Json(25);
        json_obj["active"] = Json(false);

        // Add nested array
        JsonArray scores = {Json(88), Json(95), Json(72)};
        json_obj["scores"] = Json(scores);

        // Add nested object
        Json address;
        address["street"] = Json("456 Oak Ave");
        address["city"] = Json("Springfield");
        json_obj["address"] = address;

        // Write to file
        std::string output_file = "/tmp/test_output.json";
        json_obj.ToFile(output_file);

        // Read back and verify
        Json loaded_json = Json::FromFile(output_file);

        EXPECT_TRUE(loaded_json.isObject());
        EXPECT_EQ(loaded_json["name"].as<std::string>(), "Bob");
        EXPECT_EQ(loaded_json["age"].as<int32_t>(), 25);
        EXPECT_FALSE(loaded_json["active"].as<bool>());

        // Verify array
        EXPECT_TRUE(loaded_json["scores"].isArray());
        EXPECT_EQ(loaded_json["scores"].size(), 3U);
        EXPECT_EQ(loaded_json["scores"][1].as<int32_t>(), 95);

        // Verify nested object
        EXPECT_EQ(loaded_json["address"]["street"].as<std::string>(), "456 Oak Ave");
        EXPECT_EQ(loaded_json["address"]["city"].as<std::string>(), "Springfield");

        // Clean up
        std::remove(output_file.c_str());
    }

    TEST_F(JSONTest, ToFilePrettyFormat)
    {
        // Create a JSON object
        Json json_obj;
        json_obj["name"] = Json("Carol");
        json_obj["numbers"] = Json(JsonArray{Json(1), Json(2), Json(3)});

        Json nested;
        nested["key"] = Json("value");
        json_obj["nested"] = nested;

        // Write with pretty formatting
        std::string pretty_file = "/tmp/test_pretty.json";
        json_obj.ToFile(pretty_file, true);

        // Read the file content as string to verify formatting
        std::ifstream file(pretty_file);
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        // Check that pretty formatting includes newlines and indentation
        EXPECT_TRUE(content.find('\n') != std::string::npos);
        EXPECT_TRUE(content.find("  ") != std::string::npos); // Should have indentation

        // Verify it's still valid JSON by parsing it back
        Json loaded_json = Json::FromFile(pretty_file);
        EXPECT_EQ(loaded_json["name"].as<std::string>(), "Carol");
        EXPECT_EQ(loaded_json["nested"]["key"].as<std::string>(), "value");

        // Clean up
        std::remove(pretty_file.c_str());
    }

    TEST_F(JSONTest, ToFileCompactFormat)
    {
        // Create a JSON object
        Json json_obj;
        json_obj["test"] = Json("compact");
        json_obj["array"] = Json(JsonArray{Json(1), Json(2)});

        // Write with compact formatting (default)
        std::string compact_file = "/tmp/test_compact.json";
        json_obj.ToFile(compact_file, false);

        // Read the file content as string
        std::ifstream file(compact_file);
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        // Check that compact format doesn't include extra whitespace
        EXPECT_TRUE(content.find('\n') == std::string::npos);
        EXPECT_TRUE(content.find("  ") == std::string::npos); // No indentation

        // Verify it's still valid JSON
        Json loaded_json = Json::FromFile(compact_file);
        EXPECT_EQ(loaded_json["test"].as<std::string>(), "compact");

        // Clean up
        std::remove(compact_file.c_str());
    }

    TEST_F(JSONTest, ToFileErrors)
    {
        Json json_obj;
        json_obj["test"] = Json("value");

        // Test with invalid file path (directory that doesn't exist)
        EXPECT_THROW(json_obj.ToFile("/non/existent/directory/file.json"), std::runtime_error);

        // Test with invalid file name (empty string)
        EXPECT_THROW(json_obj.ToFile(""), std::runtime_error);
    }

    TEST_F(JSONTest, ToFileRoundTrip)
    {
        // Test round trip: create -> save -> load -> save -> load
        Json original = parseJson(complex_json);

        std::string temp_file = "/tmp/roundtrip_test.json";

        // First save
        original.ToFile(temp_file, true);

        // Load back
        Json loaded = Json::FromFile(temp_file);

        // Save again
        loaded.ToFile(temp_file, false);

        // Load again
        Json final_loaded = Json::FromFile(temp_file);

        // Should be identical to original
        EXPECT_EQ(original, final_loaded);

        // Clean up
        std::remove(temp_file.c_str());
    }

    TEST_F(JSONTest, ToFileCustomIndent)
    {
        // Create a nested JSON object
        Json json_obj;
        json_obj["level1"] = Json("value1");

        Json nested;
        nested["level2"] = Json("value2");

        Json deep_nested;
        deep_nested["level3"] = Json("value3");
        nested["nested"] = deep_nested;

        json_obj["nested_obj"] = nested;
        json_obj["array"] = Json(JsonArray{Json(1), Json(2), Json(3)});

        // Test with 2-space indentation
        std::string file_2_spaces = "/tmp/test_2_spaces.json";
        json_obj.ToFile(file_2_spaces, true, 2);

        // Read content to verify indentation
        std::ifstream file_2(file_2_spaces);
        std::string content_2((std::istreambuf_iterator<char>(file_2)),
                              std::istreambuf_iterator<char>());
        file_2.close();

        // Should contain 2-space indentation patterns
        EXPECT_TRUE(content_2.find("  \"level1\"") != std::string::npos);
        EXPECT_TRUE(content_2.find("    \"level2\"") != std::string::npos);   // nested level
        EXPECT_TRUE(content_2.find("      \"level3\"") != std::string::npos); // deep nested

        // Test with 8-space indentation
        std::string file_8_spaces = "/tmp/test_8_spaces.json";
        json_obj.ToFile(file_8_spaces, true, 8);

        // Read content to verify indentation
        std::ifstream file_8(file_8_spaces);
        std::string content_8((std::istreambuf_iterator<char>(file_8)),
                              std::istreambuf_iterator<char>());
        file_8.close();

        // Should contain 8-space indentation patterns
        EXPECT_TRUE(content_8.find("        \"level1\"") != std::string::npos);
        EXPECT_TRUE(content_8.find("                \"level2\"") != std::string::npos); // 16 spaces for nested

        // Verify both files parse to the same JSON
        Json loaded_2 = Json::FromFile(file_2_spaces);
        Json loaded_8 = Json::FromFile(file_8_spaces);

        EXPECT_EQ(loaded_2, loaded_8);
        EXPECT_EQ(loaded_2, json_obj);

        // Clean up
        std::remove(file_2_spaces.c_str());
        std::remove(file_8_spaces.c_str());
    }

    TEST_F(JSONTest, ToFileDefaultIndent)
    {
        // Test that default indent is 4 spaces
        Json json_obj;
        json_obj["test"] = Json("value");

        Json nested;
        nested["inner"] = Json("inner_value");
        json_obj["nested"] = nested;

        std::string default_file = "/tmp/test_default_indent.json";
        json_obj.ToFile(default_file, true); // Should use default 4-space indent

        // Read content
        std::ifstream file(default_file);
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        // Should contain 4-space indentation
        EXPECT_TRUE(content.find("    \"test\"") != std::string::npos);
        EXPECT_TRUE(content.find("        \"inner\"") != std::string::npos); // nested level (8 spaces)

        // Clean up
        std::remove(default_file.c_str());
    }

    TEST_F(JSONTest, ToFileIndentWithCompact)
    {
        // Test that indent parameter is ignored when pretty=false
        Json json_obj;
        json_obj["test"] = Json("compact");

        std::string compact_file = "/tmp/test_compact_indent.json";
        json_obj.ToFile(compact_file, false, 8); // Indent should be ignored

        // Read content
        std::ifstream file(compact_file);
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        // Should not contain any extra spaces or newlines
        EXPECT_TRUE(content.find('\n') == std::string::npos);
        EXPECT_TRUE(content.find("        ") == std::string::npos); // No 8-space indents

        // Should still be valid JSON
        Json loaded = Json::FromFile(compact_file);
        EXPECT_EQ(loaded["test"].as<std::string>(), "compact");

        // Clean up
        std::remove(compact_file.c_str());
    }

    TEST_F(JSONTest, TestFileAccessPatterns)
    {
        // Load the test JSON file
        std::string test_file_path = "/Users/danielpi/work/LumosAlgo/src/json/test/data/test_file.json";
        Json json_obj = Json::FromFile(test_file_path);

        // Test basic object access patterns
        Json json_sub_obj = json_obj["an_object"];
        const std::string name = json_sub_obj["key"].as<std::string>();
        const std::string also_name = json_obj["an_object"]["key"].as<std::string>();

        EXPECT_EQ(name, "value");
        EXPECT_EQ(also_name, "value");
        EXPECT_EQ(name, also_name);

        // Test nested object access
        Json json_nested_obj = json_obj["nested"];
        std::vector<std::uint16_t> nested_array = json_nested_obj["nested_array"].as<std::vector<std::uint16_t>>();
        const std::string s = json_nested_obj["nested_value"].as<std::string>();

        EXPECT_EQ(nested_array.size(), 3U);
        EXPECT_EQ(nested_array[0], 5);
        EXPECT_EQ(nested_array[1], 6);
        EXPECT_EQ(nested_array[2], 7);
        EXPECT_EQ(s, "nested_string_value");

        // Test array of objects access
        Json obj0 = json_obj["array_of_objects"][0];
        Json obj1 = json_obj["array_of_objects"][1];

        EXPECT_TRUE(obj0.isObject());
        EXPECT_TRUE(obj1.isObject());

        const std::string array_obj_str = obj1["obj_key1"].as<std::string>();
        EXPECT_EQ(array_obj_str, "obj_value2");

        // Test direct chained access
        EXPECT_EQ(json_obj["array_of_objects"][0]["obj_key1"].as<std::string>(), "obj_value1");
        EXPECT_EQ(json_obj["array_of_objects"][1]["obj_key1"].as<std::string>(), "obj_value2");

        // Test various type conversions
        EXPECT_EQ(json_obj["an_int"].as<int32_t>(), 42);
        EXPECT_EQ(json_obj["a_negative_int"].as<int32_t>(), -7);
        EXPECT_TRUE(json_obj["null_value"].isNull());
        EXPECT_EQ(json_obj["a_string"].as<std::string>(), "hello");
        EXPECT_TRUE(json_obj["a_bool"].as<bool>());
        EXPECT_FLOAT_EQ(json_obj["a_float"].as<float>(), 3.14f);

        // Test array access
        std::vector<int32_t> an_array = json_obj["an_array"].as<std::vector<int32_t>>();
        EXPECT_EQ(an_array.size(), 4U);
        EXPECT_EQ(an_array[0], 1);
        EXPECT_EQ(an_array[3], 4);
    }

    TEST_F(JSONTest, AssignmentPatterns)
    {
        Json json_obj;

        // Test initializer list assignment for object
        json_obj["extra"] = Json(JsonObject{{"a", 10}, {"b", 20.3}, {"c", false}});

        EXPECT_TRUE(json_obj["extra"].isObject());
        EXPECT_EQ(json_obj["extra"]["a"].as<int32_t>(), 10);
        EXPECT_DOUBLE_EQ(json_obj["extra"]["b"].as<double>(), 20.3);
        EXPECT_FALSE(json_obj["extra"]["c"].as<bool>());

        // Test direct value assignments
        json_obj["status"] = "updated";
        json_obj["num1"] = 0.1;
        json_obj["num2"] = 2;
        json_obj["num3"] = -3;
        json_obj["a_bool_val"] = true;

        // Verify all assignments worked
        EXPECT_EQ(json_obj["status"].as<std::string>(), "updated");
        EXPECT_DOUBLE_EQ(json_obj["num1"].as<double>(), 0.1);
        EXPECT_EQ(json_obj["num2"].as<int32_t>(), 2);
        EXPECT_EQ(json_obj["num3"].as<int32_t>(), -3);
        EXPECT_TRUE(json_obj["a_bool_val"].as<bool>());

        // Test initializer list assignment for array
        json_obj["array_values"] = {1, "hello", true, 3.14};

        EXPECT_TRUE(json_obj["array_values"].isArray());
        EXPECT_EQ(json_obj["array_values"].size(), 4U);
        EXPECT_EQ(json_obj["array_values"][0].as<int32_t>(), 1);
        EXPECT_EQ(json_obj["array_values"][1].as<std::string>(), "hello");
        EXPECT_TRUE(json_obj["array_values"][2].as<bool>());
        EXPECT_DOUBLE_EQ(json_obj["array_values"][3].as<double>(), 3.14);

        // Test nested initializer lists
        json_obj["nested_data"] = Json(JsonObject{{"users", Json(JsonObject{{"name", "Alice"}, {"age", 30}})}, {"active", true}});

        EXPECT_TRUE(json_obj["nested_data"].isObject());
        EXPECT_TRUE(json_obj["nested_data"]["users"].isObject());
        EXPECT_EQ(json_obj["nested_data"]["users"]["name"].as<std::string>(), "Alice");
        EXPECT_EQ(json_obj["nested_data"]["users"]["age"].as<int32_t>(), 30);
        EXPECT_TRUE(json_obj["nested_data"]["active"].as<bool>());

        // Test modifying existing values
        json_obj["num2"] = 42;
        json_obj["status"] = "modified";

        EXPECT_EQ(json_obj["num2"].as<int32_t>(), 42);
        EXPECT_EQ(json_obj["status"].as<std::string>(), "modified");

        // Test assignment chaining
        json_obj["chain"]["level1"]["level2"] = "deep_value";
        EXPECT_EQ(json_obj["chain"]["level1"]["level2"].as<std::string>(), "deep_value");
    }

    TEST_F(JSONTest, ArrayConstructionAndAssignment)
    {
        Json json_obj;

        // Test the specific cases requested
        Json nested_array = {{1, 2}, {3, 4}};
        Json an_array = {1, 2, 3, 4};
        json_obj["nested_array"] = nested_array;
        json_obj["an_array"] = an_array;

        // Verify nested_array structure
        EXPECT_TRUE(nested_array.isArray());
        EXPECT_EQ(nested_array.size(), 2U);

        // First sub-array
        EXPECT_TRUE(nested_array[0].isArray());
        EXPECT_EQ(nested_array[0].size(), 2U);
        EXPECT_EQ(nested_array[0][0].as<int32_t>(), 1);
        EXPECT_EQ(nested_array[0][1].as<int32_t>(), 2);

        // Second sub-array
        EXPECT_TRUE(nested_array[1].isArray());
        EXPECT_EQ(nested_array[1].size(), 2U);
        EXPECT_EQ(nested_array[1][0].as<int32_t>(), 3);
        EXPECT_EQ(nested_array[1][1].as<int32_t>(), 4);

        // Verify simple array structure
        EXPECT_TRUE(an_array.isArray());
        EXPECT_EQ(an_array.size(), 4U);
        EXPECT_EQ(an_array[0].as<int32_t>(), 1);
        EXPECT_EQ(an_array[1].as<int32_t>(), 2);
        EXPECT_EQ(an_array[2].as<int32_t>(), 3);
        EXPECT_EQ(an_array[3].as<int32_t>(), 4);

        // Verify assignments to json_obj
        EXPECT_TRUE(json_obj["nested_array"].isArray());
        EXPECT_EQ(json_obj["nested_array"].size(), 2U);
        EXPECT_EQ(json_obj["nested_array"][0][0].as<int32_t>(), 1);
        EXPECT_EQ(json_obj["nested_array"][0][1].as<int32_t>(), 2);
        EXPECT_EQ(json_obj["nested_array"][1][0].as<int32_t>(), 3);
        EXPECT_EQ(json_obj["nested_array"][1][1].as<int32_t>(), 4);

        EXPECT_TRUE(json_obj["an_array"].isArray());
        EXPECT_EQ(json_obj["an_array"].size(), 4U);
        EXPECT_EQ(json_obj["an_array"][0].as<int32_t>(), 1);
        EXPECT_EQ(json_obj["an_array"][3].as<int32_t>(), 4);

        // Test more complex nested arrays
        Json complex_nested = {
            {1, "hello", true},
            {2.5, false, "world"},
            {{10, 20}, {30, 40}}};

        EXPECT_TRUE(complex_nested.isArray());
        EXPECT_EQ(complex_nested.size(), 3U);

        // First sub-array (mixed types)
        EXPECT_EQ(complex_nested[0][0].as<int32_t>(), 1);
        EXPECT_EQ(complex_nested[0][1].as<std::string>(), "hello");
        EXPECT_TRUE(complex_nested[0][2].as<bool>());

        // Second sub-array (mixed types)
        EXPECT_DOUBLE_EQ(complex_nested[1][0].as<double>(), 2.5);
        EXPECT_FALSE(complex_nested[1][1].as<bool>());
        EXPECT_EQ(complex_nested[1][2].as<std::string>(), "world");

        // Third sub-array (nested arrays)
        EXPECT_TRUE(complex_nested[2].isArray());
        EXPECT_EQ(complex_nested[2].size(), 2U);
        EXPECT_EQ(complex_nested[2][0][0].as<int32_t>(), 10);
        EXPECT_EQ(complex_nested[2][0][1].as<int32_t>(), 20);
        EXPECT_EQ(complex_nested[2][1][0].as<int32_t>(), 30);
        EXPECT_EQ(complex_nested[2][1][1].as<int32_t>(), 40);

        // Test direct assignment
        json_obj["complex"] = complex_nested;
        EXPECT_EQ(json_obj["complex"], complex_nested);

        // Test modification of assigned arrays
        json_obj["an_array"][0] = 100;
        EXPECT_EQ(json_obj["an_array"][0].as<int32_t>(), 100);

        // Original an_array should be unchanged (copy semantics)
        EXPECT_EQ(an_array[0].as<int32_t>(), 1);
    }

} // namespace lumos