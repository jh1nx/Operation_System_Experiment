%include "boot.inc"
org 0x7e00
[bits 16]

; 初始化段寄存器
xor ax, ax
mov ds, ax
mov es, ax
mov fs, ax
mov ss, ax
mov sp, 0x7c00

; 清屏
mov ax, 0x03
int 0x10


;空描述符
mov dword [GDT_START_ADDRESS+0x00],0x00
mov dword [GDT_START_ADDRESS+0x04],0x00  

;创建描述符，这是一个数据段，对应0~4GB的线性地址空间
mov dword [GDT_START_ADDRESS+0x08],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x0c],0x00cf9200    ; 粒度为4KB，存储器段描述符 

;建立保护模式下的堆栈段描述符      
mov dword [GDT_START_ADDRESS+0x10],0x00000000    ; 基地址为0x00000000，界限0x0 
mov dword [GDT_START_ADDRESS+0x14],0x00409600    ; 粒度为1个字节

;建立保护模式下的显存描述符   
mov dword [GDT_START_ADDRESS+0x18],0x80007fff    ; 基地址为0x000B8000，界限0x07FFF 
mov dword [GDT_START_ADDRESS+0x1c],0x0040920b    ; 粒度为字节

;创建保护模式下平坦模式代码段描述符
mov dword [GDT_START_ADDRESS+0x20],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x24],0x00cf9800    ; 粒度为4kb，代码段描述符 

;初始化描述符表寄存器GDTR
mov word [pgdt], 39      ;描述符表的界限
lgdt [pgdt]

in al, 0x92             ;南桥芯片内的端口 
or al, 0000_0010B
out 0x92, al            ;打开A20

cli                     ;关中断
mov eax, cr0
or eax, 0x1
mov cr0, eax            ;设置PE位

;以下进入保护模式
jmp dword CODE_SELECTOR:protect_mode_begin

;16位的描述符选择子：32位偏移
;清流水线并串行化处理器
[bits 32]
protect_mode_begin:
    ; 设置段寄存器
    mov ax, DATA_SELECTOR           ;加载数据段(0..4GB)选择子
    mov ds, ax
    mov es, ax
    mov ax, STACK_SELECTOR
    mov ss, ax
    mov ax, VIDEO_SELECTOR
    mov gs, ax
    mov esp, 0x90000  ; 设置新的栈顶
    
    ; 在保护模式下显示学号
    call xian_shi_xue_hao
    
    ; 初始化变量
    mov dword [pos], 0       ; 初始位置(左上角)
    mov dword [counter], 0   ; 字符计数器初始化为0
    mov byte [current_char], 0 ; 数字序列索引
    mov byte [front_color], 1  ; 前景色初始化
    mov byte [backround_color], 0x20 ; 背景色初始化
    mov byte [color_counter], 0 ; 颜色计数器初始化

    ; 跳转到主循环
    jmp main

; 在保护模式下显示学号
xian_shi_xue_hao:
    pushad
    
    mov edi, 1986          ; 屏幕中间位置
    mov ecx, xian_shi_xue_hao_end - message  ; 字符串长度
    mov ebx, message       
    mov ah, 0xEC           ; 红色前景，黄色背景

.display_loop:
    mov al, [ebx]          ; 获取字符
    mov [gs:edi], ax       ; 写入显存
    add edi, 2             ; 移动到下一个字符位置
    inc ebx                ; 指向下一个字符
    loop .display_loop     
    
    popad
    ret

; 主程序循环
main:
    ; 更新位置
    call update_position
    
    ; 显示字符
    call show_current_char
    
    ; 延时
    call _delay
    
    jmp main

; 显示字符的函数
show_current_char:
    pushad
    
    ; 获取当前要显示的数字
    mov ebx, current_chars
    movzx eax, byte [current_char]
    add ebx, eax
    mov al, [ebx]        ; 从数组中获取数字

    ; 设置颜色属性
    mov ah, [backround_color]  
    add ah, [front_color]  

    ; 显示字符
    mov ebx, [pos]
    mov [gs:ebx], ax     

    ; 更新color_counter并检查是否需要更改颜色(每3个字符更新一次)
    inc byte [color_counter]
    cmp byte [color_counter], 3
    jne skip_front_update

    ; 设置了16种颜色，因此每到16的倍数需要重置颜色计数器
    mov byte [color_counter], 0
    inc byte [front_color]
    cmp byte [front_color], 16
    jne skip_frontcolor_cycle

    mov byte [front_color], 1

skip_frontcolor_cycle:
skip_front_update:

    ; 字符序列循环输出
    inc byte [current_char]
    cmp byte [current_char], 10
    jne skip_string_cycle

    mov byte [current_char], 0

skip_string_cycle:

    ; 更新计数器和背景色
    inc dword [counter]
    mov eax, [counter]
    mov edx, 0
    mov ebx, 14
    div ebx  
    cmp edx, 0          ; 检查是否需要改变背景色
    jne skip_backround_change
    
    add byte [backround_color], 0x10  ; 改变背景色
    cmp byte [backround_color], 0     ; 检查是否为黑色
    jne skip_backround_change
    mov byte [backround_color], 0x20  ; 如果是黑色，改为绿色

skip_backround_change:
    popad
    ret

; 更新位置的函数
update_position:
    pushad
    
    mov eax, [pos]
    
    cmp dword [direction], 0    ; 向右
    je move_right
    cmp dword [direction], 1    ; 向下
    je move_down
    cmp dword [direction], 2    ; 向左
    je move_left
    cmp dword [direction], 3    ; 向上
    je move_up
    
move_right:
    add eax, 2
    cmp eax, 158         ; 右边界
    jle change_position

    sub eax, 2          ; 退回到最后一个有效位置
    add eax, 160        ; 向下移动一行
    mov dword [direction], 1
    jmp change_position
    
move_down:
    add eax, 160
    cmp eax, 4000        ; 下边界
    jle change_position

    sub eax, 160        ; 退回到最后一个有效位置
    sub eax, 2          ; 向左移动一列
    mov dword [direction], 2
    jmp change_position
    
move_left:
    sub eax, 2
    cmp eax, 3840        ; 左边界
    jge change_position

    add eax, 2          ; 退回到最后一个有效位置
    sub eax, 160        ; 向上移动一行
    mov dword [direction], 3
    jmp change_position
    
move_up:
    sub eax, 160
    cmp eax, 0          ; 上边界
    jge change_position

    add eax, 160        ; 退回到最后一个有效位置
    add eax, 2          ; 向右移动一列
    mov dword [direction], 0
    
change_position:
    mov [pos], eax
    popad
    ret

; 延时函数(通过循环实现)
_delay:
    pushad
    mov ecx, 0x50000  ; 32位下调整为更大的计数
delay_loop:
    loop delay_loop
    popad
    ret

jmp $ ;

; 数据区
pgdt dw 0
     dd GDT_START_ADDRESS

message db 'jhinx 23336266'
xian_shi_xue_hao_end:

current_chars db '1','3','5','7','9','0','2','4','6','8' 

current_char db 0          ; 显示当前输出的是字符串序列的第几个元素
front_color db 1          
backround_color db 0x20       
pos dd 0              ; 显示当前位置(改为32位)
direction dd 0        ; 显示当前方位(改为32位)
counter dd 0          ; 显示这一背景色下已经输出了多少个字符了(改为32位)
color_counter db 0    ; 显示这一前景色下已经输出了多少个字符了
