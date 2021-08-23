
# 公共点单程序
## 故事描述
我的名字是卡尔-拉什利（karl lashley)，我现在是一个软件开发者。

为了加深印象，读者朋友们可以将我看作一个银发赤瞳的少女，就像这样：
![伊莉雅](https://inews.gtimg.com/newsapp_bt/0/9958509777/1000)

现在先介绍我的两个工作伙伴：

布伦达-米尔纳（Brenda Milner)

![日向夏帆](https://p1.pstatp.com/large/470c00055f6400f4b090)

亨利-莫纳森（Henry Molasion)

![雪之下雪乃](http://p3.pstatp.com/large/912400014eec0cf5ebb1)

他们两个之前做的系统都是餐厅下单系统，属于不同的开发团队，现在情况有了变化，我们会组成一个新的团队，在这个新的应用中，希望可以使用之前的下单系统。

现在的问题是，布伦达-米尔纳的系统使用的数据结构是`ArrayList`，而亨利-莫纳森使用的是`Array`，但是我们希望可以基于同一份下单系统进行下单，要怎么获取菜单项呢？

让卡尔我来看一下这两位现在的实现吧。

## 布伦达-米尔纳实现
这是布伦达-米尔纳的实现方案：

```java
public class BrendaMenu {
    ArrayList<MenuItem> menuItems;

    public BrendaMenu {
        menuItems = new ArrayList<>();
        addItem("牛排",'描述内容',false,111);
        addItem("爆炒羊肉",'描述内容',false,81);
        addItem("西兰花",'描述内容',true,21);
        addItem("大白菜",'描述内容',true,14);
    }

    public void addItem(String name,
                        String description,
                        boolean vegetarian,
                        double price) {
        MenuItem menuItem = new MenuItem(name, description, vegetarian, price);
        menuItems.add(menuItem);
    }

    public ArrayList<MenuItem> getMenuItems(){
        return menuItems;
    }
    
    //其他方法
}
```

## 亨利莫纳森
这是亨利-莫纳森的方案：

```java
public class HenryMenu {
    static final int MAX_ITEMS = 6;
    int numberOfItems = 0;
    MenuItem[] menuItems;

    public HenryMenu {
        menuItems = new MenuItem[MAX_ITEMS];
        addItem("西瓜",'描述内容',true,90);
        addItem("黄瓜",'描述内容',true,20);
        addItem("东坡肉",'描述内容',true,120);
        addItem("油焖大虾",'描述内容',true,150);
    }

    public void addItem(String name,
                        String description,
                        boolean vegetarian,
                        double price) {
        MenuItem menuItem = new MenuItem(name, description, vegetarian, price);
        if (numberOfItems >= MAX_ITEMS) {
            System.err.println("抱歉，菜单已满，不能继续添加菜品！");
        } else {
            menuItems[numberOfItems] = menuItem;
            numberOfItems += 1;
        }
    }

    public MenuItem[] getMenuItems(){
        return menuItems;
    }

    //其他方法
}
```

## 需求
需要一个订单系统，这个系统需要提供这么几个接口：
- `printMenu()` 打印所有菜品
- `printVegetarian()` 打印所有素食菜品
- `printMeat()` 打印所有肉食品
- `printHighPrice()` 打印所有高价商品（高于等于100元）
- `printLowPrice()` 打印所有低价商品

## 问题
以`printMenu()`接口为例，想一下要怎么实现这个接口。

比较容易的方案：取出`BrendaMenu`和`HenryMenu`的所有菜品，然后分别循环打印：

```java
public class OrderSystem() {
    public void printMenu() {
        BrendaMenu brendaMenu = new BrendaMenu();
        ArrayList<MenuItem> brendaMenuItems = brendaMenu.getMenuItems();

        HenryMenu henryMenu = new HenryMenu();
        MenuItem[] henryMenuItems = henryMenu.getMenuItems();

        for (int i = 0; i < brendaMenuItems.size(); i++) {
            MenuItem menuItem = (MenuItem) brendaMenuItems.get(i);
            System.out.println(menuItem.getName());
            System.out.println(menuItem.getPrine() + "");
            System.out.println(menuItem.getDescription());
        }

        for (int i = 0; i < henryMenu.length; i++) {
            MenuItem menuItem = henryMenu[i];
            System.out.println(menuItem.getName());
            System.out.println(menuItem.getPrine() + "");
            System.out.println(menuItem.getDescription());
        }
    }
}
```
可以看到，我们在这里面做了两次循环，这个就是最主要的问题，在开发上会带来两个问题：
1. 我们在每个方法里都需要写这两次循环
2. 如果有第三种不同的菜单实现，我们需要为每个方法添加第三种循环

## 解决
一个的很重要的原则：抽离变化的部分。

每当需要在某个上下文中针对不同情况进行不同处理时，意味着变化的出现。

这里的写法违反了1个很重要的编程原则： 面对抽象编程，不要面对实现编程

这里只需要一个统一的接口就可以解决问题，这是我希望提供的接口：
```java
interface Iterator{
    hasNext();
    next();
}
```

原来的代码实现不需要改变，只需要实现这个接口即可。

然后改写`OrderSystem`：

```java
public class OrderSystem() {
    BrendaMenu brendaMenu;
    HenryMenu henryMenu;

    public OrderSystem(BrendaMenu brendaMenu, HenryMenu henryMenu) {
        this.brendaMenu = brendaMenu;
        this.henryMenu = henryMenu;
    }

    public void printMenu() {
        printMenu(this.brendaMenu.createIterator());
        printMenu(this.henryMenu.createIterator());
    }

    private void printMenu(Iterator iterator) {
        while(iterator.hasNext()){
            MenuItem menuItem = iterator.next();
            System.out.println(menuItem.getName());
            System.out.println(menuItem.getPrine() + "");
            System.out.println(menuItem.getDescription());
        }
    }
}
```

## 改良
1. `java.util.Iterator` 有提供自己的迭代器接口
2. `java`里面的`ArrayList` 有提供用于创建迭代器的`iterator()`方法，我们只需要为`Array`实现一个迭代器接口

## 更多的思考
...

# 添加新菜单
和我预期的一样，现在团队加入了一位新成员：威廉-斯科维尔，他之前的菜单使用的数据结构是`HashMap`

看一下要怎么加入我们的订单系统中。
1. `WilliamMenu`需要提供`createIterator`方法，幸运的是，`HashMap`已然提供了`iterator`方法
2. 在`OrderSystem`添加对应的成员变量

```java
public class OrderSystem() {
    BrendaMenu brendaMenu;
    HenryMenu henryMenu;
    WilliamMenu williamMenu;

    public OrderSystem(BrendaMenu brendaMenu, 
                       HenryMenu henryMenu,
                       WilliaMenu williaMenu) {
        this.brendaMenu = brendaMenu;
        this.henryMenu = henryMenu;
        this.williamMenu = williaMenu;
    }

    public void printMenu() {
        printMenu(this.brendaMenu.createIterator());
        printMenu(this.henryMenu.createIterator());
        printMenu(this.williamMenu.createIterator());
    }

    //...
}
```
完成了对新菜单的添加，从这次修改中可以感受到一些问题：
1. `OrderSystem`违背了开放封闭原则，每次添加都需要修改OrderSystem构造方法以及相关方法
2. 没有充分利用好多态特性，依旧有依赖实现的部分，这里看似我只改了一个`printMenu`，实际上每个方法都需要修改一次【当然，即使只需要改一个`printMenu`，也是让人难受的做法】

一个简单方案，这里不维护具体类，维护一个`ArrayList`，遍历`ArrayList`进行操作：

```java
public class OrderSystem() {
    ArrayList menus;

    public OrderSystem(ArrayList menus) {
        this.menus = menus;
    }

    public void printMenu() {
        Iterator menuIterator = menus.iterator();
        while(menuIterator.hasNext()){
            Menu menu = menuIterator.next();
            printMenu(menu.createIterator());
        }
    }

    //...
}
```
这样就舒服了，即使要添加新的菜单，也不需要修改这个类了。

# 添加多级菜单

# 人名典故
## 卡尔-拉什利（karl lashley)
是一位脑神经科学家，在19世纪40年代的时候，曾经做过一个有趣的实验：先让一些老鼠掌握跑迷宫的技能，然后分别摘除老鼠大脑中的某个部分，实验发现，无论摘除那个部分，老鼠都不会丧失跑迷宫技能。

由此，卡尔-拉什利做出结论：大脑中每个区域功能是相同的，任何一部分收到损伤，另一部分都会接替。这在当时是生物科学家共同深信不疑的看法，不过在之后的生物科学发展中，我们会知道，这是个错误的结论。

## 亨利-莫纳森（Henry Molasion)

## 布伦达-米尔纳（Brenda Milner)

## 威廉-斯科维尔（William Beechar Scoville） 