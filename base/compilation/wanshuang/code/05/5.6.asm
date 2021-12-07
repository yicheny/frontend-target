assume cs:code
code segment

    mov ax,0ffffh   ;汇编中数据不能以字母开头

    mov ds,ax       ;初始化ds=ffff    
    mov bx,0        ;初始化bx，bx寄存器在接下来会作为数据段偏移地址使用

    mov dx,0        ;初始化累加寄存器，(dx)=0
    
    mov cx,12       ;初始化循环寄存器，(cx)=12

s:  mov al,[bx]     ;将ds:bx指向的数据复制到al中
    mov ah,0        ;ah置0
    add dx,ax       ;将ax值累加到ax
    inc bx          ;bx自增1，这里相当于向下偏移一个字节单元
    loop s

    mov ax,4c00h
    int 21h

code ends
end