
CC := gcc

CONFIGS := -DCONFIG_HEAP_SIZE=4096

CFLAGS := -O0 -ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall $(CONFIGS)

LOOPDEV := /dev/loop1

ODIR = obj
SDIR = src

OBJS = \
	boot.o \
	ide.o \
	rprintf.o \
	term.o \
	main.o \
	fat.o \

OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -g -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.s
	nasm -f elf32 -g -o $@ $^

all: setup bin img

setup:
	mkdir -p obj

bin: $(OBJ)
	ld -melf_i386 obj/* -Tkernel.ld -o kernel
	size kernel


clean:
	rm -f obj/*
	rm -f disk.img
	rm -f rootfs.img
	rm -f kernel

debug:
	qemu-system-i386 -hda disk.img -S -s -k en-us &
	gdb -x gdb_init_prot_mode.txt

run:
	qemu-system-i386 -hda disk.img -k en-us

disassemble:
	objdump -b binary -m i386 -D --adjust-vma=0x100000 kernel

rootfs.img:
	dd if=/dev/zero of=rootfs.img bs=1M count=16
	mkfs.fat -F16 rootfs.img
	sudo mount rootfs.img /mnt/disk
	sudo mkdir -p /mnt/disk/boot/grub
	sudo umount /mnt/disk

img: rootfs.img
	rm -f disk.img
	dd if=/dev/zero of=disk.img count=131072 bs=512 # Make big disk image (64MB) filled with zeros
	# Copies rootfs image to the first partition
	dd if=rootfs.img of=disk.img seek=2048 conv=notrunc
	# Repartition the disk to occupy the whole image
	./partition.sh disk.img
	# Create a loopback device at offset 1M in the disk.img. This is the start of the partition we created
	sudo losetup -d $(LOOPDEV) || { echo "loop device not created"; }
	sudo losetup $(LOOPDEV) disk.img -o 1048576
	# Mount the loopback device
	sudo mount $(LOOPDEV) /mnt/disk
	# Make boot directory with grub subdir
	sudo mkdir -p /mnt/disk/boot/grub/{i386-pc,locale}
	# Copy kernel image to rootfs
	sudo cp kernel /mnt/disk/boot/
	sudo cp grub.cfg /mnt/disk/boot/grub
	# Install grub
	sudo grub-install --no-floppy --root-directory=/mnt/disk --themes= --fonts= --locales= --modules="normal part_msdos multiboot" disk.img
	
	# Create a text file and add some text to it
	#sudo touch /mnt/disk/fun.txt
	echo "Hello world!" > fun.txt
	sudo cp fun.txt /mnt/disk/
	
	# Unmount the loopback device
	sudo umount $(LOOPDEV)
	# Unbind the loopback device
	sudo losetup -d $(LOOPDEV)

mountroot:
	sudo test -s $(LOOPDEV) || { sudo losetup $(LOOPDEV) disk.img -o 1048576; }
	sudo mount $(LOOPDEV) /mnt/disk

umountroot:
	sudo umount $(LOOPDEV) || { echo "$(LOOPDEV) not mounted"; }
	sudo losetup -d $(LOOPDEV)


