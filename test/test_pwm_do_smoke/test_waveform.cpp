#include <cstdint>
#include <unity.h>

#include "../../src/smoke_waveform.hpp"

/** @brief Unity setup; the pure waveform has no persistent test state. */
void setUp() {}
/** @brief Unity teardown; the test acquires no hardware resources. */
void tearDown() {}

/** @brief Verify ramp endpoints, wrap, and DO transitions against fixtures. */
void TestWaveformBoundaries() {
    // These fixtures catch a sawtooth reset, wrong ramp duration, inverted DO,
    // off-by-one toggle, and a missing second-cycle wrap.
    constexpr uint32_t times[] = {0, 1, 999, 1000, 1999, 2000,
                                  2001, 2999, 3000, 3999, 4000, 6000};
    constexpr uint16_t pwm[] = {0, 32, 32734, 32767, 65502, 65535,
                                65502, 32800, 32767, 32, 0, 65535};
    constexpr bool digital[] = {false, false, false, true, true, false,
                                false, false, true, true, false, false};
    for (unsigned i = 0; i < sizeof(times) / sizeof(times[0]); ++i) {
        const auto output = smoke::Evaluate(times[i]);
        TEST_ASSERT_EQUAL_UINT16(pwm[i], output.pwm);
        TEST_ASSERT_EQUAL(digital[i], output.do_high);
    }
}

/** @brief Check every millisecond for monotonic rise and fall. */
void TestRampMonotonicity() {
    for (uint32_t time = 1; time <= 2000; ++time) {
        TEST_ASSERT_TRUE(smoke::Evaluate(time).pwm >=
                         smoke::Evaluate(time - 1).pwm);
        TEST_ASSERT_TRUE(smoke::Evaluate(time + 2000).pwm <=
                         smoke::Evaluate(time + 1999).pwm);
    }
}

/** @brief Run host-only Unity checks; this executable never flashes a board. */
int main() {
    UNITY_BEGIN();
    RUN_TEST(TestWaveformBoundaries);
    RUN_TEST(TestRampMonotonicity);
    return UNITY_END();
}
