#include "stm32g031xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

/* -------- Pins -------- */
#define TRIG_PIN    6   // PA6 -> TIM3_CH1 Output (TRIG)
#define LED_PIN     6   // PC6 (LED)

/* -------- Clock & Timer Definitions -------- */
// Assuming default 16MHz HSI clock
#define CORE_CLOCK_HZ         16000000UL
// Prescaler to get 1 us tick (16MHz / 16 = 1MHz)
#define TIM_PSC               (16 - 1)
#define PWM_PERIOD_US         60000UL // 60 ms repetition rate (16.6 Hz)
#define PULSE_WIDTH_US        10UL    // 10 us pulse width (High time)

/* ---------------- Function Prototypes ---------------- */
static void GPIO_Init(void);
static void TIM3_PWM_Init(void);
static void led_task(void *argument);

/* ---------------- GPIO Init ---------------- */
// Configures PC6 (LED) AND PA6 (TRIG/TIM3_CH1 Alternate Function).
static void GPIO_Init(void)
{
    /* Enable GPIOA / GPIOC clocks */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN;

    /* --- PA6 (TRIG/TIM3_CH1) as Alternate Function (AF1) --- */
    GPIOA->MODER &= ~(3U << (TRIG_PIN * 2));
    GPIOA->MODER |=  (2U << (TRIG_PIN * 2));  // Alternate Function mode (10)

    // Set AF selection: AF1 = TIM3_CH1
    GPIOA->AFR[0] &= ~(0xF << (TRIG_PIN * 4));
    GPIOA->AFR[0] |=  (1U << (TRIG_PIN * 4));

    /* --- PC6 (LED) as output --- */
    GPIOC->MODER &= ~(3U << (LED_PIN * 2));
    GPIOC->MODER |=  (1U << (LED_PIN * 2));

    GPIOC->OTYPER &= ~(1U << LED_PIN);            // Push-pull
    GPIOC->OSPEEDR |= (1U << (LED_PIN * 2));      // Medium speed
    GPIOC->PUPDR &= ~(3U << (LED_PIN * 2));       // No pull-up/down
}

/* ---------------- TIM3: Continuous 10 us TRIG pulse (on PA6) ---------------- */
static void TIM3_PWM_Init(void)
{
    /* Enable TIM3 clock */
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;

    // 1. Timebase Configuration
    TIM3->PSC = TIM_PSC;                // Prescaler for 1 us tick
    TIM3->ARR = PWM_PERIOD_US - 1;      // Auto-Reload Register sets 60 ms period

    // 2. Output Compare Channel 1 (OC1) Configuration
    TIM3->CCR1 = PULSE_WIDTH_US;        // 10 us pulse

    // 3. PWM Mode 1 (OC1M=110)
    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
    TIM3->CCMR1 |=  (6U << TIM_CCMR1_OC1M_Pos);

    // Enable preload register (OC1PE=1)
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

    // 4. Output Enable (CC1E=1)
    TIM3->CCER |= TIM_CCER_CC1E;

    // 5. Control Register
    TIM3->CR1 |= TIM_CR1_ARPE;          // Auto-reload preload

    // Generate update event
    TIM3->EGR |= TIM_EGR_UG;

    // 6. Start timer
    TIM3->CR1 |= TIM_CR1_CEN;
}

/* ---------------- LED Blinky Task ---------------- */
static void led_task(void *argument)
{
    (void)argument;

    for (;;)
    {
        GPIOC->ODR ^= (1U << LED_PIN);          // Toggle PC6
        vTaskDelay(pdMS_TO_TICKS(1000));        // 1s delay
    }
}

/* ---------------- Main ---------------- */
int main(void)
{
    GPIO_Init();
    TIM3_PWM_Init();

    xTaskCreate(
        led_task,
        "LED",
        128,
        NULL,
        1,
        NULL
    );

    vTaskStartScheduler();

    while (1) {}
}
