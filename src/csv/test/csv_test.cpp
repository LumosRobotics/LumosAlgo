#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "csv/csv.impl.h"

namespace lumos {

// Test fixture for CSV functions tests
class CSVTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test CSV data
        simple_csv = "name,age,city\nJohn,25,NYC\nJane,30,LA\nBob,35,Chicago";
        
        quoted_csv = "name,description,value\n"
                    "\"John Doe\",\"A person with, comma\",100\n"
                    "\"Jane Smith\",\"Another \"\"quoted\"\" person\",200\n"
                    "Simple,No quotes needed,300";
        
        empty_lines_csv = "name,age\n\nJohn,25\n\nJane,30\n\n";
        
        irregular_csv = "a,b,c\n1,2\n3,4,5,6\n7";
        
        // Test data structures
        simple_data = {
            {"name", "age", "city"},
            {"John", "25", "NYC"},
            {"Jane", "30", "LA"},
            {"Bob", "35", "Chicago"}
        };
        
        // Test configuration
        custom_config.delimiter = ';';
        custom_config.quote_char = '\'';
        custom_config.has_header = true;
        custom_config.trim_whitespace = true;
        
        // Test file path
        test_filename = "/tmp/lumos_test.csv";
    }
    
    void TearDown() override {
        // Clean up test file
        std::remove(test_filename.c_str());
    }
    
    std::string simple_csv, quoted_csv, empty_lines_csv, irregular_csv;
    CSVData simple_data;
    CSVConfig custom_config;
    std::string test_filename;
};

// =============================================================================
// BASIC CSV OPERATIONS TESTS
// =============================================================================

TEST_F(CSVTest, ReadCSVFromString) {
    CSVData data = readCSVFromString(simple_csv);
    
    ASSERT_EQ(data.size(), 4U);
    ASSERT_EQ(data[0].size(), 3U);
    
    EXPECT_EQ(data[0][0], "name");
    EXPECT_EQ(data[0][1], "age");
    EXPECT_EQ(data[0][2], "city");
    
    EXPECT_EQ(data[1][0], "John");
    EXPECT_EQ(data[1][1], "25");
    EXPECT_EQ(data[1][2], "NYC");
}

TEST_F(CSVTest, WriteCSVToString) {
    std::string result = writeCSVToString(simple_data);
    CSVData parsed_back = readCSVFromString(result);
    
    ASSERT_EQ(parsed_back.size(), simple_data.size());
    for (size_t i = 0; i < simple_data.size(); ++i) {
        ASSERT_EQ(parsed_back[i].size(), simple_data[i].size());
        for (size_t j = 0; j < simple_data[i].size(); ++j) {
            EXPECT_EQ(parsed_back[i][j], simple_data[i][j]);
        }
    }
}

TEST_F(CSVTest, ReadWriteCSVFile) {
    // Write to file
    EXPECT_TRUE(writeCSV(test_filename, simple_data));
    
    // Read back from file
    CSVData read_data = readCSV(test_filename);
    
    ASSERT_EQ(read_data.size(), simple_data.size());
    for (size_t i = 0; i < simple_data.size(); ++i) {
        ASSERT_EQ(read_data[i].size(), simple_data[i].size());
        for (size_t j = 0; j < simple_data[i].size(); ++j) {
            EXPECT_EQ(read_data[i][j], simple_data[i][j]);
        }
    }
}

TEST_F(CSVTest, ReadNonExistentFile) {
    EXPECT_THROW(readCSV("/non/existent/file.csv"), std::runtime_error);
}

// =============================================================================
// CSV PARSING AND FORMATTING TESTS
// =============================================================================

TEST_F(CSVTest, ParseCSVLine) {
    CSVRow row = parseCSVLine("John,25,NYC");
    
    ASSERT_EQ(row.size(), 3U);
    EXPECT_EQ(row[0], "John");
    EXPECT_EQ(row[1], "25");
    EXPECT_EQ(row[2], "NYC");
}

TEST_F(CSVTest, ParseCSVLineWithQuotes) {
    CSVRow row = parseCSVLine("\"John Doe\",\"A person with, comma\",100");
    
    ASSERT_EQ(row.size(), 3U);
    EXPECT_EQ(row[0], "John Doe");
    EXPECT_EQ(row[1], "A person with, comma");
    EXPECT_EQ(row[2], "100");
}

