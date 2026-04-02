# Writing a Driver

This guide covers how to implement a wolfHAL driver for a specific platform.

## Common Pattern

Every device type in wolfHAL follows the same structure:

1. A **driver vtable** — a struct of function pointers that the platform driver
   must populate.
2. A **device struct** — contains a register map, a pointer to the driver
   vtable, and a pointer to platform-specific configuration.
3. A **generic dispatch layer** — validates inputs and calls through the vtable.

To write a driver for a device type, you implement the functions defined in that
type's vtable and expose them as a const driver instance.

### File Layout

For a device type `foo` on platform `myplatform`:

- `wolfHAL/foo/myplatform_foo.h` — configuration types and driver extern
- `src/foo/myplatform_foo.c` — driver implementation and vtable definition

### Driver Vtable

Every vtable includes `Init` and `Deinit`. The remaining functions are specific
to the device type. All functions receive a pointer to the device instance as
their first argument and return `whal_Error`.

```c
const whal_FooDriver whal_MyplatformFoo_Driver = {
    .Init   = whal_MyplatformFoo_Init,
    .Deinit = whal_MyplatformFoo_Deinit,
    /* device-specific operations */
};
```

### Configuration

The device struct's `cfg` field points to your platform-specific configuration.
Cast it in your driver functions:

```c
static whal_Error whal_MyplatformFoo_Init(whal_Foo *fooDev)
{
    whal_MyplatformFoo_Cfg *cfg = (whal_MyplatformFoo_Cfg *)fooDev->cfg;
    /* ... */
}
```

### No Cross-Driver Calls

Register-level drivers must not call other wolfHAL drivers. A UART driver
must not call the clock driver to enable its own clock, and a clock driver
must not call the flash driver to set wait states. The board is responsible
for all cross-peripheral dependencies — enabling clocks, configuring power
supplies, and setting flash latency — before calling a driver's Init.

This ensures drivers are pure register-level abstractions with no hidden
dependencies, making them usable with or without the vtable dispatch layer.

Bus-device drivers (e.g., SPI flash) are the exception — they inherently
need to call their underlying bus driver (SPI, I2C) to communicate with the
device.

### Register Access

wolfHAL provides register access helpers in `wolfHAL/regmap.h`:

- `whal_Reg_Write()` — write a full register value
- `whal_Reg_Read()` — read a full register value
- `whal_Reg_Update()` — read-modify-write a specific field within a register
- `whal_Reg_Get()` — read and extract a masked field from a register

Use `Write` and `Read` for whole-register access. Use `Update` and `Get` when
you need to modify or read individual fields within a register without
disturbing other bits.

### Timeouts

Most drivers poll hardware status registers in busy-wait loops (e.g., waiting
for a DMA transfer to complete, a flash erase to finish, or an AES computation
to produce a result). wolfHAL provides an optional timeout mechanism to bound
these waits and prevent infinite hangs if hardware misbehaves.

#### Timeout Struct

The board creates a `whal_Timeout` instance with a tick source and a timeout
duration. Drivers receive a pointer to it through their configuration struct:

```c
typedef struct {
    uint32_t timeoutTicks;     /* max ticks before timeout */
    uint32_t startTick;        /* snapshot, set by START */
    uint32_t (*GetTick)(void); /* board-provided tick source */
} whal_Timeout;
```

The tick units are determined by the board's `GetTick` implementation. A 1 kHz
SysTick gives millisecond ticks; a 1 MHz timer gives microsecond ticks. Drivers
do not need to know the tick rate.

#### whal_Reg_ReadPoll

For the common case of polling a register bit, use `whal_Reg_ReadPoll` from
`wolfHAL/regmap.h`:

```c
whal_Error whal_Reg_ReadPoll(size_t base, size_t offset,
                              size_t mask, size_t value,
                              whal_Timeout *timeout);
```

This polls until `(reg & mask) == value` or the timeout expires. Pass the mask
as the value to wait for bits to be set, or `0` to wait for bits to be clear:

```c
/* Wait for TXE flag to be set */
err = whal_Reg_ReadPoll(base, SPI_SR_REG, SPI_SR_TXE_Msk,
                        SPI_SR_TXE_Msk, cfg->timeout);

/* Wait for BSY flag to be clear */
err = whal_Reg_ReadPoll(base, SPI_SR_REG, SPI_SR_BSY_Msk,
                        0, cfg->timeout);
```

