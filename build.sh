fasm bootloader.asm bootloader.bin

gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c ata.c -o ata.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c cpu.c -o cpu.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c input.c -o input.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c io.c -o io.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c larpfs.c -o larpfs.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c lib.c -o lib.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c main.c -o main.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c shell.c -o shell.o
gcc -m32 -fno-pie -ffreestanding -fno-stack-protector -O0 -c vga.c -o vga.o

ld -m elf_i386 -T linker.ld --oformat binary -o kernel.bin main.o ata.o cpu.o input.o io.o larpfs.o lib.o shell.o vga.o

dd if=/dev/zero of=larpos.img bs=512 count=2880
dd if=bootloader.bin of=larpos.img conv=notrunc
dd if=kernel.bin of=larpos.img seek=1 conv=notrunc
