[bits 32]

global asm_hello_world
global asm_lidt
global asm_unhandled_interrupt
global asm_halt
global asm_out_port
global asm_in_port
global asm_enable_interrupt
global asm_disable_interrupt
global asm_interrupt_status
global asm_init_page_reg
global asm_segmentation_fault
ASM_UNHANDLED_INTERRUPT_INFO db 'Unhandled interrupt happened, halt...'
                             db 0

ASM_PAGE_FAULT_INFO db 'Segmentation fault happened, halt...'
                    db 0

ASM_IDTR dw 0
         dd 0

; void asm_segmentation_fault()
asm_segmentation_fault:
    cli
    mov esi, ASM_PAGE_FAULT_INFO
    xor ebx, ebx
    mov ah, 0x04  
.output_information:
    cmp byte[esi], 0
    je .end
    mov al, byte[esi]
    mov word[gs:bx], ax
    inc esi
    add ebx, 2
    jmp .output_information
.end:
    ; 获取导致段错误的地址（存储在CR2寄存器中）
    mov eax, cr2
    ; 输出错误地址（这里简化处理，实际上可以格式化输出地址）
    add ebx, 88   ; 下一行显示
    mov ah, 0x04  
    mov al, '0'
    mov word[gs:ebx], ax
    add ebx, 2
    mov al, 'x'
    mov word[gs:ebx], ax
    add ebx, 2

    mov ecx, 8              ; 8个十六进制字符
    mov edx, eax            ; 保存原始地址
    
.hex_loop:
    rol edx, 4              ; 循环左移4位
    mov al, dl
    and al, 0x0F            ; 取低4位
    
    ; 转换为ASCII字符
    cmp al, 9
    jbe .digit
    add al, 7               ; A-F转换
.digit:
    add al, '0'             ; 0-9转换
    mov [gs:ebx], ax
    add ebx, 2
    loop .hex_loop
    
    jmp $
    
; void asm_init_page_reg(int *directory);
asm_init_page_reg:
    push ebp
    mov ebp, esp

    push eax

    mov eax, [ebp + 4 * 2]
    mov cr3, eax ; 放入页目录表地址
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax           ; 置PG=1，开启分页机制

    pop eax
    pop ebp

    ret

; int asm_interrupt_status();
asm_interrupt_status:
    xor eax, eax
    pushfd
    pop eax
    and eax, 0x200
    ret

; void asm_disable_interrupt();
asm_disable_interrupt:
    cli
    ret
; void asm_init_page_reg(int *directory);

asm_enable_interrupt:
    sti
    ret

; void asm_in_port(uint16 port, uint8 *value)
asm_in_port:
    push ebp
    mov ebp, esp

    push edx
    push eax
    push ebx

    xor eax, eax
    mov edx, [ebp + 4 * 2] ; port
    mov ebx, [ebp + 4 * 3] ; *value

    in al, dx
    mov [ebx], al

    pop ebx
    pop eax
    pop edx
    pop ebp
    ret

; void asm_out_port(uint16 port, uint8 value)
asm_out_port:
    push ebp
    mov ebp, esp

    push edx
    push eax

    mov edx, [ebp + 4 * 2] ; port
    mov eax, [ebp + 4 * 3] ; value
    out dx, al
    
    pop eax
    pop edx
    pop ebp
    ret


; void asm_unhandled_interrupt()
asm_unhandled_interrupt:
    cli
    mov esi, ASM_UNHANDLED_INTERRUPT_INFO
    xor ebx, ebx
    mov ah, 0x03
.output_information:
    cmp byte[esi], 0
    je .end
    mov al, byte[esi]
    mov word[gs:bx], ax
    inc esi
    add ebx, 2
    jmp .output_information
.end:
    jmp $

; void asm_lidt(uint32 start, uint16 limit)
asm_lidt:
    push ebp
    mov ebp, esp
    push eax

    mov eax, [ebp + 4 * 3]
    mov [ASM_IDTR], ax
    mov eax, [ebp + 4 * 2]
    mov [ASM_IDTR + 2], eax
    lidt [ASM_IDTR]

    pop eax
    pop ebp
    ret

asm_hello_world:
    push eax
    xor eax, eax

    mov ah, 0x03 ;青色
    mov al, 'H'
    mov [gs:2 * 0], ax

    mov al, 'e'
    mov [gs:2 * 1], ax

    mov al, 'l'
    mov [gs:2 * 2], ax

    mov al, 'l'
    mov [gs:2 * 3], ax

    mov al, 'o'
    mov [gs:2 * 4], ax

    mov al, ' '
    mov [gs:2 * 5], ax

    mov al, 'W'
    mov [gs:2 * 6], ax

    mov al, 'o'
    mov [gs:2 * 7], ax

    mov al, 'r'
    mov [gs:2 * 8], ax

    mov al, 'l'
    mov [gs:2 * 9], ax

    mov al, 'd'
    mov [gs:2 * 10], ax

    pop eax
    ret

asm_halt:
    jmp $
