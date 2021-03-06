0. 前置知识
1. 通用寄存器
2. 字在寄存器中的存储
3. 几条汇编指令
4. 物理地址
5. 16位结构的CPU
6. 8086CPU给出物理地址的方法
7. 段地址*16 + 偏移地址 = 物理地址的本质含义
8. 段的概念
9. 段寄存器
10. CS和IP
11. 修改CS和IP的指令
12. 代码段
13. 实验1：查看CPU和内存，使用机器指令和汇编指令编程



# 前置知识-2
## CPU部件组成
一个典型的CPU由运算器、控制器、寄存器等器件构成，这些器件通过内部总线相连接。

## CPU部件作用
- 运算器 信息处理
- 寄存器 信息存储
- 控制器 器件控制
- 内部总线 连接各个器件，在它们之间进行数据传送

## 寄存器
- 对汇编程序员而言，CPU中的主要部件是寄存器
- 寄存器是程序员可以用指令读写的部件
- 程序员通过改变各种寄存器中的内容来实现对CPU的控制


# 通用寄存器-0
## 定义
- AX BX CX DX四个寄存器用于存放一般性数据，被称作通用寄存器
- 8086CPU的所有寄存器都是16位的，可以存放两个字节

## 兼容
- 8086CPU上一代的寄存器是8位的，为了保证兼容，使原来基于上代CPU编写的程序稍加修改就可以运行在8086上，8086的通用寄存器可以分为两个独立使用的8位寄存器使用
 
## 高位寄存器和低位寄存器
16位寄存器的0-7位构成低位寄存器，比如AL，8-15构成了高位寄存器，比如AH


# 字在寄存器中的存储-0
- 出于对兼容性的考虑，8086CPU可以一次性处理两种尺寸的数据
- 字节：记为byte，一个字节由8bit组成，可以存在8位寄存器中
- 字：记为word，一个字由两个字节组成，这两个字节分别称为这个字的高位字节和低位字节

## 关于数制
- 任何数据在计算机底层都是以二进制保存的
- 人们习惯使用十进制
- 由于一个存储单元可以存放8位数据，CPU的寄存器中可以存放n个8位数据，很多时候需要直观看出组成数据的各个字节数据的值，16进制可以满足这个需求。

## 区分进制
- 在二进制数字后面加`B`
- 十进制后面什么也不加
- 在十六进制后面加`H`


# 几条汇编指令-2
> 汇编指令不区分大小写

- `mov ax data` 将数据送到`ax`寄存器
- `mov bx data` 将数据送到`bx`寄存器
- `add ax data` 将寄存器`ax`的数据加上送入的数据
- `add bx data` 将寄存器`bx`的数据加上送入的数据


# 物理地址-0
## 定义
- CPU在访问存储单元时，需要给出存储单元的地址
- 所有存储单元组成的内存地址空间是一个一维的线性空间
- 每个存储单元在这个空间中有着唯一的地址，这个地址叫做物理地址

## 形成
- CPU通过地址总线送入存储器的必须是一个内存单元的物理地址
- 在CPU向存储器发送物理地址之前，必须在内部形成这个物理地址
- 不同的CPU可以有不同的形成物理地址的方式


# 16位结构的CPU-0
## 定义
概括的说，16结构（16位机、字长为16位也是常见说法）描述了一个CPU具有以下几方面的结构特性：
1. 运算器一次最多可以处理16位数据
1. 寄存器的最大宽度为16位
1. 寄存器和运算器之间的通路为16位

## 对于地址的意义
- N位结构的CPU，能够一次性处理、传输、暂时存储的信息的最大长度是N位
- 而内存单元的地址在送上地址总线之前，必须在CPU中处理、传输、暂时存储


# 8086CPU给出物理地址的方法
## 寻址能力
- 8086CPU是16位结构，内部处理、传输、暂存的大小是2^16(64KB)
- 从内部结构来看，如果8086直接将地址传出，那么寻址能力只有64K
- 8086CPU在内部采用将两个16位地址合成的方式形成一个20位物理地址
- 因此8086CPU有20根地址总线，寻址能力达到2^20(1MB)

## 合成方式
- 地址加法器采用**物理地址 = 段地址*16 + 偏移地址**的方式合成物理地址

## 合成具体步骤
1. CPU中两个位置提供16位地址，一个叫做段地址，一个叫做偏移地址
2. 段地址和偏移地址通过内部总线送入到一个叫做地址加法器的部件
3. 地址加法器将两个16位地址合成一个20位的物理地址
4. 地址加法器通过内部总线将合成的20位物理地址送入到输入输出控制电路   
5. 输入输出控制电路将20位物理地址送到地址总线
6. 20位物理地址被地址总线送到存储器


# 段地址*16 + 偏移地址 = 物理地址的本质含义
- CPU在访问内存时，使用一个基础地址（段地址*16）和一个相对基础地址的物理地址相加，给出内存单元的地址
- 更一般地说，8086CPU的这种寻址功能是“基础地址+偏移地址=物理地址”寻址模式的一种集体实现