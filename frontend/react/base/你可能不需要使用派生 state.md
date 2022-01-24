[TOC]

# 1. 什么时候使用派生state
官方没有给出准确的答案，不过官方指示我们什么时候不需要使用派生state，很多开发者使用派生state的场景都是不必要的，我们有更好的选择，以下是官方明确的不需要使用派生state的场景：
1. 缓存计算数据
2. 保存props
3. 和当前state比较不一致后更新state

关于这些场景，不使用派生state，官方都给出了推荐方案，接下来的部分会介绍。

如果排除以上场景，你仍然感到需要使用派生state，官方推荐使用`getDerivedStateFromProps`，这个API唯一的作用就是：**在props变化时更新state**

官方对于派生state的原则是：保守使用，尽可能不用。因为此原则的存在，所以官方并没有提供很少派生state的实例。以下两个实例为官方示例：
1. [props 的 offset 变化时，修改当前的滚动方向](https://react.docschina.org/blog/2018/03/27/update-on-async-rendering.html#updating-state-based-on-props)
2. [根据 props 变化加载外部数据](https://react.docschina.org/blog/2018/03/27/update-on-async-rendering.html#fetching-external-data-when-props-change)


# 2. 派生state常见bug
先介绍两个基本概念：**受控**和**非受控**
- 用props传入数据，被称作受控组件【因为组件受props控制】
- 数据只保存在组件内部的state的话，是非受控组件【因为外部没办法直接控制state】

编写React组件有一个很重要的原则：**保证数据源清晰且唯一**

因此，不要将受控组件与非受控组件混为一谈，如果一个派生state可以通过`setState`更新，这个值就不是单一来源了——这会引发很严重的问题，绝对不要这么做。

针对这个问题，展示一个反实践案例：
> 示例中使用了 componentWillReceiveProps ，使用 getDerivedStateFromProps 也是一样。

```
class EmailInput extends Component {
  state = { email: this.props.email };

  render() {
    return <input onChange={this.handleChange} value={this.state.email} />;
  }

  handleChange = event => {
    this.setState({ email: event.target.value });
  };

  componentWillReceiveProps(nextProps) {
    // 这会覆盖所有组件内的 state 更新！
    // 不要这样做。
    this.setState({ email: nextProps.email });
  }
}
```

这是React的官方案例，ok，很好的一个反例。

在这个案例中`this.state.email`被两处控制：
- `handleChange`回调函数
- `componentWillReceiveProps`生命周期
这个值即被内部控制，同时又受到外部props的影响，数据来源不唯一，非常糟糕。

可能会遇到什么问题？比如说，我们在输入框内输入内容，修改state，但是如果父组件重新渲染，state被props重新赋值，我们输入值就会被丢失。
> 有一个常见误解是：`getDerivedStateFromProps`和`componentWillReceiveProps`只在props变化时触发，实际上并不是，只要组件重渲染就会触发这两个生命周期。因此，在两个方法内直接复制props给state是不安全，会导致state异常渲染。

怎么解决这个问题？通过`shouldComponentUpdate`？
1. 首先，官方一再强调：**`shouldComponentUpdate`只用于性能优化，不用于阻塞渲染**，这个想法一开始就是错误的方向。`shouldComponentUpdate`最佳实践是性能，不要想着靠它修复问题。
2. 而且，如果不修改`componentWillReceiveProps`部分的逻辑，想仅靠`shouldComponentUpdate`控制渲染基本是不可能的，即便可以做到也会非常困难。<br>
为什么？`shouldComponentUpdate`、`componentWillReceiveProps`在每次渲染都会执行，也就是说修改`state`、`props`的操作都造成生命周期的执行，而且，即便不通过`props`传递值，父组件的state更新也会引发子组件的渲染，在实践中，会有很多`props`和`state`的存在，想要创建一个完全可靠的`shouldComponent`会越来越难，而且，一开始就说了，`shouldComponentUpdate`只是性能优化的手段，不要想靠它修复问题<br>
这是官方给的一个问题案例：[案例](https://codesandbox.io/s/jl0w6r9w59)

除了`shouldComponentUpdate`，另一种更为常见的错误思路是在`componentWillReceiveProps`加一个判断`nextProps.email !== this.props.email`，这样便可以保证只有`props`被修改时才会重置state值，解决了信息丢失的问题。<br>

```

class EmailInput extends Component {
  state = {
    email: this.props.email
  };

  componentWillReceiveProps(nextProps) {
    // 只要 props.email 改变，就改变 state
    if (nextProps.email !== this.props.email) {
      this.setState({
        email: nextProps.email
      });
    }
  }
  
  // ...
}
```
我一开始解决方案也是这个，因为从需求来说这个方案很明确的解决了需求，然而实际上存在一些隐患。

有这么一种情况，现在是一个用户编辑信息的页面，我们根据用户姓名【实际上是id】标志身份，用户可以输入邮箱、手机号、密码等信息。

从逻辑角度考虑，每次用户切换，身份信息都会被重置。假设有两个用户填写了相同的邮箱，假设初始邮箱都是`www.abc@gmail.com`，你修改了A用户邮箱为`www.xyz@gamil.com`，然后切换到B用户，此时你会发现邮箱竟然没有被重置！

因为`props.email`的值一直没有改变，我们输入的值更新的一直是`this.state.email`的值。

这个设计是有问题的，问题的根源便在于：数据的来源不唯一。

不过很幸运，官方为我们提供了解决方案，关键在于:**任何数据，都要保证只有一个数据来源，而且要避免直接复制它**

下来学习官方提供的几种解决方案。

# 3. 推荐方案
## 3.1 完全可控的组件
从组件里删除`state`，如果props包含email，将其作为唯一数据源即可，这样就不会有和`state`冲突的问题。<br>
这个方案另有的好处是组件会更轻量，更易阅读、理解和维护。
```
function EmailInput(props) {
  return <input onChange={props.onChange} value={props.email} />;
}
```
这样就可以以最简单的方式实现这个组件。但是如果我们希望保存临时的值，那么可以在父组件手动执行这个操作<br>
[官方示例](https://codesandbox.io/s/7154w1l551)

## 3.2 使用key重置非可控组件【官方推荐】
另一种方案是选择使用state存储临时的值，可以接受`props`传递的值，但是`props`值仅作为初始值设置，修改之后的值与`props`完全无关了，建议此时props传递进来的值以`init`或`default`作为标志。
```
class EmailInput extends Component {
  state = { email: this.props.defaultEmail };

  handleChange = event => {
    this.setState({ email: event.target.value });
  };

  render() {
    return <input onChange={this.handleChange} value={this.state.email} />;
  }
}
```
为了实现之前的用户切换信息重置的效果，在使用时我们借助`key`这个特殊的React属性。<br>
当`key`变化时，React会创建一个新的组件而不是更新既有组件。<br>
一般key被我们用来渲染动态列表，但是在这里也可以使用。官方的例子里`key`使用了`user.id`这个属性
```
<EmailInput
  defaultEmail={this.props.user.email}
  key={this.props.user.id}
/>
```
每次Id更改，都会重新创建EmailInput，然后重置`defaultEmail`属性。

**大部分情况下，这是重置`state`的最好方法。**

或许你认为这么做会很慢，实际上并不是。组件简单的情况下这点的性能可以忽略不计。而如果在组件树的更新上有很重的逻辑，这样反而会更快，因为省略了子组件的diff

## 3.3 用`props`的`ID`重置非受控组件
以下两种情况可能会让我们选择此方案：
- 如果某些情况下 key 不起作用（?可能是组件初始化的开销太大）
- 如果你想更灵活的控制`state`的重置<br>
以上两种情况你都可以使用此方案。
```
class EmailInput extends Component {
  state = {
    email: this.props.defaultEmail,
    prevPropsUserID: this.props.userID
  };

  static getDerivedStateFromProps(props, state) {
    // 只要当前 user 变化，
    // 重置所有跟 user 相关的状态。
    // 这个例子中，只有 email 和 user 相关。
    if (props.userID !== state.prevPropsUserID) {
      return {
        prevPropsUserID: props.userID,
        email: props.defaultEmail
      };
    }
    return null;
  }

  // ...
}
```
在这个案例中，官方使用了`props.userID`作为重置的信号，每次重置的`state`是由我们自由决定的，我们可以更自由的控制需要重置的`state`
> 注，这里使用`getDerivedStateFromProps`或`componentWillReceiveProps`都可以

此方案优点是比`key`重置更灵活、适应性更强，缺陷是写法稍微麻烦一些【官方明确提到这一点】

## 3.4 使用实例方法重置非受控组件
有些更为少见的情况：没有合适的`key`或`ID`值，我们也想重置组件。<br>
官方给了两种可行的方案
- 给一个随机值或递增值作为`key`
- 使用实例方法强制重置内部状态

```
class EmailInput extends Component {
  state = {
    email: this.props.defaultEmail
  };

  resetEmailForNewUser(newEmail) {
    this.setState({ email: newEmail });
  }

  // ...
}
```

非受控组件提供一个自己的重置方法，父组件在外面通过`ref`拿到这个方法然后在有需要的时候执行即可。比如：
`this.inputRef.current.resetEmailForNewUser(selectedAccount.email);`

[官方示例](https://codesandbox.io/s/l70krvpykl)

refs 在某些情况下很有用，比如这个。

官方建议谨慎使用，这个命令式的方法是非理想的，很多时候会和`componentDidUpdate`一样导致两次渲染。【示例中渲染只有一次】

# 4.组件设计思路归纳
1. 首先，最重要的是确定组件是受控组件还是非受控组件。<br>
官方倾向于优先实现受控组件。不要用`props`派生`state`，比如不要子组件中利用`state.value`追踪`props.value`的值，而是在父组件里管理子组件的状态，比如`state.committedValue`的值，直接在父组件控制子组件里的值，这样数据才更加明确可测
2. 不受控组件方案
如果你需要在`props`变化时重置`state`，官方给出了几种方案：
- 使用`key`重置【官方推荐】
- 仅改变某些状态，观察特殊的`props`属性变化【比如`props.userID`】
- 使用ref调用实例方法

# 5. 尝试`memoization`
实践中，为了避免多余的复杂运算， 我们会只在输入变化时进行运算——这种技术被称作`memoization`

一些开发者喜欢使用派生`state`来做`memoization`，然而这并不是一种好的方式。<br>
两种情况，第一种 ，如果这个`state`仅仅是为了避免昂贵计算而创建的，那么它是不必要的，我们有更好的方式<br>
第二种，如果这个`state`的数据来源不唯一，这种方式会很容易引发bug，一则`state`数量会逐渐增多，会越来越难管理，二则如此一来我们必须追踪这个`state`的两份变化，每次进行改动，都必须追踪和管理两处数据来源。

官方给了一个示例：<br>
这里涉及了三个值：`props.list`、`state.value`、`filterList`<br>
需求是这样的：从父组件传入原始数组`props.list`，每次输入会更改`state.value`，`props.list`会筛选出包含`state.value`的项组成一个新数组`filterList`<br>
有一个实际的例子就是：下拉框根据输入内容过滤显示相应的列表
```
// 注意：这个例子是反例实践
class Example extends Component {
  state = {
    filterText: "",
  };

  static getDerivedStateFromProps(props, state) {
    // 列表变化或者过滤文本变化时都重新过滤。
    // 注意我们要存储 prevFilterText 和 prevPropsList 来检测变化。
    if (
      props.list !== state.prevPropsList ||
      state.prevFilterText !== state.filterText
    ) {
      return {
        prevPropsList: props.list,
        prevFilterText: state.filterText,
        filteredList: props.list.filter(item => item.text.includes(state.filterText))
      };
    }
    return null;
  }

  handleChange = event => {
    this.setState({ filterText: event.target.value });
  };

  render() {
    return (
      <Fragment>
        <input onChange={this.handleChange} value={this.state.filterText} />
        <ul>{this.state.filteredList.map(item => <li key={item.id}>{item.text}</li>)}</ul>
      </Fragment>
    );
  }
}
```
ok，这个例子实现了需求，不过因为它是通过派生`state`实现的，存在两份数据来源，所以必须同时追踪`props`和`state`的变化。从代码设计上来说，太复杂了。<br>

使用`PureComponent`可以更好的实现这个需求：
```
// PureComponents 只会在 state 或者 prop 的值修改时才会再次渲染。
// 通过对 state 和 prop 的 key 做浅比较（ shallow comparison ）来确定有没有变化。
class Example extends PureComponent {
  // state 只需要保存 filter 的值：
  state = {
    filterText: ""
  };

  handleChange = event => {
    this.setState({ filterText: event.target.value });
  };

  render() {
    // PureComponent 的 render 只有
    // 在 props.list 或 state.filterText 变化时才会调用
    const filteredList = this.props.list.filter(
      item => item.text.includes(this.state.filterText)
    )

    return (
      <Fragment>
        <input onChange={this.handleChange} value={this.state.filterText} />
        <ul>{filteredList.map(item => <li key={item.id}>{item.text}</li>)}</ul>
      </Fragment>
    );
  }
}
```
可以看到，在这里使用了`PureComponent`之后，我们就不需要使用`getDerivedStateFromProps`进行复杂的逻辑判断了，我们将过滤的逻辑直接放入了`render`之中。<br>
在没有其他更多`props`和`state`的情况下，这是个好方案，然而在有其他`props`和`state`的情况下，依旧会造成多余的计算。<br>

ok，现在来介绍官方的推荐的方案：使用`memoization`函数来避免多余计算。
```
import memoize from "memoize-one";

class Example extends Component {
  // state 只需要保存当前的 filter 值：
  state = { filterText: "" };

  // 在 list 或者 filter 变化时，重新运行 filter：
  filter = memoize(
    (list, filterText) => list.filter(item => item.text.includes(filterText))
  );

  handleChange = event => {
    this.setState({ filterText: event.target.value });
  };

  render() {
    // 计算最新的过滤后的 list。
    // 如果和上次 render 参数一样，`memoize-one` 会重复使用上一次的值。
    const filteredList = this.filter(this.props.list, this.state.filterText);

    return (
      <Fragment>
        <input onChange={this.handleChange} value={this.state.filterText} />
        <ul>{filteredList.map(item => <li key={item.id}>{item.text}</li>)}</ul>
      </Fragment>
    );
  }
}
```
写法更加简洁，而且效果比使用派生`state`要好的多！

以下是使用`memoization`的一些约束：
- 大部分情况下，**每个组件都需要引入`memoization`函数，以避免实例间的相互影响**【？】
- 一般情况下，我们会**限制 memoization 帮助函数的缓存空间，以免内存泄漏**。（上面的例子中，使用 memoize-one 只缓存最后一次的参数和结果）
- 如果每次父组件都传入新的`props.list`，那就不需要考虑太多，不过大多数情况下本方法都是有用的。

# 结束语
实际开发中，组件被分为受控组件与非受控组件。一个应当遵守的重要原则就是：**组件`state`的数据来源要清晰且唯一**，如此一来就可以避免很多问题。<br>
对于`getDerivedStateFromProps`以及其他派生`state`，这是一个高级复杂的功能，官方一再强调，应当保守使用这个功能。