#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

#ifdef WHAL_TEST_ENABLE_CLOCK
void whal_Test_Clock(void);
#ifdef WHAL_TEST_ENABLE_CLOCK_PLATFORM
void whal_Test_Clock_Platform(void);
#endif
#endif

#ifdef WHAL_TEST_ENABLE_GPIO
void whal_Test_Gpio(void);
#ifdef WHAL_TEST_ENABLE_GPIO_PLATFORM
void whal_Test_Gpio_Platform(void);
#endif
#endif

#ifdef WHAL_TEST_ENABLE_FLASH
void whal_Test_Flash(void);
#ifdef WHAL_TEST_ENABLE_FLASH_PLATFORM
void whal_Test_Flash_Platform(void);
#endif
#endif

#ifdef WHAL_TEST_ENABLE_TIMER
void whal_Test_Timer(void);
#endif

#ifdef WHAL_TEST_ENABLE_RNG
void whal_Test_Rng(void);
#ifdef WHAL_TEST_ENABLE_RNG_PLATFORM
void whal_Test_Rng_Platform(void);
#endif
#endif

#ifdef WHAL_TEST_ENABLE_IPC
void whal_Test_Ipc(void);
#ifdef WHAL_TEST_ENABLE_IPC_PLATFORM
void whal_Test_Ipc_Platform(void);
#endif
#endif

int g_whalTestPassed;
int g_whalTestFailed;
int g_whalTestCurFailed;

void whal_Test_Puts(const char *s)
{
    while (*s) {
        if (*s == '\n')
            whal_Uart_Send(&g_whalUart, "\r\n", 2);
        else
            whal_Uart_Send(&g_whalUart, s, 1);
        s++;
    }
}

void main(void)
{
    g_whalTestPassed = 0;
    g_whalTestFailed = 0;

    if (Board_Init() != WHAL_SUCCESS)
        while (1);

    whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1);

    whal_Test_Printf("wolfHAL HW Test Suite\n");
    whal_Test_Printf("=====================\n");

#ifdef WHAL_TEST_ENABLE_CLOCK
    whal_Test_Clock();
#ifdef WHAL_TEST_ENABLE_CLOCK_PLATFORM
    whal_Test_Clock_Platform();
#endif
#endif

#ifdef WHAL_TEST_ENABLE_GPIO
    whal_Test_Gpio();
#ifdef WHAL_TEST_ENABLE_GPIO_PLATFORM
    whal_Test_Gpio_Platform();
#endif
#endif

#ifdef WHAL_TEST_ENABLE_FLASH
    whal_Test_Flash();
#ifdef WHAL_TEST_ENABLE_FLASH_PLATFORM
    whal_Test_Flash_Platform();
#endif
#endif

#ifdef WHAL_TEST_ENABLE_TIMER
    whal_Test_Timer();
#endif

#ifdef WHAL_TEST_ENABLE_RNG
    whal_Test_Rng();
#ifdef WHAL_TEST_ENABLE_RNG_PLATFORM
    whal_Test_Rng_Platform();
#endif
#endif

#ifdef WHAL_TEST_ENABLE_IPC
    whal_Test_Ipc();
#ifdef WHAL_TEST_ENABLE_IPC_PLATFORM
    whal_Test_Ipc_Platform();
#endif
#endif

    WHAL_TEST_SUMMARY();

    if (g_whalTestFailed == 0) {
        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1);
        while (1);
    }

    while (1) {
        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1);
        Board_WaitMs(100);
        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 0);
        Board_WaitMs(100);
    }
}
