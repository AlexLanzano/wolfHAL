#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/timer/timer.h>
#include <wolfHAL/bitops.h>
#include "stm32wb55xx_nucleo.h"
#include "../test.h"

int g_whalTestPassed;
int g_whalTestFailed;
int g_whalTestCurFailed;

volatile size_t g_tick = 0;

void SysTick_Handler(void)
{
    g_tick++;
}

/* whalTest_Puts: send a string over UART, translating \n to \r\n */
void whalTest_Puts(const char *s)
{
    while (*s) {
        if (*s == '\n')
            whal_Uart_Send(&g_whalUart, (const uint8_t *)"\r\n", 2);
        else
            whal_Uart_Send(&g_whalUart, (const uint8_t *)s, 1);
        s++;
    }
}

/* Busy-wait delay using SysTick */
static void delay_ms(size_t ms)
{
    size_t start = g_tick;
    while (g_tick - start < ms)
        ;
}

void test_clock(void);
void test_gpio(void);
void test_flash(void);
void test_timer(void);

void main(void)
{
    whal_Error err;

    g_whalTestPassed = 0;
    g_whalTestFailed = 0;

    /* Bootstrap: clock -> GPIO -> UART -> timer */
    err = whal_Clock_Init(&g_whalClock);
    if (err)
        goto fail;

    err = whal_Gpio_Init(&g_whalGpio);
    if (err)
        goto fail;

    /* LED on to indicate boot */
    whal_Gpio_Set(&g_whalGpio, LED_PIN, 1);

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        goto fail;

    err = whal_Flash_Init(&g_whalFlash);
    if (err)
        goto fail;

    err = whal_Timer_Init(&g_whalTimer);
    if (err)
        goto fail;

    err = whal_Timer_Start(&g_whalTimer);
    if (err)
        goto fail;

    whalTest_Printf("wolfHAL HW Test Suite\n");
    whalTest_Printf("=====================\n");

    /* Run test suites */
    test_clock();
    test_gpio();
    test_flash();
    test_timer();

    WHAL_TEST_SUMMARY();

    /* Visual indication: solid LED = all pass, blink = failure */
    if (g_whalTestFailed == 0) {
        whal_Gpio_Set(&g_whalGpio, LED_PIN, 1);
        while (1)
            ;
    }

fail:
    /* Rapid blink = failure */
    while (1) {
        whal_Gpio_Set(&g_whalGpio, LED_PIN, 1);
        delay_ms(100);
        whal_Gpio_Set(&g_whalGpio, LED_PIN, 0);
        delay_ms(100);
    }
}
