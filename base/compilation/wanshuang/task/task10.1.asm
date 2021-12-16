;显示字符串

;知乎：为什么显卡在内存中的映射是0b8000H
;链接：https://www.zhihu.com/question/269649445

assume cs:code

data segment
        db 'Welcome to masm!',0
data ends

code segment

start:  mov dh,8
        mov dl,3
        mov cl,2
        mov ax,data
        mov ds,ax
        mov si,0
        call show_str
        mov ax,4c00h
        int 21h

show_str:
        push ax
        push bx
        push bp         ;bp用来读取显存中的偏移地址
        push es         ;es用来读取显存中的起始地址

        mov ax,0b800h
        mov es,ax         ;将起始地址存入es

        mov al,dh         ;显存偏移地址行号的偏移计算
        mov ah,0
        sub al,1
        mov bl,0a0h
        mul bl
        mov bp,ax

        mov ax,0         ;显存偏移地址列号的偏移计算
        mov al,dl
        mov bl,2
        mul bl
        add bp,ax        ;偏移地址都加到bp中

s:      push cx
        mov cl,[si]
        mov ch,0
        jcxz ok        ;根据0判断循环的结束。程序结束时需要pop cx
        pop cx
        mov al,[si]        ;低位存字母
        mov ah,cl          ;高位存颜色
        mov es:[bp],ax     ;设置到屏幕内存中
        inc si
        add bp,2
        jmp short s

ok:     pop cx          ;逐个pop用过寄存器
        pop es
        pop bp
        pop bx
        pop ax
        ret

code ends
end start