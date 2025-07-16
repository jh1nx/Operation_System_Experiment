org 0x7c00
[bits 16]
xor ax, ax ;eax = 0
;初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

;初始化栈指针
mov sp, 0x7c00
mov ax, 0xb800
mov gs, ax

mov ax, 0x03
int 0x10

;显示字符串
mov ax, 1986           ;屏幕中间位置
mov di, ax
mov si, message
mov cx, 14             ;字符串长度为7（2*7=14）
mov ah, 0xEC          ;红色前景，黄色背景

xian_shi_xue_hao:
    mov al, [si]
    mov [gs:di], ax
    add di, 2
    inc si
    loop xian_shi_xue_hao

;初始化变量
mov word [pos], 0       ;初始位置(左上角)
mov word [counter], 0   ;字符计数器初始化为0
mov byte [current_char], 0 ;数字序列索引
mov byte [front_color], 1  ;前景色初始化
mov byte [backround_color], 0x20 ;背景色初始化

main:
    ;更新位置
    call update_position
    
    ;显示字符
    call show_current_char
    
    ;延时
    call _delay
    
    jmp main

;显示字符的函数
show_current_char:

    ;获取当前要显示的数字
    mov bx, current_chars
    movzx ax, byte [current_char]     ;(扩展成16位)
    add bx, ax
    mov al, [bx]        ;从数组中获取数字

    ;设置颜色属性
    mov ah, [backround_color]  
    add ah, [front_color]  

    ;显示字符
    mov bx, [pos]
    mov [gs:bx], ax     

    ;更新color_counter并检查是否需要更改颜色(每3个字符更新一次)
    inc byte [color_counter]
    cmp byte [color_counter], 3
    jne skip_front_update

    ;设置了16种颜色，因此每到16的倍数需要重置颜色计数器
    mov byte [color_counter], 0
    inc byte [front_color]
    cmp byte [front_color], 16
    jne skip_frontcolor_cycle

    mov byte [front_color], 1

skip_frontcolor_cycle:
skip_front_update:

    ;字符序列循环输出
    inc byte [current_char]
    cmp byte [current_char], 10
    jne skip_string_cycle

    mov byte [current_char], 0

skip_string_cycle:

    ;更新计数器和背景色
    inc word [counter]
    mov ax, [counter]
    mov dx, 0
    mov bx, 14
    div bx  
    cmp dx, 0          ;检查是否需要改变背景色
    jne skip_backround_change
    
    add byte [backround_color], 0x10  ;改变背景色
    cmp byte [backround_color], 0     ;检查是否为黑色
    jne skip_backround_change
    mov byte [backround_color], 0x20  ;如果是黑色，改为绿色

skip_backround_change:
    ret

;更新位置的函数
update_position:
    mov ax, [pos]
    
    cmp word [direction], 0    ;向右
    je move_right
    cmp word [direction], 1    ;向下
    je move_down
    cmp word [direction], 2    ;向左
    je move_left
    cmp word [direction], 3    ;向上
    je move_up
    
move_right:
    add ax, 2
    cmp ax, 158         ;右边界
    jle change_position

    sub ax, 2          ;退回到最后一个有效位置
    add ax, 160        ;向下移动一行
    mov word [direction], 1
    jmp change_position
    
move_down:
    add ax, 160
    cmp ax, 4000        ;下边界
    jle change_position

    sub ax, 160        ;退回到最后一个有效位置
    sub ax, 2          ;向左移动一列
    mov word [direction], 2
    jmp change_position
    
move_left:
    sub ax, 2
    cmp ax, 3840        ;左边界
    jge change_position

    add ax, 2          ;退回到最后一个有效位置
    sub ax, 160        ;向上移动一行
    mov word [direction], 3
    jmp change_position
    
move_up:
    sub ax, 160
    cmp ax, 0          ;上边界
    jge change_position

    add ax, 160        ;退回到最后一个有效位置
    add ax, 2          ;向右移动一列
    mov word [direction], 0
    
change_position:
    mov [pos], ax
    ret

;延时函数(通过嵌套循环实现)
_delay:
    pusha
    mov cx, 10
delay_outer:
    push cx
    mov cx, 0xFFFF
delay_inner:
    loop delay_inner
    pop cx
    loop delay_outer
    popa
    ret

;数据区
message db 'jhinx 23336266'  
current_chars db '1','3','5','7','9','0','2','4','6','8' 
current_char db 0          ;显示当前输出的是字符串序列的第几个元素
front_color db 1          
backround_color db 0x20       
pos dw 0              ;显示当前位置
direction dw 0        ;显示当前方位
counter dw 0          ;显示这一背景色下已经输出了多少个字符了
color_counter db 0    ;显示这一前景色下已经输出了多少个字符了


times 510 - ($ - $$) db 0
db 0x55, 0xaa

