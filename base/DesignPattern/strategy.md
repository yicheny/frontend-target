[TOC]

我是个做鸭子的，准确的说，是做鸭子应用的。

我们公司是软件公司，最近我参与开发的应用是一款模拟鸭子的应用，在这个应用里我们可以模拟各种鸭子。

# 红头鸭、绿头鸭
现在是开发初期，需要实现两种鸭子：红头鸭、绿头鸭。
```java
public class RedDuck{
    public void quack();//嘎嘎叫
    public void swim();//游泳
    public void display();//外观
}

public class GreenDuck{
    public void quack();//嘎嘎叫
    public void swim();//游泳
    public void display();//外观
}
```
![不愧是我](https://wx3.sinaimg.cn/large/ae2c6e1dly1go6mq58szmj20qo0pgmyl.jpg)

不愧是我，这么快就实现了需求。

![开心](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTwKBdZBKWMtm3ptNWfHU7SzM7KhaP5nOOcog&usqp=CAU)

接下来就可以顺理成章的摸鱼了~~~

![思考](http://m.biaoqingb.com/uploads/img1/20200905/26d8d27378871daf5d24398785de563b.jpg)

嗯？这里好像可以复用？

改进一波：
```java
public class Duck{
    public void quack();//嘎嘎叫
    public void swim();//游泳
    public abstract void display();//外观-抽象接口
}

public class RedDuck extends Duck{
    public void display();//外观-具体实现
}
public class GreenDuck extends Duck{
    public void display();//外观-具体实现
}
```

![骄傲](http://inews.gtimg.com/newsapp_bt/0/13487245246/1000)

将公共功能和样式分离，既封装了重复，又隔离了变化，真是完美的代码。

# 添加飞行能力
正准备摸鱼的时候，产品过来了。

我：“品哥，我已经实现需求了，正准备过去找你呢！我给你演示下效果”

![做的好](https://qq.yh31.com/tp/by/201210291036051450.jpg)

产品：”做的好。不过之前需求上有些缺失，鸭子应该是可以飞的，给之前的鸭子都加上这个功能吧！“

![SB](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQkvDGCM_I9VmfxNVhTtj4AGuobEewKUhiTqw&usqp=CAU)

我的想法：”会飞的能叫鸭子吗？那是鹅“

不过作为一个成熟的职场人士，我怎么会将想法直接说出来呢。

![装傻](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSJCUnqF4jdIXU_y-j6ovwAXCKsDE_x4Rib5Q&usqp=CAU)

我：”厉害呀，不愧是你，竟然可以想出这么专业的功能，牛逼👍！“

![厉害呀](https://img95.699pic.com/element/40124/7317.png_860.png)

问题不大，之前已经抽离了重复，虽然鸭子种类挺多，但是只需要在公共类`Duck`里添加飞行功能，所有鸭子都可以飞了。

```java
public class Duck{
    public void fly();//鸭子会飞啦！
}
```

优秀的程序员总是可以提前预料的变化，我很快完成了任务

![不愧是我](https://wx3.sinaimg.cn/large/ae2c6e1dly1go6mq58szmj20qo0pgmyl.jpg)

# 橡皮鸭、木头鸭
我：”品哥，功能加上了“

![做的好](https://qq.yh31.com/tp/by/201210291036051450.jpg)

产品：”做的好。我刚刚又有了一个巧妙的想法，我觉得我们可以再加两种特殊的鸭子：橡皮鸭和木头鸭，木头鸭可以游泳，橡皮鸭可以嘎嘎叫、可以游泳，不过两种鸭子都不会飞。“

![笑容凝固](http://www.zhaodanji.com/uploadfile/2017/0728/20170728010427705.jpg)

我：”啊这，木头鸭和橡皮鸭虽然带个’鸭‘字，但是和”鸭子“不是同一类生物吧，说到底这两个生物都算不上啊！”

产品：“你说到很好，正是因此这两种鸭子才能给客户带来深刻的印象，这是一次了不起的创新和尝试！”

我：

![你说是就是吧](http://wx1.sinaimg.cn/large/006R4mNygy1fvf07xxu4fj306p07kaa5.jpg)

这次需求比上一次麻烦些，因为方法存在三种情况：
1. **完全公用的**，所有的鸭子都拥有的能力，比如说游泳，目前所有鸭子都会游泳
2. **部分公用**，一部分鸭子拥有且重复的能力，比如飞行和嘎嘎叫
3. **完全独立**，每个鸭子都不同的能力，比如说外观，每种鸭子的外观都是不同的

想要实现产品的需求，我可以直接添加两种新类型的鸭子，然后通过**覆写**的方式将某个子类鸭子的行为没有的行为给覆盖成空方法，比如这样：

```java
public class WoodDuck extends Duck{
    public void quack(){} //空方法
    public void fly(){} //空方法
}
```

看起来还好，实际上是有负担的，现在我是通过**继承**提供公共方法的，如果以后公共方法每次增加，就意味这我需要对所有的鸭子子类做检查是否需要覆盖。

比如说，现在想添加一个吃东西的方法`eat`，而木头鸭和橡皮鸭是不需要吃东西的，所以应当覆盖。现在鸭子数量还不算多，如果鸭子种类逐渐变多【这很有可能】，这意味着我需要对每种鸭子都检查和覆盖一遍，很难受。

另一种可能，如果之后产品又要增加一种鸭子【比如火箭鸭吧】，火箭鸭只需要一个行为：飞行，那么需要将飞行以外的能力全部覆盖掉。

原本我只需要实现或继承一个`fly`行为就好，但是到时候实现的时候我不得不将所有行为都覆盖一遍，假设有100种基础行为，岂不是需要覆盖99次？

比如说99次，9次也恶心啊。

通过覆写或者继承看来不太行啊，换一个方向思考下。

![喝茶](https://c-ssl.duitang.com/uploads/item/201703/05/20170305152157_FxwGz.thumb.400_0.jpeg)

之前我的设计其实还不错，现在的问题点是：有一部分行为是部分复用的。

子类怎么添加这些行为比较合理？

继承耦合性太高，每次调整超类都会影响到所有子类，如果不用继承，每次自己实现也很麻烦。

一个简单的思路：将这部分行为抽离成单独的类使用，如果需要某个行为，则委托对应的行为类。

目前我们`fly`、`quack`是需要抽离出来的，其他部分保持不变。
```java
public class Duck{
    protected FlyBehavior flyBehavior;//子类可以在构造器设置初始的默认行为

    public void performFly(){
        flyBehavior.fly();
    }
}

public class FlyBehavior{
    public void fly(){
        //飞行行为的实现
    }
}

public class RedDuck extends Duck{
    public void performFly(){}
}
```

现在我是怎么处理不同行为的：
1. **完全复用** 通过继承实现
2. **部分复用** 通过委托【组合】实现，由单独的行为类实现行为，子类通过委托使用相应方法。
3. **完全独立** 通过超类定义抽象方法，由子类实现

这个设计很棒，再也不怕产品提新的需求了。

![特别骄傲](http://hbimg.b0.upaiyun.com/ac5e6cfeadebfceeb314d9e124bd31644b910d55446c-5vD8hq_fw658)

# 动态变化行为
产品过来了：我有一个新的想法……

我：没问题，是准备加个新鸭子，还是准备加个新行为？

![自信](https://m.biaoqingb.com/uploads/img1/20201130/b749a35f280228e2b568963b45be8665.jpg)

产品：都不是，我希望咱们的鸭子可以更灵活一些，就比如这个绿头鸭，我希望绿头鸭可以选择发出嘎嘎叫、呱呱叫或者咯咯叫，还有飞行，我希望同一种鸭子可以选择喷射飞、旋转飞、振翅飞，它是灵活可以选择的。

产品：这个想法很棒吧！客户一定很喜欢，你觉得怎么样？

![你觉得怎么样](https://i2.kknews.cc/SIG=3b43lak/ctp-vzntr/1522976350465rs3n08n70n.jpg)

我的内心：

![你过来](http://i04picsos.sogoucdn.com/b4a3b347b0fb04be)

产品的想法还是太自由，超出了我预期的想法。

![思考](http://m.biaoqingb.com/uploads/img1/20200905/26d8d27378871daf5d24398785de563b.jpg)

不过现在的设计下，满足这个需求似乎也很简单？

针对产品的需求，我马上想法了一个可行的简单方案。
```java
public class Duck(){
    public void setFly(FlyBehavior flyBehavior){
        this.flyBehavoir = flyBehavior;
    }
}
```
提供一个方法自由切换就完事了。

这就是委托的好处，委托是"HAS-A"【有一个】的关系，而继承是"IS-A"【是一个】的关系，IS-A是难以修改和替换的，HAS-A是很方便替换掉的。

至此，今天的需求结束，很充实的一天。

# 总结
今天的体悟：
1. 抽离变化的部分【封装变化】
2. 组合相比继承更加灵活
3. 针对接口，而不是针对实现编程（这一点故事里没重点强调，实际上同一类行为应当一个接口约束，比如说无论是振翅飞、喷射飞、旋转飞其接口应该是一致的，这样才可以自由的进行替换）

# if-非OO开发者
如果不用类，面对故事里的有什么好的方案吗？