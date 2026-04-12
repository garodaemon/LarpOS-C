extern void ata_read_sector(unsigned int lba, unsigned char *buf);
extern void ata_write_sector(unsigned int lba, unsigned char *buf);

__asm__(
    ".section .text.entry\n"
    "mov $0x10, %ax\n"
    "mov %ax, %ds\n"
    "mov %ax, %es\n"
    "mov %ax, %fs\n"
    "mov %ax, %gs\n"
    "mov %ax, %ss\n"
    "mov $0x90000, %esp\n"
    "call kernel_main\n"
    "jmp .\n"
    ".global irq0_handler\n"
    "irq0_handler:\n"
    "pusha\n"
    "call timer_handler\n"
    "popa\n"
    "iret\n"
    ".global irq1_handler\n"
    "irq1_handler:\n"
    "pusha\n"
    "call keyboard_handler\n"
    "popa\n"
    "iret\n"
    ".global syscall_handler\n"
    "syscall_handler:\n"
    "pusha\n"
    "call syscall_handler_c\n"
    "popa\n"
    "iret\n"
);

void print_char(char c, int x, int y, unsigned char color) {
    volatile char *video = (volatile char*)0xB8000;
    int offset = (y * 80 + x) * 2;
    video[offset] = c;
    video[offset + 1] = color;
}

void print_string(char *str, int x, int y, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        print_char(str[i], x + i, y, color);
        i++;
    }
}

void clear_screen() {
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            print_char(' ', x, y, 0x07);
        }
    }
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

