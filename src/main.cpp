// Upstream core_riscv.h uses C-style volatile compound assignments. Keep the
// C++20 deprecation exception inside the vendor boundary, not project code.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#include <ch32v20x.h>
#pragma GCC diagnostic pop

#include "smoke_waveform.hpp"

namespace {

// TIM2/3 share APB1. Use 65535 counts so CCR=65535 is a true 100%.
constexpr uint16_t kPwmPeriod = 65534;
constexpr uint16_t kPwmPrescaler = 1;
volatile uint32_t milliseconds = 0;

/** @brief Return the APB1 timer clock, including the prescaler x2 rule. */
uint32_t GetTimerClockHz() {
    RCC_ClocksTypeDef clocks = {};
    RCC_GetClocksFreq(&clocks);
    return clocks.PCLK1_Frequency == clocks.HCLK_Frequency
               ? clocks.PCLK1_Frequency
               : clocks.PCLK1_Frequency * 2;
}

/** @brief Configure PA0 PWM and PA3 push-pull, both initially inactive. */
void InitOutputs() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_3);
    GPIO_InitTypeDef gpio = {};
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);

    TIM_TimeBaseInitTypeDef timer = {};
    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Period = kPwmPeriod;
    timer.TIM_Prescaler = kPwmPrescaler;
    TIM_TimeBaseInit(TIM2, &timer);

    TIM_OCInitTypeDef channel = {};
    TIM_OCStructInit(&channel);
    channel.TIM_OCMode = TIM_OCMode_PWM1;
    channel.TIM_OutputState = TIM_OutputState_Enable;
    channel.TIM_OCPolarity = TIM_OCPolarity_High;
    channel.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &channel);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_GenerateEvent(TIM2, TIM_EventSource_Update);

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);
    TIM_Cmd(TIM2, ENABLE);
}

/** @brief Start a 1 ms TIM3 tick; keep PA0/PA3 inactive on invalid clocks. */
bool StartTick() {
    const uint32_t timer_hz = GetTimerClockHz();
    if (timer_hz < 1000000 || timer_hz % 1000000 != 0 ||
        timer_hz / 1000000 > 65536) {
        return false;
    }
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef timer = {};
    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = static_cast<uint16_t>(timer_hz / 1000000 - 1);
    timer.TIM_Period = 999;
    TIM_TimeBaseInit(TIM3, &timer);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM_Cmd(TIM3, ENABLE);
    return true;
}

}  // namespace

extern "C" {

/** @brief Increment the millisecond clock only; WCH vector ABI is required. */
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/** @brief Preserve the SDK's NMI vector entry point. */
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/** @brief Force PA0/PA3 low and halt on an unrecoverable CPU fault. */
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        milliseconds = milliseconds + 1;
    }
}

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_3);
    GPIO_InitTypeDef gpio = {};
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);
    while (true) {}
}

}  // extern "C"

/** @brief Run the isolated PWM/DO check without blocking delay calls. */
int main() {
    SystemCoreClockUpdate();
    InitOutputs();
    if (!StartTick()) {
        while (true) {}
    }

    uint32_t previous_ms = 0;
    uint32_t phase_ms = 0;
    while (true) {
        const uint32_t now_ms = milliseconds;
        const uint32_t elapsed_ms = now_ms - previous_ms;
        if (elapsed_ms == 0) {
            continue;
        }
        previous_ms = now_ms;
        // Subtraction handles uint32 tick wrap; reduce before adding.
        phase_ms = (phase_ms + elapsed_ms % smoke::kCycleMs) % smoke::kCycleMs;
        const smoke::Output output = smoke::Evaluate(phase_ms);
        TIM_SetCompare1(TIM2, output.pwm);
        GPIO_WriteBit(GPIOA, GPIO_Pin_3, output.do_high ? Bit_SET : Bit_RESET);
    }
}
