#include "system.h"

// gdt
struct g1 {
    unsigned short l_l;
    unsigned short b_l;
    unsigned char b_m;
    unsigned char a_c;
    unsigned char g_r;
    unsigned char b_h;
} __attribute__((packed));

struct g2 {
    unsigned short l;
    unsigned int b;
} __attribute__((packed));

struct g1 g3[5];
struct g2 g4;

void sg(int n, unsigned long b, unsigned long l, unsigned char a, unsigned char g) {
    g3[n].b_l = (b&0xFFFF);
    g3[n].b_m = (b>>16)&0xFF;
    g3[n].b_h = (b>>24)&0xFF;
    g3[n].l_l = (l&0xFFFF);
    g3[n].g_r = ((l>>16)&0x0F);
    g3[n].g_r |= (g&0xF0);
    g3[n].a_c = a;
}

void ig() {
    g4.l = (sizeof(struct g1)*5)-1;
    g4.b = (unsigned int)&g3;
    // kernel
    sg(0,0,0,0,0);
    sg(1,0,0xFFFFFFFF,0x9A,0xCF);
    sg(2,0,0xFFFFFFFF,0x92,0xCF);
    // user
    sg(3,0,0xFFFFFFFF,0xFA,0xCF);
    sg(4,0,0xFFFFFFFF,0xF2,0xCF);
    __asm__ volatile("lgdt %0" : : "m"(g4));
}

// idt
struct i1 {
    unsigned short b_l;
    unsigned short s_l;
    unsigned char z;
    unsigned char f;
    unsigned short b_h;
} __attribute__((packed));

struct i2 {
    unsigned short l;
    unsigned int b;
} __attribute__((packed));

struct i1 i3[256];
struct i2 i4;

extern void i0();
extern void i1();
extern void s_c();

void sd(unsigned char n, unsigned long b, unsigned short s, unsigned char f) {
    i3[n].b_l = (b&0xFFFF);
    i3[n].b_h = (b>>16)&0xFFFF;
    i3[n].s_l = s;
    i3[n].z = 0; 
    i3[n].f = f;
}

// pic
void rm() {
    ob(0x20,0x11); ob(0xA0,0x11);
    ob(0x21,0x20); ob(0xA1,0x28);
    ob(0x21,0x04); ob(0xA1,0x02);
    ob(0x21,0x01); ob(0xA1,0x01);
    ob(0x21,0x00); ob(0xA1,0x00);
}

void ii() {
    i4.l = (sizeof(struct i1)*256)-1;
    i4.b = (unsigned int)&i3;
    for(int i=0; i<256; i++) sd(i,0,0,0);
    rm();
    // irq0 & irq1
    sd(32,(unsigned int)i0,0x08,0x8E);
    sd(33,(unsigned int)i1,0x08,0x8E);
    // syscall
    sd(128,(unsigned int)s_c,0x08,0xEE);
    __asm__ volatile("lidt %0" : : "m"(i4));
}

// pit
void it() {
    int d=119318;
    ob(0x43,0x36); ob(0x40,d&0xFF); ob(0x40,d>>8);
}
