#include <stdint.h>

#ifndef rcc_lib
#define rcc_lib

//RCC - over here when we put the outer *, we're saying that I want to immediately start working in the memory
// and not work with a pointer that points to a memory, so in subsequent uses of RCC, I'm working with memory 
#define RCC 0x40023800

#define RCC_CR (*((volatile uint32_t *)(RCC + 0x00)))
#define RCC_PLLCFGR (*((volatile uint32_t *)(RCC + 0x04)))
#define RCC_CFGR (*((volatile uint32_t *)(RCC + 0x08)))

//AHB Peripheral enable
#define RCC_AHB1ENR (*((volatile uint32_t *)(RCC + 0x30)))
//APB1 Peripheral clock enable
#define RCC_APB1ENR (*((volatile uint32_t *)(RCC + 0x40)))
//APB2 Peripheral clock enable
#define RCC_APB2ENR (*((volatile uint32_t *)(RCC + 0x44)))

//PWR
#define PWR 0x40007000
#define PWR_CR (*((volatile uint32_t *)(PWR + 0x00)))
#define PWR_CSR (*((volatile uint32_t *)(PWR + 0x04)))

//Flash
#define FLASH 0x40023C00
#define FLASH_ACR (*((volatile uint32_t *)(FLASH + 0x00)))

void init_rcc_pll(void);

void turn_on_gpioa(void);

void turn_on_tim1(void);

void turn_on_dma2_clock(void);

#endif