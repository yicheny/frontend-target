assume cs:codesg,ds:datasg


datasg segment
    db 'BaSiC'          
    db 'iNfOrMaTiOn'   
datasg ends


codesg segment

    start:  mov ax,datasg
            mov ds,ax

            mov bx,0
            
            mov cx,5
        s:  mov al,[bx]
            and al,11011111b    ;对第一个字符串的字符转换大写
            mov [bx],al

            mov al,[5+bx]       ;这里的5是'BaSiC'的长度，用以偏移至第二个字符串
            or al,0010000b      ;对第二个字符串的字符转换小写
            mov [5+bx],al
            inc bx
            loop s
            
codesg ends


ends start