unsigned short inw(unsigned short port) {
    unsigned short result;
    __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(unsigned short port, unsigned short data) {
    __asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

void move_cursor(int x, int y) {
    unsigned short pos = y * 80 + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

int strcmp(char *str1, char *str2) {
    int i = 0;
    while (str1[i] == str2[i]) {
        if (str1[i] == '\0') return 1;
        i++;
    }
    return 0;
}

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct gdt_entry gdt[5];
struct gdt_ptr gp;

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gp.base = (unsigned int)&gdt;
    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    __asm__ volatile("lgdt %0" : : "m"(gp));
}

struct idt_entry {
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void irq0_handler();
extern void irq1_handler();
extern void syscall_handler();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    pic_remap();
    idt_set_gate(32, (unsigned int)irq0_handler, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq1_handler, 0x08, 0x8E);
    idt_set_gate(128, (unsigned int)syscall_handler, 0x08, 0xEE);
    __asm__ volatile("lidt %0" : : "m"(idtp));
}

volatile unsigned int timer_ticks = 0;
void timer_handler() {
    timer_ticks++;
    outb(0x20, 0x20);
}

void syscall_handler_c() {
    outb(0x20, 0x20);
}

void timer_install() {
    int divisor = 11931;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-',
    '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

int cursor_x = 22;
int cursor_y = 9;
char input_buffer[64];
int buffer_index = 0;

volatile unsigned char key_pressed = 0;
volatile unsigned char last_scancode = 0;

void keyboard_handler() {
    last_scancode = inb(0x60);
    key_pressed = 1;
    outb(0x20, 0x20);
}

unsigned int heap_current = 0x100000;

void* malloc(int size) {
    void* ptr = (void*)heap_current;
    heap_current += size;
    return ptr;
}

void free(void* ptr) {
}

void print_banner() {
    print_string("  _                  ____   ____  _____  _____ ", 2, 1, 0x0D);
    print_string(" | |    __ _ _ __ |  _ \\ / __ \\ / ____|  / ____|", 2, 2, 0x0D);
    print_string(" | |   / _` | '__|| |_) | |  | | (___   | |     ", 2, 3, 0x0D);
    print_string(" | |  / _` | |    |  __/| |  | |\\___ \\  | |     ", 2, 4, 0x0D);
    print_string(" | |__| (_| | |    | |   | |__| |____) | | |____ ", 2, 5, 0x0D);
    print_string(" |_____\\__,_|_|    |_|    \\____/|_____/  \\_____| 0.3", 2, 6, 0x0D);
    print_string("------------------- CODE NAME: LARPLANDO ----------------", 2, 7, 0x07);
}

extern void larpfs_init();
extern int larpfs_find(char *filename);
extern char* larpfs_get_content(int index);
extern char* larpfs_get_name(int index);
extern int larpfs_get_count();
extern void larpfs_add_file(char *filename, char *data);
extern void larpfs_save_file(char *filename, char *data);

void execute_command() {
    input_buffer[buffer_index] = '\0';
    if (buffer_index > 0) {
        if (strcmp(input_buffer, "clear") == 1) {
            clear_screen();
            cursor_y = -1;
        } else if (strcmp(input_buffer, "help") == 1) {
            cursor_y++;
            print_string("cmds: help clear uname pwd ls cat mkdir lsblk reboot", 0, cursor_y, 0x07);
        } else if (strcmp(input_buffer, "uname") == 1) {
            cursor_y++;
            print_string("LarpOS C 0.3 (larplando)", 0, cursor_y, 0x0D);
        } else if (strcmp(input_buffer, "pwd") == 1) {
            cursor_y++;
            print_string("/home/garodaemon", 0, cursor_y, 0x0B);
        } else if (strcmp(input_buffer, "lsblk") == 1) {
            cursor_y++;
            print_string("NAME    MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT", 0, cursor_y, 0x0B);
            cursor_y++;
            print_string("ld0       1:0    0   128M  0 disk ", 0, cursor_y, 0x07);
            cursor_y++;
            print_string("`-ld0p1   1:1    0   128M  0 part /", 0, cursor_y, 0x07);
        } else if (strcmp(input_buffer, "ls") == 1) {
            cursor_y++;
            int count = larpfs_get_count();
            int current_x = 0;
            for(int i = 0; i < count; i++) {
                char* fname = larpfs_get_name(i);
                print_string(fname, current_x, cursor_y, 0x0E);
                int len = 0;
                while(fname[len] != '\0') len++;
                current_x += len + 2;
                if(current_x > 60) {
                    current_x = 0;
                    cursor_y++;
                }
            }
        } else if (input_buffer[0] == 'c' && input_buffer[1] == 'a' && input_buffer[2] == 't' && input_buffer[3] == ' ') {
            char *fname = &input_buffer[4];
            int idx = larpfs_find(fname);
            cursor_y++;
            if (idx != -1) {
                print_string(larpfs_get_content(idx), 0, cursor_y, 0x0F);
            } else {
                print_string("Dosya bulunamadi.", 0, cursor_y, 0x0C);
            }
        } else if (input_buffer[0] == 'm' && input_buffer[1] == 'k' && input_buffer[2] == 'd' && input_buffer[3] == 'i' && input_buffer[4] == 'r' && input_buffer[5] == ' ') {
            char *dname = &input_buffer[6];
            larpfs_add_file(dname, "</home/tsukasa is larp and fraud>");
            cursor_y++;
            print_string("Klasor olusturuldu.", 0, cursor_y, 0x0A);
        } else if (strcmp(input_buffer, "reboot") == 1) {
            outb(0x64, 0xFE);
        } else {
            cursor_y++;
            print_string("Command not found.", 0, cursor_y, 0x0C);
        }
    }
    buffer_index = 0;
}

void run_terminal() {
    if (!key_pressed) {
        __asm__ volatile("hlt");
        return;
    }
    
    key_pressed = 0;
    unsigned char scancode = last_scancode;
    
    if (scancode & 0x80) return;
    
    if (scancode < 128) {
        char c = scancode_to_ascii[scancode];
        if (c == '\n') {
            execute_command();
            cursor_y++;
            if (cursor_y >= 25) {
                clear_screen();
                cursor_y = 0;
            }
            cursor_x = 0;
            print_string("garodaemon@larplando:~$ ", cursor_x, cursor_y, 0x0A);
            cursor_x = 22;
        } else if (c == '\b') {
            if (cursor_x > 22 && buffer_index > 0) {
                cursor_x--;
                buffer_index--;
                print_char(' ', cursor_x, cursor_y, 0x07);
            }
        } else if (c != 0 && buffer_index < 63) {
            input_buffer[buffer_index] = c;
            buffer_index++;
            print_char(c, cursor_x, cursor_y, 0x07);
            cursor_x++;
            if (cursor_x >= 80) {
                cursor_x = 0;
                cursor_y++;
            }
        }
        move_cursor(cursor_x, cursor_y);
    }
}

void kernel_main() {
    clear_screen();
    print_banner();
    gdt_install();
    idt_install();
    timer_install();
    larpfs_init();
    __asm__ volatile("sti");
    print_string("garodaemon@larplando:~$ ", 0, 9, 0x0A);
    move_cursor(22, 9);
    while (1) {
        run_terminal();
    }
}
