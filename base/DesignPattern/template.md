[TOC]

# 故事
## 实现咖啡类
事情一开始很简单，根据公司业务需求，需要实现一个咖啡类，这个类用来冲泡咖啡。

冲泡咖啡分为以下几个步骤：
1. 将开水煮沸
2. 使用开水冲泡咖啡
3. 把咖啡倒进杯子
4. 加糖和牛奶

实现咖啡类：
```java
public class Coffee {
    //冲泡
    void prepareRecipe() {
        boilWater();
        brewCofferGriinds();
        pourInCup();
        addSugarAndMilk();
    }

    //...步骤方法实现
}
```

## 实现茶类
完成咖啡类之后，我收到一个新的需求任务，公司现在支持泡茶业务，需要实现一个茶类，这个类可以泡茶。

泡茶的步骤：
1. 将开水煮沸
2. 使用开水冲泡**茶叶**
3. 把**茶**倒进杯子
4. 加**柠檬**

实现茶类：
```java
public class Tea {
    //冲泡
    void prepareRecipe() {
        boilWater();
        steepTeaBag();//注意这里不同啦！
        pourInCup();
        addLemon();//注意这里不同啦！
    }

    //...步骤方法实现
}
```

我们实现了茶类，这里稍微提一下`pureInCup()`这个方法，这个方法是将饮料倒进杯子里，这里我默认这个方法与具体饮料类型无关，因此在咖啡类和茶类中的实现是相同的。

我假设这个方法实现是类似这样的，可以看到，实现与饮料类型无关：
```java
public void pourInCup() {
    System.out.println("倒入茶杯中！")    
}
```

如果有人认为这里不同饮料的实现并不相同，也是合理的想法，这个方法也可能是类似这种实现：
```java
public void pourInCup() {
    System.out.println("将咖啡倒入茶杯中！")    
}
```

不过这个方法并不影响核心思路，可以认为我这里的实现是与饮料类型无关的。

## 抽取`boliWater()`、`pourInCup()`
现在我们可以看到，茶类和咖啡类的实现中有着两个完全相同的方法：
- `boilWater` 煮开水
- `pourInCup` 倒入茶杯

现在我们想优化下实现，复用这两个方法。

有哪些复用方案？

具体实现可以想到很多，然而总的来说，可以分成两种：继承、委托。

我的原则是：
1. 完全复用，所有子类都是相同的实现，优先考虑继承
2. 部分复用，一部分子类是相同的实现，优先考虑委托
3. 完全独立，交由子类实现

这里使用继承是合适的，简洁而且优雅。

我大概描述下思路：
- `Drink` 饮料类，这是一个公共类，实现
    - `boilWater()`
    - `pourInCup()`
- `Coffee` 继承`Drink`，并实现
    - `prepareRecipe()`
    - `brewCofferGriinds()`
    - `addSugarAndMilk()`
- `Tea` 继承`Drink`，并实现
    - `prepareRecipe()`
    - `steepTeaBag()`
    - `addLemon()`
    
我们已经完成了初步的复用，然而我们还可以更进一步。

## 复用`prepareRecipe()`
仔细分析下冲泡流程，我们可以发现，目前两种饮料的冲泡步骤极为相似，这里冲泡饮料的步骤顺序是相同的。

相同，则意味着重复，如果抽离出来实现会更简洁，而且维护也更方便【相对于顺序的调整】。

现在复用`prepareRecipe()`，你有哪些方案？

### **愚蠢**的方案：添加判断
这里我先介绍一种**愚蠢**的方案，这种方案是很容易想到的，然而也充满风险，最好不要使用。

说一下这种方案的思路：
- 将`prepareRecipe()`提取到`Drink`，由其实现
- 修改`prepareRecipe()`方法，在内部进行判定，根据不同的饮料类调用不同的方法

代码实现：
```java
public class Drink{
    void prepareRecipe(){
        boilWater();
        if(this instanceof Coffee) brewCofferGrinds();
        if(this instanceof Tea) steepTeaBag(); 
        pourInCup();
        if(this instanceof Coffee) addSugarAndMilk();
        if(this instanceof Tea) addLemon();
    }
}
```
这种做法使得`prepareRecipe`逻辑变得复杂，这里的判断是不必要的，而且这种写法，意味着每次添加新的饮料都需要修改这个方法，这违背了哪些原则：
1. `S` 单一职责原则：增加了“判断”这一额外职责
2. `O` 开放扩展，封闭修改：每次增加饮料，都需要修改这个方法

这种写法使得阅读的维护的负担加重了，这种写法难以适应变化，是一种糟糕的做法。

### 泛化和继承
这里不需要增加判断，我们还是可以通过继承来实现。

有人可能会疑惑：这里有部分步骤不一样啊，要怎么处理呢？

答案是：“泛化”，理解起来就是我们给这个步骤起一个更宽泛、更抽象的名称。

比如说，小明在吃香蕉，小刚在吃菠萝，我们可以描述为小明在吃水果，小刚也在吃水果。

冲泡咖啡和冲泡茶包，我们可以起名为冲泡或者冲泡饮料；添加糖和牛奶，添加柠檬，我们可以起名为添加调料。

代码实现：
```java
public class Drink{
    void prepareRecipe(){
        boilWater();
        brew();//注意这里改名了！
        pourInCup();
        addCondiments();//注意这里改名了！
    }
    
    //注意，将预定需要的步骤定义成抽象方法，这样可以统一接口以及强制子类实现这些步骤
    abstract void brew(){}
    abstract void addCondiments(){}
}
```
这样是不是很清爽？

然后我们对子类`Coffee`进行修改：
```java
public class Coffee{
    void brew(){} //原来的brewCofferGrinds()
    void addCondiments(){} //原来的addSugarAndMilk()
}
```
`Tea`也是类似的修改。

# `hook`
我们将超类中默认不做事的方法称作`hook`。

用途：
1. 在步骤中埋点【实现步骤中可选的部分】
2. 控制步骤是否执行

# 好莱坞原则
高层组件对底层组件：别调用我们，我们会调用你。

# 定义
模板方法定义了一个算法的步骤，并允许子类为一个或多个步骤提供实现。