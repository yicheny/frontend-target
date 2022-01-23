;逆转数据
;在代码段中使用数据和栈

assume cs:code

code segment:
    ;数据空间
    dw 0123h,0456h,0789h,0abch,0defh,0fedh,0cbah,0987h 
    ;栈空间
    dw 0,0,0,0,0,0,0,0

start:  mov ax,cs
        mov ss,ax
        mov sp,32
        mov bx,0

        mov cx,8
    s:  push cs:[bx]
        add bx,2
        loop s
        
        mov bx,0
        mov cx,8
    s0: pop cs:[bx]
        add bx,2
        loop s0

        mov ax,4c00h
        int 21h

code ends
end start