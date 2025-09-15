#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "string/string.impl.h"

namespace lumos {

// Test fixture for String functions tests
class StringTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test strings
        empty_str = "";
        simple_str = "hello";
        long_str = "The quick brown fox jumps over the lazy dog";
        repeated_str = "hello hello hello";
        special_chars_str = "Hello, World! @#$%^&*()";
        delimiter_str = "apple,banana,cherry";
        multi_delimiter_str = "one::two::three::four";
        no_delimiter_str = "singleword";
        
        // Test substrings
        simple_substring = "ell";
        missing_substring = "xyz";
        empty_substring = "";
        full_match = "hello";
        
        // Replacement strings
        old_substr = "hello";
        new_substr = "hi";
        multi_old = "::";
        multi_new = " - ";
        
        // Delimiters
        comma_delimiter = ",";
        double_colon_delimiter = "::";
        space_delimiter = " ";
        missing_delimiter = "xyz";
    }
    
    std::string empty_str, simple_str, long_str, repeated_str, special_chars_str;
    std::string delimiter_str, multi_delimiter_str, no_delimiter_str;
    std::string simple_substring, missing_substring, empty_substring, full_match;
    std::string old_substr, new_substr, multi_old, multi_new;
    std::string comma_delimiter, double_colon_delimiter, space_delimiter, missing_delimiter;
};

// =============================================================================
// CONTAINS FUNCTION TESTS
// =============================================================================

TEST_F(StringTest, ContainsBasic) {
    EXPECT_TRUE(contains(simple_str, simple_substring));
    EXPECT_FALSE(contains(simple_str, missing_substring));
    EXPECT_TRUE(contains(simple_str, full_match));
}

TEST_F(StringTest, ContainsEmptyString) {
    EXPECT_TRUE(contains(simple_str, empty_substring));
    EXPECT_TRUE(contains(empty_str, empty_substring));
}

TEST_F(StringTest, ContainsEmptyHaystack) {
    EXPECT_FALSE(contains(empty_str, simple_substring));
    EXPECT_TRUE(contains(empty_str, empty_substring));
}

TEST_F(StringTest, ContainsLongString) {
    EXPECT_TRUE(contains(long_str, "quick"));
    EXPECT_TRUE(contains(long_str, "fox"));
    EXPECT_TRUE(contains(long_str, "dog"));
    EXPECT_FALSE(contains(long_str, "cat"));
}

TEST_F(StringTest, ContainsCaseSensitive) {
    EXPECT_TRUE(contains(special_chars_str, "Hello"));
    EXPECT_FALSE(contains(special_chars_str, "hello"));
    EXPECT_TRUE(contains(special_chars_str, "World!"));
    EXPECT_TRUE(contains(special_chars_str, "@#$"));
}

TEST_F(StringTest, ContainsMultipleOccurrences) {
    EXPECT_TRUE(contains(repeated_str, "hello"));
    EXPECT_TRUE(contains(repeated_str, " "));
}

// =============================================================================
// REPLACE FUNCTION TESTS
// =============================================================================

TEST_F(StringTest, ReplaceBasic) {
    std::string result = replace(simple_str, old_substr, new_substr);
    EXPECT_EQ(result, "hi");
}

TEST_F(StringTest, ReplaceNotFound) {
    std::string result = replace(simple_str, missing_substring, new_substr);
    EXPECT_EQ(result, simple_str);
}

TEST_F(StringTest, ReplaceEmptyOld) {
    std::string result = replace(simple_str, empty_substring, new_substr);
    EXPECT_EQ(result, simple_str);
}

TEST_F(StringTest, ReplaceEmptyNew) {
    std::string result = replace(simple_str, old_substr, empty_substring);
    EXPECT_EQ(result, "");
}

TEST_F(StringTest, ReplaceEmptyString) {
    std::string result = replace(empty_str, old_substr, new_substr);
    EXPECT_EQ(result, empty_str);
}

TEST_F(StringTest, ReplaceMultipleOccurrences) {
    std::string result = replace(repeated_str, old_substr, new_substr);
    EXPECT_EQ(result, "hi hi hi");
}

TEST_F(StringTest, ReplacePartialMatch) {
    std::string result = replace(simple_str, "ell", "ELL");
    EXPECT_EQ(result, "hELLo");
}

