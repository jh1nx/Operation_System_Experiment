;org 0x7c00
[bits 16]
xor ax, ax ; eax = 0

mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00
mov ax, 0xb800
mov gs, ax

;实现光标的位置获取
mov AH, 03H
mov BX, 00H
int 10H

;实现光标的移动
mov ah, 0x02
mov bh, 0x00
mov dh, 10
mov dl, 20
int 0x10

jmp $;
times 510 - ($ - $$) db 0
db 0x55, 0xaa

    

