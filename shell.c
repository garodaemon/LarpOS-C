extern void f_rm(char *fn);
extern unsigned int get_msize(void* p);
extern unsigned int h_p;
extern volatile unsigned int t1;
extern void fs_save(); // larpfs'den cektik

#include "system.h"
#include "larpfs.h"

char input_buffer[64];
int buffer_index = 0;

// forth
int forth_stack[128];
int forth_sp = 0;

void f_push(int v) { forth_stack[forth_sp++] = v; }
int f_pop() { return forth_stack[--forth_sp]; }

char f_dict_names[64][16];
char f_dict_bodies[64][256];
int f_dict_count = 0;
int f_compile_mode = 0;
char current_word[16];
char current_body[256];
int body_idx = 0;

void f_define(char* name, char* body) {
    int i=0; while(name[i]) { f_dict_names[f_dict_count][i] = name[i]; i++; }
    f_dict_names[f_dict_count][i] = 0;
    i=0; while(body[i]) { f_dict_bodies[f_dict_count][i] = body[i]; i++; }
    f_dict_bodies[f_dict_count][i] = 0;
    f_dict_count++;
}

void run_forth(char* code) {
    char buf[512];
    int bi=0;
    while(code[bi] && bi<511) { buf[bi] = code[bi]; bi++; }
    buf[bi] = 0;

    char* saveptr;
    char* token = tk(buf, ' ', &saveptr);
    while (token != 0) {
        if (f_compile_mode) {
            if (cmp(token, ";") == 1) {
                current_body[body_idx] = 0;
                f_define(current_word, current_body);
                f_compile_mode = 0;
            } else {
                int k=0;
                if(body_idx > 0) current_body[body_idx++] = ' ';
                while(token[k]) current_body[body_idx++] = token[k++];
            }
        } else {
            if (cmp(token, ":") == 1) {
                f_compile_mode = 1;
                token = tk(0, ' ', &saveptr);
                if(token) {
                    int k=0; while(token[k]) { current_word[k] = token[k]; k++; }
                    current_word[k] = 0;
                    body_idx = 0;
                }
            } else if (im(token)) {
                f_push(a2(token));
            } else if (cmp(token, "+") == 1) {
                f_push(f_pop() + f_pop());
            } else if (cmp(token, "-") == 1) {
                int a = f_pop(); f_push(f_pop() - a);
            } else if (cmp(token, ".") == 1) {
                char numbuf[16];
                i2(f_pop(), numbuf);
                nl_();
                s_(numbuf, 24, r5, 0x0F);
            } else if (cmp(token, "EMIT") == 1) {
                char c = (char)f_pop();
                c_(c, ttt, r5, 0x0F);
                ttt+=8;
            } else {
                int found = 0;
                for(int i=f_dict_count-1; i>=0; i--) {
                    if (cmp(token, f_dict_names[i]) == 1) {
                        run_forth(f_dict_bodies[i]);
                        found = 1;
                        break;
                    }
                }
                if(!found) {
                    nl_();
                    s_("?", 24, r5, 0x0C);
                }
            }
        }
        token = tk(0, ' ', &saveptr);
    }
}

// vim nano emacs and ee killer
char edit_buffer[2048];

