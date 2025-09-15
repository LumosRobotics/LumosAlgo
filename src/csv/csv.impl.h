#ifndef CSV_IMPL_H
#define CSV_IMPL_H

#include "csv.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <tuple>
#include <type_traits>
#include <stdexcept>
#include <cstdint>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

// =============================================================================
// BASIC CSV OPERATIONS
// =============================================================================

LUMOS_INLINE CSVData readCSV(const std::string& filename, const CSVConfig& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    CSVData data;
    std::string line;
    
    while (std::getline(file, line)) {
        if (config.skip_empty_lines && line.empty()) {
            continue;
        }
        
        CSVRow row = parseCSVLine(line, config);
        if (!row.empty() || !config.skip_empty_lines) {
            data.push_back(row);
        }
    }
    
    return data;
}

LUMOS_INLINE CSVData readCSVFromString(const std::string& csv_content, const CSVConfig& config) {
    std::istringstream stream(csv_content);
    CSVData data;
    std::string line;
    
    while (std::getline(stream, line)) {
        if (config.skip_empty_lines && line.empty()) {
            continue;
        }
        
        CSVRow row = parseCSVLine(line, config);
        if (!row.empty() || !config.skip_empty_lines) {
            data.push_back(row);
        }
    }
    
    return data;
}

LUMOS_INLINE bool writeCSV(const std::string& filename, const CSVData& data, const CSVConfig& config) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& row : data) {
        file << formatCSVLine(row, config) << '\n';
    }
    
    return true;
}

LUMOS_INLINE std::string writeCSVToString(const CSVData& data, const CSVConfig& config) {
    std::ostringstream stream;
    
    for (const auto& row : data) {
        stream << formatCSVLine(row, config) << '\n';
    }
    
    return stream.str();
}

// =============================================================================
// CSV PARSING AND FORMATTING
// =============================================================================

LUMOS_INLINE CSVRow parseCSVLine(const std::string& line, const CSVConfig& config) {
    CSVRow row;
    std::string field;
    bool in_quotes = false;
    bool escape_next = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (escape_next) {
            field += c;
            escape_next = false;
            continue;
        }
        
        if (c == config.escape_char && in_quotes) {
            if (i + 1 < line.length() && line[i + 1] == config.quote_char) {
                field += config.quote_char;
                ++i; // Skip the next quote
            } else {
                escape_next = true;
            }
        } else if (c == config.quote_char) {
            if (in_quotes) {
                // Check for escaped quote
                if (i + 1 < line.length() && line[i + 1] == config.quote_char) {
                    field += config.quote_char;
                    ++i; // Skip the second quote
                } else {
                    in_quotes = false;
                }
            } else {
                in_quotes = true;
            }
        } else if (c == config.delimiter && !in_quotes) {
            if (config.trim_whitespace) {
                // Trim whitespace
                size_t start = field.find_first_not_of(" \t");
                size_t end = field.find_last_not_of(" \t");
                if (start != std::string::npos) {
                    field = field.substr(start, end - start + 1);
                } else {
                    field = "";
                }
            }
            row.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    
    // Add the last field
    if (config.trim_whitespace) {
        size_t start = field.find_first_not_of(" \t");
        size_t end = field.find_last_not_of(" \t");
        if (start != std::string::npos) {
            field = field.substr(start, end - start + 1);
        } else {
            field = "";
        }
    }
    row.push_back(field);
    
    return row;
}

LUMOS_INLINE std::string formatCSVLine(const CSVRow& row, const CSVConfig& config) {
    std::string line;
    
    for (size_t i = 0; i < row.size(); ++i) {
        if (i > 0) {
            line += config.delimiter;
        }
        
        std::string field = row[i];
        if (needsQuoting(field, config)) {
            field = escapeCSVField(field, config);
            line += config.quote_char + field + config.quote_char;
        } else {
            line += field;
        }
    }
    
    return line;
}

// =============================================================================
// ADVANCED CSV OPERATIONS
// =============================================================================

LUMOS_INLINE CSVData filterRows(const CSVData& data, std::function<bool(const CSVRow&, size_t)> predicate) {
    CSVData filtered;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (predicate(data[i], i)) {
            filtered.push_back(data[i]);
        }
    }
    
    return filtered;
}

LUMOS_INLINE CSVData selectColumns(const CSVData& data, const std::vector<size_t>& column_indices) {
    CSVData result;
    
    for (const auto& row : data) {
        CSVRow new_row;
        for (size_t col_idx : column_indices) {
            if (col_idx < row.size()) {
                new_row.push_back(row[col_idx]);
            } else {
                new_row.push_back("");
            }
        }
        result.push_back(new_row);
    }
    
    return result;
}

