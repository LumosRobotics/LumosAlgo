#include "lumos/test_reader/text_reader.impl.h"
#include <iostream>

int main()
{
    try
    {
        // Test 1: ReadFile function
        std::cout << "=== Test 1: ReadFile ===\n";
        const std::string content = ReadFile("/Users/danielpi/work/LumosAlgo/src/json/test/data/test_file.json");

        std::cout << "File content read successfully:" << std::endl;
        std::cout << "Content length: " << content.length() << " characters" << std::endl;
        std::cout << "First 100 characters:" << std::endl;
        std::cout << content.substr(0, 100) << std::endl;

        // Test 2: WriteToFile with string
        std::cout << "\n=== Test 2: WriteToFile (string) ===\n";
        const std::string test_text = "Hello, World!\nThis is line 2\nLine 3 with spaces   \n\nLine 5 (empty line above)";
        WriteToFile(test_text, "output_string.txt");
        std::cout << "Successfully wrote string to 'output_string.txt'" << std::endl;

        // Verify by reading it back
        const std::string read_back = ReadFile("output_string.txt");
        std::cout << "Read back content (length=" << read_back.length() << "):\n";
        std::cout << read_back << std::endl;

        // Test 3: WriteToFile with vector of strings
        std::cout << "\n=== Test 3: WriteToFile (vector<string>) ===\n";
        const std::vector<std::string> lines = {
            "Line 1",
            "Line 2 with spaces   ",
            "", // Empty line
            "Line 4 (empty line above)",
            "Final line"};
        WriteToFile(lines, "output_lines.txt");
        std::cout << "Successfully wrote " << lines.size() << " lines to 'output_lines.txt'" << std::endl;

        // Verify by reading it back
        const std::string read_lines = ReadFile("output_lines.txt");
        std::cout << "Read back content (length=" << read_lines.length() << "):\n";
        std::cout << read_lines << std::endl;

        // Test 4: Overwrite existing file
        std::cout << "\n=== Test 4: Overwrite existing file ===\n";
        WriteToFile("This overwrites the previous content", "output_string.txt");
        const std::string overwritten = ReadFile("output_string.txt");
        std::cout << "Overwritten content: " << overwritten << std::endl;

        // Test 5: Error handling for read
        std::cout << "\n=== Test 5: Error handling ===\n";
        try
        {
            const std::string invalid_content = ReadFile("/non/existent/file.txt");
        }
        catch (const std::runtime_error &e)
        {
            std::cout << "Expected read error caught: " << e.what() << std::endl;
        }

        // Test 6: Error handling for write (invalid path)
        try
        {
            WriteToFile("test", "/invalid/path/that/does/not/exist/file.txt");
        }
        catch (const std::runtime_error &e)
        {
            std::cout << "Expected write error caught: " << e.what() << std::endl;
        }

        std::cout << "\n=== All tests completed successfully! ===\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}