#ifndef WHAL_PIC32CZ_CLOCK_H
#define WHAL_PIC32CZ_CLOCK_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/clock/clock.h>

/*
 * @file pic32cz_clock.h
 * @brief PIC32CZ clock system driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * The PIC32CZ clock subsystems:
 *   OSCCTRL — PLL0/PLL1 fractional PLLs (each up to 4 outputs).
 *   GCLK    — N generic clock generators routed to peripheral channels.
 *   MCLK    — main clock controller (CPU divider + peripheral bus gates).
 *
 * Path:  Reference -> PLL -> GCLK Generator -> GCLK Peripheral Channel -> Peripheral
 *                                            -> MCLK -> CPU/bus clocks
 */

/*
 * @brief PLL instance.
 */
typedef enum {
    WHAL_PIC32CZ_PLL0,
    WHAL_PIC32CZ_PLL1,
} whal_Pic32cz_Clock_PllInst;

/*
 * @brief PLL reference clock source selection.
 */
typedef enum {
    WHAL_PIC32CZ_REFSEL_GCLK,
    WHAL_PIC32CZ_REFSEL_XOSC,
    WHAL_PIC32CZ_REFSEL_DFLL48M,
} whal_Pic32cz_Clock_PllRefSel;

/*
 * @brief PLL loop filter bandwidth selection.
 */
typedef enum {
    WHAL_PIC32CZ_BWSEL_4MHz_TO_10MHz  = 1,
    WHAL_PIC32CZ_BWSEL_10MHz_TO_20MHz,
    WHAL_PIC32CZ_BWSEL_20MHz_TO_30MHz,
    WHAL_PIC32CZ_BWSEL_30MHz_TO_60MHz,
} whal_Pic32cz_Clock_PllBwSel;

/*
 * @brief GCLK generator source selection.
 */
typedef enum {
    WHAL_PIC32CZ_GENSRC_PLL0_CLOCKOUT0 = 0x6,
    WHAL_PIC32CZ_GENSRC_PLL0_CLOCKOUT1,
    WHAL_PIC32CZ_GENSRC_PLL0_CLOCKOUT2,
    WHAL_PIC32CZ_GENSRC_PLL0_CLOCKOUT3,
} whal_Pic32cz_Clock_GenSrc;

/* PLL output post-divider field positions for PLLxPOSTDIVA */
#define WHAL_PIC32CZ_POSTDIV0_Pos (0)
#define WHAL_PIC32CZ_POSTDIV0_Msk (WHAL_BITMASK(6) << 0)
#define WHAL_PIC32CZ_POSTDIV1_Pos (8)
#define WHAL_PIC32CZ_POSTDIV1_Msk (WHAL_BITMASK(6) << 8)
#define WHAL_PIC32CZ_POSTDIV2_Pos (16)
#define WHAL_PIC32CZ_POSTDIV2_Msk (WHAL_BITMASK(6) << 16)
#define WHAL_PIC32CZ_POSTDIV3_Pos (24)
#define WHAL_PIC32CZ_POSTDIV3_Msk (WHAL_BITMASK(6) << 24)
#define WHAL_PIC32CZ_OUTEN0_Pos (7)
#define WHAL_PIC32CZ_OUTEN0_Msk (1UL << 7)
#define WHAL_PIC32CZ_OUTEN1_Pos (15)
#define WHAL_PIC32CZ_OUTEN1_Msk (1UL << 15)
#define WHAL_PIC32CZ_OUTEN2_Pos (23)
#define WHAL_PIC32CZ_OUTEN2_Msk (1UL << 23)
#define WHAL_PIC32CZ_OUTEN3_Pos (31)
#define WHAL_PIC32CZ_OUTEN3_Msk (1UL << 31)

/*
 * @brief One PLL output configuration. f_out = f_vco / postDiv.
 */
typedef struct {
    size_t postDivMask;
    size_t postDivPos;
    size_t outEnMask;
    size_t outEnPos;
    uint8_t postDiv;
} whal_Pic32cz_Clock_PllOutCfg;

/*
 * @brief PLL configuration.
 *   f_vco = (f_ref / refDiv) * fbDiv
 *   refDiv: 1-63
 *   fbDiv:  16-1023
 */
typedef struct {
    whal_Pic32cz_Clock_PllInst pllInst;
    whal_Pic32cz_Clock_PllRefSel refSel;
    whal_Pic32cz_Clock_PllBwSel bwSel;
    uint8_t refDiv;
    uint16_t fbDiv;
    uint8_t outCfgCount;
    const whal_Pic32cz_Clock_PllOutCfg *outCfg;
} whal_Pic32cz_Clock_PllCfg;

/*
 * @brief GCLK generator configuration.
 *   gen:    generator index (0-11)
 *   genSrc: clock source for this generator
 *   genDiv: divider (1-65535, 0 = off)
 */
typedef struct {
    uint8_t gen;
    whal_Pic32cz_Clock_GenSrc genSrc;
    uint16_t genDiv;
} whal_Pic32cz_Clock_GenCfg;

/*
 * @brief Peripheral clock descriptor (GCLK channel + MCLK enable bit).
 */
typedef struct {
    size_t gclkPeriphChannel;
    uint8_t gclkPeriphSrc;
    size_t mclkEnableInst;
    size_t mclkEnableMask;
    size_t mclkEnablePos;
} whal_Pic32cz_Clock_PeriphClk;

/*
 * @brief Configure and enable a PLL. Blocks until lock is detected.
 */
whal_Error whal_Pic32cz_Clock_EnablePll(whal_Clock *clkDev,
                                       const whal_Pic32cz_Clock_PllCfg *cfg);

/*
 * @brief Configure and enable a GCLK generator. Blocks until sync clears.
 */
whal_Error whal_Pic32cz_Clock_EnableGclkGen(whal_Clock *clkDev,
                                           const whal_Pic32cz_Clock_GenCfg *cfg);

/*
 * @brief Set the MCLK CPU divider. Blocks until ready.
 */
whal_Error whal_Pic32cz_Clock_SetMclkDiv(whal_Clock *clkDev, uint8_t div);

/*
 * @brief Enable a peripheral clock — connects GCLK channel to a generator
 *        and enables the peripheral's MCLK bus gate.
 */
whal_Error whal_Pic32cz_Clock_EnablePeriphClk(whal_Clock *clkDev,
                                             const whal_Pic32cz_Clock_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Pic32cz_Clock_DisablePeriphClk(whal_Clock *clkDev,
                                              const whal_Pic32cz_Clock_PeriphClk *clk);

#endif /* WHAL_PIC32CZ_CLOCK_H */
