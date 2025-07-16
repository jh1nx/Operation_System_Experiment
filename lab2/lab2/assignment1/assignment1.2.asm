org 0x7c00
[bits 16]
xor ax, ax ; eax = 0
; 初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
mov ax, 0xb800
mov gs, ax


mov ah, 0x42 ; 红色背景，绿色前景
mov al, '2'
mov [gs:2 * 1290], ax

mov al, '3'
mov [gs:2 * 1291], ax

mov al, '3'
mov [gs:2 * 1292], ax

mov al, '3'
mov [gs:2 * 1293], ax

mov al, '6'
mov [gs:2 * 1294], ax

mov al, '2'
mov [gs:2 * 1295], ax

mov al, '6'
mov [gs:2 * 1296], ax

mov al, '6'
mov [gs:2 * 1297], ax


jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa
