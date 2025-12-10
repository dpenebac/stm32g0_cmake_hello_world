#include "stm32g031xx.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 6   // PC6

/* ---------------- GPIO Init (PC6 output) ---------------- */
static void GPIO_Init(void)
{
    /* Enable GPIOC clock */
    RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

    /* PC6 = output mode (01) */
    GPIOC->MODER &= ~(3U << (LED_PIN * 2));
    GPIOC->MODER |=  (1U << (LED_PIN * 2));

    /* Push-pull */
    GPIOC->OTYPER &= ~(1U << LED_PIN);

    /* Medium speed */
    GPIOC->OSPEEDR |=  (1U << (LED_PIN * 2));

    /* No pull-up/down */
    GPIOC->PUPDR &= ~(3U << (LED_PIN * 2));
}

/* ---------------- LED Blinky Task ---------------- */
static void led_task(void *argument)
{
    (void)argument;

    for (;;)
    {
        GPIOC->ODR ^= (1U << LED_PIN);       // Toggle PC6
        vTaskDelay(pdMS_TO_TICKS(500));      // 500 ms delay
    }
}

/* ---------------- Main ---------------- */
int main(void)
{
    /* Initialize GPIO before scheduler */
    GPIO_Init();

    /* Create LED task */
    xTaskCreate(
        led_task,
        "LED",
        128,
        NULL,
        1,
        NULL
    );

    /* Start FreeRTOS scheduler (never returns) */
    vTaskStartScheduler();

    /* Should never reach here */
    while (1)
    {
    }
}
