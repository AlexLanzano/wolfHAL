#include <wolfHAL/wolfHAL.h>
#include "pic32cz_curiosity_ultra.h"

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
    uint8_t data[] = "Hello world!\r\n";
    uint8_t test[] = "test1\r\n";
    uint8_t tmp[sizeof(test)] = {0};

    err = whal_Clock_Init(&g_whalClock);
    if (err) {
        goto loop;
    }

    err = whal_Gpio_Init(&g_whalGpio);
    if (err) {
        goto loop;
    }

    err = whal_Uart_Init(&g_whalUart);
    if (err) {
        goto loop;
    }

    err = whal_Uart_Send(&g_whalUart, data, sizeof(data));
    if (err) {
        goto loop;
    }

    err = whal_Flash_Init(&g_whalFlash);
    if (err) {
        goto loop;
    }

    err = whal_Flash_Erase(&g_whalFlash, 0x0C000000, 0x1000);
    if (err) {
        goto loop;
    }

    do {
        err = whal_Flash_Write(&g_whalFlash, 0x0C000000, test, sizeof(test));
    } while (err == WHAL_ENOTREADY);

    if (err) {
        goto loop;
    }

    err = whal_Flash_Read(&g_whalFlash, 0x0C000000, tmp, sizeof(tmp));
    if (err) {
        goto loop;
    }

    err = whal_Uart_Send(&g_whalUart, tmp, sizeof(tmp));
    if (err) {
        goto loop;
    }

    err = whal_Timer_Init(&g_whalTimer);
    if (err) {
        goto loop;
    }

    whal_Timer_Start(&g_whalTimer);

    while (1) {
        whal_Gpio_Set(&g_whalGpio, 0, 1);

        WaitMs(1000);

        whal_Gpio_Set(&g_whalGpio, 0, 0);

        WaitMs(1000);
    }

loop:
    while (1);
}
