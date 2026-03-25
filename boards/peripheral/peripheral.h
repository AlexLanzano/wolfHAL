#ifndef BOARD_PERIPHERAL_H
#define BOARD_PERIPHERAL_H

#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/block/block.h>
#include <stddef.h>
#include <stdint.h>

/* Peripheral block device test configuration */
typedef struct {
    const char *name;
    whal_Block *dev;
    uint8_t *blockBuf;   /* Test buffer (must hold at least 2 blocks) */
    size_t blockBufSz;   /* Size of blockBuf in bytes */
    size_t blockSz;      /* Block size in bytes */
    uint8_t erasedByte;  /* Expected byte value after erase (0x00 or 0xFF) */
} whal_PeripheralBlock_Cfg;

extern whal_PeripheralBlock_Cfg g_peripheralBlock[];

#endif /* BOARD_PERIPHERAL_H */
