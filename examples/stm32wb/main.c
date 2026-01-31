#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/bitops.h>
#include "stm32wb55xx_nucleo.h"

volatile size_t g_tick = 0;
volatile uint8_t g_waiting = 0;
volatile uint8_t g_tickOverflow = 0;

void SysTick_Handler()
{
    size_t tickBefore = g_tick++;
    if (g_waiting) {
        if (tickBefore > g_tick)
            g_tickOverflow = 1;
    }
}

void WaitMs(size_t ms)
{
    size_t startCount = g_tick;
    g_waiting = 1;
    while (1) {
        size_t currentCount = g_tick;
        if (g_tickOverflow) {
            if ((SIZE_MAX - startCount) + currentCount > ms) {
                break;
            }
        } else if (currentCount - startCount > ms) {
            break;
        }
    }

    g_waiting = 0;
    g_tickOverflow = 0;
}

void main(void)
{
    whal_Error err;

    err = whal_Clock_Init(&rcc); 
    if (err) {
        goto loop;
    }

    err = whal_Gpio_Init(&gpio);
    if (err) {
        goto loop;
    }
    err = whal_Gpio_Set(&gpio, LED_PIN, 1);
    if (err) {
        goto loop;
    }

    err = whal_Uart_Init(&lpuart1);
    if (err) {
        goto loop;
    }

    err = whal_Flash_Init(&flash);
    if (err) {
        goto loop;
    }

    err = whal_Timer_Init(&sysTickTimer);
    if (err) {
        goto loop;
    }

    err = whal_Timer_Start(&sysTickTimer);
    if (err) {
        goto loop;
    }

    whal_Flash_Unlock(&flash, 0, 0);
    
    uint8_t data[] = "TESTING TESTING HELLO\r\n";
    uint8_t tmp[sizeof(data)] = {0};
    whal_Flash_Erase(&flash, 0x08080000, 0x1000);

    do {
        err = whal_Flash_Write(&flash, 0x08080000, data, sizeof(data));
    } while (err == WHAL_ENOTREADY);

    whal_Flash_Read(&flash, 0x08080000, tmp, sizeof(tmp));

    whal_Flash_Lock(&flash, 0, 0);

    whal_Uart_Send(&lpuart1, tmp, sizeof(tmp));

    while (1) {
        uint8_t input[8];
        err = whal_Uart_Send(&lpuart1, (uint8_t *)"Enter Stuff:\r\n", 14);
        if (err) {
            goto loop;
        }

        err = whal_Uart_Recv(&lpuart1, input, sizeof(input));
        if (err) {
            goto loop;
        }

        err = whal_Uart_Send(&lpuart1, input, sizeof(input));
        if (err) {
            goto loop;
        }
        err = whal_Gpio_Set(&gpio, LED_PIN, 1);
        if (err) {
            goto loop;
        }

        WaitMs(1000);

        err = whal_Gpio_Set(&gpio, LED_PIN, 0);
        if (err) {
            goto loop;
        }

        WaitMs(1000);
    }

loop:
    while (1);

}
