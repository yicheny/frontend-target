# 使用`Debug`，将程序段写入内存，逐条执行，观察CPU中相关寄存器中内容的变化
```
机器码          汇编指令             寄存器变化a
b8 20 4e        mov ax,4E20H        ax=4E20
05 16 14        add ax,1416H        ax=6236
bb 00 20        mov bx,2000H        ax=6236 bx=2000                
01 d8           add ax,bx           ax=8236 bx=2000
89 c3           mov bx,ax           ax=8236 bx=8236
01 d8           add ax,bx           ax=046C bx=8236
b8 1a 00        mov ax,001AH        ax=001A bx=8236
bb 26 00        mov bx,0026H        ax=001A bx=0026
00 d8           add al,bl           ah=00 al=40 bh=00 bl=26 
00 dc           add ah,bl           ah=26 al=40 bh=00 bl=26
00 c7           add bh,al           ah=26 al=40 bh=40 bl=26
b4 00           mov ah,0            ah=00 al=40 bh=40 bl=26
00 d8           add al,bl           ah=00 al=66 bh=40 bl=26
04 9c           add al,9CH          ah=00 al=02 bh=40 bl=26
```

# 将下面3条指令写入`2000:0`开始的内存单元中，利用这3条指令计算2的8次方
```
mov ax,1
add ax,ax
jump 2000:0003
```

# 查看内存中的内容
PC主板上的ROM中写有一个生产日期，在内存`FFF00H`~`FFFFFH`的某几个单元，请找到这个生产日期并试图改变它