#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <random>

#include "lumos/binary_io/binary_io.h"

namespace lumos
{

    class BinaryIOTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Create a temporary directory for test files
            test_dir = std::filesystem::temp_directory_path() / "binary_io_test";
            std::filesystem::create_directories(test_dir);
        }

        void TearDown() override
        {
            // Clean up test files
            std::error_code ec;
            std::filesystem::remove_all(test_dir, ec);
        }

        std::filesystem::path test_dir;

        // Helper function to create a test file with specific content
        void CreateTestFile(const std::string &filename, const std::vector<uint8_t> &content)
        {
            std::filesystem::path file_path = test_dir / filename;
            std::ofstream file(file_path, std::ios::binary);
            ASSERT_TRUE(file.is_open());

            if (!content.empty())
            {
                file.write(reinterpret_cast<const char *>(content.data()), content.size());
            }

            ASSERT_TRUE(file.good());
        }

        // Helper function to generate random binary data
        std::vector<uint8_t> GenerateRandomData(size_t size)
        {
            std::vector<uint8_t> data(size);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint8_t> dis(0, 255);

            for (auto &byte : data)
            {
                byte = dis(gen);
            }

            return data;
        }
    };

    // =============================================================================
    // Basic File Reading Tests
    // =============================================================================

    TEST_F(BinaryIOTest, ReadEmptyFile)
    {
        CreateTestFile("empty.bin", {});

        std::string file_path = (test_dir / "empty.bin").string();
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_TRUE(result.empty());
    }

    TEST_F(BinaryIOTest, ReadSingleByte)
    {
        std::vector<uint8_t> test_data = {0xFF};
        CreateTestFile("single.bin", test_data);

        std::string file_path = (test_dir / "single.bin").string();
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, ReadSmallBinaryFile)
    {
        std::vector<uint8_t> test_data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD};
        CreateTestFile("small.bin", test_data);

        std::string file_path = (test_dir / "small.bin").string();
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, ReadLargeBinaryFile)
    {
        const size_t large_size = 100000; // 100KB
        std::vector<uint8_t> test_data = GenerateRandomData(large_size);
        CreateTestFile("large.bin", test_data);

        std::string file_path = (test_dir / "large.bin").string();
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_EQ(result.size(), large_size);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, ReadBinaryWithNullBytes)
    {
        std::vector<uint8_t> test_data = {0x00, 0x00, 0x00, 0x42, 0x00, 0x00};
        CreateTestFile("nulls.bin", test_data);

        std::string file_path = (test_dir / "nulls.bin").string();
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, ReadBinaryWithAllByteValues)
    {
        std::vector<uint8_t> test_data;
        for (int i = 0; i <= 255; ++i)
        {
            test_data.push_back(static_cast<uint8_t>(i));
        }
        CreateTestFile("all_bytes.bin", test_data);

        std::string file_path = (test_dir / "all_bytes.bin").string();
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_EQ(result, test_data);
        EXPECT_EQ(result.size(), 256);
    }

    // =============================================================================
    // Error Handling Tests
    // =============================================================================

    TEST_F(BinaryIOTest, ReadNonExistentFile)
    {
        std::string non_existent_path = (test_dir / "does_not_exist.bin").string();

        EXPECT_THROW(binary_io::ReadBinaryData(non_existent_path), std::runtime_error);
    }

    TEST_F(BinaryIOTest, ReadFromDirectory)
    {
        std::string dir_path = test_dir.string();

        EXPECT_THROW(binary_io::ReadBinaryData(dir_path), std::runtime_error);
    }

    // =============================================================================
    // Write Binary Data Tests
    // =============================================================================

    TEST_F(BinaryIOTest, WriteEmptyFile)
    {
        std::vector<uint8_t> empty_data;
        std::string file_path = (test_dir / "write_empty.bin").string();

        EXPECT_NO_THROW(binary_io::WriteBinaryData(empty_data, file_path));

        // Verify the file exists and is empty
        EXPECT_TRUE(binary_io::FileExists(file_path));
        EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
    }

    TEST_F(BinaryIOTest, WriteSingleByte)
    {
        std::vector<uint8_t> test_data = {0xAB};
        std::string file_path = (test_dir / "write_single.bin").string();

        EXPECT_NO_THROW(binary_io::WriteBinaryData(test_data, file_path));

        // Read back and verify
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, WriteAndReadRoundTrip)
    {
        std::vector<uint8_t> test_data = GenerateRandomData(1000);
        std::string file_path = (test_dir / "roundtrip.bin").string();

        // Write data
        EXPECT_NO_THROW(binary_io::WriteBinaryData(test_data, file_path));

        // Read back
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        // Verify they match
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, WriteToInvalidPath)
    {
        std::vector<uint8_t> test_data = {0x42};
        std::string invalid_path = "/invalid/path/that/does/not/exist/file.bin";

        EXPECT_THROW(binary_io::WriteBinaryData(test_data, invalid_path), std::runtime_error);
    }

    // =============================================================================
    // Utility Function Tests
    // =============================================================================

    TEST_F(BinaryIOTest, GetFileSize)
    {
        std::vector<uint8_t> test_data = GenerateRandomData(12345);
        CreateTestFile("size_test.bin", test_data);

        std::string file_path = (test_dir / "size_test.bin").string();
        size_t file_size = binary_io::GetFileSize(file_path);

        EXPECT_EQ(file_size, 12345);
    }

    TEST_F(BinaryIOTest, GetFileSizeEmpty)
    {
        CreateTestFile("empty_size_test.bin", {});

        std::string file_path = (test_dir / "empty_size_test.bin").string();
        size_t file_size = binary_io::GetFileSize(file_path);

        EXPECT_EQ(file_size, 0);
    }

    TEST_F(BinaryIOTest, GetFileSizeNonExistent)
    {
        std::string non_existent_path = (test_dir / "does_not_exist.bin").string();

        EXPECT_THROW(binary_io::GetFileSize(non_existent_path), std::runtime_error);
    }

    TEST_F(BinaryIOTest, FileExistsTrue)
    {
        CreateTestFile("exists_test.bin", {0x42});

        std::string file_path = (test_dir / "exists_test.bin").string();
        EXPECT_TRUE(binary_io::FileExists(file_path));
    }

    TEST_F(BinaryIOTest, FileExistsFalse)
    {
        std::string non_existent_path = (test_dir / "does_not_exist.bin").string();
        EXPECT_FALSE(binary_io::FileExists(non_existent_path));
    }

    TEST_F(BinaryIOTest, FileExistsDirectory)
    {
        // Test that FileExists returns true for directories
        EXPECT_TRUE(binary_io::FileExists(test_dir.string()));
    }

    // =============================================================================
    // Edge Cases and Special Scenarios
    // =============================================================================

    TEST_F(BinaryIOTest, ReadWriteLargeFile)
    {
        const size_t mega_byte = 1024 * 1024;
        const size_t file_size = 5 * mega_byte; // 5MB file

        std::vector<uint8_t> test_data = GenerateRandomData(file_size);
        std::string file_path = (test_dir / "large_file.bin").string();

        // Write large file
        EXPECT_NO_THROW(binary_io::WriteBinaryData(test_data, file_path));

        // Verify size
        EXPECT_EQ(binary_io::GetFileSize(file_path), file_size);

        // Read back (this tests memory allocation for large files)
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);

        EXPECT_EQ(result.size(), file_size);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryIOTest, OverwriteExistingFile)
    {
        std::vector<uint8_t> original_data = {0x01, 0x02, 0x03};
        std::vector<uint8_t> new_data = {0xFF, 0xFE, 0xFD, 0xFC};
        std::string file_path = (test_dir / "overwrite_test.bin").string();

        // Write original data
        binary_io::WriteBinaryData(original_data, file_path);
        std::vector<uint8_t> read_original = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(read_original, original_data);

        // Overwrite with new data
        binary_io::WriteBinaryData(new_data, file_path);
        std::vector<uint8_t> read_new = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(read_new, new_data);
        EXPECT_NE(read_new, original_data);
    }

    TEST_F(BinaryIOTest, UnicodeFileName)
    {
        std::vector<uint8_t> test_data = {0x42, 0x43, 0x44};
        std::string unicode_filename = "测试文件.bin"; // Chinese characters
        std::string file_path = (test_dir / unicode_filename).string();

        // This test may not work on all systems due to filesystem encoding
        try
        {
            binary_io::WriteBinaryData(test_data, file_path);
            std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
            EXPECT_EQ(result, test_data);
        }
        catch (const std::runtime_error &)
        {
            // Skip test if filesystem doesn't support Unicode filenames
            GTEST_SKIP() << "Filesystem doesn't support Unicode filenames";
        }
    }

    // =============================================================================
    // Integration Tests
    // =============================================================================

    TEST_F(BinaryIOTest, MultipleFilesSequential)
    {
        const int num_files = 10;
        std::vector<std::vector<uint8_t>> test_datasets;
        std::vector<std::string> file_paths;

        // Create multiple test files with different data
        for (int i = 0; i < num_files; ++i)
        {
            std::vector<uint8_t> data = GenerateRandomData(100 + i * 50);
            test_datasets.push_back(data);

            std::string filename = "multi_" + std::to_string(i) + ".bin";
            std::string file_path = (test_dir / filename).string();
            file_paths.push_back(file_path);

            binary_io::WriteBinaryData(data, file_path);
        }

        // Read all files back and verify
        for (int i = 0; i < num_files; ++i)
        {
            std::vector<uint8_t> result = binary_io::ReadBinaryData(file_paths[i]);
            EXPECT_EQ(result, test_datasets[i]) << "Mismatch in file " << i;
        }
    }

    // =============================================================================
    // BinaryFileReader Class Tests
    // =============================================================================

    class BinaryFileReaderTest : public BinaryIOTest
    {
        // Inherits all the helper functions from BinaryIOTest
    };

    TEST_F(BinaryFileReaderTest, DefaultConstructor)
    {
        binary_io::BinaryFileReader reader;

        EXPECT_TRUE(reader.IsEmpty());
        EXPECT_EQ(reader.GetSize(), 0);
        EXPECT_EQ(reader.GetData(), nullptr);
        EXPECT_TRUE(reader.GetFilePath().empty());
    }

    TEST_F(BinaryFileReaderTest, LoadEmptyFile)
    {
        CreateTestFile("empty.bin", {});
        std::string file_path = (test_dir / "empty.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_TRUE(reader.IsEmpty());
        EXPECT_EQ(reader.GetSize(), 0);
        EXPECT_EQ(reader.GetData(), nullptr);
        EXPECT_EQ(reader.GetFilePath(), file_path);
    }

    TEST_F(BinaryFileReaderTest, LoadSingleByte)
    {
        std::vector<uint8_t> test_data = {0xFF};
        CreateTestFile("single.bin", test_data);
        std::string file_path = (test_dir / "single.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_FALSE(reader.IsEmpty());
        EXPECT_EQ(reader.GetSize(), 1);
        EXPECT_NE(reader.GetData(), nullptr);
        EXPECT_EQ(reader.GetFilePath(), file_path);
        EXPECT_EQ(reader[0], 0xFF);
        EXPECT_EQ(reader.At(0), 0xFF);
    }

    TEST_F(BinaryFileReaderTest, LoadSmallBinaryFile)
    {
        std::vector<uint8_t> test_data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD};
        CreateTestFile("small.bin", test_data);
        std::string file_path = (test_dir / "small.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_FALSE(reader.IsEmpty());
        EXPECT_EQ(reader.GetSize(), test_data.size());
        EXPECT_NE(reader.GetData(), nullptr);
        EXPECT_EQ(reader.GetFilePath(), file_path);

        // Check all bytes
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            EXPECT_EQ(reader[i], test_data[i]) << "Mismatch at index " << i;
            EXPECT_EQ(reader.At(i), test_data[i]) << "Mismatch at index " << i;
        }

        // Verify ToVector() returns the same data
        std::vector<uint8_t> vector_copy = reader.ToVector();
        EXPECT_EQ(vector_copy, test_data);
    }

    TEST_F(BinaryFileReaderTest, LoadLargeBinaryFile)
    {
        const size_t large_size = 100000; // 100KB
        std::vector<uint8_t> test_data = GenerateRandomData(large_size);
        CreateTestFile("large.bin", test_data);
        std::string file_path = (test_dir / "large.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_FALSE(reader.IsEmpty());
        EXPECT_EQ(reader.GetSize(), large_size);
        EXPECT_NE(reader.GetData(), nullptr);

        // Spot check some random indices
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, large_size - 1);

        for (int i = 0; i < 100; ++i)
        { // Check 100 random positions
            size_t index = dis(gen);
            EXPECT_EQ(reader[index], test_data[index]) << "Mismatch at index " << index;
        }
    }

    TEST_F(BinaryFileReaderTest, LoadFileWithNullBytes)
    {
        std::vector<uint8_t> test_data = {0x00, 0x00, 0x00, 0x42, 0x00, 0x00};
        CreateTestFile("nulls.bin", test_data);
        std::string file_path = (test_dir / "nulls.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_EQ(reader.GetSize(), test_data.size());
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            EXPECT_EQ(reader[i], test_data[i]);
        }
    }

    TEST_F(BinaryFileReaderTest, LoadFileWithAllByteValues)
    {
        std::vector<uint8_t> test_data;
        for (int i = 0; i <= 255; ++i)
        {
            test_data.push_back(static_cast<uint8_t>(i));
        }
        CreateTestFile("all_bytes.bin", test_data);
        std::string file_path = (test_dir / "all_bytes.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_EQ(reader.GetSize(), 256);
        for (int i = 0; i <= 255; ++i)
        {
            EXPECT_EQ(reader[i], static_cast<uint8_t>(i));
        }
    }

    // Copy Constructor Tests
    TEST_F(BinaryFileReaderTest, CopyConstructor)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
        CreateTestFile("copy_test.bin", test_data);
        std::string file_path = (test_dir / "copy_test.bin").string();

        binary_io::BinaryFileReader original(file_path);
        binary_io::BinaryFileReader copy(original);

        EXPECT_EQ(copy.GetSize(), original.GetSize());
        EXPECT_EQ(copy.GetFilePath(), original.GetFilePath());
        EXPECT_NE(copy.GetData(), original.GetData()); // Different memory addresses

        // Verify data is the same
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            EXPECT_EQ(copy[i], original[i]);
            EXPECT_EQ(copy[i], test_data[i]);
        }
    }

    TEST_F(BinaryFileReaderTest, CopyConstructorEmptyReader)
    {
        binary_io::BinaryFileReader original;
        binary_io::BinaryFileReader copy(original);

        EXPECT_TRUE(copy.IsEmpty());
        EXPECT_EQ(copy.GetSize(), 0);
        EXPECT_EQ(copy.GetData(), nullptr);
        EXPECT_TRUE(copy.GetFilePath().empty());
    }

    // Move Constructor Tests
    TEST_F(BinaryFileReaderTest, MoveConstructor)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
        CreateTestFile("move_test.bin", test_data);
        std::string file_path = (test_dir / "move_test.bin").string();

        binary_io::BinaryFileReader original(file_path);
        const uint8_t *original_ptr = original.GetData();

        binary_io::BinaryFileReader moved(std::move(original));

        EXPECT_EQ(moved.GetSize(), test_data.size());
        EXPECT_EQ(moved.GetFilePath(), file_path);
        EXPECT_EQ(moved.GetData(), original_ptr); // Same memory address

        // Original should be empty after move
        EXPECT_TRUE(original.IsEmpty());
        EXPECT_EQ(original.GetSize(), 0);
        EXPECT_EQ(original.GetData(), nullptr);
        EXPECT_TRUE(original.GetFilePath().empty());

        // Verify moved data is correct
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            EXPECT_EQ(moved[i], test_data[i]);
        }
    }

    // Copy Assignment Tests
    TEST_F(BinaryFileReaderTest, CopyAssignment)
    {
        std::vector<uint8_t> test_data1 = {0x01, 0x02, 0x03};
        std::vector<uint8_t> test_data2 = {0xFF, 0xFE, 0xFD, 0xFC};

        CreateTestFile("copy1.bin", test_data1);
        CreateTestFile("copy2.bin", test_data2);

        std::string file_path1 = (test_dir / "copy1.bin").string();
        std::string file_path2 = (test_dir / "copy2.bin").string();

        binary_io::BinaryFileReader reader1(file_path1);
        binary_io::BinaryFileReader reader2(file_path2);

        reader1 = reader2; // Copy assignment

        EXPECT_EQ(reader1.GetSize(), test_data2.size());
        EXPECT_EQ(reader1.GetFilePath(), file_path2);
        EXPECT_NE(reader1.GetData(), reader2.GetData()); // Different memory addresses

        // Verify data is the same
        for (size_t i = 0; i < test_data2.size(); ++i)
        {
            EXPECT_EQ(reader1[i], reader2[i]);
            EXPECT_EQ(reader1[i], test_data2[i]);
        }
    }

    TEST_F(BinaryFileReaderTest, SelfCopyAssignment)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
        CreateTestFile("self_copy.bin", test_data);
        std::string file_path = (test_dir / "self_copy.bin").string();

        binary_io::BinaryFileReader reader(file_path);
        const uint8_t *original_ptr = reader.GetData();

        reader = reader; // Self assignment

        EXPECT_EQ(reader.GetSize(), test_data.size());
        EXPECT_EQ(reader.GetFilePath(), file_path);
        EXPECT_EQ(reader.GetData(), original_ptr); // Should remain the same

        // Verify data is still correct
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            EXPECT_EQ(reader[i], test_data[i]);
        }
    }

    // Move Assignment Tests
    TEST_F(BinaryFileReaderTest, MoveAssignment)
    {
        std::vector<uint8_t> test_data1 = {0x01, 0x02, 0x03};
        std::vector<uint8_t> test_data2 = {0xFF, 0xFE, 0xFD, 0xFC};

        CreateTestFile("move1.bin", test_data1);
        CreateTestFile("move2.bin", test_data2);

        std::string file_path1 = (test_dir / "move1.bin").string();
        std::string file_path2 = (test_dir / "move2.bin").string();

        binary_io::BinaryFileReader reader1(file_path1);
        binary_io::BinaryFileReader reader2(file_path2);

        const uint8_t *reader2_ptr = reader2.GetData();

        reader1 = std::move(reader2); // Move assignment

        EXPECT_EQ(reader1.GetSize(), test_data2.size());
        EXPECT_EQ(reader1.GetFilePath(), file_path2);
        EXPECT_EQ(reader1.GetData(), reader2_ptr); // Same memory address

        // reader2 should be empty after move
        EXPECT_TRUE(reader2.IsEmpty());
        EXPECT_EQ(reader2.GetSize(), 0);
        EXPECT_EQ(reader2.GetData(), nullptr);
        EXPECT_TRUE(reader2.GetFilePath().empty());

        // Verify moved data is correct
        for (size_t i = 0; i < test_data2.size(); ++i)
        {
            EXPECT_EQ(reader1[i], test_data2[i]);
        }
    }

    // LoadFile Tests
    TEST_F(BinaryFileReaderTest, LoadFileOverwritesPreviousData)
    {
        std::vector<uint8_t> test_data1 = {0x01, 0x02, 0x03};
        std::vector<uint8_t> test_data2 = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB};

        CreateTestFile("load1.bin", test_data1);
        CreateTestFile("load2.bin", test_data2);

        std::string file_path1 = (test_dir / "load1.bin").string();
        std::string file_path2 = (test_dir / "load2.bin").string();

        binary_io::BinaryFileReader reader(file_path1);
        EXPECT_EQ(reader.GetSize(), test_data1.size());

        // Load a different file
        reader.LoadFile(file_path2);

        EXPECT_EQ(reader.GetSize(), test_data2.size());
        EXPECT_EQ(reader.GetFilePath(), file_path2);

        // Verify new data is correct
        for (size_t i = 0; i < test_data2.size(); ++i)
        {
            EXPECT_EQ(reader[i], test_data2[i]);
        }
    }

    // Clear Tests
    TEST_F(BinaryFileReaderTest, ClearData)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
        CreateTestFile("clear_test.bin", test_data);
        std::string file_path = (test_dir / "clear_test.bin").string();

        binary_io::BinaryFileReader reader(file_path);
        EXPECT_FALSE(reader.IsEmpty());

        reader.Clear();

        EXPECT_TRUE(reader.IsEmpty());
        EXPECT_EQ(reader.GetSize(), 0);
        EXPECT_EQ(reader.GetData(), nullptr);
        EXPECT_TRUE(reader.GetFilePath().empty());
    }

    // Index Access Error Tests
    TEST_F(BinaryFileReaderTest, IndexOutOfBoundsOperator)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03};
        CreateTestFile("bounds_test.bin", test_data);
        std::string file_path = (test_dir / "bounds_test.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_THROW(reader[3], std::out_of_range);
        EXPECT_THROW(reader[100], std::out_of_range);
    }

    TEST_F(BinaryFileReaderTest, IndexOutOfBoundsAt)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03};
        CreateTestFile("bounds_at_test.bin", test_data);
        std::string file_path = (test_dir / "bounds_at_test.bin").string();

        binary_io::BinaryFileReader reader(file_path);

        EXPECT_THROW(reader.At(3), std::out_of_range);
        EXPECT_THROW(reader.At(100), std::out_of_range);
    }

    TEST_F(BinaryFileReaderTest, IndexAccessEmptyReader)
    {
        binary_io::BinaryFileReader reader;

        EXPECT_THROW(reader[0], std::out_of_range);
        EXPECT_THROW(reader.At(0), std::out_of_range);
    }

    // Error Handling Tests
    TEST_F(BinaryFileReaderTest, LoadNonExistentFile)
    {
        std::string non_existent_path = (test_dir / "does_not_exist.bin").string();

        EXPECT_THROW(binary_io::BinaryFileReader reader(non_existent_path), std::runtime_error);

        binary_io::BinaryFileReader reader;
        EXPECT_THROW(reader.LoadFile(non_existent_path), std::runtime_error);
    }

    TEST_F(BinaryFileReaderTest, LoadFromDirectory)
    {
        std::string dir_path = test_dir.string();

        EXPECT_THROW(binary_io::BinaryFileReader reader(dir_path), std::runtime_error);

        binary_io::BinaryFileReader reader;
        EXPECT_THROW(reader.LoadFile(dir_path), std::runtime_error);
    }

    // ToVector Tests
    TEST_F(BinaryFileReaderTest, ToVectorEmptyReader)
    {
        binary_io::BinaryFileReader reader;
        std::vector<uint8_t> result = reader.ToVector();
        EXPECT_TRUE(result.empty());
    }

    TEST_F(BinaryFileReaderTest, ToVectorCreatesCopy)
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
        CreateTestFile("tovector_test.bin", test_data);
        std::string file_path = (test_dir / "tovector_test.bin").string();

        binary_io::BinaryFileReader reader(file_path);
        std::vector<uint8_t> copy = reader.ToVector();

        EXPECT_EQ(copy, test_data);
        EXPECT_NE(copy.data(), reader.GetData()); // Different memory addresses

        // Modifying the copy shouldn't affect the reader
        copy[0] = 0xFF;
        EXPECT_NE(copy[0], reader[0]);
    }

    // Integration Tests
    TEST_F(BinaryFileReaderTest, MultipleReadersSimultaneous)
    {
        const int num_readers = 5;
        std::vector<std::vector<uint8_t>> test_datasets;
        std::vector<std::string> file_paths;
        std::vector<std::unique_ptr<binary_io::BinaryFileReader>> readers;

        // Create multiple test files and readers
        for (int i = 0; i < num_readers; ++i)
        {
            std::vector<uint8_t> data = GenerateRandomData(100 + i * 50);
            test_datasets.push_back(data);

            std::string filename = "multi_reader_" + std::to_string(i) + ".bin";
            CreateTestFile(filename, data);

            std::string file_path = (test_dir / filename).string();
            file_paths.push_back(file_path);

            readers.push_back(std::make_unique<binary_io::BinaryFileReader>(file_path));
        }

        // Verify all readers have correct data
        for (int i = 0; i < num_readers; ++i)
        {
            EXPECT_EQ(readers[i]->GetSize(), test_datasets[i].size()) << "Reader " << i;
            EXPECT_EQ(readers[i]->GetFilePath(), file_paths[i]) << "Reader " << i;

            // Spot check some data
            for (size_t j = 0; j < std::min(size_t(10), test_datasets[i].size()); ++j)
            {
                EXPECT_EQ((*readers[i])[j], test_datasets[i][j]) << "Reader " << i << ", index " << j;
            }
        }
    }

    TEST_F(BinaryFileReaderTest, LargeFileMemoryManagement)
    {
        const size_t mega_byte = 1024 * 1024;
        const size_t file_size = 5 * mega_byte; // 5MB file

        std::vector<uint8_t> test_data = GenerateRandomData(file_size);
        CreateTestFile("large_memory_test.bin", test_data);
        std::string file_path = (test_dir / "large_memory_test.bin").string();

        // Create reader in a scope to test destructor
        {
            binary_io::BinaryFileReader reader(file_path);
            EXPECT_EQ(reader.GetSize(), file_size);
            EXPECT_NE(reader.GetData(), nullptr);

            // Spot check some random positions
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> dis(0, file_size - 1);

            for (int i = 0; i < 50; ++i)
            {
                size_t index = dis(gen);
                EXPECT_EQ(reader[index], test_data[index]) << "Mismatch at index " << index;
            }
        }
        // Reader should be destroyed here, freeing memory
    }

    // =============================================================================
    // BinaryFileWriter Class Tests
    // =============================================================================

    class BinaryFileWriterTest : public BinaryIOTest
    {
        // Inherits all the helper functions from BinaryIOTest
    };

    TEST_F(BinaryFileWriterTest, ConstructorOverWriteImmediate)
    {
        std::string file_path = (test_dir / "overwrite_immediate.bin").string();

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            EXPECT_TRUE(writer.IsOpen());
            EXPECT_EQ(writer.GetFilePath(), file_path);
            EXPECT_EQ(writer.GetWriteMode(), binary_io::WriteMode::OverWrite);
            EXPECT_EQ(writer.GetFlushMode(), binary_io::FlushMode::Immediate);
            EXPECT_EQ(writer.GetBytesWritten(), 0);
        }

        // File should exist and be empty
        EXPECT_TRUE(binary_io::FileExists(file_path));
        EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
    }

    TEST_F(BinaryFileWriterTest, ConstructorAppendUponClosing)
    {
        std::string file_path = (test_dir / "append_upon_closing.bin").string();

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::Append, binary_io::FlushMode::UponClosing);
            EXPECT_TRUE(writer.IsOpen());
            EXPECT_EQ(writer.GetWriteMode(), binary_io::WriteMode::Append);
            EXPECT_EQ(writer.GetFlushMode(), binary_io::FlushMode::UponClosing);
        }
    }

    TEST_F(BinaryFileWriterTest, WriteSingleByteImmediate)
    {
        std::string file_path = (test_dir / "single_byte.bin").string();

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(static_cast<uint8_t>(0xFF));
            EXPECT_EQ(writer.GetBytesWritten(), 1);
        }

        // Verify file content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(result[0], 0xFF);
    }

    TEST_F(BinaryFileWriterTest, WriteMultipleBytesImmediate)
    {
        std::string file_path = (test_dir / "multiple_bytes.bin").string();
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(test_data);
            EXPECT_EQ(writer.GetBytesWritten(), test_data.size());
        }

        // Verify file content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryFileWriterTest, WriteRawDataImmediate)
    {
        std::string file_path = (test_dir / "raw_data.bin").string();
        uint8_t test_data[] = {0xAA, 0xBB, 0xCC, 0xDD};

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(test_data, sizeof(test_data));
            EXPECT_EQ(writer.GetBytesWritten(), sizeof(test_data));
        }

        // Verify file content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), sizeof(test_data));
        for (size_t i = 0; i < sizeof(test_data); ++i)
        {
            EXPECT_EQ(result[i], test_data[i]);
        }
    }

    TEST_F(BinaryFileWriterTest, WriteSingleByteUponClosing)
    {
        std::string file_path = (test_dir / "single_byte_buffered.bin").string();

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
            writer.Write(static_cast<uint8_t>(0x42));
            EXPECT_EQ(writer.GetBytesWritten(), 1);

            // File should not contain data yet
            if (binary_io::FileExists(file_path))
            {
                EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
            }
        }

        // After destructor, file should contain the data
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), 1);
        EXPECT_EQ(result[0], 0x42);
    }

    TEST_F(BinaryFileWriterTest, WriteMultipleBytesUponClosing)
    {
        std::string file_path = (test_dir / "multiple_bytes_buffered.bin").string();
        std::vector<uint8_t> test_data = {0x10, 0x20, 0x30, 0x40, 0x50};

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);

            // Write data in chunks
            writer.Write(test_data[0]);
            writer.Write(std::vector<uint8_t>(test_data.begin() + 1, test_data.begin() + 3));
            writer.Write(&test_data[3], 2);

            EXPECT_EQ(writer.GetBytesWritten(), test_data.size());

            // File should not contain data yet (or be empty if it exists)
            if (binary_io::FileExists(file_path))
            {
                EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
            }
        }

        // After destructor, file should contain all the data
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryFileWriterTest, ManualFlush)
    {
        std::string file_path = (test_dir / "manual_flush.bin").string();
        std::vector<uint8_t> test_data = {0xF1, 0xF2, 0xF3};

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
            writer.Write(test_data);

            // File should not contain data yet
            if (binary_io::FileExists(file_path))
            {
                EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
            }

            // Manual flush should write the data
            writer.Flush();

            // Now file should contain the data
            std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
            EXPECT_EQ(result, test_data);
        }
    }

    TEST_F(BinaryFileWriterTest, ManualClose)
    {
        std::string file_path = (test_dir / "manual_close.bin").string();
        std::vector<uint8_t> test_data = {0xC1, 0xC2, 0xC3, 0xC4};

        binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
        writer.Write(test_data);

        EXPECT_TRUE(writer.IsOpen());

        // Manual close should flush and close
        writer.Close();

        EXPECT_FALSE(writer.IsOpen());

        // File should contain the data
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, test_data);

        // Further writes should throw
        EXPECT_THROW(writer.Write(static_cast<uint8_t>(0x99)), std::runtime_error);
    }

    TEST_F(BinaryFileWriterTest, OverwriteMode)
    {
        std::string file_path = (test_dir / "overwrite_test.bin").string();

        // Create initial file with some content
        std::vector<uint8_t> initial_data = {0x01, 0x02, 0x03, 0x04, 0x05};
        binary_io::WriteBinaryData(initial_data, file_path);

        // Verify initial content
        std::vector<uint8_t> read_initial = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(read_initial, initial_data);

        // Overwrite with new content
        std::vector<uint8_t> new_data = {0xFF, 0xFE};
        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(new_data);
        }

        // Verify only new content remains
        std::vector<uint8_t> final_result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(final_result, new_data);
        EXPECT_NE(final_result, initial_data);
    }

    TEST_F(BinaryFileWriterTest, AppendMode)
    {
        std::string file_path = (test_dir / "append_test.bin").string();

        // Create initial file with some content
        std::vector<uint8_t> initial_data = {0x10, 0x20, 0x30};
        binary_io::WriteBinaryData(initial_data, file_path);

        // Append new content
        std::vector<uint8_t> append_data = {0x40, 0x50};
        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::Append, binary_io::FlushMode::Immediate);
            writer.Write(append_data);
        }

        // Verify combined content
        std::vector<uint8_t> expected_data = initial_data;
        expected_data.insert(expected_data.end(), append_data.begin(), append_data.end());

        std::vector<uint8_t> final_result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(final_result, expected_data);
    }

    TEST_F(BinaryFileWriterTest, AppendToNonExistentFile)
    {
        std::string file_path = (test_dir / "new_append_file.bin").string();
        std::vector<uint8_t> test_data = {0xA1, 0xA2, 0xA3};

        // File doesn't exist initially
        EXPECT_FALSE(binary_io::FileExists(file_path));

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::Append, binary_io::FlushMode::Immediate);
            writer.Write(test_data);
        }

        // File should be created with the data
        EXPECT_TRUE(binary_io::FileExists(file_path));
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryFileWriterTest, MoveConstructor)
    {
        std::string file_path = (test_dir / "move_constructor.bin").string();
        std::vector<uint8_t> test_data = {0x91, 0x92, 0x93};

        binary_io::BinaryFileWriter original(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
        original.Write(test_data);

        EXPECT_TRUE(original.IsOpen());
        EXPECT_EQ(original.GetBytesWritten(), test_data.size());

        // Move constructor
        binary_io::BinaryFileWriter moved(std::move(original));

        EXPECT_TRUE(moved.IsOpen());
        EXPECT_EQ(moved.GetFilePath(), file_path);
        EXPECT_EQ(moved.GetBytesWritten(), test_data.size());

        // Original should be in moved-from state
        EXPECT_FALSE(original.IsOpen());
        EXPECT_EQ(original.GetBytesWritten(), 0);

        // Moved writer should work normally
        moved.Close();

        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, test_data);
    }

    TEST_F(BinaryFileWriterTest, MoveAssignment)
    {
        std::string file_path1 = (test_dir / "move_assign1.bin").string();
        std::string file_path2 = (test_dir / "move_assign2.bin").string();

        std::vector<uint8_t> data1 = {0x11, 0x12};
        std::vector<uint8_t> data2 = {0x21, 0x22, 0x23};

        binary_io::BinaryFileWriter writer1(file_path1, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
        binary_io::BinaryFileWriter writer2(file_path2, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);

        writer1.Write(data1);
        writer2.Write(data2);

        // Move assignment
        writer1 = std::move(writer2);

        EXPECT_TRUE(writer1.IsOpen());
        EXPECT_EQ(writer1.GetFilePath(), file_path2);
        EXPECT_EQ(writer1.GetBytesWritten(), data2.size());

        EXPECT_FALSE(writer2.IsOpen());
        EXPECT_EQ(writer2.GetBytesWritten(), 0);

        writer1.Close();

        // File2 should contain data2
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path2);
        EXPECT_EQ(result, data2);
    }

    TEST_F(BinaryFileWriterTest, WriteEmptyData)
    {
        std::string file_path = (test_dir / "empty_write.bin").string();

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);

            // Write empty vector
            writer.Write(std::vector<uint8_t>());

            // Write null pointer
            writer.Write(nullptr, 0);

            // Write zero-size data
            uint8_t dummy = 0;
            writer.Write(&dummy, 0);

            EXPECT_EQ(writer.GetBytesWritten(), 0);
        }

        // File should exist but be empty
        EXPECT_TRUE(binary_io::FileExists(file_path));
        EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
    }

    TEST_F(BinaryFileWriterTest, LargeFileWrite)
    {
        std::string file_path = (test_dir / "large_write.bin").string();
        const size_t large_size = 100000; // 100KB

        std::vector<uint8_t> large_data = GenerateRandomData(large_size);

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(large_data);
            EXPECT_EQ(writer.GetBytesWritten(), large_size);
        }

        // Verify file content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), large_size);
        EXPECT_EQ(result, large_data);
    }

    TEST_F(BinaryFileWriterTest, LargeFileWriteBuffered)
    {
        std::string file_path = (test_dir / "large_write_buffered.bin").string();
        const size_t large_size = 50000; // 50KB

        std::vector<uint8_t> large_data = GenerateRandomData(large_size);

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);

            // Write in chunks
            const size_t chunk_size = 1000;
            for (size_t i = 0; i < large_size; i += chunk_size)
            {
                size_t current_chunk = std::min(chunk_size, large_size - i);
                writer.Write(&large_data[i], current_chunk);
            }

            EXPECT_EQ(writer.GetBytesWritten(), large_size);
        }

        // Verify file content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), large_size);
        EXPECT_EQ(result, large_data);
    }

    TEST_F(BinaryFileWriterTest, MultipleWritesSameFile)
    {
        std::string file_path = (test_dir / "multiple_writes.bin").string();

        std::vector<uint8_t> data1 = {0x01, 0x02};
        std::vector<uint8_t> data2 = {0x03, 0x04, 0x05};
        std::vector<uint8_t> data3 = {0x06};

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(data1);
            writer.Write(data2);
            writer.Write(data3);

            EXPECT_EQ(writer.GetBytesWritten(), data1.size() + data2.size() + data3.size());
        }

        // Verify combined content
        std::vector<uint8_t> expected;
        expected.insert(expected.end(), data1.begin(), data1.end());
        expected.insert(expected.end(), data2.begin(), data2.end());
        expected.insert(expected.end(), data3.begin(), data3.end());

        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result, expected);
    }

    TEST_F(BinaryFileWriterTest, ErrorInvalidPath)
    {
        std::string invalid_path = "/invalid/path/that/does/not/exist/file.bin";

        EXPECT_THROW(
            binary_io::BinaryFileWriter writer(invalid_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate),
            std::runtime_error);
    }

    TEST_F(BinaryFileWriterTest, WriteAfterClose)
    {
        std::string file_path = (test_dir / "write_after_close.bin").string();

        binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
        writer.Write(static_cast<uint8_t>(0x42));
        writer.Close();

        EXPECT_FALSE(writer.IsOpen());
        EXPECT_THROW(writer.Write(static_cast<uint8_t>(0x43)), std::runtime_error);
        EXPECT_THROW(writer.Write(std::vector<uint8_t>{0x44}), std::runtime_error);

        uint8_t data = 0x45;
        EXPECT_THROW(writer.Write(&data, 1), std::runtime_error);
    }

    TEST_F(BinaryFileWriterTest, FlushAfterClose)
    {
        std::string file_path = (test_dir / "flush_after_close.bin").string();

        binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
        writer.Write(static_cast<uint8_t>(0x42));
        writer.Close();

        // Flushing after close should be a no-op (not throw)
        EXPECT_NO_THROW(writer.Flush());
    }

    TEST_F(BinaryFileWriterTest, BinaryDataIntegrity)
    {
        std::string file_path = (test_dir / "binary_integrity.bin").string();

        // Test data with all possible byte values including nulls
        std::vector<uint8_t> test_data;
        for (int i = 0; i <= 255; ++i)
        {
            test_data.push_back(static_cast<uint8_t>(i));
        }

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(test_data);
        }

        // Verify all bytes are preserved correctly
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), 256);
        EXPECT_EQ(result, test_data);

        // Double-check specific problematic bytes
        EXPECT_EQ(result[0], 0x00);   // Null byte
        EXPECT_EQ(result[10], 0x0A);  // Line feed
        EXPECT_EQ(result[13], 0x0D);  // Carriage return
        EXPECT_EQ(result[255], 0xFF); // Max byte value
    }

    TEST_F(BinaryFileWriterTest, WriteStringImmediate)
    {
        std::string file_path = (test_dir / "string_immediate.bin").string();
        std::string test_string = "Hello, World!";

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(test_string);
            EXPECT_EQ(writer.GetBytesWritten(), test_string.size());
        }

        // Verify file content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), test_string.size());

        // Convert back to string and verify
        std::string result_string(result.begin(), result.end());
        EXPECT_EQ(result_string, test_string);
    }

    TEST_F(BinaryFileWriterTest, WriteStringBuffered)
    {
        std::string file_path = (test_dir / "string_buffered.bin").string();
        std::string test_string = "Buffered string data";

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
            writer.Write(test_string);
            EXPECT_EQ(writer.GetBytesWritten(), test_string.size());

            // File should not contain data yet (or be empty if it exists)
            if (binary_io::FileExists(file_path))
            {
                EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
            }
        }

        // After destructor, file should contain the string data
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        std::string result_string(result.begin(), result.end());
        EXPECT_EQ(result_string, test_string);
    }

    TEST_F(BinaryFileWriterTest, WriteEmptyString)
    {
        std::string file_path = (test_dir / "empty_string.bin").string();
        std::string empty_string = "";

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(empty_string);
            EXPECT_EQ(writer.GetBytesWritten(), 0);
        }

        // File should exist but be empty
        EXPECT_TRUE(binary_io::FileExists(file_path));
        EXPECT_EQ(binary_io::GetFileSize(file_path), 0);
    }

    TEST_F(BinaryFileWriterTest, WriteStringWithSpecialCharacters)
    {
        std::string file_path = (test_dir / "string_special_chars.bin").string();
        std::string test_string = "Line1\nLine2\rLine3\tTabbed\0NullByte";
        test_string += static_cast<char>(0xFF); // Add a byte with value 255

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(test_string);
            EXPECT_EQ(writer.GetBytesWritten(), test_string.size());
        }

        // Verify all bytes are preserved correctly, including null bytes
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), test_string.size());

        // Compare byte by byte
        for (size_t i = 0; i < test_string.size(); ++i)
        {
            EXPECT_EQ(result[i], static_cast<uint8_t>(test_string[i])) << "Mismatch at index " << i;
        }
    }

    TEST_F(BinaryFileWriterTest, WriteMultipleStrings)
    {
        std::string file_path = (test_dir / "multiple_strings.bin").string();
        std::string str1 = "First";
        std::string str2 = "Second";
        std::string str3 = "Third";

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(str1);
            writer.Write(str2);
            writer.Write(str3);

            EXPECT_EQ(writer.GetBytesWritten(), str1.size() + str2.size() + str3.size());
        }

        // Verify concatenated content
        std::string expected = str1 + str2 + str3;
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        std::string result_string(result.begin(), result.end());
        EXPECT_EQ(result_string, expected);
    }

    TEST_F(BinaryFileWriterTest, WriteMixedDataTypes)
    {
        std::string file_path = (test_dir / "mixed_data.bin").string();

        std::string text = "Header:";
        std::vector<uint8_t> binary_data = {0x01, 0x02, 0x03, 0x04};
        uint8_t single_byte = 0xFF;
        std::string footer = ":Footer";

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(text);
            writer.Write(binary_data);
            writer.Write(single_byte);
            writer.Write(footer);

            size_t expected_size = text.size() + binary_data.size() + 1 + footer.size();
            EXPECT_EQ(writer.GetBytesWritten(), expected_size);
        }

        // Verify mixed content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        size_t pos = 0;

        // Check header
        for (size_t i = 0; i < text.size(); ++i, ++pos)
        {
            EXPECT_EQ(result[pos], static_cast<uint8_t>(text[i]));
        }

        // Check binary data
        for (size_t i = 0; i < binary_data.size(); ++i, ++pos)
        {
            EXPECT_EQ(result[pos], binary_data[i]);
        }

        // Check single byte
        EXPECT_EQ(result[pos++], single_byte);

        // Check footer
        for (size_t i = 0; i < footer.size(); ++i, ++pos)
        {
            EXPECT_EQ(result[pos], static_cast<uint8_t>(footer[i]));
        }

        EXPECT_EQ(pos, result.size());
    }

    TEST_F(BinaryFileWriterTest, WriteStringAppendMode)
    {
        std::string file_path = (test_dir / "string_append.bin").string();

        // Create initial file with some content
        std::string initial_text = "Initial content";
        binary_io::WriteBinaryData(std::vector<uint8_t>(initial_text.begin(), initial_text.end()), file_path);

        // Append string content
        std::string append_text = " + Appended text";
        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::Append, binary_io::FlushMode::Immediate);
            writer.Write(append_text);
        }

        // Verify combined content
        std::string expected = initial_text + append_text;
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        std::string result_string(result.begin(), result.end());
        EXPECT_EQ(result_string, expected);
    }

    TEST_F(BinaryFileWriterTest, WriteLargeString)
    {
        std::string file_path = (test_dir / "large_string.bin").string();

        // Create a large string (10KB)
        const size_t large_size = 10000;
        std::string large_string;
        large_string.reserve(large_size);

        for (size_t i = 0; i < large_size; ++i)
        {
            large_string += static_cast<char>('A' + (i % 26));
        }

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::UponClosing);
            writer.Write(large_string);
            EXPECT_EQ(writer.GetBytesWritten(), large_size);
        }

        // Verify large string content
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), large_size);

        std::string result_string(result.begin(), result.end());
        EXPECT_EQ(result_string, large_string);
    }

    TEST_F(BinaryFileWriterTest, WriteUnicodeString)
    {
        std::string file_path = (test_dir / "unicode_string.bin").string();

        // UTF-8 encoded string with various characters
        std::string unicode_string = "Hello 世界! 🌍 Café naïve résumé";

        {
            binary_io::BinaryFileWriter writer(file_path, binary_io::WriteMode::OverWrite, binary_io::FlushMode::Immediate);
            writer.Write(unicode_string);
            EXPECT_EQ(writer.GetBytesWritten(), unicode_string.size());
        }

        // Verify UTF-8 bytes are preserved exactly
        std::vector<uint8_t> result = binary_io::ReadBinaryData(file_path);
        EXPECT_EQ(result.size(), unicode_string.size());

        std::string result_string(result.begin(), result.end());
        EXPECT_EQ(result_string, unicode_string);
    }

} // namespace lumos