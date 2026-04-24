#include "system.h"

// timer
volatile unsigned int t1=0;
int c_v=1;

void t_i() {
    t1++;
    if(t1%10==0) { c_v=!c_v; b_(ttt,r5,8,8,c_v?0x07:0x00); }
    ob(0x20,0x20);
}

void s_irq() { ob(0x20,0x20); }

// keyboard
volatile unsigned char s_h=0;
volatile unsigned char c_p=0;
volatile unsigned char k_p=0;
volatile unsigned char l_s=0; // scancode

char k_1[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-',
    '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

char k_2[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-',
    '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

char ps(unsigned char s) {
    char c = k_1[s];
    if(c>='a'&&c<='z') {
        if(s_h^c_p) return k_2[s];
        return c;
    }
    if(s_h) return k_2[s];
    return c;
}

void k_i() {
    l_s=ib(0x60);
    k_p=1;
    ob(0x20,0x20);
}
