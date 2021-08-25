[TOC]

# 自动化遥控器
## 单一执行遥控
我们现在有一个遥控器，它可以做的事情很多，比如：打开/关闭冰箱、打开/关闭电灯、打开/关闭电视机、打开/关闭洗衣机、调节空调温度……

不同的机器其开关及提供的接口是不同的，我不会在遥控器上将每个功能进行实现，原因在于：
1. 遥控器要做的事情太多，阅读维护不易
2. 要跟很多执行类直接打交道，违背开放封闭原则
3. 无法将指令优雅的组合成高阶指令【无法动态组合指令】

### 思路描述
核心思路将工作交给不同的命令对象，让命令对象和对应的电器类打交道。

1. 在`Client`类中进行各种操作，比如说按下“打开冰箱”这些。
2. 遥控器装载上各种指令，它的作用是对命令做一些管理和记录
3. 指令类负责和对应的电器类打交道，调用其提供的API【指令类需要一个统一的接口，以便遥控器装载】

好处是显而易见，比如说，我想换一种电器、又或者某个电器的API改了，那么我只需要调整单独的命令类，而无需改变遥控器本身。

而遥控器也可以根据需要装载不同的指令，实现各种“快捷键”或其他功能。

## 智能组合遥控器
一个新的需求：按下按钮时不需要立刻执行，只是记录，当我真正点击`run`按钮时，将我刚刚按下的按钮执行，类似可以自定义一种快捷键。

...

## 撤销按钮
我可以轻松记录上一次按下的指令，但是问题在于：不同的指令是对应这另一套不同的按钮的。

比如按下“打开冰箱”，撤销的话就应该是“关闭冰箱”。

总之，我们不希望撤销的时候通过判断执行撤销。

记录所有指令然后重新执行也并不靠谱，因为理论上用户可以无限次的按钮的，没有任何限制。

一些方案：
1. 在遥控器进行维护，指定对应撤销关系——违反开发封闭原则
2. 在命令类进行维护，提供对应的撤销方法——合理

### 状态相关的撤销
如果是开、关这种相互对应的关系，可以很容易的实现撤销。

但是有些情况下可能会更复杂一些，比如：某个关于电风扇的指令，提供了高、中、低三种风速指令

类似这种拥有状态的指令，我们需要记录一些数据，以实现撤销指令。

#### 电风扇类
```java
public class CeilingFan {
    public static final int HIGH = 3;
    public static final int MEDIUM = 2;
    public static final int LOW = 1;
    public static final int OFF = 0;
    String location;
    int speed;
    
    public CeilingFan(String location){
        this.location = location;
        speed = OFF;
    }
    
    public void high(){
        speed = HIGH;
    }
    
    public void medium(){
        speed = MEDIUM;
    }
    
    public void low(){
        speed = LOW;
    }
    
    public void off(){
        speed = OFF;
    }
    
    public void setSpeed(int speed){
        this.speed = speed;
    }
    
    public int getSpeed(){
        return speed;
    }
}
```

#### 电风扇指令类
指令的撤销命令需要记录上一次的状态。
```java
public class CeilingFanHighCommand implements Command{
    CeilingFan ceilingFan;
    int prevSpeed;
    
    public CeilingFanHighCommand(CeilingFan ceilingFan){
        this.ceilingFan = ceilingFan;
    }
    
    public void execute(){
        prevSpeed = ceilingFan.getSpeed();
        ceilingFan.high();
    }
    
    public void undo(){
        ceilingFan.setSpeed(prevSpeed);
    }
}
```

#### 测试
```java
public class RemoteLoader{
    public static void main(String[] args){
        //生成遥控器
        RemoteControlWithUndo remoteControl = new RemoteControlWithUndo();
        
        //生成指令
        CeilingFan ceilingFan = new CeilingFan("Living Room");
        CeilingFanMediumCommand ceilingFanMedium = new CeilingFanMediumCommand(ceilingFan);
        CeilingFanHighCommand ceilingFanHigh = new CeilingFanHighCommand(ceilingFan);
        CeilingFanOffCommand ceilingFanOff = new CeilFanOffCommand(ceilingFan);

        //遥控器装载指令
        remoteControl.setCommand(0,ceilingFanMedium,ceilingFanOff);//中速按钮
        remoteControl.setCommand(1,ceilingFanHigh,ceilingFanOff);//高速按钮
        
        //遥控器操作
        remoteControl.onButtonWasPushed(0);//以中速打开风扇
        remoteControl.offButtonWasPushed(0);//关闭
        System.out.println(remoteControl);//撤销
        remoteControl.undoButtonWasPushed();
        remoteControl.onButtonWasPushed(1);//开启高速风扇
        System.out.println(remoteControl);
        remoteControl.undoButtonWasPushed();//撤销
    }
}
```