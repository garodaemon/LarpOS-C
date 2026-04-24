#include "system.h"
#include "larpfs.h"

// kernel if you can call it
__asm__(
    ".code32\n"
    ".section .text.entry\n"
    "mov $0x10, %ax\n"
    "mov %ax, %ds\n"
    "mov %ax, %es\n"
    "mov %ax, %fs\n"
    "mov %ax, %gs\n"
    "mov %ax, %ss\n"
    "mov $0x90000, %esp\n"
    "call km\n"
    "jmp .\n"
    ".global i0\n"
    "i0:\n"
    "pushal\n"
    "call t_i\n"
    "popal\n"
    "iretl\n"
    ".global i1\n"
    "i1:\n"
    "pushal\n"
    "call k_i\n"
    "popal\n"
    "iretl\n"
    ".global s_c\n"
    "s_c:\n"
    "pushal\n"
    "call s_irq\n"
    "popal\n"
    "iretl\n"
);

void km() {
    ig();
    ii();
    it();
    f_ini();
    
    // desktop
    d_();
    
    __asm__ volatile("sti");
    
    // userspace maybe?
    shell_init(); 
}