A NULL timeout pointer means unbounded wait (poll forever).

#### Driver-Specific Helpers

When a driver has many polling sites that also need post-poll cleanup (e.g.,
clearing a flag), wrap the pattern in a local helper to avoid code duplication:

```c
static whal_Error WaitForCCF(size_t base, whal_Timeout *timeout)
{
    whal_Error err;
    err = whal_Reg_ReadPoll(base, AES_SR_REG, AES_SR_CCF_Msk,
                            AES_SR_CCF_Msk, timeout);
    if (err)
        return err;
    whal_Reg_Update(base, AES_CR_REG, AES_CR_CCFC_Msk, AES_CR_CCFC_Msk);
    return WHAL_SUCCESS;
}
```

This keeps code size small — one function body shared across all call sites
instead of inlined polling loops at each location.

#### Cleanup on Timeout

When a timeout occurs during an operation that has enabled a hardware mode
(e.g., flash programming mode, AES enable), the driver must still clean up
before returning. Use a `goto cleanup` pattern:

```c
whal_Error err = WHAL_SUCCESS;

whal_Reg_Update(base, CR_REG, PG_Msk, 1);   /* enable programming mode */

for (...) {
    err = whal_Reg_ReadPoll(base, SR_REG, BSY_Msk, 0, cfg->timeout);
    if (err)
        goto cleanup;
}

cleanup:
    whal_Reg_Update(base, CR_REG, PG_Msk, 0);   /* always disable */
    return err;
```

Never return directly from inside a polling loop if the peripheral is in a
mode that requires cleanup.

#### Compile-Time Disable

Define `WHAL_CFG_NO_TIMEOUT` to remove all timeout logic from the binary.
When defined, `WHAL_TIMEOUT_START` becomes a no-op and `WHAL_TIMEOUT_EXPIRED`
always evaluates to `0`, so polling loops run until the hardware condition is
met with no overhead.

#### Adding Timeout to a Config Struct

Driver config structs that use polling should include an optional timeout
pointer:

```c
typedef struct {
    /* ... other config fields ... */
    whal_Timeout *timeout;
} whal_MyplatformFoo_Cfg;
```

The timeout is optional — if the board does not set it (NULL), all waits are
unbounded.

### Avoiding Bloat

When a peripheral has multiple distinct operating modes or configurations,
consider splitting the driver into separate implementations rather than one
monolithic driver. For example, if a clock controller supports several different
clock sources, each with its own initialization logic, you can provide a
separate driver for each source. This way users only link the code they actually
need, and unused configurations are not compiled into the final binary.

### Reusing a Driver Across Platforms

Many MCU families share identical peripheral IP blocks. For example, the
STM32WB and STM32H5 have register-compatible GPIO and UART peripherals.
Rather than duplicating driver code, create thin alias files for the new
platform that re-export the existing driver under platform-specific names.

#### Header

The alias header `typedef`s the config structs and `#define`s the driver
instance, functions, and any enum constants:

```c
#ifndef WHAL_STM32H5_GPIO_H
#define WHAL_STM32H5_GPIO_H

#include <wolfHAL/gpio/stm32wb_gpio.h>

typedef whal_Stm32wbGpio_Cfg    whal_Stm32h5Gpio_Cfg;
typedef whal_Stm32wbGpio_PinCfg whal_Stm32h5Gpio_PinCfg;

#define whal_Stm32h5Gpio_Driver whal_Stm32wbGpio_Driver
#define whal_Stm32h5Gpio_Init   whal_Stm32wbGpio_Init
#define whal_Stm32h5Gpio_Deinit whal_Stm32wbGpio_Deinit
#define whal_Stm32h5Gpio_Get    whal_Stm32wbGpio_Get
#define whal_Stm32h5Gpio_Set    whal_Stm32wbGpio_Set

/* Re-export enum constants under the new platform name */
#define WHAL_STM32H5_GPIO_MODE_OUT WHAL_STM32WB_GPIO_MODE_OUT
/* ... */

#endif
```

Use `typedef` for types (gives proper type-checking and debugger visibility)
and `#define` for the driver instance and functions (which are values, not
types).

#### Source

The source file includes the original implementation directly:

```c
#include "stm32wb_gpio.c"
```

