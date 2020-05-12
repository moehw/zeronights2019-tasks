nasm -f bin loader.asm -o loader.bin
gcc main.c -nostdlib -masm=intel -m32 -o code.bin
# sleep 1
objcopy -O binary code.bin

# loader is first sector
dd if=loader.bin of=disk.bin

# MBR signature
echo -e -n "\x55\xAA" | dd of=disk.bin bs=1 count=2 seek=510

# code on 0x200
dd if=code.bin skip=3693 of=disk.bin seek=512 bs=1 count=10000

# zeroes
dd if=bare of=disk.bin oflag=append conv=notrunc

# glyph on 0x2000
dd if=glyph.bin of=disk.bin bs=1 seek=8192 count=4096

# picture on 0x3000
# dd if=tv.bmp of=disk.bin bs=1 seek=12288 count=10000

# additional zero sector
#dd if=bare of=disk.bin bs=1 seek=45056 count=512
dd if=bare of=disk.bin bs=1 seek=12288 count=512

qemu-system-i386 disk.bin
