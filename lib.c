#include "system.h"

// allocado uhh allocatr nah alocator puff you get it
unsigned int h_p = 0x100000;

struct m_b {
    unsigned int s;
    int f;
    struct m_b *n;
};

struct m_b *m_h = 0;

void* malloc(int z) {
    z = (z + 3) & ~3; 

    struct m_b *c = m_h;
    while(c) {
        if(c->f && c->s >= (unsigned int)z) {
            c->f = 0;
            return (void*)((unsigned int)c + sizeof(struct m_b));
        }
        c = c->n;
    }
    
    struct m_b *n_b = (struct m_b*)h_p;
    n_b->s = z;
    n_b->f = 0;
    n_b->n = m_h;
    m_h = n_b;
    
    h_p += (sizeof(struct m_b) + z);
    return (void*)((unsigned int)n_b + sizeof(struct m_b));
}

void fx(void* p) {
    if(!p) return;
    struct m_b *b = (struct m_b*)((unsigned int)p - sizeof(struct m_b));
    b->f = 1;

    struct m_b *curr = m_h;
    while(curr) {
        if (curr->f) {
            struct m_b *scan = m_h;
            while(scan) {
                if (scan->f && scan != curr) {
                    if ((unsigned int)curr + sizeof(struct m_b) + curr->s == (unsigned int)scan) {
                        curr->s += sizeof(struct m_b) + scan->s;
                        struct m_b *rem = m_h;
                        if (rem == scan) {
                            m_h = scan->n;
                        } else {
                            while(rem && rem->n != scan) rem = rem->n;
                            if(rem) rem->n = scan->n;
                        }
                        scan = m_h;
                        continue;
                    }
                }
                scan = scan->n;
            }
        }
        curr = curr->n;
    }
}

unsigned int get_msize(void* p) {
    if(!p) return 0;
    struct m_b *b = (struct m_b*)((unsigned int)p - sizeof(struct m_b));
    return b->s;
}

// utils 
int cmp(char *a, char *b) {
    int i=0;
    while(a[i]==b[i]) { if(a[i]=='\0') return 1; i++; }
    return 0;
}

int im(char* s) {
    int i=0; if(s[0]=='-') i++;
    if(s[i]=='\0') return 0;
    for(;s[i]!='\0';i++) if(s[i]<'0'||s[i]>'9') return 0;
    return 1;
}

int a2(char* s) {
    int r=0, n=1, i=0;
    if(s[0]=='-') { n=-1; i++; }
    for(;s[i]!='\0';i++) r=r*10+s[i]-'0';
    return r*n;
}

void i2(int n, char* b) {
    int i=0, s=n;
    if(n<0) n=-n;
    do { b[i++]=n%10+'0'; } while((n/=10)>0);
    if(s<0) b[i++]='-';
    b[i]='\0';
    for(int j=0; j<i/2; j++) { char t=b[j]; b[j]=b[i-1-j]; b[i-1-j]=t; }
}

char* tk(char* s, char d, char** v_) {
    char* n = s?s:*v_;
    if(!n||*n=='\0') return 0;
    while(*n==d) n++;
    if(*n=='\0') return 0;
    char* st = n;
    while(*n!='\0'&&*n!=d) n++;
    if(*n!='\0') { *n='\0'; n++; }
    *v_ = n;
    return st;
}
