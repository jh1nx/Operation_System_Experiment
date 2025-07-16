build:
	qemu-img create hd3.img 10m
	nasm -f bin mbr3.asm -o mbr3.bin
	nasm -f bin assignment3_32bits.asm -o assignment3_32bits.bin
	dd if=mbr3.bin of=hd3.img bs=512 count=1 seek=0 conv=notrunc
	dd if=assignment3_32bits.bin of=hd3.img bs=512 count=5 seek=1 conv=notrunc
run:
	qemu-system-i386 -hda hd3.img -serial null -parallel stdio 
clean:
	rm -fr *.bin
