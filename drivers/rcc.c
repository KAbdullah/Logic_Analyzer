#include <stdint.h>

//RCC - over here when we put the outer *, we're saying that I want to immediately start working in the memory
// and not work with a pointer that points to a memory, so in subsequent uses of RCC, I'm working with memory 
#define RCC 0x40023800

#define RCC_CR (*((volatile uint32_t *)(RCC + 0x00)))
#define RCC_PLLCFGR (*((volatile uint32_t *)(RCC + 0x04)))
#define RCC_CFGR (*((volatile uint32_t *)(RCC + 0x08)))

//GPIOB
#define RCC_AHB1ENR (*((volatile uint32_t *)(RCC + 0x30)))
//I2C
#define RCC_APB1ENR (*((volatile uint32_t *)(RCC + 0x40)))

//PWR
#define PWR 0x40007000
#define PWR_CR (*((volatile uint32_t *)(PWR + 0x00)))
#define PWR_CSR (*((volatile uint32_t *)(PWR + 0x04)))

//Flash
#define FLASH 0x40023C00
#define FLASH_ACR (*((volatile uint32_t *)(FLASH + 0x00)))

void init_rcc_pll(void) {
  //Turn HSIO clock on
  RCC_CR |= (1 << 0);
  RCC_PLLCFGR = (RCC_PLLCFGR & ~(0b111111 << 0)) | (16U << 0);

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

  //Reset values of PLL register then Set the multiplication factor to 11, 16 * 11 = 176
  RCC_PLLCFGR &= ~(0b111111111 << 6);
  RCC_PLLCFGR |= (180U << 6);

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
  //Turn on GPIOB clock
  RCC_AHB1ENR |= (1 << 0);
}


void turn_on_gpiob(void) {
  //Turn on GPIOB clock
  RCC_AHB1ENR |= (1 << 1);
}