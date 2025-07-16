build1:
	nasm -f bin mbr1.1.asm -o mbr1.1.bin
	nasm -f bin bootloader1.asm -o bootloader1.bin
	dd if=mbr1.1.bin of=hd1.1.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader1.bin of=hd1.1.img bs=512 count=5 seek=1 conv=notrunc
build2:
	nasm -f bin mbr1.2.asm -o mbr1.2.bin
	nasm -f bin bootloader1.asm -o bootloader1.bin
	dd if=mbr1.2.bin of=hd1.2.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader1.bin of=hd1.2.img bs=512 count=5 seek=1 conv=notrunc
run1:
	qemu-system-i386 -hda hd1.1.img -serial null -parallel stdio 
run2:
	qemu-system-i386 -hda hd1.2.img -serial null -parallel stdio 
clean:
	rm -fr *.bin
