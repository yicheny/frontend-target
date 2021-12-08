assume cs:code,ds:data,ss:stack


data segment
    dw 0123H,0456H,0789H,0abcH,0defh,0fedh,0cbah,0987h
data ends


stack segment
    dw 0,0,0,0,0,0,0,0
stack ends


code segment
start:  mov ax,stack
        mov ss,ax
        mov sp,16       ;设置ss:sp指向stack:16
        mov ax,data
        mov ds,ax       ;ds指向data段
        mov bx,0        ;bx设置为0，则ds:bx指向data段的第一个单元
        
        mov cx,8
s:      push [bx]
        add bx,2        ;+2是因为字型数据占据两个字节单元
        loop s          ;将data段中的0-16单元中的8个字型数据依次入栈
        mov bx,0        ;置0，等会将栈中数据依次取出，顺序放回data段，这样数据顺序就被逆转了
        
        mov cx,8
s0:     pop [bx]
        add bx,2
        loop s0
        mov ax,4c00h
        int 21
code ends


end start
