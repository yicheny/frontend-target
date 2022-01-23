# 问题
1. 将`task3.asm`编译成`task3.exe`
2. 使用`debug`追踪`task.exe`的执行情况，写出每一步执行后，相关寄存器中的内容和栈顶内容
3. `PSP`的头两个字节是`CD20`，使用`debug`加载`task3.exe`，查看`PSP`的内容

# `task3.asm`代码
```asm

```

# 解答
## 2. 追踪程序
```
指令            数据变化
mov ax,2000     ax=2000
mov ss,ax       sp=2000
add,sp,4        sp=0004
push ax         sp=0002
push bx         sp=0000
pop ax          sp=0002,ax=0000
pop bx          sp=0004,bx=2000

//这两步用于退出
(t)mov ax,4c00  ax=4c00
(p)int 21       Program terminated normally
```

## 3. `PSP`内容
如图 ![](https://pic.imgdb.cn/item/61adda432ab3f51d91d491ba.jpg)
