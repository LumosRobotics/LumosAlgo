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
                // Save original settings
                original_use_colors_ = internal::getUseColors();
                original_show_file_ = internal::getShowFile();
                original_show_func_ = internal::getShowFunc();
                original_show_line_ = internal::getShowLineNumber();
                original_show_thread_ = internal::getShowThreadId();
                original_output_stream_ = internal::getOutputStream();

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
                internal::setUseColors(original_use_colors_);
                internal::setShowFile(original_show_file_);
                internal::setShowFunc(original_show_func_);
                internal::setShowLineNumber(original_show_line_);
                internal::setShowThreadId(original_show_thread_);
                internal::setOutputStream(original_output_stream_);
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
            bool original_use_colors_;
            bool original_show_file_;
            bool original_show_func_;
            bool original_show_line_;
            bool original_show_thread_;
            std::ostream *original_output_stream_;
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

    } // namespace logging
} // namespace lumos