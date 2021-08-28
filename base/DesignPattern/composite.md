[TOC]

故事前提见`iterator`文档

# 新的需求
团队新加入了新的成员怀尔德-彭菲尔德。

他的菜单类不同于以往，因为菜单内容很多，他为每个菜单进行了分类，除了基础菜单，他还分出了”甜品“，“汤类”这两类子菜单。

因为子菜单的存在，直接加入这个菜单，那么会无法打印子菜单的内容。

我们当然希望可以拿到所有的菜单项，这就是需求。

# 问题

# 方案-内部迭代
## `MenuComponent`
```java
public abstract class MenuComponent{
    //这部分是组合的方法
    public void add(MenuComponent menuComponent){
        throw new UnsupportedOperationException();
    }

    public void remove(MenuComponent menuComponent){
        throw new UnsupportedOperationException();
    }

    public MenuComponent getChild(int i){
        throw new UnsupportedOperationException();
    }
    
    //这部分是菜单项的方法【或者认为是通用方法】
    public String getName(){
        throw new UnsupportedOperationException();
    }

    public String getDescription(){
        throw new UnsupportedOperationException();
    }

    public double getPrice(){
        throw new UnsupportedOperationException();
    }

    public boolean isVegetarian(){
        throw new UnsupportedOperationException();
    }

    public void print(){
        throw new UnsupportedOperationException();
    }
}
```

## `MenuItem`
```java
public class MenuItem extends MenuComponent {
    String name;
    String description;
    String vegetarian;
    double price;
    
    public MenuItem(String name,String description,boolean vegetarian,double price){
        this.name = name;
        this.description = description;
        this.vegetarian = vegetarian;
        this.price = price;
    }

    public String getName(){}
    public String getDescription(){}
    public double getPrice(){}
    public boolean isVegetarian(){}
    
    public void print(){
        //...
    }
}
```

## `Menu`
```java
public class Menu extends MenuComponent{
    ArrayList menuComponents = new ArrayList();
    String name;
    String description;
    
    public Menu(String name,String description){
        this.name = name;
        this.description = description;
    }
    
    public void add(MenuComponent menuComponent){
        menuComponents.add(menuComponent);
    }

    public void remove(MenuComponent menuComponent){
        menuComponents.remove(menuComponent);
    }
    
    public MenuComponent getChild(int i){
        return (MenuComponent) menuComponents.get(i);
    }

    public String getName(){}
    public String getDescription(){}
    
    public void print(){
        Sysytem.out.println(getName());
        Sysytem.out.println(getDescription());
        Sysytem.out.println("---------------");
        
        Iterator iterator = menuComponents.itearator();
        while (iterator.hasNext()){
            menuComponent menuComponent = (MenuComponent) iterator.next();
            menuComponent.print();
        }
    }
}
```

## `Waitress`
```java
public class Waitress {
    MenuComponent allMenus;
    
    public Waitress(MenuComponent allMenus){
        this.allMenus = allMenus;
    }
    
    public void printMenu(){
        allMenus.print();
    }
}
```

## 方案-外部迭代
## `Menu`
```java
public class Menu extends MenuComponent{
    public Iterator crateIterator(){
        return new CompositeIterator(menuComponents.iterator());
    }
}
```

## `MenuItem`
```java
public class MenuItem extends MenuComponent{
    public Iterator createIterator(){
        return new NullIterator();
    }
}
```

## `CompositeIterator`
```java
import java.util.*;

public class CompositeIterator implements Iterator{
    Stack stack = new Stack();
    
    public CompositeIterator(Iterator iterator){
        stack.push(iterator);
    }
    
    public Object next(){
        if(hasNext()){
            Iterator iterator = (Iterator) stack.peek();//从stack里面取出迭代器的值【不会删除】
            MenuComponent component = (MenuComponent) iterator.next();
            if(component instanceof Menu){
                //如果被取出的元素是组合元素，那么将它放到堆栈中
                stack.push(component.createIterator());
            }
            return component;//不论是否是菜单元素，都返回这个组件
        }else{
            return null;
        }
    }
    
    public hasNext(){
        if(stack.empty()){
            return false;
        }else {
            Iterator iterator = (Iterator) stack.peek();

            //如果当前迭代器没有下一个元素，那么将这个迭代器从堆栈中推出，递归调用hasNext
            if(!iterator.hasNext()){
                stack.pop();
                return hasNext();
            //如果有，则返回true
            }else{
                return true;
            }
        }
    }
    
    public void remove(){
        throw new UnsupportedOpertaionException();
    }
}
```
可以看到，这种写法之前复杂多，为什么？

因为之前的遍历是`Menu`在**内部**自行进行遍历的，而现在的做法是实现一个**外部**迭代器，这个外部迭代器需要控制和追踪东西更多。这里我们之所以选择使用堆栈结构，是因为我们必须维护组合递归结构的位置。

# 空迭代器
如果不使用空迭代器，我们针对`createIterator`方法的使用就需要进行区分，客户代码需要条件需要来判断返回值是`null`还是`Iterator`，这会造成浮躁。

更好的方式使用空迭代器，它依旧是`Iterator`类型，只不过`hasNext`永远返回`false`，这种做法的好处就是在客户代码使用时不需要区分判断，可以统一进行处理。

```java
import java.util.Iterator;

public class NullIterator implements Iterator{
    public Object next(){
        return null;
    }
    
    public boolean hasNext(){
        return false;
    }
    
    public void remove(){
        throw new UnsupportedOperationException();
    }
}
```

# 人物相关
## 怀尔德-彭菲尔德（Wilder Penfield）
威廉-斯科维尔的朋友，布伦达-米尔纳是他的助理研究员