This works because `#include` is textual insertion — the compiler does not
distinguish `.h` from `.c`. The new platform's `Makefile.inc` compiles this
file and does **not** compile the original. The original platform's
`Makefile.inc` still compiles its own file directly. Both must never appear in
the same build.

#### When to alias vs. write a new driver

Alias when the register layout is identical — same offsets, same bit positions,
same behavior. If even one register differs (different bit position, extra
field, different reset value that affects behavior), write a new driver. A
partial alias that papers over register differences with workarounds is worse
than a clean separate implementation.

### Platform Device Macro

Add a device macro to your platform header
(`wolfHAL/platform/<vendor>/<device>.h`) so that board configs can instantiate
devices without knowing the register addresses or driver symbols:

```c
#define WHAL_MYPLATFORM_FOO_DEVICE \
    .regmap = { .base = 0x40000000, .size = 0x400 }, \
    .driver = &whal_MyplatformFoo_Driver
```

---

## Clock

Header: `wolfHAL/clock/clock.h`

The clock driver manages system clock sources and peripheral clock gating. This
is typically one of the most complex drivers because it must configure
oscillators, PLLs, clock dividers, and peripheral clock gates — all with strict
ordering requirements.

### Init

Configure and start the system clock source. This usually involves:

- Configuring the clock source (oscillator parameters, PLL multipliers and
  dividers, etc.)
- Enabling the clock source and waiting for it to stabilize (e.g., polling a
  PLL lock bit or oscillator ready flag)
- Switching the system clock mux to the new source
- Configuring any required clock dividers (CPU, bus, peripheral)

The board is responsible for setting flash wait states and enabling power
supplies before calling Init. The clock driver should only touch clock
registers.

The configuration struct should contain all parameters needed to fully describe
the desired clock tree (source selection, divider values, PLL coefficients,
etc.).

### Deinit

Shut down the clock source safely. This typically means:

- Switching back to a safe default clock source (e.g., an internal RC
  oscillator) before disabling the active source
- Disabling PLLs or high-speed oscillators

The board is responsible for reducing flash wait states after Deinit returns.

### Enable

Enable a peripheral clock gate. The `clk` parameter is an opaque pointer to a
clock descriptor that identifies which peripheral clock to enable. The
descriptor typically contains a register offset and bit mask so the driver can
set the correct enable bit in the appropriate clock gating register.

Some platforms have multiple clock domains (e.g., bus clocks and peripheral
generator clocks) that both need to be enabled for a peripheral to function. The
driver should handle all of these.

### Disable

Disable a peripheral clock gate. The inverse of Enable — clear the enable bit(s)
for the given clock descriptor.

### GetRate

Report the current system clock frequency in Hz. The driver should compute this
from the configured source, PLL coefficients, and divider settings. Store the
result in the output pointer.

---

## GPIO

Header: `wolfHAL/gpio/gpio.h`

The GPIO driver configures and controls general-purpose I/O pins. The
configuration describes a table of pins, and the API operates on pins by their
index in that table (not raw hardware pin/port numbers).

### Init

Configure all pins described in the device's configuration. For each pin this
typically involves:

- Setting the pin mode (input, output, alternate function, analog)
- Configuring output type (push-pull or open-drain), speed, and pull
  resistors as applicable
- Setting the alternate function mux if the pin is in alternate function mode
  (e.g., for UART TX/RX or SPI signals)

The board must enable GPIO port clocks before calling Init.

If any pin configuration fails, Init should stop and return an error.

### Deinit

Reset GPIO pin configurations as needed. The board is responsible for
disabling GPIO port clocks after Deinit.

### Get

Read the current state of a pin. The `pin` parameter is an index into the
configured pin table, not a raw hardware pin number. The driver should look up
the actual port and pin from the configuration.

For output pins, read the output data register (the value being driven). For
input pins, read the input data register (the value being sampled from the
pad). Store the result (0 or 1) in the output pointer.

### Set

Drive a pin to the given value (0 or 1). The `pin` parameter is an index into
the configured pin table. The driver writes to the output data register for the
corresponding port and pin.

---

## UART

Header: `wolfHAL/uart/uart.h`

The UART driver provides basic serial transmit and receive. All operations are
blocking — Send does not return until all bytes have been transmitted, and Recv
does not return until all requested bytes have been received.

### Init

Configure and enable the UART peripheral:

- Write the baud rate register value from the configuration. The board
  pre-computes this value from the clock frequency and desired baud rate
  (e.g., `BRR = clockFreq / baud`)
- Configure word length, stop bits, and parity as needed
- Enable the transmitter and receiver
- Enable the UART peripheral

The board must enable the peripheral clock before calling Init.

On platforms with synchronization requirements (e.g., Microchip SERCOM), the
driver must poll synchronization busy flags after writing to certain registers
before proceeding.

### Deinit

Disable the UART peripheral:

- Disable the transmitter and receiver
- Clear the baud rate register

### Send

Transmit `dataSz` bytes from the provided buffer. For each byte:

1. Poll the transmit-ready flag (TX empty / data register empty) until the
   hardware is ready to accept a byte
2. Write the byte to the transmit data register

After sending all bytes, poll the transmission-complete flag to ensure the last
byte has fully shifted out before returning.

### Recv

Receive `dataSz` bytes into the provided buffer. For each byte:

1. Poll the receive-ready flag (RX not empty / receive complete) until a byte
   is available
2. Read the byte from the receive data register and store it in the buffer

### SendAsync

Start a non-blocking transmit. Returns immediately after initiating the
transfer. The buffer must remain valid until the transfer completes. The
driver signals completion through a platform-specific mechanism.

Drivers that do not support async should set SendAsync to NULL in the vtable.
The dispatch layer returns WHAL_EINVAL when the caller tries to use a NULL
async function.

### RecvAsync

Start a non-blocking receive. Returns immediately after initiating the
transfer. The buffer must remain valid until the transfer completes.

The async variants are optional — a driver vtable only needs to populate
them if the platform supports non-blocking transfers. Polled-only drivers
leave these NULL.

---

## IRQ

Header: `wolfHAL/irq/irq.h`

The IRQ driver controls an interrupt controller. It provides a
platform-independent way to enable and disable individual interrupt lines.

### Init

Initialize the interrupt controller.

### Deinit

Shut down the interrupt controller.

### Enable

Enable an interrupt line. The `irqCfg` parameter is platform-specific and
can contain settings such as priority. Pass NULL for defaults.

### Disable

Disable an interrupt line.

---

## SPI

Header: `wolfHAL/spi/spi.h`

The SPI driver provides serial peripheral interface communication. A
communication session is bracketed by `StartCom` / `EndCom`, which configure
the peripheral for a specific mode and speed. All transfers within a session
use the same settings, allowing the bus to be shared between devices with
different configurations by starting a new session for each device.

### Init

Perform one-time SPI peripheral configuration that remains fixed for the
lifetime of the device. Do not configure mode, baud rate, or data size
here — these are applied per-session via `StartCom`.

The board must enable the peripheral clock before calling Init. The
configuration struct should include the peripheral clock frequency so the
driver can compute baud rate prescalers.

### Deinit

Disable the SPI peripheral.

### StartCom

Begin a communication session. Configures the peripheral from the
platform-independent `whal_Spi_ComCfg` struct:

- `freq` — bus frequency in Hz
- `mode` — SPI mode (CPOL/CPHA)
- `wordSz` — word size in bits (e.g. 8)
- `dataLines` — number of data lines (1 for standard SPI)

The driver should disable the peripheral, apply the new settings (mode, baud
rate prescaler, data size, FIFO threshold), and re-enable it. Return
`WHAL_EINVAL` if a requested setting is not supported by the hardware.

### EndCom

End the current communication session by disabling the peripheral.

### SendRecv

Perform a full-duplex SPI transfer. This is the only transfer function — there
are no separate Send or Recv operations.

The driver clocks `max(txLen, rxLen)` bytes:

- When `tx` is NULL or exhausted, send `0xFF` for remaining clocks
- When `rx` is NULL or exhausted, discard received bytes
- Every TX byte produces an RX byte; always drain the RX FIFO to prevent
  overflow

After the loop, wait for the bus to go idle before returning.

---

## Flash

Header: `wolfHAL/flash/flash.h`

The flash driver provides access to on-chip or external flash memory. Flash
has specific constraints around alignment, erase-before-write, and region
protection that the driver must handle.

### Init

Initialize the flash controller. This may involve clearing error flags or
releasing hardware mutex locks. The board must enable the flash interface clock
before calling Init.

### Deinit

Release flash controller resources.

### Lock