void run_larpedit(char* filename) {
    cl();
    ttt = 24; r5 = 62; v = 0;
    int idx = 0;

    while(1) {
        if (!k_p) {
            __asm__ volatile("hlt");
            continue;
        }
        k_p = 0;
        unsigned char sc = l_s;
        
        if (sc == 0x3A) { c_p = !c_p; continue; }
        if (sc == 0x2A || sc == 0x36) { s_h = 1; continue; }
        if (sc == 0xAA || sc == 0xB6) { s_h = 0; continue; }
        if (sc & 0x80) continue;

        if (sc == 1) { 
            b_(ttt, r5, 8, 8, 0x00); 
            edit_buffer[idx] = '\0';
            f_add(filename, edit_buffer);
            cl();
            ttt = 24; r5 = 62; v = 0;
            return;
        }

        char c = ps(sc);
        if (c == '\n') {
            b_(ttt, r5, 8, 8, 0x00); 
            edit_buffer[idx++] = c;
            nl_();
        } else if (c == '\b') {
            if (idx > 0) {
                idx--;
                b_(ttt, r5, 8, 8, 0x00); 
                if (ttt > 24) {
                    ttt -= 8;
                    b_(ttt, r5, 8, 8, 0x00); 
                } else if (v > 0) {
                    r5 -= 10;
                    v--;
                    ttt = 580;
                    b_(ttt, r5, 8, 8, 0x00); 
                }
            }
        } else if (c != 0 && idx < 2047) {
            b_(ttt, r5, 8, 8, 0x00); 
            edit_buffer[idx++] = c;
            c_(c, ttt, r5, 0x07);
            ttt += 8;
            if (ttt >= 590) nl_();
        }
    }
}