LUMOS_INLINE CSVData selectColumnsByName(const CSVData& data, const std::vector<std::string>& column_names, bool has_header) {
    if (!has_header || data.empty()) {
        return CSVData{};
    }
    
    const CSVRow& header = data[0];
    std::vector<size_t> indices;
    
    for (const std::string& name : column_names) {
        auto it = std::find(header.begin(), header.end(), name);
        if (it != header.end()) {
            indices.push_back(std::distance(header.begin(), it));
        }
    }
    
    return selectColumns(data, indices);
}

LUMOS_INLINE CSVRow getColumn(const CSVData& data, size_t column_index) {
    CSVRow column;
    
    for (const auto& row : data) {
        if (column_index < row.size()) {
            column.push_back(row[column_index]);
        } else {
            column.push_back("");
        }
    }
    
    return column;
}

LUMOS_INLINE CSVRow getColumnByName(const CSVData& data, const std::string& column_name, bool has_header) {
    if (!has_header || data.empty()) {
        return CSVRow{};
    }
    
    const CSVRow& header = data[0];
    auto it = std::find(header.begin(), header.end(), column_name);
    if (it == header.end()) {
        return CSVRow{};
    }
    
    size_t column_index = std::distance(header.begin(), it);
    return getColumn(data, column_index);
}

// =============================================================================
// CSV UTILITY FUNCTIONS
// =============================================================================

LUMOS_INLINE size_t getRowCount(const CSVData& data) {
    return data.size();
}

LUMOS_INLINE size_t getColumnCount(const CSVData& data) {
    if (data.empty()) {
        return 0;
    }
    
    size_t max_cols = 0;
    for (const auto& row : data) {
        max_cols = std::max(max_cols, row.size());
    }
    
    return max_cols;
}

LUMOS_INLINE bool validateCSV(const CSVData& data) {
    if (data.empty()) {
        return true;
    }
    
    size_t expected_cols = data[0].size();
    for (const auto& row : data) {
        if (row.size() != expected_cols) {
            return false;
        }
    }
    
    return true;
}

LUMOS_INLINE CSVData transposeCSV(const CSVData& data) {
    if (data.empty()) {
        return CSVData{};
    }
    
    size_t max_cols = getColumnCount(data);
    CSVData transposed(max_cols);
    
    for (size_t col = 0; col < max_cols; ++col) {
        for (size_t row = 0; row < data.size(); ++row) {
            if (col < data[row].size()) {
                transposed[col].push_back(data[row][col]);
            } else {
                transposed[col].push_back("");
            }
        }
    }
    
    return transposed;
}

// =============================================================================
// CSV FIELD MANIPULATION
// =============================================================================

LUMOS_INLINE std::string escapeCSVField(const std::string& field, const CSVConfig& config) {
    std::string escaped;
    
    for (char c : field) {
        if (c == config.quote_char) {
            escaped += config.escape_char;
            escaped += config.quote_char;
        } else {
            escaped += c;
        }
    }
    
    return escaped;
}

LUMOS_INLINE std::string unescapeCSVField(const std::string& field, const CSVConfig& config) {
    std::string unescaped;
    bool escape_next = false;
    
    for (char c : field) {
        if (escape_next) {
            unescaped += c;
            escape_next = false;
        } else if (c == config.escape_char) {
            escape_next = true;
        } else {
            unescaped += c;
        }
    }
    
    return unescaped;
}

LUMOS_INLINE bool needsQuoting(const std::string& field, const CSVConfig& config) {
    return field.find(config.delimiter) != std::string::npos ||
           field.find(config.quote_char) != std::string::npos ||
           field.find('\n') != std::string::npos ||
           field.find('\r') != std::string::npos ||
           (!field.empty() && (field.front() == ' ' || field.back() == ' '));
}

// =============================================================================
// CSV STATISTICS AND ANALYSIS
// =============================================================================

LUMOS_INLINE std::vector<std::string> getUniqueValues(const CSVData& data, size_t column_index) {
    std::set<std::string> unique_set;
    
    for (const auto& row : data) {
        if (column_index < row.size()) {
            unique_set.insert(row[column_index]);
        }
    }
    
    return std::vector<std::string>(unique_set.begin(), unique_set.end());
}

LUMOS_INLINE std::map<std::string, size_t> getValueCounts(const CSVData& data, size_t column_index) {
    std::map<std::string, size_t> counts;
    
    for (const auto& row : data) {
        if (column_index < row.size()) {
            counts[row[column_index]]++;
        }
    }
    
    return counts;
}

