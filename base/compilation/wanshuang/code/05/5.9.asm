assume cs:code
code segment

    mov ax,0ffffh
    mov ds,ax       ;设置(ds)=ffffh
    mov ax,0020h
    mov es,ax       ;设置(es)=0020h
    mov bx,0        ;作为数据段偏移指针，初始化为0
    
    mov cs,12       ;循环12次
s:  mov dl,[bx]     ;ds:[bx]指向的数据传送至dl
    mov es:[bx],dl  ;将dl的数据传送到es:[bx]指向的内存单元
    inc bx
    loop s

    mov ax,4c00h
    int 21h

code ends
end