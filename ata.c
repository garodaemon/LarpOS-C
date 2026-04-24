#include "ata.h"

// stop scrolling
void ata_wait() {
    while(ib(0x1F7) & 0x80);
    while(!(ib(0x1F7) & 0x08));
}

// read school school school sahur
void ata_read(unsigned int lba, char* buf) {
    // DISKI SUSTUR (Interrupt Disable)
    ob(ATA_DEV_CTRL, 0x02);

    while(ib(0x1F7) & 0x80); 
    ob(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    ob(0x1F2, 1); 
    ob(0x1F3, (unsigned char)lba);
    ob(0x1F4, (unsigned char)(lba >> 8));
    ob(0x1F5, (unsigned char)(lba >> 16));
    ob(0x1F7, 0x20);
    
    ata_wait();
    
    for(int i=0; i<256; i++) {
        unsigned short word = iw(0x1F0);
        buf[i*2] = (char)(word & 0xFF);
        buf[i*2+1] = (char)(word >> 8);
    }
}

// tripi tropi?
void ata_write(unsigned int lba, char* buf) {

    ob(ATA_DEV_CTRL, 0x02);

    while(ib(0x1F7) & 0x80);
    ob(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    ob(0x1F2, 1);
    ob(0x1F3, (unsigned char)lba);
    ob(0x1F4, (unsigned char)(lba >> 8));
    ob(0x1F5, (unsigned char)(lba >> 16));
    ob(0x1F7, 0x30);
    
    ata_wait();
    
    for(int i=0; i<256; i++) {
        unsigned short word = (buf[i*2+1] << 8) | (buf[i*2] & 0xFF);
        ow(0x1F0, word);
    }
    
    // flush like a toilet
    ob(0x1F7, 0xE7);
    while(ib(0x1F7) & 0x80);
}
