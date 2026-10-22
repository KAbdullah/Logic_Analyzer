#include <stdint.h>
#include "rcc.h"


void init_rcc_pll(void) {
  //Turn HSIO clock on
  RCC_CR |= (1 << 0);
  RCC_PLLCFGR = (RCC_PLLCFGR & ~(0b111111 << 0)) | (8U << 0);

  //Blocking until HSI starts
  while (!(RCC_CR & (1 << 1)));

  //Choose system clock to use HSI
  RCC_CFGR |= (0b00 << 0);

  //Blocking until sys switched to HSI
  while (1) {
    //Shift down by two, then 0b10 & 0b11 = 10, that means switched to PLL
    if (((RCC_CFGR >> 2) & 0x3) == 0x0) {
      break;
    }
  }

  //Reset values of PLL register then Set the multiplication factor to 180, 2 * 90 = 180
  RCC_PLLCFGR &= ~(0b111111111 << 6);
  RCC_PLLCFGR |= (180U << 6);

  RCC_PLLCFGR &= ~(3U << 16);  

  //Turn PLL ON
  RCC_CR |= (1 << 24);
  while (!(RCC_CR & (1 << 25)));

  //Turn on PWR peripheral
  RCC_APB1ENR |= (1U << 28);

  //PWR - Set Over Drive pin
  PWR_CR |= (1 << 16);
  while (!(PWR_CSR & (1 << 16)));

  //Set voltage regulator to over drive
  PWR_CR |= (1 << 17);
  while (!(PWR_CSR & (1 << 17)));


  //Select Flash latency; 5 Waits
  FLASH_ACR = (FLASH_ACR & ~(0b1111 << 0)) | (0x5 << 0);

  //Prescalers added for APB2 and APB1
  RCC_CFGR = (RCC_CFGR & ~(0b111 << 13) & ~(0b111 << 10)) | (0x4 << 13) | (0x5 << 10);

  //Wait for PLL to be locked
  while (1) {
    if (((RCC_CR >> 25) & 0b1) == 0b1) {
      break;
    }
  }

  //Choose system clock to use PLL
  RCC_CFGR &= ~(0b11 << 0);
  RCC_CFGR |= (0b10 << 0);

  //Blocking until sys switched to PLL
  while (1) {
    //Shift down by two, then 0b10 & 0b11 = 10, that means switched to PLL
    if (((RCC_CFGR >> 2) & 0x3) == 0x2) {
      break;
    }
  }

  return;
}

void turn_on_gpioa(void) {
  //Turn on GPIOA clock
  RCC_AHB1ENR |= (1U << 0);
}

void turn_on_tim1(void) {
  //Turn on TIM1 clock
  RCC_APB2ENR |= (1U << 0);
}

void turn_on_dma2_clock(void) {
  RCC_AHB1ENR |= (1U << 22);
}

