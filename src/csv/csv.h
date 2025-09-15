#ifndef CSV_H
#define CSV_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>
#include <functional>
#include <map>

// CSV Configuration struct
struct CSVConfig {
    char delimiter = ',';
    char quote_char = '"';
    char escape_char = '"';
    bool has_header = false;
    bool trim_whitespace = true;
    bool skip_empty_lines = true;
};

// CSV Row type
using CSVRow = std::vector<std::string>;
using CSVData = std::vector<CSVRow>;

// Basic CSV operations
CSVData readCSV(const std::string& filename, const CSVConfig& config = CSVConfig{});
CSVData readCSVFromString(const std::string& csv_content, const CSVConfig& config = CSVConfig{});
bool writeCSV(const std::string& filename, const CSVData& data, const CSVConfig& config = CSVConfig{});
std::string writeCSVToString(const CSVData& data, const CSVConfig& config = CSVConfig{});

// CSV parsing and formatting
CSVRow parseCSVLine(const std::string& line, const CSVConfig& config = CSVConfig{});
std::string formatCSVLine(const CSVRow& row, const CSVConfig& config = CSVConfig{});

// Advanced CSV operations
CSVData filterRows(const CSVData& data, std::function<bool(const CSVRow&, size_t)> predicate);
CSVData selectColumns(const CSVData& data, const std::vector<size_t>& column_indices);
CSVData selectColumnsByName(const CSVData& data, const std::vector<std::string>& column_names, bool has_header = true);
CSVRow getColumn(const CSVData& data, size_t column_index);
CSVRow getColumnByName(const CSVData& data, const std::string& column_name, bool has_header = true);

// CSV utility functions
size_t getRowCount(const CSVData& data);
size_t getColumnCount(const CSVData& data);
bool validateCSV(const CSVData& data);
CSVData transposeCSV(const CSVData& data);

// CSV field manipulation
std::string escapeCSVField(const std::string& field, const CSVConfig& config = CSVConfig{});
std::string unescapeCSVField(const std::string& field, const CSVConfig& config = CSVConfig{});
bool needsQuoting(const std::string& field, const CSVConfig& config = CSVConfig{});

// CSV statistics and analysis
std::vector<std::string> getUniqueValues(const CSVData& data, size_t column_index);
std::map<std::string, size_t> getValueCounts(const CSVData& data, size_t column_index);
CSVData sortByColumn(const CSVData& data, size_t column_index, bool ascending = true);

// Template function for reading CSV into vector of tuples with type conversion
template<typename... Types>
std::vector<std::tuple<Types...>> readCSVTyped(const std::string& filename, const CSVConfig& config = CSVConfig{});

template<typename... Types>
std::vector<std::tuple<Types...>> readCSVTypedFromString(const std::string& csv_content, const CSVConfig& config = CSVConfig{});

#endif // CSV_H