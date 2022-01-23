assume cs:codesg,ds:datasg


datasg segment
    db 'BaSiC'          ;小写转大写
    db 'iNfOrMaTiOn'    ;大写转小写【只处理前5位】
datasg ends


codesg segment

    start:  mov ax,datasg
            mov ds,ax

            mov bx,0

            mov cx,5
        s:  mov al,[bx]
            and al,11011111B    ;转换小写，之所以将第5位 置0，原因是ASCII大小写字母在二进制只有第5位不同，大写字母第5位是0，小写字母第5位是1，其他位置都相同，大写转换也是相同原理
            mov [bx],al         ;转换完成后，将转换后的字符从寄存器放到原来的内存单元上
            inc bx              ;bx+1，进行下一个字符的转换
            loop s  

            mov bx,5

            mov cx,5
        s0: mov al,[bx]
            or al,0010000B
            mov [bx],al
            inc bx 
            loop s0

            mov ax 4c00H
            int 21H

codesg ends


ends start