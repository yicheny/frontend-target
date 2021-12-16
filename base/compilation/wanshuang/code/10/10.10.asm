;计算data段中第一组数据的3次方，结果保存在第二组的dword单元中

assume cs:code

data segment
    dw 1,2,3,4,5,6,7,8
    dd 0,0,0,0,0,0,0,0
data ends

code segment
    start:  mov ax,data
            mov ds,ax
            mov si,0    ; 初始化参数地址
            mov di,0    ; 初始化结果【返回值】地址
        
            mov cx,8
        s:  mov bx,[si]
            call cube   ;计算三次方
            mov [di],ax     ;将低位结果放到[di]中
            mov [di].2,dx   ;将高位结果放到[di].2中
            add si,2        ;移动至下一参数地址
            add di,4    ;移动至下一返回值地址
            loop s

            mov ax,4c00h
            int 21h
        
    cube:   mov ax,bx
            mul bx 
            mul bx
            ret

code ends
end start
