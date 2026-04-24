#include "system.h"

// I/O (will explode on next vers :DDDDDDDDDDDDDDDDDD)
unsigned char ib(unsigned short p) {
    unsigned char r;
    __asm__ volatile("inb %1, %0" : "=a"(r) : "Nd"(p));
    return r;
}

void ob(unsigned short p, unsigned char d) {
    __asm__ volatile("outb %0, %1" : : "a"(d), "Nd"(p));
}

unsigned short iw(unsigned short p) {
    unsigned short r;
    __asm__ volatile("inw %1, %0" : "=a"(r) : "Nd"(p));
    return r;
}

void ow(unsigned short p, unsigned short d) {
    __asm__ volatile("outw %0, %1" : : "a"(d), "Nd"(p));
}
