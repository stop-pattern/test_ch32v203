#ifndef SRC_SMOKE_WAVEFORM_HPP_
#define SRC_SMOKE_WAVEFORM_HPP_

#include <cstdint>

namespace smoke {

inline constexpr uint32_t kRampHalfPeriodMs = 2000;
inline constexpr uint32_t kDoToggleMs = 1000;
inline constexpr uint32_t kCycleMs = 2 * kRampHalfPeriodMs;
inline constexpr uint32_t kPwmFullScale = 65535;

/** @brief One sample of the standalone PWM/DO hardware check. */
struct Output {
    uint16_t pwm;
    bool do_high;
};

/** @brief Return the repeating waveform at an elapsed time in milliseconds. */
constexpr Output Evaluate(uint32_t elapsed_ms) {
    const uint32_t phase = elapsed_ms % kCycleMs;
    const uint32_t ramp_ms = phase <= kRampHalfPeriodMs
                                 ? phase
                                 : kCycleMs - phase;
    return {static_cast<uint16_t>(ramp_ms * kPwmFullScale / kRampHalfPeriodMs),
            ((elapsed_ms / kDoToggleMs) % 2) != 0};
}

}  // namespace smoke

#endif  // SRC_SMOKE_WAVEFORM_HPP_
