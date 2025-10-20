#include <algorithm>
#include <cmath>
#include <complex>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

#include "lumos/math/filters/filters.h"

namespace lumos
{

  // Test fixture for FIR Filter tests
  class FIRFilterTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Common test coefficients
      simple_coeffs = {1.0, 0.5, 0.25};
      moving_avg_coeffs = {0.25, 0.25, 0.25, 0.25};
      low_pass_coeffs = {0.1, 0.2, 0.4, 0.2, 0.1};

      // Test input signals
      impulse_input = {1.0, 0.0, 0.0, 0.0, 0.0};
      step_input = {1.0, 1.0, 1.0, 1.0, 1.0};
      ramp_input = {1.0, 2.0, 3.0, 4.0, 5.0};

      // Filter instances
      simple_filter = FIRFilter<double>(simple_coeffs);
      moving_avg_filter = FIRFilter<double>(moving_avg_coeffs);
    }

    std::vector<double> simple_coeffs, moving_avg_coeffs, low_pass_coeffs;
    std::vector<double> impulse_input, step_input, ramp_input;
    FIRFilter<double> simple_filter, moving_avg_filter;
  };

  // Test fixture for IIR Filter tests
  class IIRFilterTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Common test coefficients
      b_coeffs = {1.0, 0.5};
      a_coeffs = {1.0, -0.5};

      // Test input signals
      impulse_input = {1.0, 0.0, 0.0, 0.0, 0.0};
      step_input = {1.0, 1.0, 1.0, 1.0, 1.0};
      ramp_input = {1.0, 2.0, 3.0, 4.0, 5.0};

      // Filter instances
      simple_filter = IIRFilter<double>(b_coeffs, a_coeffs);
      sample_rate = 1000.0;
      cutoff_freq = 100.0;
      q_factor = 0.707;
    }

    std::vector<double> b_coeffs, a_coeffs;
    std::vector<double> impulse_input, step_input, ramp_input;
    IIRFilter<double> simple_filter;
    double sample_rate, cutoff_freq, q_factor;
  };

  // =============================================================================
  // FIR FILTER TESTS
  // =============================================================================

  // CONSTRUCTOR TESTS

  TEST_F(FIRFilterTest, DefaultConstructor)
  {
    FIRFilter<double> filter;

    EXPECT_EQ(filter.getOrder(), 0U);
    EXPECT_EQ(filter.getNumCoefficients(), 0U);
    EXPECT_TRUE(filter.isEmpty());
    EXPECT_DOUBLE_EQ(filter.filter(1.0), 0.0);
  }

  TEST_F(FIRFilterTest, VectorConstructor)
  {
    FIRFilter<double> filter(simple_coeffs);

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);
    EXPECT_FALSE(filter.isEmpty());

    const auto &coeffs = filter.getCoefficients();
    EXPECT_EQ(coeffs.size(), 3U);
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, InitializerListConstructor)
  {
    FIRFilter<double> filter({1.0, 0.5, 0.25});

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);

    const auto &coeffs = filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, ArrayConstructor)
  {
    double coeffs_array[] = {1.0, 0.5, 0.25};
    FIRFilter<double> filter(2, coeffs_array);

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);

    const auto &coeffs = filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, CopyConstructor)
  {
    FIRFilter<double> filter_copy(simple_filter);

    EXPECT_EQ(filter_copy.getOrder(), simple_filter.getOrder());
    EXPECT_EQ(filter_copy.getNumCoefficients(),
              simple_filter.getNumCoefficients());

    const auto &orig_coeffs = simple_filter.getCoefficients();
    const auto &copy_coeffs = filter_copy.getCoefficients();

    EXPECT_EQ(orig_coeffs.size(), copy_coeffs.size());
    for (size_t i = 0; i < orig_coeffs.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_coeffs[i], copy_coeffs[i]);
    }
  }

  TEST_F(FIRFilterTest, MoveConstructor)
  {
    FIRFilter<double> filter_original(simple_coeffs);
    FIRFilter<double> filter_moved(std::move(filter_original));

    EXPECT_EQ(filter_moved.getOrder(), 2U);
    EXPECT_EQ(filter_moved.getNumCoefficients(), 3U);

    const auto &coeffs = filter_moved.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, CopyAssignment)
  {
    FIRFilter<double> filter;
    filter = simple_filter;

    EXPECT_EQ(filter.getOrder(), simple_filter.getOrder());
    EXPECT_EQ(filter.getNumCoefficients(), simple_filter.getNumCoefficients());

    const auto &orig_coeffs = simple_filter.getCoefficients();
    const auto &copy_coeffs = filter.getCoefficients();

    for (size_t i = 0; i < orig_coeffs.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_coeffs[i], copy_coeffs[i]);
    }
  }

  TEST_F(FIRFilterTest, MoveAssignment)
  {
    FIRFilter<double> filter_original(simple_coeffs);
    FIRFilter<double> filter;
    filter = std::move(filter_original);

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);

    const auto &coeffs = filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  // SINGLE SAMPLE FILTERING TESTS

  TEST_F(FIRFilterTest, SingleSampleFiltering)
  {
    // Test impulse response
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0); // First coefficient

    double out2 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out2, 0.5); // Second coefficient

    double out3 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out3, 0.25); // Third coefficient

    double out4 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out4, 0.0); // Should be zero now
  }

  TEST_F(FIRFilterTest, MovingAverageFiltering)
  {
    // Test moving average with step input
    double out1 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 0.25);

    double out2 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out2, 0.5);

    double out3 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out3, 0.75);

    double out4 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out4, 1.0);

    double out5 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out5, 1.0); // Should stay at 1.0
  }

  // VECTOR FILTERING TESTS

  TEST_F(FIRFilterTest, VectorFiltering)
  {
    FIRFilter<double> filter({1.0, 0.5});
    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output = filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    EXPECT_DOUBLE_EQ(output[0], 1.0); // 1.0 * 1.0
    EXPECT_DOUBLE_EQ(output[1], 2.5); // 1.0 * 2.0 + 0.5 * 1.0
    EXPECT_DOUBLE_EQ(output[2], 4.0); // 1.0 * 3.0 + 0.5 * 2.0
  }

  TEST_F(FIRFilterTest, BatchFiltering)
  {
    FIRFilter<double> filter({1.0, 0.5});
    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output(3);

    filter.filter(input.data(), output.data(), input.size());

    EXPECT_DOUBLE_EQ(output[0], 1.0);
    EXPECT_DOUBLE_EQ(output[1], 2.5);
    EXPECT_DOUBLE_EQ(output[2], 4.0);
  }

  // FILTER STATE MANAGEMENT TESTS

  TEST_F(FIRFilterTest, ResetFilter)
  {
    // Apply some input to change internal state
    simple_filter.filter(1.0);
    simple_filter.filter(2.0);

    // Reset filter
    simple_filter.reset();

    // Test that filter behaves as if newly constructed
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0);

    double out2 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out2, 0.5);
  }

  TEST_F(FIRFilterTest, SetInitialConditions)
  {
    std::vector<double> initial_state = {1.0, 2.0, 3.0};
    simple_filter.setInitialConditions(initial_state);

    // After setting initial conditions, the first output should be:
    // coeffs[0] * 0.0 + coeffs[1] * initial_state[0] + coeffs[2] *
    // initial_state[1] = 1.0 * 0.0 + 0.5 * 1.0 + 0.25 * 2.0 = 0.0 + 0.5 + 0.5
    // = 1.0
    double output = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(output, 1.0);
  }

  TEST_F(FIRFilterTest, SetInitialConditionsWrongSize)
  {
    std::vector<double> wrong_size_state = {1.0, 2.0}; // Too few elements
    EXPECT_THROW(simple_filter.setInitialConditions(wrong_size_state),
                 std::invalid_argument);
  }

  // FILTER CONFIGURATION TESTS

  TEST_F(FIRFilterTest, SetCoefficients)
  {
    std::vector<double> new_coeffs = {2.0, 1.0, 0.5};
    simple_filter.setCoefficients(new_coeffs);

    EXPECT_EQ(simple_filter.getOrder(), 2U);
    EXPECT_EQ(simple_filter.getNumCoefficients(), 3U);

    const auto &coeffs = simple_filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 2.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.5);
  }

  // FREQUENCY RESPONSE TESTS

  TEST_F(FIRFilterTest, FrequencyResponse)
  {
    FIRFilter<double> filter({1.0, 0.0}); // Simple delay
    double sample_rate = 1000.0;
    double frequency = 100.0;

    std::complex<double> response =
        filter.frequencyResponse(frequency, sample_rate);

    // For a simple delay, magnitude should be 1
    EXPECT_NEAR(std::abs(response), 1.0, 1e-10);
  }

  TEST_F(FIRFilterTest, FrequencyResponseEmptyFilter)
  {
    FIRFilter<double> empty_filter;
    std::complex<double> response = empty_filter.frequencyResponse(100.0, 1000.0);

    EXPECT_DOUBLE_EQ(response.real(), 0.0);
    EXPECT_DOUBLE_EQ(response.imag(), 0.0);
  }

  // GROUP DELAY TESTS

  TEST_F(FIRFilterTest, GroupDelay)
  {
    FIRFilter<double> filter({1.0, 0.0, 0.0, 0.0, 1.0}); // Order 4
    double group_delay = filter.getGroupDelay();
    EXPECT_DOUBLE_EQ(group_delay,
                     2.0); // (4-1)/2 = 1.5, wait that's wrong... (4)/2 = 2
  }

  TEST_F(FIRFilterTest, GroupDelayEmptyFilter)
  {
    FIRFilter<double> empty_filter;
    double group_delay = empty_filter.getGroupDelay();
    EXPECT_DOUBLE_EQ(group_delay, 0.0);
  }

  // FACTORY METHOD TESTS

  TEST_F(FIRFilterTest, MovingAverageFactory)
  {
    FIRFilter<double> ma_filter = FIRFilter<double>::movingAverage(4);

    EXPECT_EQ(ma_filter.getOrder(), 3U);
    EXPECT_EQ(ma_filter.getNumCoefficients(), 4U);

    const auto &coeffs = ma_filter.getCoefficients();
    for (size_t i = 0; i < coeffs.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(coeffs[i], 0.25);
    }
  }

  TEST_F(FIRFilterTest, MovingAverageFactoryZeroSize)
  {
    EXPECT_THROW(FIRFilter<double>::movingAverage(0), std::invalid_argument);
  }

  TEST_F(FIRFilterTest, LowPassFactory)
  {
    FIRFilter<double> lp_filter = FIRFilter<double>::lowPass(10, 100.0, 1000.0);

    EXPECT_EQ(lp_filter.getOrder(), 10U);
    EXPECT_EQ(lp_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(lp_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, LowPassFactoryZeroOrder)
  {
    EXPECT_THROW(FIRFilter<double>::lowPass(0, 100.0, 1000.0),
                 std::invalid_argument);
  }

  TEST_F(FIRFilterTest, HighPassFactory)
  {
    FIRFilter<double> hp_filter = FIRFilter<double>::highPass(10, 100.0, 1000.0);

    EXPECT_EQ(hp_filter.getOrder(), 10U);
    EXPECT_EQ(hp_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(hp_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, BandPassFactory)
  {
    FIRFilter<double> bp_filter =
        FIRFilter<double>::bandPass(10, 50.0, 150.0, 1000.0);

    EXPECT_EQ(bp_filter.getOrder(), 10U);
    EXPECT_EQ(bp_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(bp_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, DifferentiatorFactory)
  {
    FIRFilter<double> diff_filter = FIRFilter<double>::differentiator(10);

    EXPECT_EQ(diff_filter.getOrder(), 10U);
    EXPECT_EQ(diff_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(diff_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, DifferentiatorFactoryZeroOrder)
  {
    EXPECT_THROW(FIRFilter<double>::differentiator(0), std::invalid_argument);
  }

  TEST_F(FIRFilterTest, IntegratorFactory)
  {
    FIRFilter<double> int_filter = FIRFilter<double>::integrator(10);

    EXPECT_EQ(int_filter.getOrder(), 10U);
    EXPECT_EQ(int_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(int_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, IntegratorFactoryZeroOrder)
  {
    EXPECT_THROW(FIRFilter<double>::integrator(0), std::invalid_argument);
  }

  // TYPE ALIASES TESTS

  TEST_F(FIRFilterTest, TypeAliases)
  {
    FIRFilterd filter_d({1.0, 0.5});
    FIRFilterf filter_f({1.0f, 0.5f});

    EXPECT_EQ(filter_d.getOrder(), 1U);
    EXPECT_EQ(filter_f.getOrder(), 1U);

    EXPECT_DOUBLE_EQ(filter_d.filter(1.0), 1.0);
    EXPECT_FLOAT_EQ(filter_f.filter(1.0f), 1.0f);
  }

  // =============================================================================
  // IIR FILTER TESTS
  // =============================================================================

  // CONSTRUCTOR TESTS

  TEST_F(IIRFilterTest, DefaultConstructor)
  {
    IIRFilter<double> filter;

    EXPECT_EQ(filter.getNumeratorOrder(), 0U);
    EXPECT_EQ(filter.getDenominatorOrder(), 0U);
    EXPECT_EQ(filter.getOrder(), 0U);
    EXPECT_TRUE(filter.isEmpty());
    EXPECT_DOUBLE_EQ(filter.filter(1.0), 0.0);
  }

  TEST_F(IIRFilterTest, VectorConstructor)
  {
    IIRFilter<double> filter(b_coeffs, a_coeffs);

    EXPECT_EQ(filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter.getDenominatorOrder(), 1U);
    EXPECT_EQ(filter.getOrder(), 1U);
    EXPECT_FALSE(filter.isEmpty());

    const auto &b = filter.getNumeratorCoefficients();
    const auto &a = filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  TEST_F(IIRFilterTest, InitializerListConstructor)
  {
    IIRFilter<double> filter({1.0, 0.5}, {1.0, -0.5});

    EXPECT_EQ(filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter.getDenominatorOrder(), 1U);

    const auto &b = filter.getNumeratorCoefficients();
    const auto &a = filter.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  TEST_F(IIRFilterTest, ArrayConstructor)
  {
    double b_array[] = {1.0, 0.5};
    double a_array[] = {1.0, -0.5};
    IIRFilter<double> filter(1, b_array, 1, a_array);

    EXPECT_EQ(filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter.getDenominatorOrder(), 1U);

    const auto &b = filter.getNumeratorCoefficients();
    const auto &a = filter.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  TEST_F(IIRFilterTest, ConstructorInvalidDenominator)
  {
    std::vector<double> invalid_a = {0.0, 1.0}; // First coefficient is zero
    EXPECT_THROW(IIRFilter<double>(b_coeffs, invalid_a), std::invalid_argument);
  }

  TEST_F(IIRFilterTest, ConstructorEmptyDenominator)
  {
    std::vector<double> empty_a;
    EXPECT_THROW(IIRFilter<double>(b_coeffs, empty_a), std::invalid_argument);
  }

  TEST_F(IIRFilterTest, CopyConstructor)
  {
    IIRFilter<double> filter_copy(simple_filter);

    EXPECT_EQ(filter_copy.getNumeratorOrder(), simple_filter.getNumeratorOrder());
    EXPECT_EQ(filter_copy.getDenominatorOrder(),
              simple_filter.getDenominatorOrder());

    const auto &orig_b = simple_filter.getNumeratorCoefficients();
    const auto &copy_b = filter_copy.getNumeratorCoefficients();
    const auto &orig_a = simple_filter.getDenominatorCoefficients();
    const auto &copy_a = filter_copy.getDenominatorCoefficients();

    EXPECT_EQ(orig_b.size(), copy_b.size());
    EXPECT_EQ(orig_a.size(), copy_a.size());

    for (size_t i = 0; i < orig_b.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_b[i], copy_b[i]);
    }
    for (size_t i = 0; i < orig_a.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_a[i], copy_a[i]);
    }
  }

  TEST_F(IIRFilterTest, MoveConstructor)
  {
    IIRFilter<double> filter_original(b_coeffs, a_coeffs);
    IIRFilter<double> filter_moved(std::move(filter_original));

    EXPECT_EQ(filter_moved.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter_moved.getDenominatorOrder(), 1U);

    const auto &b = filter_moved.getNumeratorCoefficients();
    const auto &a = filter_moved.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  // SINGLE SAMPLE FILTERING TESTS

  TEST_F(IIRFilterTest, SingleSampleFiltering)
  {
    // Simple first-order filter: y[n] = x[n] + 0.5*x[n-1] + 0.5*y[n-1]
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0); // 1.0 + 0.5*0 + 0.5*0

    double out2 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out2, 1.0); // 0 + 0.5*1 + 0.5*1

    double out3 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out3, 0.5); // 0 + 0.5*0 + 0.5*1
  }

  TEST_F(IIRFilterTest, VectorFiltering)
  {
    IIRFilter<double> filter({1.0}, {1.0, -0.5});
    std::vector<double> input = {1.0, 0.0, 0.0};
    std::vector<double> output = filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    EXPECT_DOUBLE_EQ(output[0], 1.0);
    EXPECT_DOUBLE_EQ(output[1], 0.5);  // 0 + 0.5*1
    EXPECT_DOUBLE_EQ(output[2], 0.25); // 0 + 0.5*0.5
  }

  TEST_F(IIRFilterTest, BatchFiltering)
  {
    IIRFilter<double> filter({1.0}, {1.0, -0.5});
    std::vector<double> input = {1.0, 0.0, 0.0};
    std::vector<double> output(3);

    filter.filter(input.data(), output.data(), input.size());

    EXPECT_DOUBLE_EQ(output[0], 1.0);
    EXPECT_DOUBLE_EQ(output[1], 0.5);
    EXPECT_DOUBLE_EQ(output[2], 0.25);
  }

  // FILTER STATE MANAGEMENT TESTS

  TEST_F(IIRFilterTest, ResetFilter)
  {
    // Apply some input to change internal state
    simple_filter.filter(1.0);
    simple_filter.filter(2.0);

    // Reset filter
    simple_filter.reset();

    // Test that filter behaves as if newly constructed
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0);
  }

  TEST_F(IIRFilterTest, SetInitialConditions)
  {
    std::vector<double> x_initial = {1.0, 2.0};
    std::vector<double> y_initial = {0.5, 1.0};

    simple_filter.setInitialConditions(x_initial, y_initial);

    double output = simple_filter.filter(0.0);
    // Expected: 0*1 + 0.5*1 + 0.5*0.5 = 0.75
    EXPECT_DOUBLE_EQ(output, 0.75);
  }

  TEST_F(IIRFilterTest, SetInitialConditionsWrongSize)
  {
    std::vector<double> x_wrong = {1.0}; // Too few elements
    std::vector<double> y_wrong = {0.5}; // Too few elements
    std::vector<double> y_correct = {0.5, 1.0};

    EXPECT_THROW(simple_filter.setInitialConditions(x_wrong, y_correct),
                 std::invalid_argument);
    EXPECT_THROW(simple_filter.setInitialConditions(b_coeffs, y_wrong),
                 std::invalid_argument);
  }

  // FILTER CONFIGURATION TESTS

  TEST_F(IIRFilterTest, SetCoefficients)
  {
    std::vector<double> new_b = {2.0, 1.0};
    std::vector<double> new_a = {1.0, -0.25};

    simple_filter.setCoefficients(new_b, new_a);

    EXPECT_EQ(simple_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(simple_filter.getDenominatorOrder(), 1U);

    const auto &b = simple_filter.getNumeratorCoefficients();
    const auto &a = simple_filter.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 2.0);
    EXPECT_DOUBLE_EQ(b[1], 1.0);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.25);
  }

  TEST_F(IIRFilterTest, SetCoefficientsInvalidDenominator)
  {
    std::vector<double> new_b = {1.0};
    std::vector<double> invalid_a = {0.0, 1.0}; // First coefficient is zero

    EXPECT_THROW(simple_filter.setCoefficients(new_b, invalid_a),
                 std::invalid_argument);
  }

  // FREQUENCY RESPONSE TESTS

  TEST_F(IIRFilterTest, FrequencyResponse)
  {
    IIRFilter<double> filter({1.0}, {1.0, 0.0}); // Simple passthrough
    double sample_rate = 1000.0;
    double frequency = 100.0;

    std::complex<double> response =
        filter.frequencyResponse(frequency, sample_rate);

    // For a simple passthrough, magnitude should be 1
    EXPECT_NEAR(std::abs(response), 1.0, 1e-10);
  }

  TEST_F(IIRFilterTest, FrequencyResponseEmptyFilter)
  {
    IIRFilter<double> empty_filter;
    std::complex<double> response = empty_filter.frequencyResponse(100.0, 1000.0);

    EXPECT_DOUBLE_EQ(response.real(), 0.0);
    EXPECT_DOUBLE_EQ(response.imag(), 0.0);
  }

  // FACTORY METHOD TESTS

  TEST_F(IIRFilterTest, FirstOrderLowPassFactory)
  {
    IIRFilter<double> lp_filter =
        IIRFilter<double>::firstOrderLowPass(cutoff_freq, sample_rate);

    EXPECT_EQ(lp_filter.getNumeratorOrder(), 0U);
    EXPECT_EQ(lp_filter.getDenominatorOrder(), 1U);
    EXPECT_FALSE(lp_filter.isEmpty());

    const auto &b = lp_filter.getNumeratorCoefficients();
    const auto &a = lp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 1U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, FirstOrderHighPassFactory)
  {
    IIRFilter<double> hp_filter =
        IIRFilter<double>::firstOrderHighPass(cutoff_freq, sample_rate);

    EXPECT_EQ(hp_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(hp_filter.getDenominatorOrder(), 1U);
    EXPECT_FALSE(hp_filter.isEmpty());

    const auto &b = hp_filter.getNumeratorCoefficients();
    const auto &a = hp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderLowPassFactory)
  {
    IIRFilter<double> lp_filter =
        IIRFilter<double>::secondOrderLowPass(cutoff_freq, q_factor, sample_rate);

    EXPECT_EQ(lp_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(lp_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(lp_filter.isEmpty());

    const auto &b = lp_filter.getNumeratorCoefficients();
    const auto &a = lp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderHighPassFactory)
  {
    IIRFilter<double> hp_filter = IIRFilter<double>::secondOrderHighPass(
        cutoff_freq, q_factor, sample_rate);

    EXPECT_EQ(hp_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(hp_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(hp_filter.isEmpty());

    const auto &b = hp_filter.getNumeratorCoefficients();
    const auto &a = hp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderBandPassFactory)
  {
    double center_freq = 100.0;
    IIRFilter<double> bp_filter = IIRFilter<double>::secondOrderBandPass(
        center_freq, q_factor, sample_rate);

    EXPECT_EQ(bp_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(bp_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(bp_filter.isEmpty());

    const auto &b = bp_filter.getNumeratorCoefficients();
    const auto &a = bp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderNotchFactory)
  {
    double center_freq = 100.0;
    IIRFilter<double> notch_filter =
        IIRFilter<double>::secondOrderNotch(center_freq, q_factor, sample_rate);

    EXPECT_EQ(notch_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(notch_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(notch_filter.isEmpty());

    const auto &b = notch_filter.getNumeratorCoefficients();
    const auto &a = notch_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, IntegratorFactory)
  {
    IIRFilter<double> int_filter = IIRFilter<double>::integrator(sample_rate);

    EXPECT_EQ(int_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(int_filter.getDenominatorOrder(), 1U);
    EXPECT_FALSE(int_filter.isEmpty());

    const auto &b = int_filter.getNumeratorCoefficients();
    const auto &a = int_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -1.0);
  }

  TEST_F(IIRFilterTest, DifferentiatorFactory)
  {
    IIRFilter<double> diff_filter =
        IIRFilter<double>::differentiator(sample_rate);

    EXPECT_EQ(diff_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(diff_filter.getDenominatorOrder(), 0U);
    EXPECT_FALSE(diff_filter.isEmpty());

    const auto &b = diff_filter.getNumeratorCoefficients();
    const auto &a = diff_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 1U);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
  }

  TEST_F(IIRFilterTest, DCBlockerFactory)
  {
    IIRFilter<double> dc_blocker = IIRFilter<double>::dcBlocker(1.0, sample_rate);

    EXPECT_EQ(dc_blocker.getNumeratorOrder(), 1U);
    EXPECT_EQ(dc_blocker.getDenominatorOrder(), 1U);
    EXPECT_FALSE(dc_blocker.isEmpty());

    const auto &b = dc_blocker.getNumeratorCoefficients();
    const auto &a = dc_blocker.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
  }

  // TYPE ALIASES TESTS

  TEST_F(IIRFilterTest, TypeAliases)
  {
    IIRFilterd filter_d({1.0}, {1.0, -0.5});
    IIRFilterf filter_f({1.0f}, {1.0f, -0.5f});

    EXPECT_EQ(filter_d.getNumeratorOrder(), 0U);
    EXPECT_EQ(filter_f.getNumeratorOrder(), 0U);

    EXPECT_DOUBLE_EQ(filter_d.filter(1.0), 1.0);
    EXPECT_FLOAT_EQ(filter_f.filter(1.0f), 1.0f);
  }

  // EDGE CASES AND ERROR HANDLING

  TEST_F(IIRFilterTest, EmptyFilterBehavior)
  {
    IIRFilter<double> empty_filter;

    EXPECT_DOUBLE_EQ(empty_filter.filter(1.0), 0.0);
    EXPECT_DOUBLE_EQ(empty_filter.filter(5.0), 0.0);

    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output = empty_filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    for (double val : output)
    {
      EXPECT_DOUBLE_EQ(val, 0.0);
    }
  }

  TEST_F(FIRFilterTest, EmptyFilterBehavior)
  {
    FIRFilter<double> empty_filter;

    EXPECT_DOUBLE_EQ(empty_filter.filter(1.0), 0.0);
    EXPECT_DOUBLE_EQ(empty_filter.filter(5.0), 0.0);

    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output = empty_filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    for (double val : output)
    {
      EXPECT_DOUBLE_EQ(val, 0.0);
    }
  }

  // FILTERING STABILITY TESTS

  TEST_F(IIRFilterTest, StableLowPassFiltering)
  {
    IIRFilter<double> stable_filter =
        IIRFilter<double>::firstOrderLowPass(100.0, 1000.0);

    // Apply a constant input - should converge to a stable value
    std::vector<double> outputs;
    for (int i = 0; i < 100; ++i)
    {
      outputs.push_back(stable_filter.filter(1.0));
    }

    // Check that the output doesn't explode
    for (double output : outputs)
    {
      EXPECT_LT(std::abs(output), 10.0); // Should be bounded
    }

    // Check that it converges (last few values should be similar)
    EXPECT_NEAR(outputs[99], outputs[98], 1e-6);
    EXPECT_NEAR(outputs[98], outputs[97], 1e-6);
  }

  // COMPREHENSIVE INTEGRATION TESTS

  TEST_F(FIRFilterTest, MovingAverageIntegrationTest)
  {
    // Test moving average with known signal
    FIRFilter<double> ma_filter = FIRFilter<double>::movingAverage(3);

    std::vector<double> input = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> output = ma_filter.filter(input);

    // Expected outputs for 3-point moving average
    EXPECT_DOUBLE_EQ(output[0], 1.0 / 3.0);               // [1]/3
    EXPECT_DOUBLE_EQ(output[1], (1.0 + 2.0) / 3.0);       // [1,2]/3
    EXPECT_DOUBLE_EQ(output[2], (1.0 + 2.0 + 3.0) / 3.0); // [1,2,3]/3
    EXPECT_DOUBLE_EQ(output[3], (2.0 + 3.0 + 4.0) / 3.0); // [2,3,4]/3
    EXPECT_DOUBLE_EQ(output[4], (3.0 + 4.0 + 5.0) / 3.0); // [3,4,5]/3
  }

  TEST_F(IIRFilterTest, IntegratorIntegrationTest)
  {
    // Test integrator with known signal
    IIRFilter<double> integrator =
        IIRFilter<double>::integrator(1.0); // 1 Hz sample rate

    std::vector<double> input = {1.0, 1.0, 1.0, 1.0}; // Step input
    std::vector<double> output = integrator.filter(input);

    // Output should be approximately a ramp (cumulative sum scaled by Ts/2)
    EXPECT_GT(output[0], 0.0);
    EXPECT_GT(output[1], output[0]);
    EXPECT_GT(output[2], output[1]);
    EXPECT_GT(output[3], output[2]);
  }

  // PERFORMANCE AND PRECISION TESTS

  TEST_F(FIRFilterTest, PrecisionTest)
  {
    // Test with very small coefficients
    FIRFilter<double> precise_filter({1e-10, 1e-10, 1e-10});

    double output = precise_filter.filter(1e10);
    EXPECT_NEAR(output, 1e-10 * 1e10, 1e-15); // Should be 1.0
  }

  TEST_F(IIRFilterTest, PrecisionTest)
  {
    // Test with very small coefficients
    IIRFilter<double> precise_filter({1e-10}, {1.0, 1e-10});

    double output = precise_filter.filter(1e10);
    EXPECT_NEAR(output, 1e-10 * 1e10, 1e-15); // Should be 1.0
  }

} // namespace lumos
