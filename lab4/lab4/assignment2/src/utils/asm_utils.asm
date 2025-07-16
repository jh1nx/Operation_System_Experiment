[bits 32]

global asm_show_number

asm_show_number:
    push eax
    xor eax, eax

    mov ah, 0x12 
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

    pop eax
    ret
