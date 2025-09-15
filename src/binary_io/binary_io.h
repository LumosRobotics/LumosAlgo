#ifndef BINARY_IO_H
#define BINARY_IO_H

#include <vector>
#include <string>
#include <cstdint>

namespace binary_io {

/**
 * @brief Reads a file as binary data
 * @param file_path Path to the file to read
 * @return Vector of bytes containing the file data
 * @throws std::runtime_error if file cannot be opened or read
 */
std::vector<uint8_t> ReadBinaryData(const std::string& file_path);

/**
 * @brief Writes binary data to a file
 * @param data Vector of bytes to write
 * @param file_path Path to the output file
 * @throws std::runtime_error if file cannot be opened or written
 */
void WriteBinaryData(const std::vector<uint8_t>& data, const std::string& file_path);

/**
 * @brief Gets the size of a file in bytes
 * @param file_path Path to the file
 * @return Size of the file in bytes
 * @throws std::runtime_error if file cannot be accessed
 */
size_t GetFileSize(const std::string& file_path);

/**
 * @brief Checks if a file exists
 * @param file_path Path to the file
 * @return True if file exists, false otherwise
 */
bool FileExists(const std::string& file_path);

/**
 * @brief Write mode for BinaryFileWriter
 */
enum class WriteMode {
    OverWrite,  ///< Overwrite existing file content
    Append      ///< Append to existing file content
};

/**
 * @brief Flush behavior for BinaryFileWriter
 */
enum class FlushMode {
    Immediate,    ///< Write data immediately on each Write() call
    UponClosing   ///< Buffer data and write only when closing or destructing
};

/**
 * @brief A class for reading binary files with internal data storage
 * 
 * This class reads binary file data and stores it internally using a raw pointer,
 * avoiding unnecessary copying of data. It provides methods to access the data
 * and query information about the loaded file.
 */
class BinaryFileReader {
public:
    /**
     * @brief Default constructor - creates an empty reader
     */
    BinaryFileReader();
    
    /**
     * @brief Constructor that immediately loads a file
     * @param file_path Path to the file to read
     * @throws std::runtime_error if file cannot be opened or read
     */
    explicit BinaryFileReader(const std::string& file_path);
    
    /**
     * @brief Copy constructor
     */
    BinaryFileReader(const BinaryFileReader& other);
    
    /**
     * @brief Move constructor
     */
    BinaryFileReader(BinaryFileReader&& other) noexcept;
    
    /**
     * @brief Copy assignment operator
     */
    BinaryFileReader& operator=(const BinaryFileReader& other);
    
    /**
     * @brief Move assignment operator
     */
    BinaryFileReader& operator=(BinaryFileReader&& other) noexcept;
    
    /**
     * @brief Destructor - frees allocated memory
     */
    ~BinaryFileReader();
    
    /**
     * @brief Loads a file into the internal buffer
     * @param file_path Path to the file to read
     * @throws std::runtime_error if file cannot be opened or read
     */
    void LoadFile(const std::string& file_path);
    
    /**
     * @brief Gets a pointer to the raw data
     * @return Pointer to the binary data, or nullptr if no data loaded
     */
    const uint8_t* GetData() const;
    
    /**
     * @brief Gets the size of the loaded data
     * @return Size of the data in bytes
     */
    size_t GetSize() const;
    
    /**
     * @brief Checks if any data is loaded
     * @return True if data is loaded, false otherwise
     */
    bool IsEmpty() const;
    
    /**
     * @brief Clears the loaded data and frees memory
     */
    void Clear();
    
    /**
     * @brief Gets a byte at a specific index
     * @param index Index of the byte to retrieve
     * @return The byte at the specified index
     * @throws std::out_of_range if index is out of bounds
     */
    uint8_t operator[](size_t index) const;
    
    /**
     * @brief Gets a byte at a specific index with bounds checking
     * @param index Index of the byte to retrieve
     * @return The byte at the specified index
     * @throws std::out_of_range if index is out of bounds
     */
    uint8_t At(size_t index) const;
    
    /**
     * @brief Creates a copy of the data as a vector
     * @return Vector containing a copy of the binary data
     */
    std::vector<uint8_t> ToVector() const;
    