// exec
void execute_command() {
    input_buffer[buffer_index] = '\0';
    if (buffer_index > 0) {
        // commands
        if (cmp(input_buffer, "clear") == 1) {
            cl();
            ttt = 24; r5 = 52; v = -1;
        } else if (cmp(input_buffer, "help") == 1) {
            nl_();
            s_("cmds: help clear uname pwd ls cat mkdir lsblk reboot lforth edit", 24, r5, 0x07);
            nl_();
            s_("      lfetch rm touch echo stat free lsmod whoami calc color rand sync", 24, r5, 0x07);
        } else if (cmp(input_buffer, "uname") == 1) {
            nl_();
            s_("LarpOS C 0.5 (LARPFORNIA)", 24, r5, 0x0D);
        } else if (cmp(input_buffer, "pwd") == 1) {
            nl_();
            s_("/home/garodaemon", 24, r5, 0x0B);
        } else if (cmp(input_buffer, "lsblk") == 1) {
            nl_(); s_("NAME    MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT", 24, r5, 0x0B);
            nl_(); s_("ld0       1:0    0   128M  0 disk ", 24, r5, 0x07);
            nl_(); s_("`-ld0p1   1:1    0   128M  0 part /", 24, r5, 0x07);
        } else if (cmp(input_buffer, "ls") == 1) {
            nl_();
            int count = f_gcnt();
            int current_x = 24;
            for(int i = 0; i < count; i++) {
                char* fname = f_gn(i);
                s_(fname, current_x, r5, 0x0E);
                int len = 0;
                while(fname[len] != '\0') len++;
                current_x += (len * 8) + 16;
                if(current_x > 500) {
                    current_x = 24;
                    nl_();
                }
            }
        } else if (input_buffer[0] == 'c' && input_buffer[1] == 'a' && input_buffer[2] == 't' && input_buffer[3] == ' ') {
            char *fname = &input_buffer[4];
            int idx = f_fnd(fname);
            nl_();
            if (idx != -1) {
                s_(f_gc(idx), 24, r5, 0x0F);
            } else {
                s_("File not found.", 24, r5, 0x0C);
            }
        } else if (input_buffer[0] == 'm' && input_buffer[1] == 'k' && input_buffer[2] == 'd' && input_buffer[3] == 'i' && input_buffer[4] == 'r' && input_buffer[5] == ' ') {
            char *dname = &input_buffer[6];
            f_add(dname, "</home/tsukasa is larp and fraud>");
            nl_();
            s_("New file created.", 24, r5, 0x0A);
        } else if (input_buffer[0] == 'e' && input_buffer[1] == 'd' && input_buffer[2] == 'i' && input_buffer[3] == 't' && input_buffer[4] == ' ') {
            char *fname = &input_buffer[5];
            run_larpedit(fname);
        } else if (input_buffer[0] == 'l' && input_buffer[1] == 'f' && input_buffer[2] == 'o' && input_buffer[3] == 'r' && input_buffer[4] == 't' && input_buffer[5] == 'h' && input_buffer[6] == ' ') {
            char *args = &input_buffer[7];
            char *saveptr;
            char *file_in = tk(args, ' ', &saveptr);
            char *file_out = tk(0, ' ', &saveptr);
            if (file_in && file_out) {
                int idx = f_fnd(file_in);
                if (idx != -1) {
                    char *content = f_gc(idx);
                    run_forth(content);
                    f_add(file_out, "FORTH_BIN");
                    nl_(); s_("compiled:", 24, r5, 0x0A);
                    s_(file_out, 100, r5, 0x0E);
                } else {
                    nl_(); s_("not found", 24, r5, 0x0C);
                }
            } else {
                nl_(); s_("usage: lforth in.fs out", 24, r5, 0x0C);
            }
        } else if (cmp(input_buffer, "lfetch") == 1) {
            nl_();
            s_("    __      ", 24, r5, 0x0E); s_("garodaemon@larpos", 140, r5, 0x0A); r5+=10;
            s_("   / /      ", 24, r5, 0x0E); s_("-----------------", 140, r5, 0x07); r5+=10;
            s_("  / /       ", 24, r5, 0x0E); s_("OS: LarpOS 0.5 Beta 4", 140, r5, 0x0B); r5+=10;
            s_(" / /        ", 24, r5, 0x0E); s_("Kernel: Larpfornia C", 140, r5, 0x0B); r5+=10;
            
            unsigned int sec = t1 / 100;
            char ut[16]; i2(sec, ut);
            s_("/ /____     ", 24, r5, 0x0E); s_("Uptime: ", 140, r5, 0x0B); s_(ut, 200, r5, 0x07); s_("s", 230, r5, 0x07); r5+=10;
            
            char mem[16]; i2((h_p - 0x100000) / 1024, mem);
            s_("\\_____/     ", 24, r5, 0x0E); s_("Memory: ", 140, r5, 0x0B); s_(mem, 200, r5, 0x07); s_(" KB / 4MB", 235, r5, 0x07); r5+=10;
            
            nl_();
            b_(140, r5, 12, 12, 0x04);
            b_(155, r5, 12, 12, 0x02);
            b_(170, r5, 12, 12, 0x0E);
            b_(185, r5, 12, 12, 0x01);
            b_(200, r5, 12, 12, 0x05);
            r5+=5;
        } else if (input_buffer[0] == 'r' && input_buffer[1] == 'm' && input_buffer[2] == ' ') {
            char *fn = &input_buffer[3];
            f_rm(fn);
            nl_();
            s_("File removed.", 24, r5, 0x0A);
        } else if (input_buffer[0] == 't' && input_buffer[1] == 'o' && input_buffer[2] == 'u' && input_buffer[3] == 'c' && input_buffer[4] == 'h' && input_buffer[5] == ' ') {
            char *fn = &input_buffer[6];
            f_add(fn, "");
            nl_();
            s_("File touched.", 24, r5, 0x0A);
        } else if (input_buffer[0] == 'e' && input_buffer[1] == 'c' && input_buffer[2] == 'h' && input_buffer[3] == 'o' && input_buffer[4] == ' ') {
            char *text = &input_buffer[5];
            nl_();
            s_(text, 24, r5, 0x0F);
        } else if (input_buffer[0] == 's' && input_buffer[1] == 't' && input_buffer[2] == 'a' && input_buffer[3] == 't' && input_buffer[4] == ' ') {
            char *fn = &input_buffer[5];
            int idx = f_fnd(fn);
            nl_();
            if (idx != -1) {
                char sbuf[16];
                i2(get_msize(f_gc(idx)), sbuf);
                s_("File: ", 24, r5, 0x0B); s_(fn, 70, r5, 0x0F); nl_();
                s_("Size: ", 24, r5, 0x0B); s_(sbuf, 70, r5, 0x0F); s_(" bytes", 120, r5, 0x07);
            } else {
                s_("File not found.", 24, r5, 0x0C);
            }
        } else if (cmp(input_buffer, "free") == 1) {
            nl_();
            char mbuf[16];
            s_("Used Heap: ", 24, r5, 0x0B);
            i2(h_p - 0x100000, mbuf);
            s_(mbuf, 120, r5, 0x07); s_(" bytes", 180, r5, 0x07);
        } else if (cmp(input_buffer, "lsmod") == 1) {
            nl_();
            s_("Module                 Size  Used by", 24, r5, 0x0B); nl_();
            s_("larp_vga                16K   kernel", 24, r5, 0x07); nl_();
            s_("larp_ps2                8K    input", 24, r5, 0x07); nl_();
            s_("larp_fs                 12K   kernel", 24, r5, 0x07);
        } else if (cmp(input_buffer, "whoami") == 1) {
            nl_();
            s_("garodaemon", 24, r5, 0x0A);
        } else if (input_buffer[0] == 'c' && input_buffer[1] == 'a' && input_buffer[2] == 'l' && input_buffer[3] == 'c' && input_buffer[4] == ' ') {
             nl_();
             s_("Use lforth for math.", 24, r5, 0x0C);
        } else if (cmp(input_buffer, "rand") == 1) {
             nl_();
             char rbuf[16];
             i2((t1 * 32719 + 3) % 32749, rbuf);
             s_(rbuf, 24, r5, 0x0F);
        } else if (input_buffer[0] == 'c' && input_buffer[1] == 'o' && input_buffer[2] == 'l' && input_buffer[3] == 'o' && input_buffer[4] == 'r' && input_buffer[5] == ' ') {
            nl_();
            s_("Color changed (WIP)", 24, r5, 0x0E);
        } else if (cmp(input_buffer, "sync") == 1) {
            nl_();
            s_("Flushing buffers to disk...", 24, r5, 0x0E);
            fs_save();
            nl_();
            s_("Disk synced.", 24, r5, 0x0A);
        } else if (cmp(input_buffer, "reboot") == 1) {
            ob(0x64, 0xFE);
        } else {
            nl_();
            s_("Command not found.", 24, r5, 0x0C);
        }
    }
    buffer_index = 0;
}