TEST_F(CSVTest, ParseCSVLineWithEscapedQuotes) {
    CSVRow row = parseCSVLine("\"Jane Smith\",\"Another \"\"quoted\"\" person\",200");
    
    ASSERT_EQ(row.size(), 3U);
    EXPECT_EQ(row[0], "Jane Smith");
    EXPECT_EQ(row[1], "Another \"quoted\" person");
    EXPECT_EQ(row[2], "200");
}

TEST_F(CSVTest, FormatCSVLine) {
    CSVRow row = {"John Doe", "A person with, comma", "100"};
    std::string formatted = formatCSVLine(row);
    
    // Should automatically quote fields that need it
    EXPECT_EQ(formatted, "\"John Doe\",\"A person with, comma\",100");
}

TEST_F(CSVTest, ParseFormatRoundTrip) {
    std::string original = "\"John Doe\",\"A person with, comma\",100";
    CSVRow parsed = parseCSVLine(original);
    std::string formatted = formatCSVLine(parsed);
    CSVRow parsed_again = parseCSVLine(formatted);
    
    ASSERT_EQ(parsed.size(), parsed_again.size());
    for (size_t i = 0; i < parsed.size(); ++i) {
        EXPECT_EQ(parsed[i], parsed_again[i]);
    }
}

// =============================================================================
// CUSTOM CONFIGURATION TESTS
// =============================================================================

