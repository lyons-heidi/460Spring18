ENTRY(start) /* define start as the entry address */
SECTIONS /* program sections */
{
    . = 0x10000; /* loading address, required by QEMU */
    .text : { ts.o *(.text) } /* all text in .text section */
    .data : { *(.data) } /* all data in .data section */
    .bss : { *(.bss) } /* all bss in .bss section */
    .data : { * (image1.o) }
    . =ALIGN(4);
    . =. + 0x1000; /* 4 KB stack space */
    stack_top =.; /* stack_top is a symbol exported by linker */
}