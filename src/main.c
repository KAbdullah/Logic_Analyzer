#include <stdint.h>
#include "../drivers/rcc.c"

#define SYST_CSR   (*((volatile uint32_t *)0xE000E010))
#define SYST_RVR   (*((volatile uint32_t *)0xE000E014))

void test_systick_speed(void) {
    // Enable the green User LED on the Nucleo board (Pin PA5)
    RCC_AHB1ENR |= (1 << 0);
    *((volatile uint32_t *)0x40020000) &= ~(0x3 << 10);
    *((volatile uint32_t *)0x40020000) |=  (0x1 << 10); // Output mode

    SYST_RVR = 16000000  - 1;
    SYST_CSR = 0x5;

    while(1) {

      // Check if the SysTick count reached 0 (Bit 16 flips to 1)
      if ((SYST_CSR & (1 << 16))) {
        // Toggle Pin PA5 (LED)
        *((volatile uint32_t *)(0x40020000 + 0x14)) ^= (1 << 5); 
      }
    }
}

int main(void) {

  init_rcc_pll();

  test_systick_speed();

  while (1) {

  }
}