Write-protect a flash region to prevent modification. On some platforms this is
a global lock (the `addr` and `len` parameters are ignored and the entire flash
is locked). On others it may protect specific regions. After locking, Write and
Erase operations on the protected region should fail until Unlock is called.

The implementation varies significantly by platform — some use an unlock key
sequence (where Lock simply sets a lock bit), while others have dedicated
write-protect registers for individual regions.

### Unlock

Remove write protection to allow Write and Erase operations. On platforms that
use a key sequence, this typically involves writing two specific magic values to
a key register in the correct order. An incorrect sequence may trigger a bus
fault (this is a hardware security feature).

### Read

Read data from flash. On most platforms, flash is memory-mapped, so this is a
straightforward memory copy from the flash address into the provided buffer. No
special flash controller interaction is needed.

Some platforms may require acquiring a mutex or performing cache maintenance
before reading.

### Write

Program data into flash starting at the given address. The caller must ensure
the region is unlocked and erased before writing (flash can only transition bits
from 1 to 0; erasing sets all bits to 1).

Key constraints:
- **Alignment**: writes must be aligned to the platform's programming unit
  (e.g., 8-byte double-word on STM32, 8 or 32 bytes on PIC32CZ). The driver
  should validate alignment and return an error if misaligned.
- **Programming unit**: the driver writes data in hardware-defined chunks. Some
  platforms support multiple write sizes (e.g., single double-word vs. quad
  double-word) and the driver can optimize by using larger writes when
  alignment permits.
- **Busy polling**: after each write, poll the flash controller's busy flag
  until the operation completes.
- **Error flags**: clear any pending error flags before starting, and check for
  new errors after each operation.

### Erase

Erase a flash region. Flash erase operates at sector/page granularity
(typically 4 KB). The driver should:

- Calculate the page range covering the requested address and size
- Erase each page individually, polling for completion between pages
- Validate that the region is unlocked before erasing

The `addr` does not need to be page-aligned — the driver should erase all pages
that overlap with the requested range. Bus-device flash drivers (e.g., SPI-NOR)
may enforce stricter alignment requirements where the underlying hardware
requires aligned erase addresses.

---

## Block

Header: `wolfHAL/block/block.h`

The block driver provides access to block-addressable storage devices such as
SD cards and eMMC. Unlike flash, block devices are addressed by block number
rather than byte address, and all operations work in units of fixed-size blocks
(e.g. 512 bytes).

Block drivers are bus-device drivers — they call their underlying bus driver
(SPI, SDIO) to communicate with the storage device. This is the expected
exception to the no-cross-driver-calls rule.

### Init

Initialize the storage device. This includes any device-specific initialization
sequence required to bring the device into a usable state. The board must have
already initialized the bus driver and enabled the relevant clocks and GPIOs
before calling Init.

### Deinit

Release the storage device.

### Read

Read one or more blocks from the device into a buffer. The driver should handle
both single-block and multi-block reads based on the block count.

### Write

Write one or more blocks from a buffer to the device. The driver should handle
both single-block and multi-block writes, and wait for the device to finish
programming before returning.

### Erase

Erase a range of blocks on the device.

---

## Timer

Header: `wolfHAL/timer/timer.h`

The timer driver provides periodic tick or counter functionality. The most
common implementation is the ARM Cortex-M SysTick timer, which provides a
simple periodic interrupt for system timekeeping.

### Init

Configure the timer hardware but do **not** start it. This includes:

- Setting the reload value (determines the tick interval:
  interval = reload / clock_frequency)
- Selecting the clock source (e.g., CPU clock vs. external reference)
- Enabling or disabling the tick interrupt

The timer should not begin counting until Start is called.

### Deinit

Stop the timer and release resources.

### Start

Start the timer counting. The timer begins decrementing from the configured
reload value, generating interrupts (if enabled) each time it reaches zero and
reloading automatically.

### Stop

Stop the timer without resetting it. The counter holds its current value.

### Reset

Reset the timer counter back to its initialized state.

---

## RNG

Header: `wolfHAL/rng/rng.h`

The RNG driver provides access to a hardware random number generator. The
hardware typically uses an analog entropy source to produce true random numbers.

### Init

