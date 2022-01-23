```
指令            数据
mov ax,ffff     
mov ds,ax       
mov ax,2200
mov ss,ax
mov sp,0100
mov ax,[0]      ax=C0EA
add ax,[2]      ax=C0FC
mov bx,[4]      bx=30F0
add bx,[6]      bx=6021
push ax         sp=00FE->C0FC
push bx         sp=00FC->6021
pop ax          sp=00FE ax=6021
pop bx          sp=0100 bx=C0FC
push [4]        sp=00FE->30F0
push [6]        sp=00FC->2F31
```