;将代码、数据、栈放到不同的段中

assume cs:code,ds:data,ss:stack

;数据段
data segment
    dw 0123H,0456H,0789H,0abcH,0defh,0fedh,0cbah,0987h
data ends

;栈段
stack segment
    dw 0,0,0,0,0,0,0,0
stack ends

;代码段
code segment
;start初始化寄存器信息
start:  mov ax,stack
        mov ss,ax
        mov sp,16       ;设置ss:sp指向stack:16
        mov ax,data
        mov ds,ax       ;ds指向data段
        mov bx,0        ;bx设置为0，则ds:bx指向data段的第一个单元

;s代码是将data数据推入栈
        mov cx,8
s:      push [bx]
        add bx,2        ;+2是因为字型数据占据两个字节单元
        loop s          ;将data段中的0-16单元中的8个字型数据依次入栈
        mov bx,0        ;置0，等会将栈中数据依次取出，顺序放回data段，这样数据顺序就被逆转了

;s0代码是将栈中数据推入data
        mov cx,8
s0:     pop [bx]
        add bx,2
        loop s0

        mov ax,4c00h
        int 21

code ends
end start
