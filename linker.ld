ENTRY(Reset_Handler)

MEMORY {
  FLASH(rx): ORIGIN = 0x08000000, LENGTH = 512K
  SRAM1(wrx): ORIGIN = 0x20000000, LENGTH = 112K
  SRAM2(wrx): ORIGIN = 0x2001C000, LENGTH = 16K
}

_estack = ORIGIN(SRAM1) + LENGTH(SRAM1);
_Min_Heap_Size = 0x400;

SECTIONS 
{
  .isr_vector : {
    KEEP(*(isr_vector));
    . = ALIGN(4);
  } > FLASH

  .text : {
    . = ALIGN(4);
    _stext = .;
    PROVIDE(stext = .);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)

    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    _etext = .;
    PROVIDE(etext = .);
  } > FLASH

  .rodata : {
    . = ALIGN(4);
    _srodata = .;
    PROVIDE(srodata = .);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
    _erodata = .;
    PROVIDE(erodata = .);
  } > FLASH

  /*VMA AT> LMA*/
  .data : {
    . = ALIGN(4);
    _sdata = .;
    PROVIDE(sdata = .);
    *(.data)
    *(.data*)
    *(.RamFunc) 
    *(.RamFunc*)
    . = ALIGN(4);
    _edata = .;
    PROVIDE(edata = .);
  } > SRAM1 AT> FLASH
  
  /*returns the absolute load memory address (LMA)*/
  _sidata = LOADADDR(.data);

  .bss : {
    . = ALIGN(4);
    _sbss = .;
    PROVIDE(sbss = .);
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;
    PROVIDE(ebss = .);
    . = ALIGN(4);
  } > SRAM1

  .user_heap_stack : {
    . = ALIGN(4);
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = ALIGN(4);
  } > SRAM1

}