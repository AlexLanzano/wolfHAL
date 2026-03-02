#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>

extern whal_Clock g_whalClock;
extern whal_Gpio g_whalGpio;
extern whal_Timer g_whalTimer;
extern whal_Uart g_whalUart;
extern whal_Flash g_whalFlash;
extern whal_Rng g_whalRng;

extern volatile size_t g_tick;

#define BOARD_LED_PIN         0
#define BOARD_FLASH_TEST_ADDR 0x08080000
#define BOARD_FLASH_SECTOR_SZ 0x1000

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
