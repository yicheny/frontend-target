;执行完成后，(ip)=0，cs:ip指向代码段的第一条指令

assume cs:code

stack segment
    db 16 dup (0)
stack ends

code segment
            mov ax,4c00h ;这里是程序的第一条指令
            int 21h

    start:  mov ax,stack
            mov ss,ax ;将stack地址段设置为ss
            mov sp,16 ;设置sp
            mov ax,0    
            push ax   ;将数据0推入栈
            mov bx,0
            ret       ;pop ip，所以此时ip变成0，指向程序的第一条指令  
code ends
end start