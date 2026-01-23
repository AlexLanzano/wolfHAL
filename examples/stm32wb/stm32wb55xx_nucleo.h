#ifndef STM32WB55XX_NUCLEO_H
#define STM32WB55XX_NUCLEO_H

#include <wolfHAL/platform/st/stm32wb55xx.h>

/*
 * @file stm32wb55xx_nucleo.h
 * @brief Board-specific handles for the STM32WB55xx Nucleo example.
 */

/* Friendly pin index mapping for the example board. */
enum {
    LED_PIN,
    LPUART1_TX_PIN,
    LPUART1_RX_PIN,
};

/* RCC clock controller instance. */
extern whal_Clock rcc;

/* GPIO controller instance. */
extern whal_Gpio gpio;

/* SysTick timer instance. */
extern whal_Timer sysTickTimer;

/* LPUART1 UART instance. */
extern whal_Uart lpuart1;

/* Flash controller instance. */
extern whal_Flash flash;


#endif /* STM32WB55XX_NUCLEO_H */
