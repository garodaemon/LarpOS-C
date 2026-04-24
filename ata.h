#ifndef ATA_H
#define ATA_H

extern unsigned char ib(unsigned short port);
extern void ob(unsigned short port, unsigned char data);
extern unsigned short iw(unsigned short port);
extern void ow(unsigned short port, unsigned short data);

// ports
#define ATA_DATA        0x1F0
#define ATA_SEC_CNT     0x1F2
#define ATA_LBA_LO      0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HI      0x1F5
#define ATA_DRIVE_SEL   0x1F6
#define ATA_STATUS      0x1F7
#define ATA_COMMAND     0x1F7

#define ATA_DEV_CTRL    0x3F6

// cmd
#define ATA_CMD_READ    0x20
#define ATA_CMD_WRITE   0x30
#define ATA_CMD_FLUSH   0xE7

void ata_read(unsigned int lba, char* buf);
void ata_write(unsigned int lba, char* buf);
void ata_wait();

#endif
