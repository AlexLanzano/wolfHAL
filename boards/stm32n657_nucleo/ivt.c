#include <stdint.h>
#include <stddef.h>

extern uint32_t _estack[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];

extern void main();

void __attribute__((naked,noreturn)) Default_Handler()
{
    while(1);
}

void Reset_Handler() __attribute__((weak));
void NMI_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HardFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void MemManage_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BusFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void UsageFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SecureFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SVC_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));

/* STM32N657 peripheral interrupts - minimal set */
void PVD_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DTS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RCC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FLASH_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI8_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI9_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI10_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI11_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI12_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI13_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI14_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI15_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void IWDG_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel8_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel9_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel10_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel11_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel12_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel13_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel14_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void GPDMA1_Channel15_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));

#define RESERVED Default_Handler

void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;

    for (size_t i = 0; i < n; i++)
        d[i] = s[i];

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    unsigned char v = (unsigned char)c;

    for (size_t i = 0; i < n; i++)
        p[i] = v;

    return s;
}

void (* const interrupt_vector_table[])() __attribute__((section(".isr_vector"))) = {
    (void (*)())_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    SecureFault_Handler,
    RESERVED,
    RESERVED,
    RESERVED,
    SVC_Handler,
    DebugMon_Handler,
    RESERVED,
    PendSV_Handler,
    SysTick_Handler,
    /* STM32N657 peripheral interrupts */
    PVD_IRQHandler,                     /* 0 */
    DTS_IRQHandler,                     /* 1 */
    RESERVED,                           /* 2 */
    RCC_IRQHandler,                     /* 3 */
    RESERVED,                           /* 4 */
    FLASH_IRQHandler,                   /* 5 */
    RESERVED,                           /* 6 */
    RESERVED,                           /* 7 */
    RESERVED,                           /* 8 */
    RESERVED,                           /* 9 */
    RESERVED,                           /* 10 */
    EXTI0_IRQHandler,                   /* 11 */
    EXTI1_IRQHandler,                   /* 12 */
    EXTI2_IRQHandler,                   /* 13 */
    EXTI3_IRQHandler,                   /* 14 */
    EXTI4_IRQHandler,                   /* 15 */
    EXTI5_IRQHandler,                   /* 16 */
    EXTI6_IRQHandler,                   /* 17 */
    EXTI7_IRQHandler,                   /* 18 */
    EXTI8_IRQHandler,                   /* 19 */
    EXTI9_IRQHandler,                   /* 20 */
    EXTI10_IRQHandler,                  /* 21 */
    EXTI11_IRQHandler,                  /* 22 */
    EXTI12_IRQHandler,                  /* 23 */
    EXTI13_IRQHandler,                  /* 24 */
    EXTI14_IRQHandler,                  /* 25 */
    EXTI15_IRQHandler,                  /* 26 */
    IWDG_IRQHandler,                    /* 27 */
    RESERVED,                           /* 28 */
    GPDMA1_Channel0_IRQHandler,         /* 29 */
    GPDMA1_Channel1_IRQHandler,         /* 30 */
    GPDMA1_Channel2_IRQHandler,         /* 31 */
    GPDMA1_Channel3_IRQHandler,         /* 32 */
    GPDMA1_Channel4_IRQHandler,         /* 33 */
    GPDMA1_Channel5_IRQHandler,         /* 34 */
    GPDMA1_Channel6_IRQHandler,         /* 35 */
    GPDMA1_Channel7_IRQHandler,         /* 36 */
    GPDMA1_Channel8_IRQHandler,         /* 37 */
    GPDMA1_Channel9_IRQHandler,         /* 38 */
    GPDMA1_Channel10_IRQHandler,        /* 39 */
    GPDMA1_Channel11_IRQHandler,        /* 40 */
    GPDMA1_Channel12_IRQHandler,        /* 41 */
    GPDMA1_Channel13_IRQHandler,        /* 42 */
    GPDMA1_Channel14_IRQHandler,        /* 43 */
    GPDMA1_Channel15_IRQHandler,        /* 44 */
};

void __attribute__((naked)) Reset_Handler()
{
    __asm__("ldr r0, =_estack\n\t"
            "mov sp, r0");

    /* Copy data section from flash to RAM */
    uint32_t data_section_size = _edata - _sdata;
    memcpy(_sdata, _sidata, data_section_size * 4);

    /* Zero out bss */
    uint32_t bss_section_size = _ebss - _sbss;
    memset(_sbss, 0, bss_section_size * 4);

    /* Set Interrupt Vector Table Offset */
    uint32_t *vtor = (uint32_t *)0xE000ED08;
    *vtor = (uint32_t)interrupt_vector_table;

    main();
}
