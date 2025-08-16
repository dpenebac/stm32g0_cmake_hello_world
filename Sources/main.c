#include "stm32g031xx.h"
#include <stdint.h>

/* -------- Pins -------- */
#define TRIG_PIN    0   // PA0 -> HC-SR04 TRIG (MCU output, 3V3 OK)
#define ECHO_PIN    1   // PA1 -> HC-SR04 ECHO (needs level shift to 3V3!)
#define LED_PIN     6   // PC6 LED

/* -------- SysTick: 1 us tick @ 16 MHz -------- */
static void SysTick_Init(void) {
    SysTick->LOAD = (16000000/1000000) - 1;  // 1 us
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

static void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) { /* wait 1 us */ }
    }
}

/* -------- GPIO init -------- */
static void GPIO_Init(void) {
    /* Enable GPIOA / GPIOC clocks */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN;

    /* PA0 (TRIG) as output, push-pull */
    GPIOA->MODER &= ~(3U << (TRIG_PIN*2));
    GPIOA->MODER |=  (1U << (TRIG_PIN*2));
    GPIOA->OTYPER &= ~(1U << TRIG_PIN);

    /* PA1 (ECHO) as input */
    GPIOA->MODER &= ~(3U << (ECHO_PIN*2));

    /* PC6 (LED) as output, push-pull */
    GPIOC->MODER &= ~(3U << (LED_PIN*2));
    GPIOC->MODER |=  (1U << (LED_PIN*2));
    GPIOC->OTYPER &= ~(1U << LED_PIN);
}

/* -------- HC-SR04 read (polling) --------
   Returns distance in cm (0 on timeout) */
static uint32_t HCSR04_Read(void) {
    /* 1) 10 us TRIG pulse */
    GPIOA->ODR &= ~(1U << TRIG_PIN);
    delay_us(2);
    GPIOA->ODR |=  (1U << TRIG_PIN);
    delay_us(10);
    GPIOA->ODR &= ~(1U << TRIG_PIN);

    /* 2) Wait for ECHO high (timeout ~30 ms) */
    uint32_t timeout_us = 30000;
    while (!(GPIOA->IDR & (1U << ECHO_PIN))) {
        if (timeout_us-- == 0) return 0;
        delay_us(1);
    }

    /* 3) Measure ECHO high width (max ~30 ms ≈ >5 m) */
    uint32_t width_us = 0;
    while (GPIOA->IDR & (1U << ECHO_PIN)) {
        if (width_us++ > 30000) break;
        delay_us(1);
    }

    /* 4) Convert time-of-flight to cm: us / 58 */
    return width_us / 58U;
}

int main(void) {
    SysTick_Init();
    GPIO_Init();

    while (1) {
        uint32_t dist_cm = HCSR04_Read();

        /* LED on PC6: ON if object < 10 cm, else OFF */
        if (dist_cm < 10) {
            GPIOC->ODR |=  (1U << LED_PIN);   // PC6 = 1
        } else {
            GPIOC->ODR &= ~(1U << LED_PIN);   // PC6 = 0
        }

        /* Minimum ~60 ms between measurements to avoid ring/echo */
        delay_us(60000);
    }
}