// shell bash killer
void run_terminal() {
    if (!k_p) {
        __asm__ volatile("hlt");
        return;
    }
    
    k_p = 0;
    unsigned char scancode = l_s;
    
    if (scancode == 0x3A) { c_p = !c_p; return; }
    if (scancode == 0x2A || scancode == 0x36) { s_h = 1; return; }
    if (scancode == 0xAA || scancode == 0xB6) { s_h = 0; return; }
    if (scancode & 0x80) return;
    
    if (scancode < 128) {
        char c = ps(scancode);
        if (c == '\n') {
            b_(ttt, r5, 8, 8, 0x00); 
            execute_command();
            nl_();
            s_("garodaemon@LARPFORNIA:~$ ", 24, r5, 0x0A);
            ttt = 224;
        } else if (c == '\b') {
            if (ttt > 224 && buffer_index > 0) {
                b_(ttt, r5, 8, 8, 0x00); 
                ttt -= 8;
                buffer_index--;
                b_(ttt, r5, 8, 8, 0x00); 
            }
        } else if (c != 0 && buffer_index < 63) {
            b_(ttt, r5, 8, 8, 0x00); 
            input_buffer[buffer_index] = c;
            buffer_index++;
            c_(c, ttt, r5, 0x07);
            ttt += 8;
            if (ttt >= 590) {
                nl_();
                ttt = 24;
            }
        }
    }
}

// SystemD
void shell_init() {
    s_("garodaemon@LARPFORNIA:~$ ", 24, r5, 0x0A);
    ttt = 224;
    while(1) {
        run_terminal();
    }
}
