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

    err = whal_Clock_Init(&g_whalClock); 
    if (err) {
        goto loop;
    }

    err = whal_Gpio_Init(&g_whalGpio);
    if (err) {
        goto loop;
    }
    err = whal_Gpio_Set(&g_whalGpio, LED_PIN, 1);
    if (err) {
        goto loop;
    }

    err = whal_Uart_Init(&g_whalUart);
    if (err) {
        goto loop;
    }

    err = whal_Flash_Init(&g_whalFlash);
    if (err) {
        goto loop;
    }

    err = whal_Timer_Init(&g_whalTimer);
    if (err) {
        goto loop;
    }

    err = whal_Timer_Start(&g_whalTimer);
    if (err) {
        goto loop;
    }

    whal_Flash_Unlock(&g_whalFlash, 0, 0);
    
    uint8_t data[] = "TESTING TESTING HELLO\r\n";
    uint8_t tmp[sizeof(data)] = {0};
    whal_Flash_Erase(&g_whalFlash, 0x08080000, 0x1000);

    do {
        err = whal_Flash_Write(&g_whalFlash, 0x08080000, data, sizeof(data));
    } while (err == WHAL_ENOTREADY);

    whal_Flash_Read(&g_whalFlash, 0x08080000, tmp, sizeof(tmp));

    whal_Flash_Lock(&g_whalFlash, 0, 0);

    whal_Uart_Send(&g_whalUart, tmp, sizeof(tmp));

    while (1) {
        uint8_t input[8];
        err = whal_Uart_Send(&g_whalUart, (uint8_t *)"Enter Stuff:\r\n", 14);
        if (err) {
            goto loop;
        }

        err = whal_Uart_Recv(&g_whalUart, input, sizeof(input));
        if (err) {
            goto loop;
        }

        err = whal_Uart_Send(&g_whalUart, input, sizeof(input));
        if (err) {
            goto loop;
        }
        err = whal_Gpio_Set(&g_whalGpio, LED_PIN, 1);
        if (err) {
            goto loop;
        }

        WaitMs(1000);

        err = whal_Gpio_Set(&g_whalGpio, LED_PIN, 0);
        if (err) {
            goto loop;
        }

        WaitMs(1000);
    }

loop:
    while (1);

}
