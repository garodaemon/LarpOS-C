ENTRY(kernel_main)
SECTIONS
{
    . = 0x10000;
    .text : {
        *(.text.entry)
        *(.text)
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}
