;测试显存相关内容

;1. 从数据段中取出数据
;2. 将数据放到显存中即可显示

assume cs:code

data segment
    db 'x'
data ends

code segment
    start:  
    
code ends
end start