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

### Register Access

wolfHAL provides register access helpers in `wolfHAL/regmap.h`:

- `whal_Reg_Write()` — write a full register value
- `whal_Reg_Read()` — read a full register value
- `whal_Reg_Update()` — read-modify-write a specific field within a register
- `whal_Reg_Get()` — read and extract a masked field from a register

Use `Write` and `Read` for whole-register access. Use `Update` and `Get` when
you need to modify or read individual fields within a register without
disturbing other bits.

### Avoiding Bloat

When a peripheral has multiple distinct operating modes or configurations,
consider splitting the driver into separate implementations rather than one
monolithic driver. For example, if a clock controller supports several different
clock sources, each with its own initialization logic, you can provide a
separate driver for each source. This way users only link the code they actually
need, and unused configurations are not compiled into the final binary.

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

- Setting flash wait states **before** increasing the clock frequency (critical
  for correct operation — the CPU will fault if flash is too slow for the new
  clock speed)
- Configuring the clock source (oscillator parameters, PLL multipliers and
  dividers, etc.)
- Enabling the clock source and waiting for it to stabilize (e.g., polling a
  PLL lock bit or oscillator ready flag)
- Switching the system clock mux to the new source
- Configuring any required clock dividers (CPU, bus, peripheral)

The configuration struct should contain all parameters needed to fully describe
the desired clock tree (source selection, divider values, PLL coefficients,
etc.).

### Deinit

Shut down the clock source safely. This typically means:

- Switching back to a safe default clock source (e.g., an internal RC
  oscillator) before disabling the active source
- Disabling PLLs or high-speed oscillators
- Reducing flash wait states to match the slower clock

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

- Enabling the clock for the pin's GPIO port (via the clock driver)
- Setting the pin mode (input, output, alternate function, analog)
- Configuring output type (push-pull or open-drain), speed, and pull
  resistors as applicable
- Setting the alternate function mux if the pin is in alternate function mode
  (e.g., for UART TX/RX or SPI signals)

If any pin configuration fails, Init should stop and return an error.

### Deinit

Disable GPIO port clocks. Pin registers do not need to be explicitly reset
since disabling the clock effectively resets the port.

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

- Enable the peripheral clock (via the clock driver)
- Query the clock frequency to calculate the baud rate register value. Some
  platforms require a simple division (e.g., BRR = clock / baud), while others
  use a more complex formula involving oversampling ratios
- Configure word length, stop bits, and parity as needed
- Enable the transmitter and receiver
- Enable the UART peripheral

On platforms with synchronization requirements (e.g., Microchip SERCOM), the
driver must poll synchronization busy flags after writing to certain registers
before proceeding.

### Deinit

Disable the UART peripheral:

- Disable the transmitter and receiver
- Clear the baud rate register
- Disable the peripheral clock

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

---

## SPI

Header: `wolfHAL/spi/spi.h`

The SPI driver provides serial peripheral interface communication. SPI
operations take an additional `spiComCfg` parameter that carries per-transfer
communication settings. This allows the same SPI bus to communicate with
multiple devices using different modes and speeds without reinitializing the
peripheral.

### Init

Configure and enable the SPI peripheral:

- Enable the peripheral clock
- Set master mode and configure slave select management (typically software-
  managed via GPIO)
- Set the data frame size (usually 8-bit)
- Do not configure mode or baud rate here — these are applied per-transfer via
  `spiComCfg`

### Deinit

Disable the SPI peripheral and its clock.

### SendRecv

Perform a full-duplex SPI transfer. This is the primary transfer function —
Send and Recv are convenience wrappers around it.

The `spiComCfg` parameter is an opaque pointer to a platform-specific
communication config that describes the SPI mode (CPOL/CPHA), baud rate, and
any other per-transfer settings. The driver should:

1. Apply the communication config (disable the peripheral, update mode and
   baud rate registers, re-enable)
2. Transfer `max(txLen, rxLen)` bytes. For each byte:
   - Write a byte from `tx` to the transmit register (if `tx` is provided)
   - Read a byte from the receive register into `rx` (if `rx` is provided)
3. Wait for the bus to go idle before returning

### Send

Transmit-only convenience wrapper. Calls SendRecv with no receive buffer.

### Recv

Receive-only convenience wrapper. Calls SendRecv, clocking out dummy bytes to
generate the SPI clock while capturing received data.

---

## Flash

Header: `wolfHAL/flash/flash.h`

The flash driver provides access to on-chip or external flash memory. Flash
has specific constraints around alignment, erase-before-write, and region
protection that the driver must handle.

### Init

Initialize the flash controller. This typically just enables the flash
interface clock. Some platforms may also need to clear error flags or release
hardware mutex locks.

### Deinit

Release flash controller resources and disable the clock.

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
that overlap with the requested range.

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

Initialize the RNG hardware. This usually involves enabling the peripheral
clock. Some platforms may require enabling additional clock sources that feed
the RNG's entropy source (e.g., a dedicated internal oscillator).

### Deinit

Shut down the RNG hardware and disable its clock.

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

Init should enable the peripheral clock. Deinit should disable the AES
peripheral and its clock.

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

Supply enable is often a prerequisite for other driver initialization — for
example, a PLL's analog voltage regulator must be enabled before the PLL can be
configured and locked.

### Disable

Disable a specific power supply output. The inverse of Enable.
