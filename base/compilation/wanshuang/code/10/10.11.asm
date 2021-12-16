
assume cs:code

data segment
    db 'conversation'
data ends
    
code segment
    start:  mov ax,data
            mov ds,ax   ;初始化ds
            mov si,0    ;初始化至第一个字符偏移地址
            mov cx,12   ;标识循环次数，和字符串长度相同
            call capital
            
            mov ax,4c00h
            int 21h

  capital:  and byte ptr [si],11011111b ;大写转换
            inc si
            loop capital 
            ret

code ends
end start
