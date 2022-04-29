OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY
{
    RAM (rwx)  : ORIGIN = 0x80000000, LENGTH = 640K

}

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x10000);
  .tohost : { *(.tohost) }
  . = ALIGN(0x10000);
  .text : { *(.text) }
  . = ALIGN(0x10000);
  .data : { *(.data) }
  .bss : { *(.bss) }
  .sdata : {
      __global_pointer$ = . + 0x80000;
      }
  _sstack = 0x8f000000;
  _edata = .;
  _end = .;
}