TEST_F(StringTest, ReplaceLongerReplacement) {
    std::string input = "cat";
    std::string result = replace(input, "cat", "elephant");
    EXPECT_EQ(result, "elephant");
}

TEST_F(StringTest, ReplaceShorterReplacement) {
    std::string input = "elephant";
    std::string result = replace(input, "elephant", "cat");
    EXPECT_EQ(result, "cat");
}

TEST_F(StringTest, ReplaceOverlappingPattern) {
    std::string input = "aaaa";
    std::string result = replace(input, "aa", "b");
    EXPECT_EQ(result, "bb");
}

TEST_F(StringTest, ReplaceWithDelimiters) {
    std::string result = replace(multi_delimiter_str, multi_old, multi_new);
    EXPECT_EQ(result, "one - two - three - four");
}

// =============================================================================
// SPLIT FUNCTION TESTS
// =============================================================================

TEST_F(StringTest, SplitBasic) {
    std::vector<std::string> result = split(delimiter_str, comma_delimiter);
    ASSERT_EQ(result.size(), 3U);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "banana");
    EXPECT_EQ(result[2], "cherry");
}

TEST_F(StringTest, SplitEmptyDelimiter) {
    std::vector<std::string> result = split(simple_str, empty_substring);
    ASSERT_EQ(result.size(), 1U);
    EXPECT_EQ(result[0], simple_str);
}

TEST_F(StringTest, SplitEmptyString) {
    std::vector<std::string> result = split(empty_str, comma_delimiter);
    ASSERT_EQ(result.size(), 1U);
    EXPECT_EQ(result[0], empty_str);
}

TEST_F(StringTest, SplitNoDelimiterFound) {
    std::vector<std::string> result = split(no_delimiter_str, comma_delimiter);
    ASSERT_EQ(result.size(), 1U);
    EXPECT_EQ(result[0], no_delimiter_str);
}

TEST_F(StringTest, SplitMultiCharDelimiter) {
    std::vector<std::string> result = split(multi_delimiter_str, double_colon_delimiter);
    ASSERT_EQ(result.size(), 4U);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
    EXPECT_EQ(result[3], "four");
}

TEST_F(StringTest, SplitWithSpaces) {
    std::vector<std::string> result = split(long_str, space_delimiter);
    ASSERT_EQ(result.size(), 9U);
    EXPECT_EQ(result[0], "The");
    EXPECT_EQ(result[1], "quick");
    EXPECT_EQ(result[2], "brown");
    EXPECT_EQ(result[8], "dog");
}

TEST_F(StringTest, SplitStartsWithDelimiter) {
    std::string input = ",apple,banana";
    std::vector<std::string> result = split(input, comma_delimiter);
    ASSERT_EQ(result.size(), 3U);
    EXPECT_EQ(result[0], "");
    EXPECT_EQ(result[1], "apple");
    EXPECT_EQ(result[2], "banana");
}

TEST_F(StringTest, SplitEndsWithDelimiter) {
    std::string input = "apple,banana,";
    std::vector<std::string> result = split(input, comma_delimiter);
    ASSERT_EQ(result.size(), 3U);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "banana");
    EXPECT_EQ(result[2], "");
}

TEST_F(StringTest, SplitConsecutiveDelimiters) {
    std::string input = "apple,,banana";
    std::vector<std::string> result = split(input, comma_delimiter);
    ASSERT_EQ(result.size(), 3U);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "");
    EXPECT_EQ(result[2], "banana");
}

TEST_F(StringTest, SplitOnlyDelimiters) {
    std::string input = ":::";
    std::vector<std::string> result = split(input, double_colon_delimiter);
    ASSERT_EQ(result.size(), 2U);
    EXPECT_EQ(result[0], "");
    EXPECT_EQ(result[1], ":");
}

TEST_F(StringTest, SplitSingleCharacter) {
    std::string input = "a";
    std::vector<std::string> result = split(input, comma_delimiter);
    ASSERT_EQ(result.size(), 1U);
    EXPECT_EQ(result[0], "a");
}

// =============================================================================
// EDGE CASES AND ERROR HANDLING
// =============================================================================

TEST_F(StringTest, ContainsLargerSubstring) {
    EXPECT_FALSE(contains(simple_str, long_str));
}

