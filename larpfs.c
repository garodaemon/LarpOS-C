#include "larpfs.h"
#include "ata.h"
extern void* malloc(int z);
extern void fx(void* p);
extern int cmp(char *a, char *b);
extern unsigned int get_msize(void* p);

struct l_f fs_r[16];
int f_cnt = 0;

char disk_meta[1024];

// finder
int f_fnd(char *fn) {
    for (int i=0; i<f_cnt; i++) {
        if (cmp(fs_r[i].n, fn) == 1) return i;
    }
    return -1;
}

// adding sum shi
void f_add(char *fn, char *d) {
    // guess the shit
    int l = 0;
    while (d[l] != '\0') l++;

    int ex = f_fnd(fn);
    
    // sudo apt please update
    if (ex != -1) {
        if (get_msize(fs_r[ex].c) < (unsigned int)(l + 1)) {
            fx(fs_r[ex].c); 
            fs_r[ex].c = (char*)malloc(l + 1);
        }
        int j = 0;
        while (j < l) { fs_r[ex].c[j] = d[j]; j++; }
        fs_r[ex].c[j] = '\0';
        fs_r[ex].s = j;
        return;
    }

    if (f_cnt >= 16) return;

    // copycat
    int i = 0;
    while (fn[i] != '\0' && i < 31) { fs_r[f_cnt].n[i] = fn[i]; i++; }
    fs_r[f_cnt].n[i] = '\0';

    fs_r[f_cnt].c = (char*)malloc(l + 1);

    int j = 0;
    while (j < l) { fs_r[f_cnt].c[j] = d[j]; j++; }
    fs_r[f_cnt].c[j] = '\0';
    fs_r[f_cnt].s = j;
    
    f_cnt++;
}

//  sudo rm -rf /
void f_rm(char *fn) {
    int idx = f_fnd(fn);
    if (idx != -1) {
        fx(fs_r[idx].c); 
        for (int i = idx; i < f_cnt - 1; i++) { fs_r[i] = fs_r[i+1]; }
        f_cnt--;
    }
}

// dying
void fs_save() {
    for(int i=0; i<1024; i++) disk_meta[i] = 0;
    
    // larp
    disk_meta[0] = 'L'; disk_meta[1] = 'A'; disk_meta[2] = 'R'; disk_meta[3] = 'P';
    disk_meta[4] = f_cnt & 0xFF; disk_meta[5] = (f_cnt >> 8) & 0xFF;
    
    int p = 8;
    int lba = 202; 
    
    for(int i=0; i<f_cnt; i++) {
        int k=0; while(fs_r[i].n[k]) { disk_meta[p+k] = fs_r[i].n[k]; k++; }
        p += 32;
        
        int sz = fs_r[i].s;
        disk_meta[p] = sz & 0xFF; disk_meta[p+1] = (sz>>8) & 0xFF; disk_meta[p+2] = (sz>>16) & 0xFF;
        p += 4;
        
        // chunk (maynikraf chunk?)
        int secs = (sz / 512) + 1;
        char* dptr = fs_r[i].c;
        for(int s=0; s<secs; s++) {
            char sbuf[512];
            for(int b=0; b<512; b++) {
                if (s*512+b < sz) sbuf[b] = dptr[s*512+b];
                else sbuf[b] = 0;
            }
            ata_write(lba++, sbuf);
        }
    }
    
    // burn
    ata_write(200, disk_meta);
    ata_write(201, disk_meta+512);
}

// SystemD2
void f_ini() {
    ata_read(200, disk_meta);
    ata_read(201, disk_meta+512);
    
    // old codes with tung rn
    if (disk_meta[0] == 'L' && disk_meta[1] == 'A' && disk_meta[2] == 'R' && disk_meta[3] == 'P') {
        f_cnt = disk_meta[4] | (disk_meta[5]<<8);
        int p = 8;
        int lba = 202;
        
        for(int i=0; i<f_cnt; i++) {
            int k=0; while(disk_meta[p+k] && k<31) { fs_r[i].n[k] = disk_meta[p+k]; k++; }
            fs_r[i].n[k] = 0;
            p += 32;
            
            int sz = disk_meta[p] | (disk_meta[p+1]<<8) | (disk_meta[p+2]<<16);
            p += 4;
            
            fs_r[i].s = sz;
            fs_r[i].c = (char*)malloc(sz + 1);
            
            int secs = (sz / 512) + 1;
            char* dptr = fs_r[i].c;
            for(int s=0; s<secs; s++) {
                char sbuf[512];
                ata_read(lba++, sbuf);
                for(int b=0; b<512; b++) {
                    if (s*512+b < sz) dptr[s*512+b] = sbuf[b];
                }
            }
            dptr[sz] = '\0';
        }
    } else {
        // SystemD
        f_add("notlar.txt", "LARPFORNIA 0.5 Dosya Sistemi Calisiyor.");
        f_add("readme.md", "LarpOS C 0.5. EMIT komutu eklendi. Kullanim: lforth in.fs out");
        f_add("sifreler.txt", "admin:123456");
        f_add("supporters", "Xyl, /home/kali_linx, Diavolo, turkdogru");
        f_add("math.fs", ": TOPLA + . ; 10 5 TOPLA");
        f_add("hello.fs", ": MERHABA 72 EMIT 69 EMIT 76 EMIT 76 EMIT 79 EMIT ; MERHABA");
        fs_save();
    }
}

// content stealer
char* f_gc(int i) {
    if (i >= 0 && i < f_cnt) return fs_r[i].c;
    return 0;
}

// getting ig
char* f_gn(int i) {
    if (i >= 0 && i < f_cnt) return fs_r[i].n;
    return 0;
}

// count  a:uhh okay 1, 2, 3, g: stfu dumbass
int f_gcnt() { return f_cnt; }
