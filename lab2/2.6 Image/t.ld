/****************
qemu with -kernel file.bin ==> start to execute from 0x10000 ==>
file.bin must be loaded to 0x10000 ==> . = 0x10000
************/
ENTRY(reset_start)
SECTIONS
{
  . = 0x10000;
  .text : { ts.o *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
  
  .data : { image1.o }
  .data : { volleyball.o }

  . = ALIGN(8);
  . = . + 0x4000;           /* SVC stack */
  stack_top = .;

}