TEST_F(StringTest, ReplaceSelfMatch) {
    std::string result = replace(simple_str, simple_str, new_substr);
    EXPECT_EQ(result, new_substr);
}

TEST_F(StringTest, SplitDelimiterLargerThanString) {
    std::vector<std::string> result = split(simple_str, long_str);
    ASSERT_EQ(result.size(), 1U);
    EXPECT_EQ(result[0], simple_str);
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

TEST_F(StringTest, CombinedOperations) {
    // First replace, then split
    std::string input = "hello-world-test";
    std::string replaced = replace(input, "-", ",");
    std::vector<std::string> parts = split(replaced, ",");
    
    ASSERT_EQ(parts.size(), 3U);
    EXPECT_EQ(parts[0], "hello");
    EXPECT_EQ(parts[1], "world");
    EXPECT_EQ(parts[2], "test");
}

TEST_F(StringTest, ChainedReplacements) {
    std::string input = "The quick brown fox";
    std::string result = replace(input, "quick", "slow");
    result = replace(result, "brown", "black");
    result = replace(result, "fox", "cat");
    
    EXPECT_EQ(result, "The slow black cat");
}

TEST_F(StringTest, SplitAndContains) {
    std::vector<std::string> parts = split(delimiter_str, comma_delimiter);
    
    EXPECT_TRUE(contains(parts[0], "apple"));
    EXPECT_TRUE(contains(parts[1], "banana"));
    EXPECT_TRUE(contains(parts[2], "cherry"));
    EXPECT_FALSE(contains(parts[0], "orange"));
}

// =============================================================================
// PERFORMANCE AND PRECISION TESTS
// =============================================================================

TEST_F(StringTest, LargeStringOperations) {
    // Create a large string
    std::string large_str;
    for (int i = 0; i < 1000; ++i) {
        large_str += "test ";
    }
    
    // Test contains
    EXPECT_TRUE(contains(large_str, "test"));
    
    // Test replace
    std::string replaced = replace(large_str, "test", "demo");
    EXPECT_TRUE(contains(replaced, "demo"));
    EXPECT_FALSE(contains(replaced, "test"));
    
    // Test split
    std::vector<std::string> parts = split(large_str, " ");
    EXPECT_GT(parts.size(), 1000U);
}

TEST_F(StringTest, UnicodeHandling) {
    std::string unicode_str = "Hello 世界 🌍";
    
    EXPECT_TRUE(contains(unicode_str, "世界"));
    EXPECT_TRUE(contains(unicode_str, "🌍"));
    
    std::string replaced = replace(unicode_str, "世界", "World");
    EXPECT_EQ(replaced, "Hello World 🌍");
    
    std::vector<std::string> parts = split(unicode_str, " ");
    ASSERT_EQ(parts.size(), 3U);
    EXPECT_EQ(parts[1], "世界");
}

// =============================================================================
// BASIC STRING OPERATIONS TESTS
// =============================================================================

TEST_F(StringTest, TrimFunctions) {
    std::string whitespace_str = "  \t\n hello world \r\f\v  ";
    
    EXPECT_EQ(trim(whitespace_str), "hello world");
    EXPECT_EQ(ltrim(whitespace_str), "hello world \r\f\v  ");
    EXPECT_EQ(rtrim(whitespace_str), "  \t\n hello world");
    
    EXPECT_EQ(trim(""), "");
    EXPECT_EQ(trim("   "), "");
    EXPECT_EQ(trim("hello"), "hello");
}

TEST_F(StringTest, StartsWithEndsWith) {
    EXPECT_TRUE(startsWith("hello world", "hello"));
    EXPECT_TRUE(startsWith("hello world", ""));
    EXPECT_FALSE(startsWith("hello world", "world"));
    EXPECT_FALSE(startsWith("hello", "hello world"));
    
    EXPECT_TRUE(endsWith("hello world", "world"));
    EXPECT_TRUE(endsWith("hello world", ""));
    EXPECT_FALSE(endsWith("hello world", "hello"));
    EXPECT_FALSE(endsWith("world", "hello world"));
}

TEST_F(StringTest, CaseConversion) {
    EXPECT_EQ(toLowerCase("HELLO World"), "hello world");
    EXPECT_EQ(toLowerCase(""), "");
    EXPECT_EQ(toLowerCase("123ABC"), "123abc");
    
    EXPECT_EQ(toUpperCase("hello World"), "HELLO WORLD");
    EXPECT_EQ(toUpperCase(""), "");
    EXPECT_EQ(toUpperCase("123abc"), "123ABC");
}

TEST_F(StringTest, ReverseString) {
    EXPECT_EQ(reverse("hello"), "olleh");
    EXPECT_EQ(reverse(""), "");
    EXPECT_EQ(reverse("a"), "a");
    EXPECT_EQ(reverse("12345"), "54321");
}

// =============================================================================
// STRING JOINING/BUILDING TESTS
// =============================================================================

TEST_F(StringTest, JoinFunction) {
    std::vector<std::string> parts = {"apple", "banana", "cherry"};
    EXPECT_EQ(join(parts, ","), "apple,banana,cherry");
    EXPECT_EQ(join(parts, " - "), "apple - banana - cherry");
    
    std::vector<std::string> empty_vec;
    EXPECT_EQ(join(empty_vec, ","), "");
    
    std::vector<std::string> single = {"alone"};
    EXPECT_EQ(join(single, ","), "alone");
}

TEST_F(StringTest, VariadicJoinFunction) {
    // Test basic string joining
    EXPECT_EQ(join(",", "apple", "banana", "cherry"), "apple,banana,cherry");
    EXPECT_EQ(join(" - ", "hello", "world"), "hello - world");
    
    // Test with single argument
    EXPECT_EQ(join(",", "single"), "single");
    
    // Test with different delimiters
    EXPECT_EQ(join("::", "a", "b", "c"), "a::b::c");
    EXPECT_EQ(join("", "no", "spaces"), "nospaces");
    
    // Test with const char*
    const char* c_str = "c_string";
    EXPECT_EQ(join(",", "start", c_str, "end"), "start,c_string,end");
    
    // Test with std::string variables
    std::string s1 = "first";
    std::string s2 = "second";
    std::string s3 = "third";
    EXPECT_EQ(join("-", s1, s2, s3), "first-second-third");
    
    // Test mixing std::string and const char*
    EXPECT_EQ(join("|", s1, "literal", s2), "first|literal|second");
    
    // Test empty arguments (compile time check)
    EXPECT_EQ(join(","), "");
    
    // Test many arguments
    EXPECT_EQ(join(",", "1", "2", "3", "4", "5", "6", "7"), "1,2,3,4,5,6,7");
}

TEST_F(StringTest, RepeatFunction) {
    EXPECT_EQ(repeat("hello", 3), "hellohellohello");
    EXPECT_EQ(repeat("a", 5), "aaaaa");
    EXPECT_EQ(repeat("test", 0), "");
    EXPECT_EQ(repeat("", 5), "");
}

TEST_F(StringTest, PadFunctions) {
    EXPECT_EQ(pad("hello", 9), "  hello  ");
    EXPECT_EQ(pad("hello", 10, '*'), "**hello***");
    EXPECT_EQ(pad("hello", 3), "hello"); // No padding needed
    
    EXPECT_EQ(padLeft("hello", 8), "   hello");
    EXPECT_EQ(padLeft("hello", 8, '0'), "000hello");
    
    EXPECT_EQ(padRight("hello", 8), "hello   ");
    EXPECT_EQ(padRight("hello", 8, '-'), "hello---");
}

// =============================================================================
// ADVANCED STRING PROCESSING TESTS
// =============================================================================

TEST_F(StringTest, StripFunction) {
    EXPECT_EQ(strip("hello world", "l"), "heo word");
    EXPECT_EQ(strip("hello world", "lo "), "hewrd");
    EXPECT_EQ(strip("hello", "xyz"), "hello");
    EXPECT_EQ(strip("", "abc"), "");
}

TEST_F(StringTest, ReplaceFirstLast) {
    std::string test_str = "hello hello hello";
    
    EXPECT_EQ(replaceFirst(test_str, "hello", "hi"), "hi hello hello");
    EXPECT_EQ(replaceLast(test_str, "hello", "hi"), "hello hello hi");
    
    EXPECT_EQ(replaceFirst("hello", "xyz", "abc"), "hello");
    EXPECT_EQ(replaceLast("hello", "xyz", "abc"), "hello");
    
    EXPECT_EQ(replaceFirst("hello", "", "abc"), "hello");
    EXPECT_EQ(replaceLast("hello", "", "abc"), "hello");
}

TEST_F(StringTest, FindFunctions) {
    std::string test_str = "hello world hello";
    
    EXPECT_EQ(find(test_str, "hello"), 0U);
    EXPECT_EQ(find(test_str, "hello", 1), 12U);
    EXPECT_EQ(find(test_str, "xyz"), std::string::npos);
    
    EXPECT_EQ(findLast(test_str, "hello"), 12U);
    EXPECT_EQ(findLast(test_str, "world"), 6U);
    EXPECT_EQ(findLast(test_str, "xyz"), std::string::npos);
}

TEST_F(StringTest, CountFunction) {
    EXPECT_EQ(count("hello hello hello", "hello"), 3U);
    EXPECT_EQ(count("hello hello hello", "ll"), 3U);
    EXPECT_EQ(count("hello", "xyz"), 0U);
    EXPECT_EQ(count("", "hello"), 0U);
    EXPECT_EQ(count("hello", ""), 0U);
    
    EXPECT_EQ(count("aaaa", "aa"), 2U); // Non-overlapping count
}

// =============================================================================
// STRING VALIDATION/CLASSIFICATION TESTS
// =============================================================================

TEST_F(StringTest, IsNumeric) {
    EXPECT_TRUE(isNumeric("123"));
    EXPECT_TRUE(isNumeric("123.45"));
    EXPECT_TRUE(isNumeric("+123"));
    EXPECT_TRUE(isNumeric("-123.45"));
    EXPECT_TRUE(isNumeric("0"));
    EXPECT_TRUE(isNumeric("0.0"));
    
    EXPECT_FALSE(isNumeric(""));
    EXPECT_FALSE(isNumeric("abc"));
    EXPECT_FALSE(isNumeric("12.34.56"));
    EXPECT_FALSE(isNumeric("12a34"));
    EXPECT_FALSE(isNumeric("+"));
    EXPECT_FALSE(isNumeric("-"));
}

TEST_F(StringTest, IsAlpha) {
    EXPECT_TRUE(isAlpha("hello"));
    EXPECT_TRUE(isAlpha("HELLO"));
    EXPECT_TRUE(isAlpha("HeLLo"));
    
    EXPECT_FALSE(isAlpha(""));
    EXPECT_FALSE(isAlpha("hello123"));
    EXPECT_FALSE(isAlpha("hello world"));
    EXPECT_FALSE(isAlpha("123"));
}

TEST_F(StringTest, IsAlphaNumeric) {
    EXPECT_TRUE(isAlphaNumeric("hello123"));
    EXPECT_TRUE(isAlphaNumeric("HELLO"));
    EXPECT_TRUE(isAlphaNumeric("123"));
    EXPECT_TRUE(isAlphaNumeric("abc123XYZ"));
    
    EXPECT_FALSE(isAlphaNumeric(""));
    EXPECT_FALSE(isAlphaNumeric("hello world"));
    EXPECT_FALSE(isAlphaNumeric("hello!"));
    EXPECT_FALSE(isAlphaNumeric("123.45"));
}

TEST_F(StringTest, IsEmptyBlank) {
    EXPECT_TRUE(isEmpty(""));
    EXPECT_FALSE(isEmpty(" "));
    EXPECT_FALSE(isEmpty("hello"));
    
    EXPECT_TRUE(isBlank(""));
    EXPECT_TRUE(isBlank(" "));
    EXPECT_TRUE(isBlank("   \t\n\r"));
    EXPECT_FALSE(isBlank("hello"));
    EXPECT_FALSE(isBlank(" hello "));
}

TEST_F(StringTest, IsValidEmail) {
    EXPECT_TRUE(isValidEmail("user@example.com"));
    EXPECT_TRUE(isValidEmail("test.email@domain.org"));
    EXPECT_TRUE(isValidEmail("user+tag@example.co.uk"));
    
    EXPECT_FALSE(isValidEmail(""));
    EXPECT_FALSE(isValidEmail("invalid"));
    EXPECT_FALSE(isValidEmail("@example.com"));
    EXPECT_FALSE(isValidEmail("user@"));
    EXPECT_FALSE(isValidEmail("user@.com"));
    EXPECT_FALSE(isValidEmail("user@example"));
}

TEST_F(StringTest, IsValidUrl) {
    EXPECT_TRUE(isValidUrl("http://example.com"));
    EXPECT_TRUE(isValidUrl("https://www.example.com"));
    EXPECT_TRUE(isValidUrl("https://example.com/path/to/page"));
    EXPECT_TRUE(isValidUrl("http://localhost"));
    
    EXPECT_FALSE(isValidUrl(""));
    EXPECT_FALSE(isValidUrl("example.com"));
    EXPECT_FALSE(isValidUrl("ftp://example.com"));
    EXPECT_FALSE(isValidUrl("http://"));
    EXPECT_FALSE(isValidUrl("https://"));
}

// =============================================================================
// STRING FORMATTING TESTS
// =============================================================================

TEST_F(StringTest, EscapeUnescape) {
    std::string input = "Hello\nWorld\t!";
    std::string escaped = escape(input);
    EXPECT_EQ(escaped, "Hello\\nWorld\\t!");
    
    std::string unescaped = unescape(escaped);
    EXPECT_EQ(unescaped, input);
    
    EXPECT_EQ(escape("\"Hello\""), "\\\"Hello\\\"");
    EXPECT_EQ(escape("C:\\Path\\File"), "C:\\\\Path\\\\File");
    
    EXPECT_EQ(unescape("Hello\\nWorld"), "Hello\nWorld");
    EXPECT_EQ(unescape("\\\"Hello\\\""), "\"Hello\"");
}

TEST_F(StringTest, ToTitleCase) {
    EXPECT_EQ(toTitleCase("hello world"), "Hello World");
    EXPECT_EQ(toTitleCase("HELLO WORLD"), "Hello World");
    EXPECT_EQ(toTitleCase("hELLo WoRLd"), "Hello World");
    EXPECT_EQ(toTitleCase("hello-world_test"), "Hello-World_Test");
    EXPECT_EQ(toTitleCase(""), "");
    EXPECT_EQ(toTitleCase("a"), "A");
}

// =============================================================================
// INTEGRATION TESTS FOR NEW FUNCTIONS
// =============================================================================

TEST_F(StringTest, ComplexChaining) {
    std::string input = "  HELLO, WORLD!  ";
    std::string result = trim(toLowerCase(input));
    EXPECT_EQ(result, "hello, world!");
    
    std::vector<std::string> words = split(result, ", ");
    words = {toTitleCase(words[0]), toUpperCase(words[1])};
    std::string final = join(words, " & ");
    EXPECT_EQ(final, "Hello & WORLD!");
}

TEST_F(StringTest, ValidationChain) {
    std::vector<std::string> inputs = {"123", "abc", "123abc", "test@email.com"};
    
    for (const auto& input : inputs) {
        bool numeric = isNumeric(input);
        bool alpha = isAlpha(input);
        bool alphanum = isAlphaNumeric(input);
        bool email = isValidEmail(input);
        
        // Logical consistency checks
        if (numeric) {
            EXPECT_TRUE(alphanum);
        }
        if (alpha) {
            EXPECT_TRUE(alphanum);
        }
        if (email) {
            EXPECT_FALSE(numeric);
            EXPECT_FALSE(isBlank(input));
        }
    }
}

TEST_F(StringTest, PadAndTrim) {
    std::string test = "hello";
    std::string padded = padLeft(test, 10, ' ');
    EXPECT_EQ(trim(padded), test);
    
    padded = pad(test, 11, '*');
    EXPECT_TRUE(startsWith(padded, "*"));
    EXPECT_TRUE(endsWith(padded, "*"));
    EXPECT_TRUE(contains(padded, test));
}

TEST_F(StringTest, ReplaceVariants) {
    std::string test = "foo bar foo baz foo";
    
    std::string all_replaced = replace(test, "foo", "xyz");
    std::string first_replaced = replaceFirst(test, "foo", "xyz");
    std::string last_replaced = replaceLast(test, "foo", "xyz");
    
    EXPECT_EQ(count(all_replaced, "xyz"), 3U);
    EXPECT_EQ(count(first_replaced, "xyz"), 1U);
    EXPECT_EQ(count(last_replaced, "xyz"), 1U);
    
    EXPECT_TRUE(startsWith(first_replaced, "xyz"));
    EXPECT_TRUE(endsWith(last_replaced, "xyz"));
}

} // namespace lumos