run:
	@qemu-system-i386 -hda hd2.1.img -serial null -parallel stdio 
debug:
	@qemu-system-i386 -s -S -hda hd2.1.img -serial null -parallel stdio &
	@sleep 1
	@gnome-terminal -e "gdb -q -x gdbinit"
build:
	@nasm -g -f elf32 mbr2.1.asm -o mbr2.1.o
	@ld -o mbr2.1.symbol -melf_i386 -N mbr2.1.o -Ttext 0x7c00
	@ld -o mbr2.1.bin -melf_i386 -N mbr2.1.o -Ttext 0x7c00 --oformat binary
	@nasm -g -f elf32 bootloader2.1.asm -o bootloader2.1.o
	@ld -o bootloader2.1.symbol -melf_i386 -N bootloader2.1.o -Ttext 0x7e00
	@ld -o bootloader2.1.bin -melf_i386 -N bootloader2.1.o -Ttext 0x7e00 --oformat binary
	@dd if=mbr2.1.bin of=hd2.1.img bs=512 count=1 seek=0 conv=notrunc
	@dd if=bootloader2.1.bin of=hd2.1.img bs=512 count=5 seek=1 conv=notrunc
clean:
	@rm -fr *.bin *.o

