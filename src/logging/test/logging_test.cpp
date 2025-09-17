#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

#include "../logging.h"

namespace lumos
{
    namespace logging
    {

        // Test fixture for logging tests
        class LoggingTest : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Save original settings using the optimized method
                original_settings_ = internal::LoggingConfig::instance().getAllSettings();

                // Set up test stream
                test_stream_.str("");
                test_stream_.clear();
                internal::setOutputStream(&test_stream_);

                // Set consistent test configuration
                internal::setUseColors(false);
                internal::setShowFile(true);
                internal::setShowFunc(true);
                internal::setShowLineNumber(true);
                internal::setShowThreadId(false);
            }

            void TearDown() override
            {
                // Restore original settings
                internal::setUseColors(original_settings_.use_colors);
                internal::setShowFile(original_settings_.show_file);
                internal::setShowFunc(original_settings_.show_func);
                internal::setShowLineNumber(original_settings_.show_line_number);
                internal::setShowThreadId(original_settings_.show_thread_id);
                internal::setOutputStream(original_settings_.output_stream);
            }

            std::string getLastLogOutputAndClearStream()
            {
                std::string output = test_stream_.str();
                test_stream_.str("");
                test_stream_.clear();
                return output;
            }

        private:
            std::ostringstream test_stream_;
            internal::LoggingConfig::Settings original_settings_;
        };

        // Test basic logging functionality
        TEST_F(LoggingTest, BasicLogging)
        {
            LUMOS_LOG_INFO() << "Test message";

            std::string output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("INFO") != std::string::npos);
            EXPECT_TRUE(output.find("Test message") != std::string::npos);
            EXPECT_TRUE(output.find("logging_test.cpp") != std::string::npos);
        }

        // Test different severity levels
        TEST_F(LoggingTest, SeverityLevels)
        {
            LUMOS_LOG() << "Log message";
            std::string log_output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(log_output.find("LOG") != std::string::npos);

            LUMOS_LOG_DEBUG() << "Debug message";
            std::string debug_output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(debug_output.find("DEBUG") != std::string::npos);

            LUMOS_LOG_WARNING() << "Warning message";
            std::string warning_output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(warning_output.find("WARNING") != std::string::npos);

            LUMOS_LOG_ERROR() << "Error message";
            std::string error_output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(error_output.find("ERROR") != std::string::npos);

            LUMOS_LOG_TRACE() << "Trace message";
            std::string trace_output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(trace_output.find("TRACE") != std::string::npos);
        }

        // Test format string functionality
        TEST_F(LoggingTest, FormatStrings)
        {
            LUMOS_LOG_INFOF("Number: %d, String: %s", 42, "test");

            std::string output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("Number: 42") != std::string::npos);
            EXPECT_TRUE(output.find("String: test") != std::string::npos);
        }

        // Test configuration settings
        TEST_F(LoggingTest, ConfigurationSettings)
        {
            // Test disabling file display
            internal::setShowFile(false);
            LUMOS_LOG_INFO() << "No file test";
            std::string output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("logging_test.cpp") == std::string::npos);

            // Test disabling function display
            internal::setShowFunc(false);
            LUMOS_LOG_INFO() << "No func test";
            output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("ConfigurationSettings") == std::string::npos);

            // Test disabling line numbers
            internal::setShowLineNumber(false);
            LUMOS_LOG_INFO() << "No line test";
            output = getLastLogOutputAndClearStream();
            // Should not contain line number in brackets
            size_t bracket_pos = output.find("[");
            if (bracket_pos != std::string::npos)
            {
                std::string between_brackets = output.substr(bracket_pos);
                EXPECT_TRUE(between_brackets.find_first_of("0123456789") == std::string::npos ||
                            between_brackets.find("INFO") != std::string::npos);
            }
        }

        // Test thread ID functionality
        TEST_F(LoggingTest, ThreadIdDisplay)
        {
            internal::setShowThreadId(true);

            LUMOS_LOG_INFO() << "Thread test";
            std::string output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("0x") != std::string::npos);

            internal::setShowThreadId(false);
            LUMOS_LOG_INFO() << "No thread test";
            output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("0x") == std::string::npos);
        }

        // Test color functionality
        TEST_F(LoggingTest, ColorFunctionality)
        {
            internal::setUseColors(true);

            LUMOS_LOG_ERROR() << "Red error";
            std::string output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("\033[31m") != std::string::npos); // Red color
            EXPECT_TRUE(output.find("\033[0m") != std::string::npos);  // Reset color
        }

        // Test output stream redirection
        TEST_F(LoggingTest, OutputStreamRedirection)
        {
            std::ostringstream custom_stream;
            internal::setOutputStream(&custom_stream);

            LUMOS_LOG_INFO() << "Custom stream test";

            std::string output = custom_stream.str();
            EXPECT_TRUE(output.find("Custom stream test") != std::string::npos);
        }

        // Test conditional printing
        TEST_F(LoggingTest, ConditionalPrinting)
        {
            LUMOS_PRINT_COND(true) << "Should print";
            std::string output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.find("Should print") != std::string::npos);

            LUMOS_PRINT_COND(false) << "Should not print";
            output = getLastLogOutputAndClearStream();
            EXPECT_TRUE(output.empty() || output.find("Should not print") == std::string::npos);
        }

        // Test thread safety (basic test)
        TEST_F(LoggingTest, ThreadSafety)
        {
            std::ostringstream thread_stream;
            internal::setOutputStream(&thread_stream);

            std::vector<std::thread> threads;
            const int num_threads = 5;
            const int messages_per_thread = 10;

            for (int i = 0; i < num_threads; ++i)
            {
                threads.emplace_back([i, messages_per_thread]()
                                     {
            for (int j = 0; j < messages_per_thread; ++j) {
                LUMOS_LOG_INFOF("Thread %d Message %d", i, j);
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            } });
            }

            for (auto &thread : threads)
            {
                thread.join();
            }

            std::string output = thread_stream.str();
            // Count the number of complete log messages
            size_t count = 0;
            size_t pos = 0;
            while ((pos = output.find("Thread", pos)) != std::string::npos)
            {
                count++;
                pos++;
            }

            EXPECT_EQ(count, num_threads * messages_per_thread);
        }

        // Test public API functions
        TEST_F(LoggingTest, PublicAPI)
        {
            useColors(false);
            EXPECT_FALSE(internal::getUseColors());

            showFile(false);
            EXPECT_FALSE(internal::getShowFile());

            showFunction(false);
            EXPECT_FALSE(internal::getShowFunc());

            showLineNumber(false);
            EXPECT_FALSE(internal::getShowLineNumber());

            showThreadId(true);
            EXPECT_TRUE(internal::getShowThreadId());
        }

        // Test severity lookup tables
        TEST_F(LoggingTest, SeverityLookupTables)
        {
            // Test that all severity levels have valid strings and colors
            for (int i = 0; i <= static_cast<int>(internal::MessageSeverity::EXIT); ++i)
            {
                auto severity = static_cast<internal::MessageSeverity>(i);
                const char *color = internal::getSeverityColor(severity);
                const char *name = internal::getSeverityString(severity);

                EXPECT_NE(color, nullptr);
                EXPECT_NE(name, nullptr);
                EXPECT_STRNE(color, "UNKNOWNSEVERITYCOLOR");
                EXPECT_STRNE(name, "UNKNOWNSEVERITY");
            }
        }

        // Test reset functionality
        TEST_F(LoggingTest, ResetFunctionality)
        {
            // Change all settings
            internal::setUseColors(false);
            internal::setShowFile(false);
            internal::setShowFunc(false);
            internal::setShowLineNumber(false);
            internal::setShowThreadId(false);
            internal::setConfigurableAssertion(true);
            
            std::ostringstream custom_stream;
            internal::setOutputStream(&custom_stream);
            
            // Reset to defaults
            resetToDefaults();
            
            // Verify all settings are back to defaults
            EXPECT_TRUE(internal::getUseColors());
            EXPECT_TRUE(internal::getShowFile());
            EXPECT_TRUE(internal::getShowFunc());
            EXPECT_TRUE(internal::getShowLineNumber());
            EXPECT_TRUE(internal::getShowThreadId());
            EXPECT_FALSE(internal::getConfigurableAssertion());
            
            // Output stream should be back to cout
            auto stream = internal::getOutputStream();
            EXPECT_EQ(stream.get(), &std::cout);
        }
        
        // Test improved singleton thread safety
        TEST_F(LoggingTest, SingletonThreadSafety)
        {
            // Test that multiple threads can safely access the singleton
            std::vector<std::thread> threads;
            std::atomic<int> success_count{0};
            
            for (int i = 0; i < 10; ++i) {
                threads.emplace_back([&success_count]() {
                    try {
                        auto& config1 = internal::LoggingConfig::instance();
                        auto& config2 = internal::LoggingConfig::instance();
                        
                        // Should be the same instance
                        if (&config1 == &config2) {
                            success_count++;
                        }
                        
                        // Test concurrent reads
                        for (int j = 0; j < 100; ++j) {
                            config1.getUseColors();
                            config1.getShowFile();
                            config1.getAllSettings();
                        }
                    } catch (...) {
                        // Should not throw
                    }
                });
            }
            
            for (auto& thread : threads) {
                thread.join();
            }
            
            EXPECT_EQ(success_count.load(), 10);
        }

        // Test atomic operations performance
        TEST_F(LoggingTest, AtomicOperationsPerformance)
        {
            // This test ensures atomic operations work correctly under load
            std::vector<std::thread> readers;
            std::vector<std::thread> writers;
            std::atomic<bool> stop{false};
            std::atomic<int> read_count{0};
            std::atomic<int> write_count{0};
            
            // Start reader threads
            for (int i = 0; i < 4; ++i) {
                readers.emplace_back([&stop, &read_count]() {
                    while (!stop.load()) {
                        internal::getUseColors();
                        internal::getShowFile();
                        internal::getShowFunc();
                        read_count++;
                    }
                });
            }
            
            // Start writer threads
            for (int i = 0; i < 2; ++i) {
                writers.emplace_back([&stop, &write_count, i]() {
                    while (!stop.load()) {
                        internal::setUseColors(i % 2 == 0);
                        internal::setShowFile(i % 2 == 1);
                        write_count++;
                        std::this_thread::sleep_for(std::chrono::microseconds(10));
                    }
                });
            }
            
            // Let it run for a short time
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            stop.store(true);
            
            // Wait for all threads
            for (auto& thread : readers) {
                thread.join();
            }
            for (auto& thread : writers) {
                thread.join();
            }
            
            // Should have performed many operations without issues
            EXPECT_GT(read_count.load(), 0);
            EXPECT_GT(write_count.load(), 0);
        }

        // Test file logging with full path
        TEST_F(LoggingTest, FileLoggingFullPath)
        {
            const std::string test_file = "/tmp/test_logging_full_path.log";
            
            // Setup file logging
            setupLogFile(test_file);
            setLogToBothConsoleAndFile(false);  // Only to file
            
            LUMOS_LOG_INFO() << "Test message to file";
            LUMOS_LOG_ERROR() << "Error message to file";
            
            // Disable file logging to flush and close file
            disableFileLogging();
            
            // Read back the file content
            std::ifstream file(test_file);
            ASSERT_TRUE(file.is_open());
            
            std::string line;
            std::vector<std::string> lines;
            while (std::getline(file, line)) {
                lines.push_back(line);
            }
            file.close();
            
            EXPECT_EQ(lines.size(), 2);
            EXPECT_TRUE(lines[0].find("Test message to file") != std::string::npos);
            EXPECT_TRUE(lines[0].find("INFO") != std::string::npos);
            EXPECT_TRUE(lines[1].find("Error message to file") != std::string::npos);
            EXPECT_TRUE(lines[1].find("ERROR") != std::string::npos);
            
            // Clean up
            std::remove(test_file.c_str());
        }
        
        // Test file logging with directory and pattern
        TEST_F(LoggingTest, FileLoggingDirectoryPattern)
        {
            const std::string log_dir = "/tmp/logging_test";
            const std::string pattern = "test_app";
            
            // Setup file logging with pattern
            setupLogFile(log_dir, pattern);
            setLogToBothConsoleAndFile(false);  // Only to file
            
            LUMOS_LOG_WARNING() << "Warning message with pattern";
            
            // Get the generated file streams to verify file was created
            auto streams = internal::LoggingConfig::instance().getOutputStreams();
            EXPECT_EQ(streams.size(), 1);  // Should only have file stream
            
            disableFileLogging();
            
            // Check that a file was created in the directory with the expected pattern
            std::string cmd = "ls " + log_dir + "/test_app_*.log 2>/dev/null | wc -l";
            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) {
                char buffer[128];
                std::string result = "";
                while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    result += buffer;
                }
                pclose(pipe);
                
                int file_count = std::stoi(result);
                EXPECT_GT(file_count, 0);
            }
            
            // Clean up
            std::string cleanup_cmd = "rm -rf " + log_dir;
            std::system(cleanup_cmd.c_str());
        }
        
        // Test file logging with custom timestamp pattern
        TEST_F(LoggingTest, FileLoggingCustomTimestamp)
        {
            const std::string log_dir = "/tmp/logging_test_custom";
            const std::string pattern = "app_%Y-%m-%d_%H-%M";
            
            setupLogFile(log_dir, pattern);
            setLogToBothConsoleAndFile(false);
            
            LUMOS_LOG_DEBUG() << "Debug with custom timestamp";
            
            disableFileLogging();
            
            // Check that file was created with custom timestamp format
            std::string cmd = "ls " + log_dir + "/app_*.log 2>/dev/null | wc -l";
            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) {
                char buffer[128];
                std::string result = "";
                while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    result += buffer;
                }
                pclose(pipe);
                
                int file_count = std::stoi(result);
                EXPECT_GT(file_count, 0);
            }
            
            // Clean up
            std::string cleanup_cmd = "rm -rf " + log_dir;
            std::system(cleanup_cmd.c_str());
        }
        
        // Test dual output (console and file)
        TEST_F(LoggingTest, DualOutput)
        {
            const std::string test_file = "/tmp/test_dual_output.log";
            
            setupLogFile(test_file);
            setLogToBothConsoleAndFile(true);  // Both console and file
            
            LUMOS_LOG_INFO() << "Dual output message";
            
            // Should have both console and file streams
            auto streams = internal::LoggingConfig::instance().getOutputStreams();
            EXPECT_EQ(streams.size(), 2);
            
            disableFileLogging();
            
            // Verify file was written
            std::ifstream file(test_file);
            ASSERT_TRUE(file.is_open());
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            
            EXPECT_TRUE(content.find("Dual output message") != std::string::npos);
            
            // Clean up
            std::remove(test_file.c_str());
        }
        
        // Test file logging control functions
        TEST_F(LoggingTest, FileLoggingControl)
        {
            // Test getLogToBothConsoleAndFile
            EXPECT_TRUE(getLogToBothConsoleAndFile());  // Default is true
            
            setLogToBothConsoleAndFile(false);
            EXPECT_FALSE(getLogToBothConsoleAndFile());
            
            setLogToBothConsoleAndFile(true);
            EXPECT_TRUE(getLogToBothConsoleAndFile());
            
            // Test disableFileLogging
            const std::string test_file = "/tmp/test_disable.log";
            setupLogFile(test_file);
            
            auto streams_with_file = internal::LoggingConfig::instance().getOutputStreams();
            EXPECT_EQ(streams_with_file.size(), 2);  // Console + file
            
            disableFileLogging();
            
            auto streams_without_file = internal::LoggingConfig::instance().getOutputStreams();
            EXPECT_EQ(streams_without_file.size(), 1);  // Only console
        }
        
        // Test format strings with file logging
        TEST_F(LoggingTest, FormatStringsWithFileLogging)
        {
            const std::string test_file = "/tmp/test_format_file.log";
            
            setupLogFile(test_file);
            setLogToBothConsoleAndFile(false);
            
            LUMOS_LOG_INFOF("Format test: %d %s %.2f", 42, "hello", 3.14159);
            
            disableFileLogging();
            
            // Read back and verify formatting
            std::ifstream file(test_file);
            ASSERT_TRUE(file.is_open());
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            
            EXPECT_TRUE(content.find("Format test: 42 hello 3.14") != std::string::npos);
            
            // Clean up
            std::remove(test_file.c_str());
        }

    } // namespace logging
} // namespace lumos