[TOC]

如果不使用单例模式，一般会想到哪些方式创建唯一的对象？
- 全局变量
  - 缺陷1：没有使用之前就会创建，如果这是一个非常复杂的对象，会耗费不必要的时间和内存
  - 缺陷2：只能保证提供一个全局访问点，不能保证只创建一个实例，比如两个全局变量可以使用相同类的不同实例

# 经典单例模式
```java
class Demo(){
    static private Demo instance;
    
    private Demo(){}
    
    static public Demo getInstance(){
       if(instance) return instance;
       return instance = new Demo();
    }
}
```
利用这个模式，我们可以只在需要的时候创建实例，这叫做延迟实例化（lazy instantiaze）

# 多线程处理
## 添加`synchronized`修饰符
```java
class Demo(){
  //...
  synchronized static public Demo getInstance(){ }
}
```
- 优点：写法简单
- 缺点：需要等待，速度有负面影响

## 使用急切创建实例
```java
class Demo(){
    static private Demo instance = new Demo();
    //...
}
```
- 优点：写法简单
- 缺点：对内存和速度有负面影响

## 双重检查加锁
```java
public class Demo{
    private volatile static Demo uniqueInstance;
    
    private Demo(){}
    
    public static Demo getInstance(){
        if(uniqueInstance == null){
            synchronized (Demo.class){
                if(uniqueInstance == null){
                    uniqueInstance = new Demo();
                }
            }
        }
        return uniqueInstance;
    }
}
```
解释下双重加锁的执行：首先检查实例是否创建，如果没有创建，则进行“同步”，这样只有第一次是同步的。

- 优点：性能、速度都很友好
- 缺点：写法相对复杂

# 资料
## 并发编程中的三个概念
### 1.原子性
定义：一个操作是不可中断的，要么全部执行成功要么全部执行失败。

`java`内存模型中定义了8中操作都是原子的，不可再分的：
- `lock`(锁定)：作用于主内存中的变量，它把一个变量标识为一个线程独占的状态；
- `unlock`(解锁):作用于主内存中的变量，它把一个处于锁定状态的变量释放出来，释放后的变量才可以被其他线程锁定
- `read`（读取）：作用于主内存的变量，它把一个变量的值从主内存传输到线程的工作内存中，以便后面的load动作使用；
- `load`（载入）：作用于工作内存中的变量，它把read操作从主内存中得到的变量值放入工作内存中的变量副本
- `use`（使用）：作用于工作内存中的变量，它把工作内存中一个变量的值传递给执行引擎，每当虚拟机遇到一个需要使用到变量的值的字节码指令时将会执行这个操作；
- `assign`（赋值）：作用于工作内存中的变量，它把一个从执行引擎接收到的值赋给工作内存的变量，每当虚拟机遇到一个给变量赋值的字节码指令时执行这个操作；
- `store`（存储）：作用于工作内存的变量，它把工作内存中一个变量的值传送给主内存中以便随后的write操作使用；
- `write`（操作）：作用于主内存的变量，它把store操作从工作内存中得到的变量的值放入主内存的变量中。

举例，这是一个为32位变量赋值的操作。
```
i = 9
```
赋值是具有原子性的一个操作，暂且假设32位赋值包含两个过程：为低16位赋值，为高16位赋值

如果这里不是具有原子性，那么可能会出现一种情况：为低16位赋值后突然中断，此时另一线程读取变量值，此时获取到的值是错误的。

`synchronized`满足原子性，而`volatile`不具有原子性，看一下例子：

```java
public class VolatileExample {
    private static volatile int counter = 0;

    public static void main(String[] args) {
        for (int i = 0; i < 10; i++) {
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    for (int i = 0; i < 10000; i++)
                        counter++;
                }
            });
            thread.start();
        }
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println(counter);
    }
}
```
我们开启了10个线程，每个线程自加10000次，我们预期最终运行结果是100000。

但是实际运行结果会小于100000，为什么？

`volatile`不能保证原子性，我们聚焦到`counter++`，这个操作包括了三个步骤：
1. 读取变量`counter`的值
1. 对`counter`加`1`
1. 将新值赋值给`counter`

假设线程A将`counter`读取到工作内存后，此时其他线程可能对这个`counter`进行了自增操作，如果线程A自增后对的`counter`进行赋值，那么其他线程设置的自增数值就被覆盖掉了。

`volatile`想要保证原子性，必须满足以下两条规则：
1. 运算结果并不依赖于变量的当前值，或者能够确保只有一个线程修改变量的值；
1. 变量不需要与其他的状态变量共同参与不变约束

### 2.有序性

### 3.可见性

## 参考资料
- [三大性质总结：原子性，有序性，可见性](https://www.jianshu.com/p/cf57726e77f2)
- [Java并发编程：volatile关键字解析](https://www.cnblogs.com/dolphin0520/p/3920373.html)