TEST_F(CSVTest, CustomDelimiter) {
    std::string semicolon_csv = "name;age;city\nJohn;25;NYC";
    CSVConfig config;
    config.delimiter = ';';
    
    CSVData data = readCSVFromString(semicolon_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    EXPECT_EQ(data[0][0], "name");
    EXPECT_EQ(data[0][1], "age");
    EXPECT_EQ(data[0][2], "city");
}

TEST_F(CSVTest, CustomQuoteChar) {
    std::string single_quote_csv = "name,description\n'John Doe','A person with, comma'";
    CSVConfig config;
    config.quote_char = '\'';
    
    CSVData data = readCSVFromString(single_quote_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    EXPECT_EQ(data[1][0], "John Doe");
    EXPECT_EQ(data[1][1], "A person with, comma");
}

TEST_F(CSVTest, TrimWhitespace) {
    std::string whitespace_csv = "name , age , city\n John , 25 , NYC ";
    
    CSVConfig trim_config;
    trim_config.trim_whitespace = true;
    CSVData trimmed = readCSVFromString(whitespace_csv, trim_config);
    
    CSVConfig no_trim_config;
    no_trim_config.trim_whitespace = false;
    CSVData not_trimmed = readCSVFromString(whitespace_csv, no_trim_config);
    
    EXPECT_EQ(trimmed[1][0], "John");
    EXPECT_EQ(not_trimmed[1][0], " John ");
}

TEST_F(CSVTest, SkipEmptyLines) {
    CSVConfig skip_config;
    skip_config.skip_empty_lines = true;
    CSVData skipped = readCSVFromString(empty_lines_csv, skip_config);
    
    CSVConfig no_skip_config;
    no_skip_config.skip_empty_lines = false;
    CSVData not_skipped = readCSVFromString(empty_lines_csv, no_skip_config);
    
    EXPECT_EQ(skipped.size(), 3U);  // header + 2 data rows
    EXPECT_GT(not_skipped.size(), 3U);  // includes empty lines
}

// =============================================================================
// ADVANCED CSV OPERATIONS TESTS
// =============================================================================

TEST_F(CSVTest, FilterRows) {
    CSVData filtered = filterRows(simple_data, [](const CSVRow& row, size_t index) {
        return index == 0 || (row.size() > 1 && std::stoi(row[1]) >= 30);
    });
    
    ASSERT_EQ(filtered.size(), 3U);  // header + Jane + Bob
    EXPECT_EQ(filtered[0][0], "name");  // header
    EXPECT_EQ(filtered[1][0], "Jane");
    EXPECT_EQ(filtered[2][0], "Bob");
}

TEST_F(CSVTest, SelectColumns) {
    std::vector<size_t> indices = {0, 2};  // name and city columns
    CSVData selected = selectColumns(simple_data, indices);
    
    ASSERT_EQ(selected.size(), 4U);
    ASSERT_EQ(selected[0].size(), 2U);
    
    EXPECT_EQ(selected[0][0], "name");
    EXPECT_EQ(selected[0][1], "city");
    EXPECT_EQ(selected[1][0], "John");
    EXPECT_EQ(selected[1][1], "NYC");
}

TEST_F(CSVTest, SelectColumnsByName) {
    std::vector<std::string> column_names = {"name", "city"};
    CSVData selected = selectColumnsByName(simple_data, column_names, true);
    
    ASSERT_EQ(selected.size(), 4U);
    ASSERT_EQ(selected[0].size(), 2U);
    
    EXPECT_EQ(selected[0][0], "name");
    EXPECT_EQ(selected[0][1], "city");
    EXPECT_EQ(selected[1][0], "John");
    EXPECT_EQ(selected[1][1], "NYC");
}

TEST_F(CSVTest, GetColumn) {
    CSVRow age_column = getColumn(simple_data, 1);
    
    ASSERT_EQ(age_column.size(), 4U);
    EXPECT_EQ(age_column[0], "age");
    EXPECT_EQ(age_column[1], "25");
    EXPECT_EQ(age_column[2], "30");
    EXPECT_EQ(age_column[3], "35");
}

TEST_F(CSVTest, GetColumnByName) {
    CSVRow age_column = getColumnByName(simple_data, "age", true);
    
    ASSERT_EQ(age_column.size(), 4U);
    EXPECT_EQ(age_column[0], "age");
    EXPECT_EQ(age_column[1], "25");
    EXPECT_EQ(age_column[2], "30");
    EXPECT_EQ(age_column[3], "35");
}

// =============================================================================
// CSV UTILITY FUNCTIONS TESTS
// =============================================================================

TEST_F(CSVTest, GetRowColumnCount) {
    EXPECT_EQ(getRowCount(simple_data), 4U);
    EXPECT_EQ(getColumnCount(simple_data), 3U);
    
    CSVData empty_data;
    EXPECT_EQ(getRowCount(empty_data), 0U);
    EXPECT_EQ(getColumnCount(empty_data), 0U);
}

TEST_F(CSVTest, ValidateCSV) {
    EXPECT_TRUE(validateCSV(simple_data));
    
    CSVData irregular_data = readCSVFromString(irregular_csv);
    EXPECT_FALSE(validateCSV(irregular_data));
}

TEST_F(CSVTest, TransposeCSV) {
    CSVData transposed = transposeCSV(simple_data);
    
    ASSERT_EQ(transposed.size(), 3U);  // 3 columns become 3 rows
    ASSERT_EQ(transposed[0].size(), 4U);  // 4 rows become 4 columns
    
    EXPECT_EQ(transposed[0][0], "name");
    EXPECT_EQ(transposed[0][1], "John");
    EXPECT_EQ(transposed[0][2], "Jane");
    EXPECT_EQ(transposed[0][3], "Bob");
    
    EXPECT_EQ(transposed[1][0], "age");
    EXPECT_EQ(transposed[1][1], "25");
    EXPECT_EQ(transposed[1][2], "30");
    EXPECT_EQ(transposed[1][3], "35");
}

// =============================================================================
// CSV FIELD MANIPULATION TESTS
// =============================================================================

TEST_F(CSVTest, EscapeUnescapeCSVField) {
    std::string field_with_quotes = "A \"quoted\" string";
    std::string escaped = escapeCSVField(field_with_quotes);
    std::string unescaped = unescapeCSVField(escaped);
    
    EXPECT_EQ(unescaped, field_with_quotes);
}

TEST_F(CSVTest, NeedsQuoting) {
    EXPECT_TRUE(needsQuoting("field,with,comma"));
    EXPECT_TRUE(needsQuoting("field\"with\"quote"));
    EXPECT_TRUE(needsQuoting("field\nwith\nnewline"));
    EXPECT_TRUE(needsQuoting(" field with spaces "));
    
    EXPECT_FALSE(needsQuoting("simple_field"));
    EXPECT_FALSE(needsQuoting("123"));
}

// =============================================================================
// CSV STATISTICS AND ANALYSIS TESTS
// =============================================================================

TEST_F(CSVTest, GetUniqueValues) {
    CSVData data = {
        {"color", "count"},
        {"red", "1"},
        {"blue", "2"},
        {"red", "3"},
        {"green", "1"},
        {"blue", "4"}
    };
    
    std::vector<std::string> unique_colors = getUniqueValues(data, 0);
    std::sort(unique_colors.begin(), unique_colors.end());
    
    ASSERT_EQ(unique_colors.size(), 4U);
    EXPECT_EQ(unique_colors[0], "blue");
    EXPECT_EQ(unique_colors[1], "color");
    EXPECT_EQ(unique_colors[2], "green");
    EXPECT_EQ(unique_colors[3], "red");
}

TEST_F(CSVTest, GetValueCounts) {
    CSVData data = {
        {"color", "count"},
        {"red", "1"},
        {"blue", "2"},
        {"red", "3"},
        {"green", "1"},
        {"blue", "4"}
    };
    
    std::map<std::string, size_t> counts = getValueCounts(data, 0);
    
    EXPECT_EQ(counts["red"], 2U);
    EXPECT_EQ(counts["blue"], 2U);
    EXPECT_EQ(counts["green"], 1U);
    EXPECT_EQ(counts["color"], 1U);
}

TEST_F(CSVTest, SortByColumn) {
    CSVData sorted_asc = sortByColumn(simple_data, 1, true);  // Sort by age ascending
    CSVData sorted_desc = sortByColumn(simple_data, 1, false);  // Sort by age descending
    
    // Skip header row for comparison
    EXPECT_EQ(sorted_asc[1][1], "25");  // John (youngest)
    EXPECT_EQ(sorted_asc[3][1], "35");  // Bob (oldest)
    
    EXPECT_EQ(sorted_desc[1][1], "35");  // Bob (oldest)
    EXPECT_EQ(sorted_desc[3][1], "25");  // John (youngest)
}

// =============================================================================
// EDGE CASES AND ERROR HANDLING TESTS
// =============================================================================

TEST_F(CSVTest, EmptyCSV) {
    CSVData empty_data = readCSVFromString("");
    EXPECT_TRUE(empty_data.empty());
}

TEST_F(CSVTest, SingleField) {
    CSVData single_field = readCSVFromString("single");
    ASSERT_EQ(single_field.size(), 1U);
    ASSERT_EQ(single_field[0].size(), 1U);
    EXPECT_EQ(single_field[0][0], "single");
}

TEST_F(CSVTest, OnlyDelimiters) {
    CSVData only_delims = readCSVFromString(",,,");
    ASSERT_EQ(only_delims.size(), 1U);
    ASSERT_EQ(only_delims[0].size(), 4U);
    for (const auto& field : only_delims[0]) {
        EXPECT_EQ(field, "");
    }
}

TEST_F(CSVTest, IrregularRows) {
    CSVData irregular = readCSVFromString("a,b,c\n1,2\n3,4,5,6");
    
    ASSERT_EQ(irregular.size(), 3U);
    EXPECT_EQ(irregular[0].size(), 3U);  // a,b,c
    EXPECT_EQ(irregular[1].size(), 2U);  // 1,2
    EXPECT_EQ(irregular[2].size(), 4U);  // 3,4,5,6
    
    EXPECT_FALSE(validateCSV(irregular));
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

TEST_F(CSVTest, ComplexWorkflow) {
    // Start with some data
    CSVData data = {
        {"name", "age", "department", "salary"},
        {"John", "25", "Engineering", "50000"},
        {"Jane", "30", "Marketing", "55000"},
        {"Bob", "35", "Engineering", "60000"},
        {"Alice", "28", "HR", "45000"},
        {"Charlie", "32", "Engineering", "65000"}
    };
    
    // Filter employees from Engineering department
    CSVData engineers = filterRows(data, [](const CSVRow& row, size_t index) {
        return index == 0 || (row.size() > 2 && row[2] == "Engineering");
    });
    
    // Select only name and salary columns
    std::vector<std::string> columns = {"name", "salary"};
    CSVData name_salary = selectColumnsByName(engineers, columns, true);
    
    // Sort by salary (descending)
    CSVData sorted = sortByColumn(name_salary, 1, false);
    
    // Verify results
    ASSERT_EQ(sorted.size(), 4U);  // header + 3 engineers
    EXPECT_EQ(sorted[1][0], "Charlie");  // Highest salary
    EXPECT_EQ(sorted[3][0], "John");     // Lowest salary
    
    // Convert back to string and verify it parses correctly
    std::string csv_string = writeCSVToString(sorted);
    CSVData parsed_back = readCSVFromString(csv_string);
    
    EXPECT_EQ(parsed_back.size(), sorted.size());
    EXPECT_EQ(parsed_back[1][0], "Charlie");
}

// =============================================================================
// TEMPLATED CSV READING TESTS
// =============================================================================

TEST_F(CSVTest, ReadCSVTypedBasic) {
    std::string typed_csv = "name,age,salary\nJohn,25,50000.5\nJane,30,60000.75";
    
    // This should throw because "age" and "salary" strings can't be converted to numbers
    EXPECT_THROW(
        readCSVTypedFromString<std::string, int32_t, double>(typed_csv),
        std::runtime_error
    );
}

TEST_F(CSVTest, ReadCSVTypedWithHeader) {
    std::string typed_csv = "name,age,salary\nJohn,25,50000.5\nJane,30,60000.75";
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTypedFromString<std::string, int32_t, double>(typed_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    
    // Check first data row (John)
    EXPECT_EQ(std::get<0>(data[0]), "John");
    EXPECT_EQ(std::get<1>(data[0]), 25);
    EXPECT_DOUBLE_EQ(std::get<2>(data[0]), 50000.5);
    
    // Check second data row (Jane)
    EXPECT_EQ(std::get<0>(data[1]), "Jane");
    EXPECT_EQ(std::get<1>(data[1]), 30);
    EXPECT_DOUBLE_EQ(std::get<2>(data[1]), 60000.75);
}

TEST_F(CSVTest, ReadCSVTypedIntegerTypes) {
    std::string int_csv = "i8,i16,i32,i64,u8,u16,u32,u64\n"
                         "127,32767,2147483647,9223372036854775807,255,65535,4294967295,18446744073709551615\n"
                         "-128,-32768,-2147483648,-9223372036854775808,0,0,0,0";
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTypedFromString<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>(int_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    
    // Check first data row (max values)
    EXPECT_EQ(std::get<0>(data[0]), INT8_MAX);
    EXPECT_EQ(std::get<1>(data[0]), INT16_MAX);
    EXPECT_EQ(std::get<2>(data[0]), INT32_MAX);
    EXPECT_EQ(std::get<3>(data[0]), INT64_MAX);
    EXPECT_EQ(std::get<4>(data[0]), UINT8_MAX);
    EXPECT_EQ(std::get<5>(data[0]), UINT16_MAX);
    EXPECT_EQ(std::get<6>(data[0]), UINT32_MAX);
    EXPECT_EQ(std::get<7>(data[0]), UINT64_MAX);
    
    // Check second data row (min values for signed, 0 for unsigned)
    EXPECT_EQ(std::get<0>(data[1]), INT8_MIN);
    EXPECT_EQ(std::get<1>(data[1]), INT16_MIN);
    EXPECT_EQ(std::get<2>(data[1]), INT32_MIN);
    EXPECT_EQ(std::get<3>(data[1]), INT64_MIN);
    EXPECT_EQ(std::get<4>(data[1]), 0);
    EXPECT_EQ(std::get<5>(data[1]), 0);
    EXPECT_EQ(std::get<6>(data[1]), 0);
    EXPECT_EQ(std::get<7>(data[1]), 0);
}

TEST_F(CSVTest, ReadCSVTypedFloatingPoint) {
    std::string float_csv = "float_val,double_val\n3.14159,2.718281828459045\n-1.23e-4,1.23456789e10";
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTypedFromString<float, double>(float_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    
    // Check first data row
    EXPECT_FLOAT_EQ(std::get<0>(data[0]), 3.14159f);
    EXPECT_DOUBLE_EQ(std::get<1>(data[0]), 2.718281828459045);
    
    // Check second data row
    EXPECT_FLOAT_EQ(std::get<0>(data[1]), -1.23e-4f);
    EXPECT_DOUBLE_EQ(std::get<1>(data[1]), 1.23456789e10);
}

TEST_F(CSVTest, ReadCSVTypedMixedTypes) {
    std::string mixed_csv = "name,age,height,weight,active\n"
                           "John,25,5.9,150.5,1\n"
                           "Jane,30,5.4,120.0,0";
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTypedFromString<std::string, int32_t, float, double, uint8_t>(mixed_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    
    // Check first data row
    EXPECT_EQ(std::get<0>(data[0]), "John");
    EXPECT_EQ(std::get<1>(data[0]), 25);
    EXPECT_FLOAT_EQ(std::get<2>(data[0]), 5.9f);
    EXPECT_DOUBLE_EQ(std::get<3>(data[0]), 150.5);
    EXPECT_EQ(std::get<4>(data[0]), 1);
    
    // Check second data row
    EXPECT_EQ(std::get<0>(data[1]), "Jane");
    EXPECT_EQ(std::get<1>(data[1]), 30);
    EXPECT_FLOAT_EQ(std::get<2>(data[1]), 5.4f);
    EXPECT_DOUBLE_EQ(std::get<3>(data[1]), 120.0);
    EXPECT_EQ(std::get<4>(data[1]), 0);
}

TEST_F(CSVTest, ReadCSVTypedFromFile) {
    // Create test file
    std::string file_content = "id,name,score\n1,Alice,95.5\n2,Bob,87.2";
    std::ofstream file(test_filename);
    file << file_content;
    file.close();
    
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTyped<int32_t, std::string, double>(test_filename, config);
    
    ASSERT_EQ(data.size(), 2U);
    
    EXPECT_EQ(std::get<0>(data[0]), 1);
    EXPECT_EQ(std::get<1>(data[0]), "Alice");
    EXPECT_DOUBLE_EQ(std::get<2>(data[0]), 95.5);
    
    EXPECT_EQ(std::get<0>(data[1]), 2);
    EXPECT_EQ(std::get<1>(data[1]), "Bob");
    EXPECT_DOUBLE_EQ(std::get<2>(data[1]), 87.2);
}

TEST_F(CSVTest, ReadCSVTypedErrorHandling) {
    // Test with insufficient columns
    std::string insufficient_csv = "name,age\nJohn";
    EXPECT_THROW(
        readCSVTypedFromString<std::string, int32_t, double>(insufficient_csv),
        std::invalid_argument
    );
    
    // Test with invalid number conversion
    std::string invalid_csv = "name,age\nJohn,not_a_number";
    EXPECT_THROW(
        readCSVTypedFromString<std::string, int32_t>(invalid_csv),
        std::runtime_error
    );
    
    // Test with out of range values
    std::string out_of_range_csv = "value\n256";  // Too large for uint8_t
    EXPECT_THROW(
        readCSVTypedFromString<uint8_t>(out_of_range_csv),
        std::runtime_error
    );
}

TEST_F(CSVTest, ReadCSVTypedEmptyData) {
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTypedFromString<std::string, int32_t>("", config);
    EXPECT_TRUE(data.empty());
}

TEST_F(CSVTest, ReadCSVTypedSingleColumn) {
    std::string single_csv = "numbers\n42\n13\n99";
    CSVConfig config;
    config.has_header = true;
    
    auto data = readCSVTypedFromString<int32_t>(single_csv, config);
    
    ASSERT_EQ(data.size(), 3U);
    EXPECT_EQ(std::get<0>(data[0]), 42);
    EXPECT_EQ(std::get<0>(data[1]), 13);
    EXPECT_EQ(std::get<0>(data[2]), 99);
}

TEST_F(CSVTest, ReadCSVTypedWithCustomConfig) {
    std::string custom_csv = "name;age;salary\nJohn;25;50000.5\nJane;30;60000.75";
    CSVConfig config;
    config.delimiter = ';';
    config.has_header = true;
    
    auto data = readCSVTypedFromString<std::string, int32_t, double>(custom_csv, config);
    
    ASSERT_EQ(data.size(), 2U);
    
    EXPECT_EQ(std::get<0>(data[0]), "John");
    EXPECT_EQ(std::get<1>(data[0]), 25);
    EXPECT_DOUBLE_EQ(std::get<2>(data[0]), 50000.5);
}

} // namespace lumos