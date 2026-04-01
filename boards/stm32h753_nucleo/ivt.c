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
void SVC_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));

/* STM32H753 peripheral interrupts (RM0433 Table 62) */
void WWDG_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PVD_AVD_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RTC_WKUP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FLASH_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RCC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void ADC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FDCAN1_IT0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FDCAN2_IT0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FDCAN1_IT1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FDCAN2_IT1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI9_5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_BRK_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_UP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_CC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C1_EV_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C1_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C2_EV_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C2_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USART1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USART2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USART3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI15_10_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RTC_Alarm_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM8_BRK_TIM12_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM8_UP_TIM13_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM8_CC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FMC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SDMMC1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void UART4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void UART5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM6_DAC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void ETH_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void ETH_WKUP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FDCAN_CAL_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USART6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C3_EV_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C3_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_HS_EP1_OUT_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_HS_EP1_IN_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_HS_WKUP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_HS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DCMI_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void CRYP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HASH_RNG_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FPU_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void UART7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void UART8_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SAI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LTDC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LTDC_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2D_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SAI2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void QUADSPI_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void CEC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C4_EV_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C4_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPDIF_RX_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_FS_EP1_OUT_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_FS_EP1_IN_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void OTG_FS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMAMUX1_OVR_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_Master_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_TIMA_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_TIMB_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_TIMC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_TIMD_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_TIME_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HRTIM1_FLT_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DFSDM1_FLT0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DFSDM1_FLT1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DFSDM1_FLT2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DFSDM1_FLT3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SAI3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SWPMI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM15_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM16_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM17_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void MDIOS_WKUP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void MDIOS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void JPEG_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void MDMA_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SDMMC2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HSEM1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BDMA_Channel7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void COMP1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPUART1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void CRS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void ECC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SAI4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void WAKEUP_PIN_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));

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
    RESERVED,               /* Reserved */
    RESERVED,               /* Reserved */
    RESERVED,               /* Reserved */
    RESERVED,               /* Reserved */
    SVC_Handler,
    DebugMon_Handler,
    RESERVED,               /* Reserved */
    PendSV_Handler,
    SysTick_Handler,
    /* STM32H753 peripheral interrupts */
    WWDG_IRQHandler,                /* 0 */
    PVD_AVD_IRQHandler,             /* 1 */
    TAMP_STAMP_IRQHandler,          /* 2 */
    RTC_WKUP_IRQHandler,            /* 3 */
    FLASH_IRQHandler,               /* 4 */
    RCC_IRQHandler,                 /* 5 */
    EXTI0_IRQHandler,               /* 6 */
    EXTI1_IRQHandler,               /* 7 */
    EXTI2_IRQHandler,               /* 8 */
    EXTI3_IRQHandler,               /* 9 */
    EXTI4_IRQHandler,               /* 10 */
    DMA1_Stream0_IRQHandler,        /* 11 */
    DMA1_Stream1_IRQHandler,        /* 12 */
    DMA1_Stream2_IRQHandler,        /* 13 */
    DMA1_Stream3_IRQHandler,        /* 14 */
    DMA1_Stream4_IRQHandler,        /* 15 */
    DMA1_Stream5_IRQHandler,        /* 16 */
    DMA1_Stream6_IRQHandler,        /* 17 */
    ADC_IRQHandler,                 /* 18 */
    FDCAN1_IT0_IRQHandler,          /* 19 */
    FDCAN2_IT0_IRQHandler,          /* 20 */
    FDCAN1_IT1_IRQHandler,          /* 21 */
    FDCAN2_IT1_IRQHandler,          /* 22 */
    EXTI9_5_IRQHandler,             /* 23 */
    TIM1_BRK_IRQHandler,            /* 24 */
    TIM1_UP_IRQHandler,             /* 25 */
    TIM1_TRG_COM_IRQHandler,        /* 26 */
    TIM1_CC_IRQHandler,             /* 27 */
    TIM2_IRQHandler,                /* 28 */
    TIM3_IRQHandler,                /* 29 */
    TIM4_IRQHandler,                /* 30 */
    I2C1_EV_IRQHandler,             /* 31 */
    I2C1_ER_IRQHandler,             /* 32 */
    I2C2_EV_IRQHandler,             /* 33 */
    I2C2_ER_IRQHandler,             /* 34 */
    SPI1_IRQHandler,                /* 35 */
    SPI2_IRQHandler,                /* 36 */
    USART1_IRQHandler,              /* 37 */
    USART2_IRQHandler,              /* 38 */
    USART3_IRQHandler,              /* 39 */
    EXTI15_10_IRQHandler,           /* 40 */
    RTC_Alarm_IRQHandler,           /* 41 */
    RESERVED,                       /* 42 */
    TIM8_BRK_TIM12_IRQHandler,      /* 43 */
    TIM8_UP_TIM13_IRQHandler,       /* 44 */
    TIM8_TRG_COM_TIM14_IRQHandler,  /* 45 */
    TIM8_CC_IRQHandler,             /* 46 */
    DMA1_Stream7_IRQHandler,        /* 47 */
    FMC_IRQHandler,                 /* 48 */
    SDMMC1_IRQHandler,              /* 49 */
    TIM5_IRQHandler,                /* 50 */
    SPI3_IRQHandler,                /* 51 */
    UART4_IRQHandler,               /* 52 */
    UART5_IRQHandler,               /* 53 */
    TIM6_DAC_IRQHandler,            /* 54 */
    TIM7_IRQHandler,                /* 55 */
    DMA2_Stream0_IRQHandler,        /* 56 */
    DMA2_Stream1_IRQHandler,        /* 57 */
    DMA2_Stream2_IRQHandler,        /* 58 */
    DMA2_Stream3_IRQHandler,        /* 59 */
    DMA2_Stream4_IRQHandler,        /* 60 */
    ETH_IRQHandler,                 /* 61 */
    ETH_WKUP_IRQHandler,            /* 62 */
    FDCAN_CAL_IRQHandler,           /* 63 */
    RESERVED,                       /* 64 */
    RESERVED,                       /* 65 */
    RESERVED,                       /* 66 */
    RESERVED,                       /* 67 */
    DMA2_Stream5_IRQHandler,        /* 68 */
    DMA2_Stream6_IRQHandler,        /* 69 */
    DMA2_Stream7_IRQHandler,        /* 70 */
    USART6_IRQHandler,              /* 71 */
    I2C3_EV_IRQHandler,             /* 72 */
    I2C3_ER_IRQHandler,             /* 73 */
    OTG_HS_EP1_OUT_IRQHandler,      /* 74 */
    OTG_HS_EP1_IN_IRQHandler,       /* 75 */
    OTG_HS_WKUP_IRQHandler,         /* 76 */
    OTG_HS_IRQHandler,              /* 77 */
    DCMI_IRQHandler,                /* 78 */
    CRYP_IRQHandler,                /* 79 */
    HASH_RNG_IRQHandler,            /* 80 */
    FPU_IRQHandler,                 /* 81 */
    UART7_IRQHandler,               /* 82 */
    UART8_IRQHandler,               /* 83 */
    SPI4_IRQHandler,                /* 84 */
    SPI5_IRQHandler,                /* 85 */
    SPI6_IRQHandler,                /* 86 */
    SAI1_IRQHandler,                /* 87 */
    LTDC_IRQHandler,                /* 88 */
    LTDC_ER_IRQHandler,             /* 89 */
    DMA2D_IRQHandler,               /* 90 */
    SAI2_IRQHandler,                /* 91 */
    QUADSPI_IRQHandler,             /* 92 */
    LPTIM1_IRQHandler,              /* 93 */
    CEC_IRQHandler,                 /* 94 */
    I2C4_EV_IRQHandler,             /* 95 */
    I2C4_ER_IRQHandler,             /* 96 */
    SPDIF_RX_IRQHandler,            /* 97 */
    OTG_FS_EP1_OUT_IRQHandler,      /* 98 */
    OTG_FS_EP1_IN_IRQHandler,       /* 99 */
    OTG_FS_WKUP_IRQHandler,         /* 100 */
    OTG_FS_IRQHandler,              /* 101 */
    DMAMUX1_OVR_IRQHandler,         /* 102 */
    HRTIM1_Master_IRQHandler,       /* 103 */
    HRTIM1_TIMA_IRQHandler,         /* 104 */
    HRTIM1_TIMB_IRQHandler,         /* 105 */
    HRTIM1_TIMC_IRQHandler,         /* 106 */
    HRTIM1_TIMD_IRQHandler,         /* 107 */
    HRTIM1_TIME_IRQHandler,         /* 108 */
    HRTIM1_FLT_IRQHandler,          /* 109 */
    DFSDM1_FLT0_IRQHandler,         /* 110 */
    DFSDM1_FLT1_IRQHandler,         /* 111 */
    DFSDM1_FLT2_IRQHandler,         /* 112 */
    DFSDM1_FLT3_IRQHandler,         /* 113 */
    SAI3_IRQHandler,                /* 114 */
    SWPMI1_IRQHandler,              /* 115 */
    TIM15_IRQHandler,               /* 116 */
    TIM16_IRQHandler,               /* 117 */
    TIM17_IRQHandler,               /* 118 */
    MDIOS_WKUP_IRQHandler,          /* 119 */
    MDIOS_IRQHandler,               /* 120 */
    JPEG_IRQHandler,                /* 121 */
    MDMA_IRQHandler,                /* 122 */
    RESERVED,                       /* 123 */
    SDMMC2_IRQHandler,              /* 124 */
    HSEM1_IRQHandler,               /* 125 */
    RESERVED,                       /* 126 */
    BDMA_Channel0_IRQHandler,       /* 127 */
    BDMA_Channel1_IRQHandler,       /* 128 */
    BDMA_Channel2_IRQHandler,       /* 129 */
    BDMA_Channel3_IRQHandler,       /* 130 */
    BDMA_Channel4_IRQHandler,       /* 131 */
    BDMA_Channel5_IRQHandler,       /* 132 */
    BDMA_Channel6_IRQHandler,       /* 133 */
    BDMA_Channel7_IRQHandler,       /* 134 */
    COMP1_IRQHandler,               /* 135 */
    LPTIM2_IRQHandler,              /* 136 */
    LPTIM3_IRQHandler,              /* 137 */
    LPTIM4_IRQHandler,              /* 138 */
    LPTIM5_IRQHandler,              /* 139 */
    LPUART1_IRQHandler,             /* 140 */
    RESERVED,                       /* 141 */
    CRS_IRQHandler,                 /* 142 */
    ECC_IRQHandler,                 /* 143 */
    SAI4_IRQHandler,                /* 144 */
    RESERVED,                       /* 145 */
    RESERVED,                       /* 146 */
    WAKEUP_PIN_IRQHandler,          /* 147 */
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
