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
定义：一个操作或多个操作，要么全部执行并且执行过程中不被任何因素打断，要么都不执行。

举例，这是一个为32位变量赋值的操作。
```
i = 9
```



### 2.可见性
### 3.有序性

## 参考资料
- [Java并发编程：volatile关键字解析](https://www.cnblogs.com/dolphin0520/p/3920373.html)