LUMOS_INLINE CSVData sortByColumn(const CSVData& data, size_t column_index, bool ascending) {
    CSVData sorted_data = data;
    
    std::sort(sorted_data.begin(), sorted_data.end(), 
        [column_index, ascending](const CSVRow& a, const CSVRow& b) {
            std::string val_a = (column_index < a.size()) ? a[column_index] : "";
            std::string val_b = (column_index < b.size()) ? b[column_index] : "";
            
            if (ascending) {
                return val_a < val_b;
            } else {
                return val_a > val_b;
            }
        });
    
    return sorted_data;
}

// =============================================================================
// TEMPLATED CSV READING WITH TYPE CONVERSION
// =============================================================================

namespace detail {
    // Helper function to convert string to specific type
    template<typename T>
    LUMOS_INLINE T convertFromString(const std::string& str) {
        // Check if T is one of the supported types
        static_assert(
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
            "Unsupported type for CSV conversion. Only std::string, float, double, and integer types (int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t) are supported."
        );
        
        if constexpr (std::is_same_v<T, std::string>) {
            return str;
        } else if constexpr (std::is_same_v<T, float>) {
            return std::stof(str);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(str);
        } else if constexpr (std::is_same_v<T, int8_t>) {
            int temp = std::stoi(str);
            if (temp < INT8_MIN || temp > INT8_MAX) {
                throw std::out_of_range("Value out of range for int8_t");
            }
            return static_cast<int8_t>(temp);
        } else if constexpr (std::is_same_v<T, int16_t>) {
            int temp = std::stoi(str);
            if (temp < INT16_MIN || temp > INT16_MAX) {
                throw std::out_of_range("Value out of range for int16_t");
            }
            return static_cast<int16_t>(temp);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return static_cast<int32_t>(std::stoi(str));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<int64_t>(std::stoll(str));
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            unsigned long temp = std::stoul(str);
            if (temp > UINT8_MAX) {
                throw std::out_of_range("Value out of range for uint8_t");
            }
            return static_cast<uint8_t>(temp);
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            unsigned long temp = std::stoul(str);
            if (temp > UINT16_MAX) {
                throw std::out_of_range("Value out of range for uint16_t");
            }
            return static_cast<uint16_t>(temp);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return static_cast<uint32_t>(std::stoul(str));
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return static_cast<uint64_t>(std::stoull(str));
        }
    }
    
    // Helper function to convert a CSV row to a tuple
    template<typename Tuple, std::size_t... Is>
    LUMOS_INLINE Tuple convertRowToTuple(const CSVRow& row, std::index_sequence<Is...>) {
        if (row.size() < sizeof...(Is)) {
            throw std::invalid_argument("CSV row has fewer columns than expected tuple size");
        }
        
        try {
            return std::make_tuple(convertFromString<std::tuple_element_t<Is, Tuple>>(row[Is])...);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to convert CSV row to tuple: " + std::string(e.what()));
        }
    }
    
    // Helper function to convert CSV data to vector of tuples
    template<typename... Types>
    LUMOS_INLINE std::vector<std::tuple<Types...>> convertCSVDataToTuples(const CSVData& data, const CSVConfig& config) {
        std::vector<std::tuple<Types...>> result;
        
        size_t start_row = config.has_header ? 1 : 0;
        if (config.has_header && data.empty()) {
            return result; // Empty data with header flag
        }
        
        result.reserve(data.size() - start_row);
        
        for (size_t i = start_row; i < data.size(); ++i) {
            const CSVRow& row = data[i];
            
            // Skip empty rows if configured
            if (config.skip_empty_lines && row.empty()) {
                continue;
            }
            
            try {
                auto tuple = detail::convertRowToTuple<std::tuple<Types...>>(
                    row, std::index_sequence_for<Types...>{}
                );
                result.push_back(tuple);
            } catch (const std::exception& e) {
                throw std::runtime_error("Error processing row " + std::to_string(i + 1) + ": " + e.what());
            }
        }
        
        return result;
    }
} // namespace detail

template<typename... Types>
LUMOS_INLINE std::vector<std::tuple<Types...>> readCSVTyped(const std::string& filename, const CSVConfig& config) {
    static_assert(sizeof...(Types) > 0, "At least one type must be specified for readCSVTyped");
    
    // Read the CSV data using existing function
    CSVData data = readCSV(filename, config);
    
    // Convert to typed tuples
    return detail::convertCSVDataToTuples<Types...>(data, config);
}

template<typename... Types>
LUMOS_INLINE std::vector<std::tuple<Types...>> readCSVTypedFromString(const std::string& csv_content, const CSVConfig& config) {
    static_assert(sizeof...(Types) > 0, "At least one type must be specified for readCSVTypedFromString");
    
    // Read the CSV data using existing function
    CSVData data = readCSVFromString(csv_content, config);
    
    // Convert to typed tuples
    return detail::convertCSVDataToTuples<Types...>(data, config);
}

#endif // CSV_IMPL_H