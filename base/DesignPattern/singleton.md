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