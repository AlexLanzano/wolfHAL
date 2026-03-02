#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include "board.h"

void main(void)
{
    if (Board_Init() != WHAL_SUCCESS)
        goto loop;

    while (1) {
        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1);
        whal_Uart_Send(&g_whalUart, (const uint8_t *)"Blink!\r\n", 8);
        Board_WaitMs(1000);
        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 0);
        whal_Uart_Send(&g_whalUart, (const uint8_t *)"Blink!\r\n", 8);
        Board_WaitMs(1000);
    }

loop:
    while (1);
}
