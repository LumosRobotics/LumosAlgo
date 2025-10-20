# Filters Tests

This directory contains comprehensive unit tests for the filters module of the LumosAlgo library.

## Test Coverage

The test suite covers both FIR (Finite Impulse Response) and IIR (Infinite Impulse Response) filters with the following functionality:

### FIR Filter Tests

#### Basic Operations
- **Constructors**: Default, vector, initializer list, array, copy, and move constructors
- **Assignment Operators**: Copy and move assignment operators
- **Element Access**: Coefficient access and modification
- **Memory Management**: Proper resource allocation and deallocation

#### Core Filtering Functionality
- **Single Sample Filtering**: Process individual samples
- **Vector Filtering**: Process entire vectors of samples
- **Batch Filtering**: Process arrays with pre-allocated output buffers
- **Impulse Response**: Testing with impulse and step inputs
- **Moving Average**: Specialized moving average filter behavior

#### Filter State Management
- **Reset Filter**: Clear internal delay line
- **Initial Conditions**: Set initial filter state
- **State Persistence**: Verify state is maintained between calls

#### Filter Configuration
- **Set Coefficients**: Dynamic coefficient modification
- **Filter Properties**: Order, number of coefficients, empty state
- **Coefficient Validation**: Proper error handling for invalid inputs

#### Advanced Features
- **Frequency Response**: Complex frequency response calculation
- **Group Delay**: Linear phase filter delay characteristics
- **Type Support**: Double and float precision support

#### Factory Methods
- **Moving Average**: Window-based averaging filters
- **Low-Pass Filters**: Frequency-domain low-pass filtering
- **High-Pass Filters**: Frequency-domain high-pass filtering
- **Band-Pass Filters**: Frequency-domain band-pass filtering
- **Differentiator**: Discrete-time differentiation
- **Integrator**: Discrete-time integration
- **Error Handling**: Proper validation of factory method parameters

### IIR Filter Tests

#### Basic Operations
- **Constructors**: Default, vector, initializer list, array, copy, and move constructors
- **Assignment Operators**: Copy and move assignment operators
- **Coefficient Access**: Separate numerator and denominator coefficient access
- **Order Properties**: Numerator, denominator, and overall filter order

#### Core Filtering Functionality
- **Single Sample Filtering**: Process individual samples with feedback
- **Vector Filtering**: Process entire vectors of samples
- **Batch Filtering**: Process arrays with pre-allocated output buffers
- **Feedback Behavior**: Verify proper IIR filter feedback operation
- **Stability**: Testing for stable filter behavior

#### Filter State Management
- **Reset Filter**: Clear internal input and output delay lines
- **Initial Conditions**: Set initial input and output filter states
- **State Persistence**: Verify state is maintained between calls

#### Filter Configuration
- **Set Coefficients**: Dynamic coefficient modification for both numerator and denominator
- **Coefficient Validation**: Proper error handling for invalid denominators
- **Filter Properties**: Order calculations and empty state detection

#### Advanced Features
- **Frequency Response**: Complex frequency response calculation with poles and zeros
- **Stability Analysis**: Pole-zero analysis for filter stability
- **Type Support**: Double and float precision support

#### Factory Methods
- **First-Order Filters**: Low-pass and high-pass single-pole filters
- **Second-Order Filters**: Low-pass, high-pass, band-pass, and notch filters
- **Specialized Filters**: Integrator, differentiator, and DC blocker
- **Parameter Validation**: Proper validation of design parameters

### Integration and Edge Case Tests

#### Comprehensive Integration Tests
- **Moving Average Integration**: End-to-end moving average filter testing
- **Integrator Integration**: End-to-end integrator filter testing
- **Real-World Signals**: Testing with realistic input signals

#### Edge Cases and Error Handling
- **Empty Filters**: Behavior with uninitialized filters
- **Invalid Parameters**: Proper error handling for invalid inputs
- **Boundary Conditions**: Testing at parameter limits
- **Precision Tests**: Testing with very small and very large values

#### Performance and Precision Tests
- **Numerical Precision**: Testing with extreme coefficient values
- **Filter Stability**: Long-term stability testing
- **Convergence Tests**: Verification of filter convergence properties

## Test Statistics

- **Total Tests**: 80+ individual test cases
- **FIR Filter Tests**: 40+ test cases covering all major functionality
- **IIR Filter Tests**: 40+ test cases covering all major functionality
- **Code Coverage**: Comprehensive coverage of all public methods and edge cases
- **Error Handling**: Extensive testing of error conditions and invalid inputs

## Building and Running Tests

To run the tests, use the following commands from the build directory:

```bash
# Build the tests
make filters_test

# Run the tests
./src/math/filters/test/filters_test

# Or run through CTest
ctest -R FiltersTest
```

## Test Design Principles

1. **Comprehensive Coverage**: All public methods and operators are tested
2. **Edge Case Handling**: Tests include boundary conditions and error cases
3. **Mathematical Accuracy**: Verification of filter mathematical properties
4. **Performance Testing**: Tests for numerical precision and stability
5. **Integration Testing**: End-to-end testing with realistic scenarios
6. **Error Validation**: Proper testing of exception handling and error conditions

The test suite ensures that both FIR and IIR filters work correctly across all supported data types and use cases, providing confidence in the reliability and accuracy of the filter implementations.
