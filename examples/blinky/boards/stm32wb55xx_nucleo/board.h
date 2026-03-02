#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>
#include <wolfHAL/wolfHAL.h>

extern whal_Clock g_whalClock;
extern whal_Gpio g_whalGpio;
extern whal_Timer g_whalTimer;
extern whal_Uart g_whalUart;

#define BOARD_LED_PIN 0

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