Initialize the RNG hardware. The board must enable the peripheral clock (and
any additional clock sources the RNG's entropy source requires) before calling
Init.

### Deinit

Shut down the RNG hardware.

### Generate

Fill the provided buffer with random data. The driver should:

1. Enable the RNG peripheral
2. Loop until the buffer is filled:
   - Check for hardware errors (seed errors, clock errors). If the entropy
     source has failed, disable the RNG and return `WHAL_EHARDWARE`
   - Poll the data-ready flag until a new random word is available
   - Read the random word (typically 32 bits) and extract as many bytes as
     needed into the output buffer
3. Disable the RNG peripheral before returning

The RNG is enabled only for the duration of the Generate call to minimize power
consumption and avoid unnecessary entropy source wear.

---

## Crypto

Header: `wolfHAL/crypto/crypto.h`

The crypto driver provides access to hardware cryptographic accelerators. Unlike
other device types, the crypto driver uses an **ops table** dispatch model
instead of a fixed vtable — each supported algorithm is a function pointer in a
board-defined ops table, indexed by a board-defined enum. This allows different
platforms to expose different subsets of algorithms without changing the generic
interface.

### Device Struct

The crypto device struct extends the standard model with an ops table:

```c
struct whal_Crypto {
    const whal_Regmap regmap;
    const whal_CryptoDriver *driver;
    const whal_Crypto_OpFunc *ops;
    size_t opsCount;
    const void *cfg;
};
```

The `driver` vtable handles Init/Deinit. The `ops` table maps algorithm indices
to operation functions. The board defines the enum values and populates the ops
table.

### Init / Deinit

The board must enable the peripheral clock before calling Init. Deinit should
disable the crypto accelerator peripheral.

### Operations

Each operation function has the signature:

```c
whal_Error myOp(whal_Crypto *cryptoDev, void *opArgs);
```

The `opArgs` parameter is a pointer to an algorithm-specific argument struct
(e.g., `whal_Crypto_AesEcbArgs`, `whal_Crypto_AesGcmArgs`). The operation
function casts it to the correct type. See `wolfHAL/crypto/crypto.h` for the
full set of argument structs.

### Board Integration

The board defines an enum of supported operations and a corresponding ops table:

```c
enum {
    BOARD_CRYPTO_AES_ECB,
    BOARD_CRYPTO_AES_CBC,
    BOARD_CRYPTO_OP_COUNT,
};

static const whal_Crypto_OpFunc cryptoOps[BOARD_CRYPTO_OP_COUNT] = {
    [BOARD_CRYPTO_AES_ECB] = whal_Stm32wbAes_AesEcb,
    [BOARD_CRYPTO_AES_CBC] = whal_Stm32wbAes_AesCbc,
};
```

Callers use `whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_ECB, &args)` to
invoke an operation.

---

## Supply

Header: `wolfHAL/supply/supply.h`

The supply driver controls power rails and voltage regulators. This is used on
platforms where certain peripherals (e.g., PLLs, analog blocks) require
explicit power supply enable before they can be configured.

### Init

Initialize the supply controller hardware. On many platforms this is a no-op
since the supply controller is always available.

### Deinit

Shut down the supply controller.

### Enable

Enable a specific power supply output. The `supply` parameter is an opaque
pointer to a supply descriptor (typically containing a register offset and bit
mask). The driver sets the appropriate enable bit in the supply control
register.

The board calls Supply Enable before initializing peripherals that depend on
the supply — for example, enabling a PLL's analog voltage regulator before
calling Clock Init.

### Disable

Disable a specific power supply output. The inverse of Enable.

---

## Ethernet

Header: `wolfHAL/eth/eth.h`

The Ethernet driver controls a MAC (Media Access Controller) with an integrated
DMA engine. It manages descriptor rings in RAM for transmit and receive, handles
MDIO bus access for PHY communication, and configures the MAC for the negotiated
link speed and duplex.

### Init

Initialize the MAC, DMA, and MTL. Set up TX and RX descriptor rings in RAM,
program the MAC address, and configure DMA bus mode and burst lengths. Does NOT
enable TX/RX — call Start for that. The descriptor and buffer memory must be
pre-allocated by the board and passed via the config struct. Validate all config
fields (descriptor counts > 0, buffer pointers non-NULL, buffer sizes > 0).

### Deinit

Perform a DMA software reset to clear all state.

### Start

Configure the MAC speed and duplex to match the PHY, enable MAC TX/RX, start
the DMA TX and RX engines, and kick the RX DMA by writing the tail pointer.
Speed and duplex are passed as parameters — the board reads these from the PHY
driver before calling Start.

### Stop

Stop DMA TX and RX engines, then disable MAC TX and RX.

### Send

Transmit a single Ethernet frame. Find the next available TX descriptor (OWN=0),
copy the frame into the pre-allocated TX buffer, fill in the descriptor fields
(buffer address, length, OWN=1, FD, LD), and write the DMA tail pointer to kick
transmission. Return WHAL_ENOTREADY if no descriptor is available. Validate that
the frame length does not exceed the TX buffer size.

### Recv

Receive a single Ethernet frame by polling. Check the next RX descriptor — if
OWN=0, DMA has written a frame. Read the packet length from the descriptor, copy
the frame data to the caller's buffer, re-arm the descriptor (set OWN=1), and
update the tail pointer. Return WHAL_ENOTREADY if no frame is available, or
WHAL_EHARDWARE if the error summary bit is set.

### MdioRead

Read a 16-bit PHY register via the MDIO management bus. Write the PHY address,
register address, and read command to the MDIO address register, poll for
completion, then read the data register. Use a timeout to avoid infinite hang
if the PHY is not responding.

### MdioWrite

Write a 16-bit value to a PHY register via MDIO. Write the data first, then
issue the write command and poll for completion with a timeout.

---

## DMA

Header: `wolfHAL/dma/dma.h`

The DMA driver controls a DMA controller. A single device instance represents
one controller, and individual channels are identified by index. Channel
configuration is platform-specific and passed as an opaque pointer.

DMA is a service peripheral — peripheral drivers (UART, SPI) consume it
internally. The application never calls the DMA API directly. Peripheral
drivers receive a `whal_Dma` pointer and channel number through their
configuration struct and use them to set up transfers.

### Init

Initialize the DMA controller. Clear any pending interrupt flags and reset
controller state. The board must enable the DMA controller clock before calling
Init.

### Deinit

Shut down the DMA controller.

### Configure

Configure a DMA channel for transfers. The `chCfg` parameter is a
platform-specific struct containing:

- Transfer direction (memory-to-peripheral, peripheral-to-memory, etc.)
- Source and destination addresses
- Transfer width (8, 16, 32 bit)
- Buffer address and length
- Burst size (if supported)
- Peripheral request mapping (e.g., DMAMUX request ID)

The DMA driver does not store callbacks. Instead, the board defines ISR
entries in the vector table and calls the driver's IRQ handler (e.g.,
`whal_Stm32wbDma_IRQHandler()`), passing a callback and context pointer.
The IRQ handler checks and clears the interrupt flags, then invokes the
callback. Peripheral drivers expose their completion callbacks for the
board to wire up (e.g., `whal_Stm32wbUartDma_TxCallback`).

