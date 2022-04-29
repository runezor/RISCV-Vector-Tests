OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY
{
    RAM (rwx)  : ORIGIN = 0x80000000, LENGTH = 64K

}

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x1000);
  .tohost : { *(.tohost) }
  . = ALIGN(0x1000);
  .text : { *(.text) }
  . = ALIGN(0x1000);
  .data : { *(.data) }
  .bss : { *(.bss) }
  .sdata : {
      __global_pointer$ = . + 0x800;
      }
  _sstack = 0x80f00000;
  _edata = .;
  _end = .;
}