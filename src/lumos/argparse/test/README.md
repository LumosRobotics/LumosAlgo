# Argparse Tests

This directory contains GoogleTest-based unit tests for the LumosAlgo argument parser.

## Building and Running Tests

The argparse tests are integrated into the main CMake build system.

### Using CMake (Recommended)

```bash
# From project root
cd build
cmake ..
make argparse_test

# Run the test executable directly
./src/argparse/test/argparse_test

# Or run through CTest
ctest -R argparse_test -V

# Run all tests including argparse
ctest
```

### Build Specific Target

```bash
# Build only the argparse test
make argparse_test

# Run only argparse tests through CTest
ctest -R argparse_test

# Run argparse tests with custom target
make run_argparse_tests
```

## Test Coverage

The test suite covers:

### Core Functionality
- ✅ Positional arguments
- ✅ Optional arguments (--long and -short forms)
- ✅ Different argument types (string, int, double, bool, string_list)
- ✅ Default values
- ✅ Required arguments validation
- ✅ Choice validation
- ✅ Help generation

### Advanced Features
- ✅ Mixed positional and optional arguments
- ✅ Boolean store_true actions
- ✅ Type conversion capabilities
- ✅ Error handling and validation
- ✅ Complex scenarios with multiple argument types

### Error Cases
- ✅ Invalid type conversions
- ✅ Unknown arguments
- ✅ Missing required arguments
- ✅ Invalid choices
- ✅ Missing values for arguments

## Test Structure

The tests use a GoogleTest fixture `ArgparseTest` that:
- Sets up a fresh `ArgumentParser` for each test
- Provides helper methods for common test patterns
- Ensures proper cleanup between tests

## Integration with CI/CD

Since the tests are integrated with CMake and CTest, they can be easily integrated into continuous integration pipelines:

```bash
# In CI scripts
cmake --build build --target argparse_test
ctest --test-dir build -R argparse_test --output-on-failure
```

## Known Issues

Some tests may currently fail due to implementation details being refined. The core required() functionality is implemented and working as designed.