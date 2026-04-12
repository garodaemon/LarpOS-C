extern void* malloc(int size);

struct larp_file {
    char name[32];
    char* content;
    int size;
};

struct larp_file fs_root[16];
int fs_file_count = 0;

int strcmp(char *s1, char *s2);
void print_string(char *str, int x, int y, unsigned char color);

void larpfs_add_file(char *filename, char *data) {
    if (fs_file_count >= 16) return;

    int i = 0;
    while (filename[i] != '\0' && i < 31) {
        fs_root[fs_file_count].name[i] = filename[i];
        i++;
    }
    fs_root[fs_file_count].name[i] = '\0';

    int len = 0;
    while (data[len] != '\0') {
        len++;
    }

    fs_root[fs_file_count].content = (char*)malloc(len + 1);

    int j = 0;
    while (j < len) {
        fs_root[fs_file_count].content[j] = data[j];
        j++;
    }
    fs_root[fs_file_count].content[j] = '\0';
    fs_root[fs_file_count].size = j;
    
    fs_file_count++;
}

void larpfs_init() {
    larpfs_add_file("notlar.txt", "Larplando 0.3 Dosya Sistemi Calisiyor.");
    larpfs_add_file("readme.md", "LarpOS C Edition - Larplando");
    larpfs_add_file("sifreler.txt", "admin:123456");
    larpfs_add_file("supporters", "Xyl, /home/kali_linx, Diavolo, turkdogru");
}

int larpfs_find(char *filename) {
    for (int i = 0; i < fs_file_count; i++) {
        if (strcmp(fs_root[i].name, filename) == 1) {
            return i;
        }
    }
    return -1;
}

char* larpfs_get_content(int index) {
    if (index >= 0 && index < fs_file_count) {
        return fs_root[index].content;
    }
    return 0;
}

char* larpfs_get_name(int index) {
    if (index >= 0 && index < fs_file_count) {
        return fs_root[index].name;
    }
    return 0;
}

int larpfs_get_count() {
    return fs_file_count;
}
