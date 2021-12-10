;双循环

assume cs:codesg,ds:datasg


datasg segment
    db 'ibm                '
    db 'dec                '
    db 'dos                '
    db 'vax                '
    dw 0
datasg ends


codesg segment

    start:  mov ax,datasg
            mov ds,ax
            mov bx,0            ;列偏移值
            
            mov cx,4
       s0:  mov ds:[40h],cx     ;将cx的值存到ds:[40h]内存单元中
            mov si,0
            
            mov cx,3
        s:  mov al,[bx+si]
            and al,11011111b
            mov [bx+si],al
            inc si
            loop s
            add bx,16           ;转移至下一列
            mov cx,ds:[40h]     ;从ds:[40h]取值送到cx，给s0循环使用
            loop s0

            mov ax,4c00h
            int 21h

codesg ends


end