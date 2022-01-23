assume cs:code
code segment

    mov bx,0        ;用作ds偏移地址

    mov cx,12
s:  mov ax,0ffffh   ;ax赋值0ffffh【中转】
    mov ds,ax       ;ds赋值0ffffh
    mov dl,[bx]     ;将0ffff:[bx]内存单元的数据复制到dl【中转】
    mov ax,0020h    ;
    mov ds,ax       ;ds赋值0020h
    mov [bx],dl     ;将dl数据赋值到0020:[bx]
    inc bx           
    loop s

    mov ax,4c00h
    int 21h

code ends
end