org 0x7c00
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

; 设置光标位置到 (15,0)
mov ah, 0x02
mov bh, 0x00
mov dh, 15
mov dl, 0
int 0x10

my_keyboard:
    ;利用键盘中断实现键盘输入
    mov ah, 0x00
    int 0x16

    ;检查是否为回车键(ASCII=0x0D)
    cmp al, 0x0D
    je _enter

    ;检查是否为退格键(ASCII=0x08)
    cmp al, 0x08
    je _backspace

    cmp al, 0x09
    je _tab

    ;利用键盘中断回显键盘输入
    mov ah, 0x0E    ;显示字符
    mov bh, 0x00
    mov bl, 0x07
    int 0x10

    ;继续读取键盘输入
    jmp my_keyboard

_enter:     ;处理回车键（即行号加1，列号重置为0）
    mov dl, 0
    inc dh
    mov ah, 0x02
    int 0x10

    jmp my_keyboard

_backspace:     ;处理退格键（即删除当前字符，并且列号减1）

    ;先输出一个显示退格
    mov ah, 0x0e
    mov al, 0x08
    dec dl
    mov bh, 0x00
    int 10h

    ;然后把当前光标字符改成‘ ’
    mov ah, 0x0e
    mov al, ' '
    mov bh, 0x00
    int 10h

    ;最后再次输出一个显示退格，就完成了退格任务
    mov ah, 0x0e
    mov al, 0x08
    dec dl
    mov bh, 0x00
    int 10h

    ; mov ah, 0x03
    ; mov bx, 0x00
    ; int 0x10

    ; dec dl
    ; mov ah, 0x02
    ; int 0x10

    ; mov ah, 0x0e
    ; mov al, ' '
    ; int 0x10

    ; mov ah, 0x03
    ; mov bx, 0x00
    ; int 0x10

    ; dec dl
    ; mov ah, 0x02
    ; int 0x10

    ;继续读取键盘输入
    jmp my_keyboard

_tab:
    mov ah, 0x03
    mov bx, 0x00
    int 0x10

    mov ah, 0x0e
    mov al, ' '
    int 0x10

    mov ah, 0x0e
    mov al, ' '
    int 0x10

    mov ah, 0x0e
    mov al, ' '
    int 0x10
    
    mov ah, 0x0e
    mov al, ' '
    int 0x10

    jmp my_keyboard
times 510 - ($ - $$) db 0
db 0x55, 0xaa
