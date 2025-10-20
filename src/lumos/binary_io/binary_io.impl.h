#ifndef BINARY_IO_IMPL_H
#define BINARY_IO_IMPL_H

#include "lumos/binary_io/binary_io.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <ios>

#ifdef LUMOS_BUILD_FILE
#define LUMOS_INLINE
#else
#define LUMOS_INLINE inline
#endif

namespace binary_io
{

    LUMOS_INLINE std::vector<uint8_t> ReadBinaryData(const std::string &file_path)
    {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);

        if (!file.is_open())
        {
            std::ostringstream oss;
            oss << "Failed to open file for reading: " << file_path;
            throw std::runtime_error(oss.str());
        }

        // Get file size
        std::streamsize file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file_size < 0)
        {
            std::ostringstream oss;
            oss << "Failed to determine file size: " << file_path;
            throw std::runtime_error(oss.str());
        }

        // Create vector with appropriate size
        std::vector<uint8_t> buffer(static_cast<size_t>(file_size));

        // Read the entire file
        if (file_size > 0)
        {
            file.read(reinterpret_cast<char *>(buffer.data()), file_size);

            if (!file)
            {
                std::ostringstream oss;
                oss << "Failed to read file data: " << file_path;
                throw std::runtime_error(oss.str());
            }
        }

        return buffer;
    }

    LUMOS_INLINE void WriteBinaryData(const std::vector<uint8_t> &data, const std::string &file_path)
    {
        std::ofstream file(file_path, std::ios::binary);

        if (!file.is_open())
        {
            std::ostringstream oss;
            oss << "Failed to open file for writing: " << file_path;
            throw std::runtime_error(oss.str());
        }

        if (!data.empty())
        {
            file.write(reinterpret_cast<const char *>(data.data()), data.size());

            if (!file)
            {
                std::ostringstream oss;
                oss << "Failed to write file data: " << file_path;
                throw std::runtime_error(oss.str());
            }
        }
    }

    LUMOS_INLINE size_t GetFileSize(const std::string &file_path)
    {
        std::error_code ec;
        auto file_size = std::filesystem::file_size(file_path, ec);

        if (ec)
        {
            std::ostringstream oss;
            oss << "Failed to get file size: " << file_path << " (error: " << ec.message() << ")";
            throw std::runtime_error(oss.str());
        }

        return static_cast<size_t>(file_size);
    }

    LUMOS_INLINE bool FileExists(const std::string &file_path)
    {
        std::error_code ec;
        return std::filesystem::exists(file_path, ec);
    }

    // =============================================================================
    // BinaryFileReader Class Implementation
    // =============================================================================

    LUMOS_INLINE BinaryFileReader::BinaryFileReader()
        : data_(nullptr), size_(0), file_path_()
    {
    }

    LUMOS_INLINE BinaryFileReader::BinaryFileReader(const std::string &file_path)
        : data_(nullptr), size_(0), file_path_()
    {
        LoadFile(file_path);
    }

    LUMOS_INLINE BinaryFileReader::BinaryFileReader(const BinaryFileReader &other)
        : data_(nullptr), size_(0), file_path_(other.file_path_)
    {
        if (other.data_ && other.size_ > 0)
        {
            data_ = new uint8_t[other.size_];
            size_ = other.size_;
            std::memcpy(data_, other.data_, size_);
        }
    }

    LUMOS_INLINE BinaryFileReader::BinaryFileReader(BinaryFileReader &&other) noexcept
        : data_(other.data_), size_(other.size_), file_path_(std::move(other.file_path_))
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.file_path_.clear();
    }

    LUMOS_INLINE BinaryFileReader &BinaryFileReader::operator=(const BinaryFileReader &other)
    {
        if (this != &other)
        {
            FreeData();

            file_path_ = other.file_path_;
            if (other.data_ && other.size_ > 0)
            {
                data_ = new uint8_t[other.size_];
                size_ = other.size_;
                std::memcpy(data_, other.data_, size_);
            }
        }
        return *this;
    }

    LUMOS_INLINE BinaryFileReader &BinaryFileReader::operator=(BinaryFileReader &&other) noexcept
    {
        if (this != &other)
        {
            FreeData();

            data_ = other.data_;
            size_ = other.size_;
            file_path_ = std::move(other.file_path_);

            other.data_ = nullptr;
            other.size_ = 0;
            other.file_path_.clear();
        }
        return *this;
    }

    LUMOS_INLINE BinaryFileReader::~BinaryFileReader()
    {
        FreeData();
    }

    LUMOS_INLINE void BinaryFileReader::LoadFile(const std::string &file_path)
    {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);

        if (!file.is_open())
        {
            std::ostringstream oss;
            oss << "BinaryFileReader: Failed to open file for reading: " << file_path;
            throw std::runtime_error(oss.str());
        }

        // Get file size
        std::streamsize file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file_size < 0)
        {
            std::ostringstream oss;
            oss << "BinaryFileReader: Failed to determine file size: " << file_path;
            throw std::runtime_error(oss.str());
        }

        // Free existing data
        FreeData();

        // Allocate new memory
        size_ = static_cast<size_t>(file_size);
        file_path_ = file_path;

        if (size_ > 0)
        {
            data_ = new uint8_t[size_];

            // Read the entire file
            file.read(reinterpret_cast<char *>(data_), file_size);

            if (!file)
            {
                FreeData();
                std::ostringstream oss;
                oss << "BinaryFileReader: Failed to read file data: " << file_path;
                throw std::runtime_error(oss.str());
            }
        }
    }

    LUMOS_INLINE const uint8_t *BinaryFileReader::GetData() const
    {
        return data_;
    }

    LUMOS_INLINE size_t BinaryFileReader::GetSize() const
    {
        return size_;
    }

    LUMOS_INLINE bool BinaryFileReader::IsEmpty() const
    {
        return size_ == 0;
    }

    LUMOS_INLINE void BinaryFileReader::Clear()
    {
        FreeData();
        file_path_.clear();
    }

    LUMOS_INLINE uint8_t BinaryFileReader::operator[](size_t index) const
    {
        if (index >= size_)
        {
            std::ostringstream oss;
            oss << "BinaryFileReader: Index " << index << " out of range (size: " << size_ << ")";
            throw std::out_of_range(oss.str());
        }
        return data_[index];
    }

    LUMOS_INLINE uint8_t BinaryFileReader::At(size_t index) const
    {
        if (index >= size_)
        {
            std::ostringstream oss;
            oss << "BinaryFileReader: Index " << index << " out of range (size: " << size_ << ")";
            throw std::out_of_range(oss.str());
        }
        return data_[index];
    }

    LUMOS_INLINE std::vector<uint8_t> BinaryFileReader::ToVector() const
    {
        if (!data_ || size_ == 0)
        {
            return std::vector<uint8_t>();
        }

        return std::vector<uint8_t>(data_, data_ + size_);
    }

    LUMOS_INLINE const std::string &BinaryFileReader::GetFilePath() const
    {
        return file_path_;
    }

    LUMOS_INLINE void BinaryFileReader::FreeData()
    {
        delete[] data_;
        data_ = nullptr;
        size_ = 0;
    }

    // =============================================================================
    // BinaryFileWriter Class Implementation
    // =============================================================================

    LUMOS_INLINE BinaryFileWriter::BinaryFileWriter(const std::string &file_path, WriteMode write_mode, FlushMode flush_mode)
        : file_path_(file_path), write_mode_(write_mode), flush_mode_(flush_mode),
          bytes_written_(0), is_open_(false)
    {
        InitializeFile();
    }

    LUMOS_INLINE BinaryFileWriter::BinaryFileWriter(BinaryFileWriter &&other) noexcept
        : file_path_(std::move(other.file_path_)),
          write_mode_(other.write_mode_),
          flush_mode_(other.flush_mode_),
          buffer_(std::move(other.buffer_)),
          bytes_written_(other.bytes_written_),
          is_open_(other.is_open_)
    {

        other.bytes_written_ = 0;
        other.is_open_ = false;
    }

    LUMOS_INLINE BinaryFileWriter &BinaryFileWriter::operator=(BinaryFileWriter &&other) noexcept
    {
        if (this != &other)
        {
            // Close current file if open
            if (is_open_)
            {
                try
                {
                    Close();
                }
                catch (...)
                {
                    // Ignore exceptions during cleanup
                }
            }

            file_path_ = std::move(other.file_path_);
            write_mode_ = other.write_mode_;
            flush_mode_ = other.flush_mode_;
            buffer_ = std::move(other.buffer_);
            bytes_written_ = other.bytes_written_;
            is_open_ = other.is_open_;

            other.bytes_written_ = 0;
            other.is_open_ = false;
        }
        return *this;
    }

    LUMOS_INLINE BinaryFileWriter::~BinaryFileWriter()
    {
        if (is_open_)
        {
            try
            {
                Close();
            }
            catch (...)
            {
                // Ignore exceptions in destructor
            }
        }
    }

    LUMOS_INLINE void BinaryFileWriter::Write(uint8_t byte)
    {
        if (!is_open_)
        {
            std::ostringstream oss;
            oss << "BinaryFileWriter: Cannot write to closed file: " << file_path_;
            throw std::runtime_error(oss.str());
        }

        if (flush_mode_ == FlushMode::Immediate)
        {
            WriteToFile(&byte, 1);
        }
        else
        {
            buffer_.push_back(byte);
        }

        bytes_written_++;
    }

    LUMOS_INLINE void BinaryFileWriter::Write(const std::vector<uint8_t> &data)
    {
        if (data.empty())
            return;
        Write(data.data(), data.size());
    }

    LUMOS_INLINE void BinaryFileWriter::Write(const uint8_t *data, size_t size)
    {
        if (!is_open_)
        {
            std::ostringstream oss;
            oss << "BinaryFileWriter: Cannot write to closed file: " << file_path_;
            throw std::runtime_error(oss.str());
        }

        if (size == 0 || data == nullptr)
            return;

        if (flush_mode_ == FlushMode::Immediate)
        {
            WriteToFile(data, size);
        }
        else
        {
            buffer_.insert(buffer_.end(), data, data + size);
        }

        bytes_written_ += size;
    }

    LUMOS_INLINE void BinaryFileWriter::Write(const std::string &s)
    {
        if (s.empty())
            return;
        Write(reinterpret_cast<const uint8_t *>(s.data()), s.size());
    }

    LUMOS_INLINE void BinaryFileWriter::Flush()
    {
        if (!is_open_)
        {
            return; // Nothing to flush if file is closed
        }

        if (flush_mode_ == FlushMode::UponClosing && !buffer_.empty())
        {
            WriteToFile(buffer_.data(), buffer_.size());
            buffer_.clear();
        }
    }

    LUMOS_INLINE void BinaryFileWriter::Close()
    {
        if (!is_open_)
        {
            return; // Already closed
        }

        // Flush any buffered data
        if (flush_mode_ == FlushMode::UponClosing && !buffer_.empty())
        {
            WriteToFile(buffer_.data(), buffer_.size());
            buffer_.clear();
        }

        is_open_ = false;
    }

    LUMOS_INLINE bool BinaryFileWriter::IsOpen() const
    {
        return is_open_;
    }

    LUMOS_INLINE const std::string &BinaryFileWriter::GetFilePath() const
    {
        return file_path_;
    }

    LUMOS_INLINE WriteMode BinaryFileWriter::GetWriteMode() const
    {
        return write_mode_;
    }

    LUMOS_INLINE FlushMode BinaryFileWriter::GetFlushMode() const
    {
        return flush_mode_;
    }

    LUMOS_INLINE size_t BinaryFileWriter::GetBytesWritten() const
    {
        return bytes_written_;
    }

    LUMOS_INLINE void BinaryFileWriter::WriteToFile(const uint8_t *data, size_t size)
    {
        if (size == 0 || data == nullptr)
            return;

        std::ios_base::openmode mode = std::ios::binary;

        // For overwrite mode, all writes are appends since we already truncated in InitializeFile
        // For append mode, we always append
        mode |= std::ios::out | std::ios::app;

        std::ofstream file(file_path_, mode);

        if (!file.is_open())
        {
            std::ostringstream oss;
            oss << "BinaryFileWriter: Failed to open file for writing: " << file_path_;
            throw std::runtime_error(oss.str());
        }

        file.write(reinterpret_cast<const char *>(data), size);

        if (!file)
        {
            std::ostringstream oss;
            oss << "BinaryFileWriter: Failed to write data to file: " << file_path_;
            throw std::runtime_error(oss.str());
        }
    }

    LUMOS_INLINE void BinaryFileWriter::InitializeFile()
    {
        // For OverWrite mode, we need to clear the file initially
        if (write_mode_ == WriteMode::OverWrite)
        {
            std::ofstream file(file_path_, std::ios::binary | std::ios::out | std::ios::trunc);
            if (!file.is_open())
            {
                std::ostringstream oss;
                oss << "BinaryFileWriter: Failed to initialize file: " << file_path_;
                throw std::runtime_error(oss.str());
            }
            // File is created/truncated, we can close it
            file.close();
        }
        // For Append mode, we don't need to do anything special
        // The file will be created if it doesn't exist on first write

        is_open_ = true;
    }

} // namespace binary_io

#endif // BINARY_IO_IMPL_H