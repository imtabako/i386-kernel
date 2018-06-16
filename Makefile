KERNEL_LD = link-kernel.ld
CC = i686-elf-gcc
CFLAGS = -Wall -Wextra -Iinclude -nostdlib -ffreestanding
RAM = 32M

sources = $(shell find src/ -name "*.c")
temp = $(sources:.c=.o)
temp2 = $(notdir $(temp))
objects = $(addprefix bin/, $(temp2))
headers = $(shell ls include/*.h)


all : bin/boot.iso

test : bin/boot.iso
	qemu-system-i386 -cdrom $^ -m $(RAM) >/dev/null 2<&1 &

bin/%.o : $(headers)

bin/%.o : src/%.c
	$(CC) $(CFLAGS) -c -T $(KERNEL_LD) -o $@ $<

bin/boot.bin : src/boot.s $(objects)
	$(CC) $(CFLAGS) -T $(KERNEL_LD) -o $@ $^ -lgcc


bin/boot.iso : bin/boot.bin
	mkdir -p iso/boot/grub
	cp .stage2_eltorito iso/boot/grub/stage2_eltorito
	cp bin/boot.bin iso/boot/boot.bin
	cp .menu.lst iso/boot/grub/menu.lst
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/boot.iso iso/

clean : 
	rm -f bin/boot.bin
	rm -f $(objects)
	rm -rf iso/

.PHONY : all clean
