#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/timer/timer.h>
#include "../test.h"

/*
 * Mock drivers that return SUCCESS for all operations.
 * Used to verify the generic dispatch layer.
 */

static whal_Error mockClockInit(whal_Clock *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockClockDeinit(whal_Clock *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockClockEnable(whal_Clock *d, const void *c) { (void)d; (void)c; return WHAL_SUCCESS; }
static whal_Error mockClockDisable(whal_Clock *d, const void *c) { (void)d; (void)c; return WHAL_SUCCESS; }
static whal_Error mockClockGetRate(whal_Clock *d, size_t *r) { (void)d; *r = 64000000; return WHAL_SUCCESS; }

static const whal_ClockDriver mockClockDriver = {
    .Init = mockClockInit,
    .Deinit = mockClockDeinit,
    .Enable = mockClockEnable,
    .Disable = mockClockDisable,
    .GetRate = mockClockGetRate,
};

static whal_Error mockGpioInit(whal_Gpio *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockGpioDeinit(whal_Gpio *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockGpioGet(whal_Gpio *d, size_t p, size_t *v) { (void)d; (void)p; *v = 1; return WHAL_SUCCESS; }
static whal_Error mockGpioSet(whal_Gpio *d, size_t p, size_t v) { (void)d; (void)p; (void)v; return WHAL_SUCCESS; }

static const whal_GpioDriver mockGpioDriver = {
    .Init = mockGpioInit,
    .Deinit = mockGpioDeinit,
    .Get = mockGpioGet,
    .Set = mockGpioSet,
};

static whal_Error mockUartInit(whal_Uart *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockUartDeinit(whal_Uart *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockUartSend(whal_Uart *d, const uint8_t *data, size_t sz) { (void)d; (void)data; (void)sz; return WHAL_SUCCESS; }
static whal_Error mockUartRecv(whal_Uart *d, uint8_t *data, size_t sz) { (void)d; (void)data; (void)sz; return WHAL_SUCCESS; }

static const whal_UartDriver mockUartDriver = {
    .Init = mockUartInit,
    .Deinit = mockUartDeinit,
    .Send = mockUartSend,
    .Recv = mockUartRecv,
};

static whal_Error mockFlashInit(whal_Flash *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockFlashDeinit(whal_Flash *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockFlashLock(whal_Flash *d, size_t a, size_t l) { (void)d; (void)a; (void)l; return WHAL_SUCCESS; }
static whal_Error mockFlashUnlock(whal_Flash *d, size_t a, size_t l) { (void)d; (void)a; (void)l; return WHAL_SUCCESS; }
static whal_Error mockFlashRead(whal_Flash *d, size_t a, uint8_t *data, size_t sz) { (void)d; (void)a; (void)data; (void)sz; return WHAL_SUCCESS; }
static whal_Error mockFlashWrite(whal_Flash *d, size_t a, const uint8_t *data, size_t sz) { (void)d; (void)a; (void)data; (void)sz; return WHAL_SUCCESS; }
static whal_Error mockFlashErase(whal_Flash *d, size_t a, size_t sz) { (void)d; (void)a; (void)sz; return WHAL_SUCCESS; }

static const whal_FlashDriver mockFlashDriver = {
    .Init = mockFlashInit,
    .Deinit = mockFlashDeinit,
    .Lock = mockFlashLock,
    .Unlock = mockFlashUnlock,
    .Read = mockFlashRead,
    .Write = mockFlashWrite,
    .Erase = mockFlashErase,
};

static whal_Error mockTimerInit(whal_Timer *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockTimerDeinit(whal_Timer *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockTimerStart(whal_Timer *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockTimerStop(whal_Timer *d) { (void)d; return WHAL_SUCCESS; }
static whal_Error mockTimerReset(whal_Timer *d) { (void)d; return WHAL_SUCCESS; }

static const whal_TimerDriver mockTimerDriver = {
    .Init = mockTimerInit,
    .Deinit = mockTimerDeinit,
    .Start = mockTimerStart,
    .Stop = mockTimerStop,
    .Reset = mockTimerReset,
};

/* --- Clock dispatch tests --- */

static void test_clock_null_dev(void)
{
    WHAL_ASSERT_EQ(whal_Clock_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Enable(NULL, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Disable(NULL, NULL), WHAL_EINVAL);
    size_t rate;
    WHAL_ASSERT_EQ(whal_Clock_GetRate(NULL, &rate), WHAL_EINVAL);
}

static void test_clock_null_driver(void)
{
    whal_Clock dev = { .driver = NULL };
    WHAL_ASSERT_EQ(whal_Clock_Init(&dev), WHAL_EINVAL);
}

static void test_clock_null_vtable_entry(void)
{
    static const whal_ClockDriver emptyDriver = { 0 };
    whal_Clock dev = { .driver = &emptyDriver };
    WHAL_ASSERT_EQ(whal_Clock_Init(&dev), WHAL_EINVAL);
}

static void test_clock_valid_dispatch(void)
{
    whal_Clock dev = { .driver = &mockClockDriver };
    WHAL_ASSERT_EQ(whal_Clock_Init(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Clock_Deinit(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Clock_Enable(&dev, NULL), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Clock_Disable(&dev, NULL), WHAL_SUCCESS);
    size_t rate;
    WHAL_ASSERT_EQ(whal_Clock_GetRate(&dev, &rate), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(rate, 64000000);
}

/* --- GPIO dispatch tests --- */

static void test_gpio_null_dev(void)
{
    WHAL_ASSERT_EQ(whal_Gpio_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Gpio_Set(NULL, 0, 0), WHAL_EINVAL);
    size_t val;
    WHAL_ASSERT_EQ(whal_Gpio_Get(NULL, 0, &val), WHAL_EINVAL);
}

static void test_gpio_null_driver(void)
{
    whal_Gpio dev = { .driver = NULL };
    WHAL_ASSERT_EQ(whal_Gpio_Init(&dev), WHAL_EINVAL);
}

static void test_gpio_valid_dispatch(void)
{
    whal_Gpio dev = { .driver = &mockGpioDriver };
    WHAL_ASSERT_EQ(whal_Gpio_Init(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Gpio_Set(&dev, 0, 1), WHAL_SUCCESS);
    size_t val;
    WHAL_ASSERT_EQ(whal_Gpio_Get(&dev, 0, &val), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(val, 1);
}

/* --- UART dispatch tests --- */

static void test_uart_null_dev(void)
{
    WHAL_ASSERT_EQ(whal_Uart_Init(NULL), WHAL_EINVAL);
    uint8_t buf[1];
    WHAL_ASSERT_EQ(whal_Uart_Send(NULL, buf, 1), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_Recv(NULL, buf, 1), WHAL_EINVAL);
}

static void test_uart_null_driver(void)
{
    whal_Uart dev = { .driver = NULL };
    WHAL_ASSERT_EQ(whal_Uart_Init(&dev), WHAL_EINVAL);
}

static void test_uart_valid_dispatch(void)
{
    whal_Uart dev = { .driver = &mockUartDriver };
    WHAL_ASSERT_EQ(whal_Uart_Init(&dev), WHAL_SUCCESS);
    uint8_t buf[4] = {0};
    WHAL_ASSERT_EQ(whal_Uart_Send(&dev, buf, sizeof(buf)), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Uart_Recv(&dev, buf, sizeof(buf)), WHAL_SUCCESS);
}

/* --- Flash dispatch tests --- */

static void test_flash_null_dev(void)
{
    WHAL_ASSERT_EQ(whal_Flash_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Flash_Lock(NULL, 0, 0), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Flash_Unlock(NULL, 0, 0), WHAL_EINVAL);
    uint8_t buf[1];
    WHAL_ASSERT_EQ(whal_Flash_Read(NULL, 0, buf, 1), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Flash_Write(NULL, 0, buf, 1), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Flash_Erase(NULL, 0, 1), WHAL_EINVAL);
}

static void test_flash_null_driver(void)
{
    whal_Flash dev = { .driver = NULL };
    WHAL_ASSERT_EQ(whal_Flash_Init(&dev), WHAL_EINVAL);
}

static void test_flash_valid_dispatch(void)
{
    whal_Flash dev = { .driver = &mockFlashDriver };
    WHAL_ASSERT_EQ(whal_Flash_Init(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Flash_Lock(&dev, 0, 0), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Flash_Unlock(&dev, 0, 0), WHAL_SUCCESS);
    uint8_t buf[4] = {0};
    WHAL_ASSERT_EQ(whal_Flash_Read(&dev, 0, buf, sizeof(buf)), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Flash_Write(&dev, 0, buf, sizeof(buf)), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Flash_Erase(&dev, 0, sizeof(buf)), WHAL_SUCCESS);
}

/* --- Timer dispatch tests --- */

static void test_timer_null_dev(void)
{
    WHAL_ASSERT_EQ(whal_Timer_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Timer_Start(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Timer_Stop(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Timer_Reset(NULL), WHAL_EINVAL);
}

static void test_timer_null_driver(void)
{
    whal_Timer dev = { .driver = NULL };
    WHAL_ASSERT_EQ(whal_Timer_Init(&dev), WHAL_EINVAL);
}

static void test_timer_valid_dispatch(void)
{
    whal_Timer dev = { .driver = &mockTimerDriver };
    WHAL_ASSERT_EQ(whal_Timer_Init(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Timer_Start(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Timer_Stop(&dev), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Timer_Reset(&dev), WHAL_SUCCESS);
}

void test_dispatch(void)
{
    WHAL_TEST_SUITE_START("dispatch");
    WHAL_TEST(test_clock_null_dev);
    WHAL_TEST(test_clock_null_driver);
    WHAL_TEST(test_clock_null_vtable_entry);
    WHAL_TEST(test_clock_valid_dispatch);
    WHAL_TEST(test_gpio_null_dev);
    WHAL_TEST(test_gpio_null_driver);
    WHAL_TEST(test_gpio_valid_dispatch);
    WHAL_TEST(test_uart_null_dev);
    WHAL_TEST(test_uart_null_driver);
    WHAL_TEST(test_uart_valid_dispatch);
    WHAL_TEST(test_flash_null_dev);
    WHAL_TEST(test_flash_null_driver);
    WHAL_TEST(test_flash_valid_dispatch);
    WHAL_TEST(test_timer_null_dev);
    WHAL_TEST(test_timer_null_driver);
    WHAL_TEST(test_timer_valid_dispatch);
    WHAL_TEST_SUITE_END();
}
