#ifndef SYSTEM_H
#define SYSTEM_H

unsigned char ib(unsigned short p);
void ob(unsigned short p, unsigned char d);
unsigned short iw(unsigned short p);
void ow(unsigned short p, unsigned short d);

void ig();
void ii();
void it();

// 640x480 when
extern int ttt, r5, tun, v;
void p_(int a, int b, unsigned char c);
void p__(int a, int b, unsigned char c);
void b_(int a, int b, int w, int h, unsigned char c);
void c_(char c, int a, int b, unsigned char cl);
void c__(char c, int a, int b, unsigned char cl);
void s_(char *s, int a, int b, unsigned char c);
void s__(char *s, int a, int b, unsigned char c);
void d_();
void cl();
void sc_();
void nl_();

// libs
extern unsigned int h_p;
void* malloc(int z);
void fx(void* p);
int cmp(char *a, char *b);
int im(char* s);
int a2(char* s);
void i2(int n, char* b);
char* tk(char* s, char d, char** v_);

// input
extern volatile unsigned char k_p;
extern volatile unsigned char l_s;
extern volatile unsigned char s_h;
extern volatile unsigned char c_p;
char ps(unsigned char s);

extern void shell_init();
extern void t_i();
extern void k_i();
extern void s_irq();

#endif
