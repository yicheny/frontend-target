assume cs:codesg
codesg segment
    mov ax,2000H
    mov ss,ax
    mov sp,0
    add sp,4
    push ax,
    push bx,
    pop ax,
    pop bx,
    mov ax,4c00H
    int 21H
codesg ends
end