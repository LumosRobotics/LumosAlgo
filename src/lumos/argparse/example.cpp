#include "lumos/argparse.h"
#include <iostream>

using namespace lumos::argparse;

int main(int argc, char *argv[])
{
    try
    {
        // Create argument parser with description
        ArgumentParser parser("Example program demonstrating the LumosAlgo argument parser");

        // Add positional arguments
        parser.addArgument("input_file")
            .help("Input file to process");

        parser.addArgument("output_file")
            .help("Output file for results");

        // Add optional arguments with different types
        parser.addArgument("--verbose", Argument::Type::BOOL)
            .action("store_true")
            .help("Enable verbose output");

        parser.addArgument("--count", Argument::Type::INT)
            .defaultValue("10")
            .help("Number of iterations to perform");

        parser.addArgument("--threshold", Argument::Type::DOUBLE)
            .defaultValue("0.5")
            .help("Threshold value for processing");

        parser.addArgument("--mode", Argument::Type::STRING)
            .choices({"fast", "accurate", "balanced"})
            .defaultValue("balanced")
            .help("Processing mode");

        parser.addArgument("--includes", Argument::Type::STRING_LIST)
            .nargs("+")
            .help("Additional files to include in processing");

        parser.addArgument("--config", Argument::Type::STRING)
            .help("Configuration file path");

        parser.addArgument("--parallel", Argument::Type::BOOL)
            .action("store_true")
            .help("Enable parallel processing");

        parser.addArgument("--threads", Argument::Type::INT)
            .defaultValue("4")
            .help("Number of threads for parallel processing");

        // Parse arguments
        parser.parseArgs(argc, argv);

        // Access parsed arguments
        std::cout << "=== Parsed Arguments ===" << std::endl;
        std::cout << "Input file: " << parser.get<std::string>("input_file") << std::endl;
        std::cout << "Output file: " << parser.get<std::string>("output_file") << std::endl;
        std::cout << "Verbose mode: " << (parser.get<bool>("verbose") ? "enabled" : "disabled") << std::endl;
        std::cout << "Count: " << parser.get<int>("count") << std::endl;
        std::cout << "Threshold: " << parser.get<double>("threshold") << std::endl;
        std::cout << "Mode: " << parser.get<std::string>("mode") << std::endl;
        std::cout << "Parallel: " << (parser.get<bool>("parallel") ? "enabled" : "disabled") << std::endl;
        std::cout << "Threads: " << parser.get<int>("threads") << std::endl;

        if (parser.isSet("config"))
        {
            std::cout << "Config file: " << parser.get<std::string>("config") << std::endl;
        }
        else
        {
            std::cout << "Config file: not specified" << std::endl;
        }

        if (parser.isSet("includes"))
        {
            auto includes = parser.get<std::vector<std::string>>("includes");
            std::cout << "Include files (" << includes.size() << "): ";
            for (size_t i = 0; i < includes.size(); ++i)
            {
                if (i > 0)
                    std::cout << ", ";
                std::cout << includes[i];
            }
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Include files: none" << std::endl;
        }

        std::cout << "\n=== Processing ===" << std::endl;

        // Simulate processing based on arguments
        if (parser.get<bool>("verbose"))
        {
            std::cout << "Verbose mode enabled - detailed logging" << std::endl;
        }

        std::cout << "Processing " << parser.get<std::string>("input_file")
                  << " with " << parser.get<int>("count") << " iterations" << std::endl;
        std::cout << "Using " << parser.get<std::string>("mode") << " mode" << std::endl;
        std::cout << "Threshold set to " << parser.get<double>("threshold") << std::endl;

        if (parser.get<bool>("parallel"))
        {
            std::cout << "Using " << parser.get<int>("threads") << " threads for parallel processing" << std::endl;
        }
        else
        {
            std::cout << "Using single-threaded processing" << std::endl;
        }

        std::cout << "Results will be written to " << parser.get<std::string>("output_file") << std::endl;

        std::cout << "\n=== Demonstration of Type Conversions ===" << std::endl;

        // Demonstrate type conversion capabilities
        if (parser.isSet("count"))
        {
            std::cout << "Count as string: '" << parser.get<std::string>("count") << "'" << std::endl;
            std::cout << "Count as int: " << parser.get<int>("count") << std::endl;
            std::cout << "Count as double: " << parser.get<double>("count") << std::endl;
            std::cout << "Count as bool: " << (parser.get<bool>("count") ? "true" : "false") << std::endl;
        }
    }
    catch (const ArgumentTypeError &e)
    {
        std::cerr << "Type error: " << e.what() << std::endl;
        return 1;
    }
    catch (const ArgumentError &e)
    {
        std::cerr << "Argument error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/*
Example usage:

# Basic usage
./example input.txt output.txt

# With optional arguments
./example input.txt output.txt --verbose --count 20 --mode fast

# With parallel processing
./example input.txt output.txt --parallel --threads 8

# With include files
./example input.txt output.txt --includes file1.h file2.cpp file3.h

# With all options
./example input.txt output.txt --verbose --count 50 --threshold 0.8 --mode accurate --parallel --threads 16 --config settings.conf --includes header1.h header2.h utils.cpp

# Show help
./example --help

# Error cases (will show error messages):
./example  # Missing required positional arguments
./example input.txt output.txt --count invalid  # Invalid integer
./example input.txt output.txt --mode invalid   # Invalid choice
*/