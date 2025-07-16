; If you meet compile error, try 'sudo apt install gcc-multilib g++-multilib' first

%include "head.include"

your_if:
    mov eax, [a1]

    cmp eax, 40
    jl elseif_satisfy

    add eax, 3
    cdq
    mov ecx, 5
    idiv ecx
    mov [if_flag], eax
    
    jmp end_if

elseif_satisfy:
    cmp eax, 18
    jl else_satisfy

    mov ecx, eax
    shl ecx, 1
    mov eax, 80
    sub eax, ecx
    mov [if_flag], eax
    
    jmp end_if

else_satisfy:
    shl eax, 5
    mov [if_flag], eax

end_if:

your_while:
    mov eax, [a2]
    mov esi, [while_flag]
while:
    cmp eax, 25
    jge end_while
    mov [a2], eax

    call my_random
    mov ecx,[a2]
    shl ecx, 1
    mov [esi+ecx], al

    mov eax, [a2]
    inc eax
    jmp while
    
end_while:
    mov [a2], eax

%include "end.include"

your_function:
    mov esi, [your_string]
    xor ecx, ecx

for_loop:
    mov al, [esi+ecx]
    cmp al, 0
    je end_for
    
    pushad
    add al, 9
    push eax
	call print_a_char
	add esp, 4
	popad

    inc ecx
    jmp for_loop

end_for:
    ret