    /**
     * @brief Gets the path of the last loaded file
     * @return File path, or empty string if no file loaded
     */
    const std::string& GetFilePath() const;

private:
    uint8_t* data_;           ///< Pointer to the binary data
    size_t size_;             ///< Size of the data in bytes
    std::string file_path_;   ///< Path of the loaded file
    
    /**
     * @brief Helper function to free allocated memory
     */
    void FreeData();
};

/**
 * @brief A class for writing binary files with flexible write modes
 * 
 * This class provides binary file writing capabilities with options for
 * overwriting vs appending, and immediate vs buffered writing.
 */
class BinaryFileWriter {
public:
    /**
     * @brief Constructor that opens a file for writing
     * @param file_path Path to the file to write (will be created if it doesn't exist)
     * @param write_mode Whether to overwrite or append to existing file
     * @param flush_mode Whether to write immediately or buffer until closing
     * @throws std::runtime_error if file cannot be opened for writing
     */
    BinaryFileWriter(const std::string& file_path, WriteMode write_mode, FlushMode flush_mode);
    
    /**
     * @brief Copy constructor (deleted - not copyable)
     */
    BinaryFileWriter(const BinaryFileWriter& other) = delete;
    
    /**
     * @brief Move constructor
     */
    BinaryFileWriter(BinaryFileWriter&& other) noexcept;
    
    /**
     * @brief Copy assignment operator (deleted - not copyable)
     */
    BinaryFileWriter& operator=(const BinaryFileWriter& other) = delete;
    
    /**
     * @brief Move assignment operator
     */
    BinaryFileWriter& operator=(BinaryFileWriter&& other) noexcept;
    
    /**
     * @brief Destructor - flushes any buffered data and closes the file
     */
    ~BinaryFileWriter();
    
    /**
     * @brief Writes a single byte to the file
     * @param byte The byte to write
     * @throws std::runtime_error if write fails
     */
    void Write(uint8_t byte);
    
    /**
     * @brief Writes a vector of bytes to the file
     * @param data Vector of bytes to write
     * @throws std::runtime_error if write fails
     */
    void Write(const std::vector<uint8_t>& data);
    
    /**
     * @brief Writes raw byte data to the file
     * @param data Pointer to the data to write
     * @param size Number of bytes to write
     * @throws std::runtime_error if write fails
     */
    void Write(const uint8_t* data, size_t size);
    
    /**
     * @brief Writes a string as raw binary data
     * @param s The string to write (without null terminator)
     * @throws std::runtime_error if write fails
     */
    void Write(const std::string& s);
    
    /**
     * @brief Manually flushes any buffered data to the file
     * @throws std::runtime_error if flush fails
     */
    void Flush();
    
    /**
     * @brief Closes the file (flushes buffered data if needed)
     * @throws std::runtime_error if close fails
     */
    void Close();
    
    /**
     * @brief Checks if the file is currently open
     * @return True if file is open, false otherwise
     */
    bool IsOpen() const;
    
    /**
     * @brief Gets the path of the file being written to
     * @return File path
     */
    const std::string& GetFilePath() const;
    
    /**
     * @brief Gets the current write mode
     * @return The write mode (OverWrite or Append)
     */
    WriteMode GetWriteMode() const;
    
    /**
     * @brief Gets the current flush mode
     * @return The flush mode (Immediate or UponClosing)
     */
    FlushMode GetFlushMode() const;
    
    /**
     * @brief Gets the number of bytes written so far
     * @return Number of bytes written
     */
    size_t GetBytesWritten() const;

private:
    std::string file_path_;         ///< Path to the file being written
    WriteMode write_mode_;          ///< Write mode (overwrite or append)
    FlushMode flush_mode_;          ///< Flush mode (immediate or upon closing)
    std::vector<uint8_t> buffer_;   ///< Buffer for UponClosing mode
    size_t bytes_written_;          ///< Total bytes written
    bool is_open_;                  ///< Whether the file is currently open
    
    /**
     * @brief Helper function to actually write data to the file
     * @param data Pointer to data to write
     * @param size Number of bytes to write
     */
    void WriteToFile(const uint8_t* data, size_t size);
    
    /**
     * @brief Helper function to initialize file stream
     */
    void InitializeFile();
};

} // namespace binary_io

#include "binary_io.impl.h"

#endif // BINARY_IO_H