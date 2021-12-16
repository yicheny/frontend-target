;编写子程序的标准框架
;子程序开始：   子程序使用的寄存器入栈
;              子程序内容
;              子程序使用的寄存器出栈
;              返回(ret,retf)

;这种子程序调用的原因在于：
;子程序所使用的寄存器可能在主程序中也被使用，所以子程序对其修改，可能造成主程序运行的异常
;所以在开始之前我们需要通过push记录寄存器的内容，结束时通过pop恢复寄存器的内容

; 程序
; 这个子程序每个字符串结尾都是0，用于标记字符串的检测
; 程序会依次对单词的每个字进行处理，如果不是0则进行大写转换，如果是0则结束处理
; 使用0作为结束符相比之下就不再需要使用字符串长度做参数使用【否则需要使用二层循环】

assume cs:code

data segment
    db 'word',0
    db 'unix',0
    db 'wind',0
    db 'good',0
data ends

code segment
    start:  mov ax,data
            mov ds,ax   ;初始化ds至data段地址
            mov bx,0 ;初始化bx，指向第一个单词偏移地址

            mov cx,4    
        s:  mov si,bx   
            call capital
            add bx,5
            loop s

            mov ax,4c00h
            int 21h

  capital:  push cx
            push si
  
   change:  mov cl,[si]                     ;将(ds:si)移动至cl
            mov ch,0                        ;因为下面会使用(cx)=0做判断，所以cx需要设置为0    
            ;这里很关键，因为字符串结尾是0，那么此时(cl)=0，(ch)=0，所以(cx)=0，因此满足jcxz条件
            jcxz ok                         ;(cx)=0，短转移至ok标号，结束调用 
            and byte ptr [si],11011111b     ;转换字节大写
            inc si                          ;ds:si指向下一字节单元
            jmp short capital               ;短转移至captail标号，再次调用

       ok:  pop si
            pop cx
            ret

code ends
end start