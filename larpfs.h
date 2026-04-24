#ifndef LARPFS_H
#define LARPFS_H

// structure
struct l_f {
    char n[32]; 
    char* c;    
    int s;      
};

// prototypes (anything but solving mem leak)
void f_ini();
int f_fnd(char *fn);
void f_add(char *fn, char *d);
void f_rm(char *fn);
void fs_save();
char* f_gc(int i);
char* f_gn(int i);
int f_gcnt();

#endif
