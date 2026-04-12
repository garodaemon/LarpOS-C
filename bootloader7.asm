use16
org 0x7c00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    mov [boot_drive], dl
    sti

    mov ax, 0x0003
    int 0x10

    mov si, boot_msg
    call print_rm

show_menu:
    mov si, menu_msg
    call print_rm

wait_key:
    mov ah, 0x00
    int 0x16
    cmp al, '1'
    je boot_kernel
    cmp al, '2'
    je halt_sys
    jmp wait_key

halt_sys:
    mov si, halt_msg
    call print_rm
    jmp $

boot_kernel:
    mov bl, 4

.load_chunk:
    mov di, 3

.retry:
    mov ah, 0x42
    mov si, dap
    mov dl, [boot_drive]
    int 0x13
    jnc .read_ok
    
    xor ax, ax
    int 0x13
    dec di
    jnz .retry
    jmp disk_error

.read_ok:
    mov ax, [dap_seg]
    add ax, 0x0800
    mov [dap_seg], ax
    add dword [dap_lba], 64
    dec bl
    jnz .load_chunk

    cli
    
    mov ax, 0x2401
    int 0x15
    
    in al, 0x92
    or al, 2
    out 0x92, al

    xor ax, ax
    mov ds, ax
    dec ax
    mov es, ax

    mov cx, word [0x0500]
    mov dx, word [es:0x0510]

    mov word [0x0500], 0x1234
    mov word [es:0x0510], 0x5678
    mov ax, word [0x0500]

    mov word [0x0500], cx
    mov word [es:0x0510], dx

    cmp ax, 0x5678
    je halt_sys

    xor ax, ax
    mov es, ax

    align 4
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm_start

print_rm:
    mov ah, 0x0e
.loop:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

disk_error:
    mov si, disk_err_msg
    call print_rm
    jmp halt_sys

boot_msg db 'LarpOS C 0.3', 13, 10, 0
menu_msg db '1-Start', 13, 10, 0
halt_msg db 'Halt', 0
disk_err_msg db 'Disk Err', 0
boot_drive db 0

align 4
dap:
    db 0x10
    db 0
    dw 64
dap_off:
    dw 0x0000
dap_seg:
    dw 0x1000
dap_lba:
    dd 1
    dd 0

align 4
gdt_start:
    dq 0
    dw 0xFFFF, 0x0000
    db 0x00, 10011010b, 11001111b, 0x00
    dw 0xFFFF, 0x0000
    db 0x00, 10010010b, 11001111b, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

use32
pm_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x10000

use16
db 510-($-start) dup 0
dw 0xaa55
