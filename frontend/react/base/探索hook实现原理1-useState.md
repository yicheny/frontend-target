[TOC]

# hook的规则
React核心开发团队规定了在使用hook时必须遵循的两条规则：
1. 必须在React函数中使用
2. 不要在循环、条件语句或嵌套函数中使用hook

## 关于第一条，什么是React函数？
- 返回值是`JSX`语法的函数。
- React函数就是函数式组件

## 为什么hook必须在React函数中使用？
首先回到[官方文档](https://zh-hans.reactjs.org/docs/hooks-intro.html#motivation)，设计`Hook`的动机就是考虑到在组件之间复用状态逻辑缺乏一种简洁有效的手段。

在hook出现之前，我们可以使用`HOC`和`render props`解决逻辑复用的问题，不过这些手段存在一些缺陷：
1. 需要重新组织组件结构
2. 可能形成嵌套地狱
以上两者都会造成代码理解上的困难
//待补充示例

根本上来说，hook的出现是为了解决一个问题：**React需要为共享状态逻辑提供更好的原生途径，hook使我们可以在无需修改组件结构的情况下复用状态逻辑**

由官方文档的介绍我们知道hook是为了解决组件间状态复用而诞生的，所以当然是在React函数内使用。

# `useState`实现原理推想
```jsx
function Demo(){
    const [firstName,setFirstName] = useState('王');
    const [lastName,setLastName] = useState('小明');

    return <div>
        <div>姓名：{firstName} {lastName}</div>
        <button onClick={()=>setFirstName('李')}>改姓</button>
    </div>
}
```

## 初始化
初始化时，声明了两个空数组:`status`、`setters`，和一个参数`cursor`。
> 其实还需要一个值判断是否是初始化

## 初次渲染
1. 执行`const [firstName,setFirstName] = useState('王');` 
    1. 此时`cursor`为0，我们将初始参数值`'王'`放入到`status[0]`的位置【我们将这个地址命名为firstName】
    2. 同时在`setters[0]`的位置放一个用于修改`firstName`的方法，我们称之为`setFirstName`，这个方法除了修改`firstName`，还另有一个机制，会触发视图渲染。不过，这里可能并不是立即触发视图渲染，因为一次渲染中可能同时存在多个状态改变，因为我认为可能是做了一个标记，等待所有状态改变完成再执行`render`
    3. cursor的值自增加，变为`1`
2. 执行`const [lastName,setLastName] = useState('小明');` 
    1. 同上，在`setters[1]`、`status[1]`放入对应的值
    2. cursor的值自增加，变为`2`

## 后续渲染
0. cursor重置为0
1. 执行`const [firstName,setFirstName] = useState('王');` 
    1. 从`setters[0]`、`status[0]`取出存放的值
    2. cursor的值自增加，变为`1`
2. 执行`const [lastName,setLastName] = useState('小明');` 
    1. 从`setters[1]`、`status[1]`取出存放的值
    2. cursor的值自增加，变为`2`

## 事件渲染
每一个`setter`方法都和对应位置`cursor`上的`state`绑定。

`setter`方法记住了各自的`cursor`并根据它来修改`status`中的值

# `useState`验证猜想

## 测例1
```js
let count = 0;

function Demo(){
    let a, setA;
    if(count===0){
         [a,setA] = useState(0);
      	 count++;//这一行语句至关重要，注掉这一行就不会报错了
    }
    const [b,setB] = useState(0);

    return <div>
    <div>a：{a}</div>
    <div>b：{b}</div>
    <button onClick={()=>setB(x=>++x)}>触发重渲染</button>
   </div>
}
```
运行这个实例，初始加载没有问题，点击`触发重渲染`会报错：`Error: Rendered fewer hooks than expected. This may be caused by an accidental early return statement.`(错误:呈现的钩子比预期的少。这可能是由意外的提前返回语句引起的。)

如果我们将`count++`注掉，再点击按钮，会发现一切执行正常

我们想要检测钩子和上一次是否一致，是很容易做到的，拿一个变量记录下即可。

## 测例2
```js
if(count!==0){
    [a,setA] = useState(0);
}
count++;
```
改成这样，点击按钮会报错：`Error: Rendered more hooks than during the previous render.`(错误:比上次渲染时渲染了更多的钩子。)

原因和测例1类似，是因为钩子数目对不上

## 测例3
上述例子中，报错都是钩子数目对应不上导致的，现在我们改一下，让两次渲染取出的数目不同，但是对应的钩子不同
```js
let count = 0;

function Demo(){
    let a, setA, c, setC;
    if(count===0){
         [a,setA] = useState(1);
		 count++;
    }else{
         [c,setC] = useState(100);
    }
    const [b,setB] = useState(10);

    return <div>
    <div>a：{a}</div>
    <div>b：{b}</div>
    <div>c：{c}</div>
    <button onClick={()=>setB(x=>++x)}>触发重渲染</button>
   </div>
}
```

### 运行实例的现象
初次渲染<br>
```js
a:1
b:10
c:undefined
```

点击触发重渲染
```js
a:undefined
b:11
c:1
```

后续再次点击，都是`b`的递增了。

这个现象是怎么产生的？

根据我们之前的原理分析，初始化时存的的是`[a,b]`，后续渲染取的是`[c,b]`

# 模拟实现`useState`
## 实现`y_useState`
通过上面的例子我们基本可以证实原生的`useState`是以我们推想的方式运行的，接下来我们自己模拟实现一个`useState`
```js
const status = [];
const setters = [];
let firstRun = true;
let cursor = 0;

function createSetter(cursor){
    return function setterWithCursor(newValue){
        status[cursor] = newValue;
    }
}

function y_useState(initValue){
    if(firstRun){
        status.push(initValue);
        setters.push(createSetter(cursor));
        // firstRun = false;//！不要在这里重置，这个应该在外部第一次渲染结束时重置
    }

    const setter = setters[cursor];
    const value = status[cursor];

    cursor++;

    return [value,setter];
}
```
以上是我们的核心实现，现在我们用一个实例来测试实现的`y_useState`

## 测试`y_useState`
```js
function Demo(){
    cursor = 0;//每次渲染将下标重置
    const [firstName,setFirstName] = y_useState('王');
    const [lastName,setLastName] = y_useState('小明');
    const [,forceUpdate] = useState(0);//因为y_useState没有实现视图重新渲染的部分，所以需要借助原生的进行视图更新
    firstRun = false;//

    return <div>
        <div>姓名：{firstName} {lastName}</div>
        <button onClick={()=>{
            setFirstName('李')
            setLastName('小强');
            forceUpdate(x=>++x);
        }}>修改</button>
    </div>
}
```

# React函数式组件渲染流程一览
> 注：这部分是我自己的分析
```js
1. 第一次渲染
    1.创建firstRun=true、cursor=0、status=[]、setters=[]、shouldUpdate=true
    2.调用函数式组件本身【数据的更新】
        1. first=true，则进行status和setters的存储
    3.调用完了
        1. 将firstRun置为false
        2. 更新视图渲染
        3. shouldUpdate置为false

2. 后续渲染
    1. 重置cursor=0;
    2. 调用函数式组件本身【数据的更新-执行次数是最多的】
        1. first=false进行status和setters的读取
        2. 执行中如果触发setter, 则进行判断，如果有一个新值与旧值不同【React内部使用了Object.is算法做了浅比较】，则标记shouldUpdate=true(这种做法比起在最终比较有一个好处，就是不需要记录上一次的所有值，仅比较当个cursor位置的值是否变化即可，有一个状态变化，则视图需要更新)
    3.调用完成
        1. 更新视图
            - shouldUpdate=true 更新视图渲染
            - shouldUpdate=false 不更新视图渲染
        2. shouldUpdate置为false
```