Configure sets up all channel registers but does not start the transfer.
Call Start to begin. A channel can be reconfigured between transfers (e.g.,
to change the buffer address and length) by calling Configure again.

### Start

Start a previously configured DMA channel. This enables the channel,
beginning the transfer. The channel must have been configured via Configure
before calling Start.

### Stop

Stop a DMA channel. This aborts any in-progress transfer and disables the
channel. The peripheral driver should call Stop in its cleanup path or when
a transfer needs to be cancelled.

---

## EthPhy

Header: `wolfHAL/eth_phy/eth_phy.h`

The Ethernet PHY driver handles link negotiation and status for an external PHY
chip connected to a MAC via the MDIO bus. The PHY device struct holds a pointer
to its parent MAC (for MDIO access) and the PHY address on the bus. Different
PHY chips (e.g., LAN8742A, DP83848) have different vendor-specific registers
but share the same API.

### Init

Reset the PHY via software reset (BCR bit 15), wait for the reset bit to
self-clear, then enable autonegotiation. Does not block waiting for link — the
board or application polls GetLinkState separately.

### Deinit

Power down the PHY or release resources. May be a no-op on simple PHYs.

### GetLinkState

Read the current link status, negotiated speed, and duplex mode. The IEEE 802.3
BSR register (reg 1) link bit is latching-low — read it twice and use the second
result for current status. Speed and duplex are read from a vendor-specific
status register (e.g., register 0x1F on LAN8742A). Return speed as 10 or 100,
and duplex as WHAL_ETH_DUPLEX_HALF or WHAL_ETH_DUPLEX_FULL.
