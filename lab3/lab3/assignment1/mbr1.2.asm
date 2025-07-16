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
mov ax, 1                ; 逻辑扇区号第0~15位
mov cx, 0                ; 逻辑扇区号第16~31位
mov bx, 0x7e00           ; bootloader的加载地址
load_bootloader:
    call asm_read_hard_disk  ; 读取硬盘
    inc ax
    cmp ax, 5
    jle load_bootloader
jmp 0x0000:0x7e00        ; 跳转到bootloader

jmp $ ; 死循环


;逻辑扇区号向CHS的转换公式：
;扇区号S=(逻辑扇区号L%63(每磁道的扇区数SPT))+1
;磁头号H=(L/63)%18（每柱面的磁头数）
;柱面号C=(L/63)/18

asm_read_hard_disk:                       
; CHS模式读取硬盘

; 参数列表
; ax=逻辑扇区号低16位
; cx=逻辑扇区号高16位
; ds:bx=读取出的数据放入地址
; al=扇区数量
; ch=柱面号的低8位
; cl=扇区号(低6位)|柱面号高2位(CL的高2位)
; dh=磁头号
; dl=磁盘编号(0x80是第一个硬盘)
; bx=目标内存地址
; L=逻辑扇区号(LBA)

; 返回值
; bx=bx+512
    
    mov si, 63	;每磁道有63个扇区
    mov di, 18	;每柱面有18个磁头
    
    xor dx, dx	;将dx清零
    div si
    inc dx
    mov cl, dl	;将扇区号存储到 cl
    
    xor dx, dx
    div di
    mov dh, dl	;将余数存储到dh,表示磁头号

    mov ch, al
    shr ax, 8	;将ax右移8位,获取柱面号的高2位
    and al, 0x03	;保留高2位
    or cl, al	;将柱面号的高2位与扇区号合并
    
    mov dl, 0x80	;0x80为第一个硬盘
    mov ah, 0x02	;功能号为0x02(读扇区)
    mov al, 1	;设置读取的扇区数为1
    int 0x13

    add bx, 512	;更新 bx地址
    
    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa
