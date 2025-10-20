#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <memory>

#include "lumos/argparse/argparse.h"

namespace lumos
{
    namespace argparse
    {

        // Test fixture for argparse tests
        class ArgparseTest : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Set up fresh parser for each test
                parser.reset(new ArgumentParser("Test program for argument parsing"));
            }

            void TearDown() override
            {
                parser.reset();
            }

            std::unique_ptr<ArgumentParser> parser;
        };

        // Test basic positional arguments
        TEST_F(ArgparseTest, PositionalArguments)
        {
            parser->addArgument("input_file").help("Input file path");
            parser->addArgument("output_file").help("Output file path");

            std::vector<std::string> args = {"input.txt", "output.txt"};
            parser->parseArgs(args);

            EXPECT_EQ(parser->get<std::string>("input_file"), "input.txt");
            EXPECT_EQ(parser->get<std::string>("output_file"), "output.txt");
            EXPECT_TRUE(parser->isSet("input_file"));
            EXPECT_TRUE(parser->isSet("output_file"));
        }

        // Test optional arguments
        TEST_F(ArgparseTest, OptionalArguments)
        {
            parser->addArgument("--verbose", Argument::Type::BOOL).action("store_true").help("Enable verbose output");
            parser->addArgument("--count", Argument::Type::INT).defaultValue("10").help("Number of iterations");
            parser->addArgument("--name", Argument::Type::STRING).help("User name");

            std::vector<std::string> args = {"--verbose", "--count", "42", "--name", "John"};
            parser->parseArgs(args);

            EXPECT_TRUE(parser->get<bool>("verbose"));
            EXPECT_EQ(parser->get<int>("count"), 42);
            EXPECT_EQ(parser->get<std::string>("name"), "John");
        }

        // Test short and long form arguments
        TEST_F(ArgparseTest, ShortAndLongForm)
        {
            parser->addArgument("--verbose", Argument::Type::BOOL).action("store_true");
            parser->addArgument("--count", Argument::Type::INT);

            // Test long form
            std::vector<std::string> args1 = {"--verbose", "--count", "5"};
            parser->parseArgs(args1);
            EXPECT_TRUE(parser->get<bool>("verbose"));
            EXPECT_EQ(parser->get<int>("count"), 5);

            // Reset parser for short form test
            parser.reset(new ArgumentParser());
            parser->addArgument("--verbose", Argument::Type::BOOL).action("store_true");
            parser->addArgument("--count", Argument::Type::INT);

            // Test short form
            std::vector<std::string> args2 = {"-v", "-c", "3"};
            parser->parseArgs(args2);
            EXPECT_TRUE(parser->get<bool>("verbose"));
            EXPECT_EQ(parser->get<int>("count"), 3);
        }

        // Test different argument types
        TEST_F(ArgparseTest, ArgumentTypes)
        {
            parser->addArgument("--string", Argument::Type::STRING);
            parser->addArgument("--integer", Argument::Type::INT);
            parser->addArgument("--double", Argument::Type::DOUBLE);
            parser->addArgument("--boolean", Argument::Type::BOOL).action("store_true");

            std::vector<std::string> args = {
                "--string", "hello",
                "--integer", "42",
                "--double", "3.14159",
                "--boolean"};
            parser->parseArgs(args);

            EXPECT_EQ(parser->get<std::string>("string"), "hello");
            EXPECT_EQ(parser->get<int>("integer"), 42);
            EXPECT_DOUBLE_EQ(parser->get<double>("double"), 3.14159);
            EXPECT_TRUE(parser->get<bool>("boolean"));
        }

        // Test default values
        TEST_F(ArgparseTest, DefaultValues)
        {
            parser->addArgument("--count", Argument::Type::INT).defaultValue("10");
            parser->addArgument("--name", Argument::Type::STRING).defaultValue("anonymous");
            parser->addArgument("--enabled", Argument::Type::BOOL).action("store_true");

            std::vector<std::string> args = {"--name", "Alice"}; // Only set name
            parser->parseArgs(args);

            EXPECT_EQ(parser->get<int>("count"), 10);             // Default value
            EXPECT_EQ(parser->get<std::string>("name"), "Alice"); // Explicitly set
            EXPECT_FALSE(parser->get<bool>("enabled"));           // Default false for store_true
        }

        // Test required arguments - this is the key functionality requested
        TEST_F(ArgparseTest, RequiredArguments)
        {
            parser->addArgument("--input", Argument::Type::STRING).required(true);
            parser->addArgument("output_file"); // Required positional

            // Missing required optional argument
            std::vector<std::string> args1 = {"output.txt"};
            EXPECT_THROW(parser->parseArgs(args1), ArgumentError);

            // Missing required positional argument
            std::vector<std::string> args2 = {"--input", "input.txt"};
            EXPECT_THROW(parser->parseArgs(args2), ArgumentError);

            // All required arguments provided
            std::vector<std::string> args3 = {"--input", "input.txt", "output.txt"};
            EXPECT_NO_THROW(parser->parseArgs(args3));
            EXPECT_EQ(parser->get<std::string>("input"), "input.txt");
            EXPECT_EQ(parser->get<std::string>("output_file"), "output.txt");
        }

        // Test required with default values (should not throw)
        TEST_F(ArgparseTest, RequiredWithDefaults)
        {
            parser->addArgument("--config", Argument::Type::STRING)
                .required(true)
                .defaultValue("default.conf");

            // Should not throw because default value is provided
            std::vector<std::string> args = {};
            EXPECT_NO_THROW(parser->parseArgs(args));
            EXPECT_EQ(parser->get<std::string>("config"), "default.conf");
        }

        // Test required flag explicitly set to false
        TEST_F(ArgparseTest, RequiredFalse)
        {
            parser->addArgument("--optional", Argument::Type::STRING).required(false);

            // Should not throw even when not provided
            std::vector<std::string> args = {};
            EXPECT_NO_THROW(parser->parseArgs(args));
            EXPECT_FALSE(parser->isSet("optional"));
        }

        // Test choices validation
        TEST_F(ArgparseTest, ChoicesValidation)
        {
            parser->addArgument("--mode", Argument::Type::STRING)
                .choices({"debug", "release", "test"})
                .defaultValue("release");

            // Valid choice
            std::vector<std::string> args1 = {"--mode", "debug"};
            EXPECT_NO_THROW(parser->parseArgs(args1));
            EXPECT_EQ(parser->get<std::string>("mode"), "debug");

            // Invalid choice
            std::vector<std::string> args2 = {"--mode", "invalid"};
            EXPECT_THROW(parser->parseArgs(args2), ArgumentError);
        }

        // Test string list arguments
        TEST_F(ArgparseTest, StringListArguments)
        {
            parser->addArgument("--files", Argument::Type::STRING_LIST).nargs("+");

            std::vector<std::string> args = {"--files", "file1.txt", "file2.txt", "file3.txt"};
            parser->parseArgs(args);

            auto files = parser->get<std::vector<std::string>>("files");
            EXPECT_EQ(files.size(), 3);
            EXPECT_EQ(files[0], "file1.txt");
            EXPECT_EQ(files[1], "file2.txt");
            EXPECT_EQ(files[2], "file3.txt");
        }

        // Test error handling
        TEST_F(ArgparseTest, ErrorHandling)
        {
            parser->addArgument("--count", Argument::Type::INT);

            // Invalid integer conversion
            std::vector<std::string> args1 = {"--count", "not_a_number"};
            EXPECT_THROW(parser->parseArgs(args1), ArgumentTypeError);

            // Unknown argument
            std::vector<std::string> args2 = {"--unknown"};
            EXPECT_THROW(parser->parseArgs(args2), ArgumentError);

            // Missing value for argument
            std::vector<std::string> args3 = {"--count"};
            EXPECT_THROW(parser->parseArgs(args3), ArgumentError);
        }

        // Test type conversions
        TEST_F(ArgparseTest, TypeConversions)
        {
            parser->addArgument("--value", Argument::Type::STRING).defaultValue("42");
            parser->parseArgs(std::vector<std::string>{}); // Use default

            // Test various type conversions
            EXPECT_EQ(parser->get<std::string>("value"), "42");
            EXPECT_EQ(parser->get<int>("value"), 42);
            EXPECT_DOUBLE_EQ(parser->get<double>("value"), 42.0);
            EXPECT_TRUE(parser->get<bool>("value")); // Non-zero is true

            // Test string list conversion
            auto list = parser->get<std::vector<std::string>>("value");
            EXPECT_EQ(list.size(), 1);
            EXPECT_EQ(list[0], "42");
        }

        // Test mixed positional and optional arguments
        TEST_F(ArgparseTest, MixedArguments)
        {
            parser->addArgument("input_file");
            parser->addArgument("--verbose", Argument::Type::BOOL).action("store_true");
            parser->addArgument("--count", Argument::Type::INT).defaultValue("1");
            parser->addArgument("output_file");

            std::vector<std::string> args = {"input.txt", "--verbose", "--count", "5", "output.txt"};
            parser->parseArgs(args);

            EXPECT_EQ(parser->get<std::string>("input_file"), "input.txt");
            EXPECT_TRUE(parser->get<bool>("verbose"));
            EXPECT_EQ(parser->get<int>("count"), 5);
            EXPECT_EQ(parser->get<std::string>("output_file"), "output.txt");
        }

        // Test help generation
        TEST_F(ArgparseTest, HelpGeneration)
        {
            parser->addArgument("input_file").help("Input file to process");
            parser->addArgument("--verbose", Argument::Type::BOOL).action("store_true").help("Enable verbose output");
            parser->addArgument("--count", Argument::Type::INT).defaultValue("10").help("Number of iterations");

            std::string help = parser->formatHelp();

            EXPECT_TRUE(help.find("Usage:") != std::string::npos);
            EXPECT_TRUE(help.find("input_file") != std::string::npos);
            EXPECT_TRUE(help.find("--verbose") != std::string::npos);
            EXPECT_TRUE(help.find("--count") != std::string::npos);
            EXPECT_TRUE(help.find("Input file to process") != std::string::npos);
            EXPECT_TRUE(help.find("Enable verbose output") != std::string::npos);
            EXPECT_TRUE(help.find("default: 10") != std::string::npos);
        }

        // Test boolean argument variations
        TEST_F(ArgparseTest, BooleanArguments)
        {
            parser->addArgument("--flag1", Argument::Type::BOOL).action("store_true");
            parser->addArgument("--flag2", Argument::Type::BOOL);

            std::vector<std::string> args = {"--flag1", "--flag2", "true"};
            parser->parseArgs(args);

            EXPECT_TRUE(parser->get<bool>("flag1")); // store_true action
            EXPECT_TRUE(parser->get<bool>("flag2")); // explicit true value

            // Test false values
            parser.reset(new ArgumentParser());
            parser->addArgument("--flag", Argument::Type::BOOL);

            std::vector<std::string> args2 = {"--flag", "false"};
            parser->parseArgs(args2);
            EXPECT_FALSE(parser->get<bool>("flag"));
        }

        // Test complex required scenario
        TEST_F(ArgparseTest, ComplexRequiredScenario)
        {
            parser->addArgument("input_file").help("Input file").required(true);
            parser->addArgument("--output", Argument::Type::STRING).required(true).help("Output file");
            parser->addArgument("--verbose", Argument::Type::BOOL).action("store_true").help("Verbose mode");
            parser->addArgument("--threads", Argument::Type::INT).defaultValue("4").help("Number of threads");
            parser->addArgument("--format", Argument::Type::STRING).choices({"json", "xml", "csv"}).defaultValue("json");

            // Missing required --output
            std::vector<std::string> args1 = {"input.txt", "--verbose"};
            EXPECT_THROW(parser->parseArgs(args1), ArgumentError);

            // Missing required input_file
            std::vector<std::string> args2 = {"--output", "output.txt", "--verbose"};
            EXPECT_THROW(parser->parseArgs(args2), ArgumentError);

            // All required arguments provided
            std::vector<std::string> args3 = {
                "input.txt",
                "--output", "output.txt",
                "--verbose",
                "--threads", "8",
                "--format", "xml"};

            EXPECT_NO_THROW(parser->parseArgs(args3));
            EXPECT_EQ(parser->get<std::string>("input_file"), "input.txt");
            EXPECT_EQ(parser->get<std::string>("output"), "output.txt");
            EXPECT_TRUE(parser->get<bool>("verbose"));
            EXPECT_EQ(parser->get<int>("threads"), 8);
            EXPECT_EQ(parser->get<std::string>("format"), "xml");
        }

    } // namespace argparse
} // namespace lumos

// Main function for GoogleTest
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}