[TOC]

# 什么是HOC？
高级组件（HOC）是React复用组件逻辑的一种高级技巧，HOC自身不是React API的一部分，它是一种基于React组合特性形成的设计模式。

具体定义：**高阶组件是参数是组件，返回值是新组件的函数**

```
const EnhancedComponent = higherOrderComponent(WrappedComponent);
```

组件是将`props`转换为UI，而高阶组件是将组件转换为另一个组件。

我认为学习一个知识【x】分为三部分：
1. x是什么？
2. x有什么用？
3. x怎么用？

现在我们知道了高阶组件的定义，接下来继续学习HOC解决了哪些问题？然后学习怎么使用HOC

# 使用 HOC 解决横切关注点问题
假设现在有两个组件：

组件1：
```jsx
class CommentList extends React.Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.state = {
      // 假设 "DataSource" 是个全局范围内的数据源变量
      comments: DataSource.getComments()
    };
  }

  componentDidMount() {
    // 订阅更改
    DataSource.addChangeListener(this.handleChange);
  }

  componentWillUnmount() {
    // 清除订阅
    DataSource.removeChangeListener(this.handleChange);
  }

  handleChange() {
    // 当数据源更新时，更新组件状态
    this.setState({
      comments: DataSource.getComments()
    });
  }

  render() {
    return (
      <div>
        {this.state.comments.map((comment) => (
          <Comment comment={comment} key={comment.id} />
        ))}
      </div>
    );
  }
}
```

组件2：
```jsx
class BlogPost extends React.Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.state = {
      blogPost: DataSource.getBlogPost(props.id)
    };
  }

  componentDidMount() {
    DataSource.addChangeListener(this.handleChange);
  }

  componentWillUnmount() {
    DataSource.removeChangeListener(this.handleChange);
  }

  handleChange() {
    this.setState({
      blogPost: DataSource.getBlogPost(this.props.id)
    });
  }

  render() {
    return <TextBlock text={this.state.blogPost} />;
  }
}
```

两个组件的不同点在于:
1. 执行的`handleChange`方法
2. 渲染结果

相同点在于：
在挂载时，向 DataSource 添加一个更改侦听器。
在侦听器内部，当数据源发生变化时，调用 setState。
在卸载时，删除侦听器。

你可以想象，在一个大型应用程序中，这种订阅 DataSource 和调用 setState 的模式将一次又一次地发生。

我们需要一个抽象，允许我们在一个地方定义这个逻辑，并在许多组件之间共享它。这正是高阶组件擅长的地方。

我们可以编写一个创建组件函数。该函数将接受一个子组件作为它的其中一个参数，该子组件将订阅数据作为 prop：
```jsx
// 此函数接收一个组件...
function withSubscription(WrappedComponent, selectData) {
  // ...并返回另一个组件...
  return class extends React.Component {
    constructor(props) {
      super(props);
      this.handleChange = this.handleChange.bind(this);
      this.state = {
        data: selectData(DataSource, props)//
      };
    }

    componentDidMount() {
      DataSource.addChangeListener(this.handleChange);
    }

    componentWillUnmount() {
      DataSource.removeChangeListener(this.handleChange);
    }

    handleChange() {
      this.setState({
        data: selectData(DataSource, this.props)//
      });
    }

    render() {//
      return <WrappedComponent data={this.state.data} {...this.props} />;
    }
  };
}
```
思路稍微整理下，大概就是：
1. 将公用的逻辑封装在HOC中，如果公用逻辑有非公用【变化】的部分，则通过`props`传入
2. 最后将公用部分的数据传递给包装组件，交由公用组件使用
> 注意：HOC 不会修改传入的组件，HOC 是纯函数，没有副作用

该HOC使用方式如下：
```jsx
const listUpdateData = (DataSource) => DataSource.getComments();
const CommentListWithSubscription = withSubscription(CommentList,listUpdateData);

const blogUpdateData = (DataSource, props) => DataSource.getBlogPost(props.id);
const BlogPostWithSubscription = withSubscription(BlogPost,blogUpdateData);
```

HOC模式的重点总结：
1. HOC组件不关注数据的使用方式或原因，包装组件不关注数据从何而来
2. HOC的参数，只有包装函数是必传的，其他的根据实际需要决定要传递哪些参数
3. HOC易于替换：包装组件之间的数据传递完全依赖于props，这种依赖方式使得替换HOC变得容易，只要为包装的组件提供相同的prop即可。

# 不要改变原始组件-使用组合
我们来看一个反例：
```jsx
function logProps(InputComponent) {
  InputComponent.prototype.componentDidUpdate = function(prevProps) {
    console.log('Current props: ', this.props);
    console.log('Previous props: ', prevProps);
  };
  // 返回原始的 input 组件，暗示它已经被修改。
  return InputComponent;
}

// 每次调用 logProps 时，增强组件都会有 log 输出。
const EnhancedComponent = logProps(InputComponent);
```
这个案例LogProps会改变原组件，会有这么一些不良影响：
1. 可能我们这一处需要执行相应的函数，但是这种改法会影响到所有使用该组件的场景
2. 如果有另一个HOC同样改变原组件的`componentDidUpdate`方法，则其中一个HOC会失效

在HOC中改变原组件是一种极为糟糕的做法，可以说是HOC的第一大禁忌，会产生极为恶劣的后果，谨记！

让我们看一下正确的做法：
> HOC 不应该修改传入组件，而应该使用组合的方式，通过将组件包装在容器组件中实现功能
```jsx
function logProps(WrappedComponent) {
  return class extends React.Component {
    componentDidUpdate(prevProps) {
      console.log('Current props: ', this.props);
      console.log('Previous props: ', prevProps);
    }
    render() {
      // 将 input 组件包装在容器中，而不对其进行修改。Good!
      return <WrappedComponent {...this.props} />;
    }
  }
}
```
该 HOC 与上文中修改传入组件的 HOC 功能相同，同时避免了出现冲突的情况。它同样适用于 class 组件和函数组件。而且因为它是一个纯函数，它可以与其他 HOC 组合，甚至可以与其自身组合

HOC 与容器组件模式之间有相似之处，容器组件担任分离将高层和低层关注的责任，由容器管理订阅和状态，并将 prop 传递给处理渲染 UI。HOC 使用容器作为其实现的一部分，你可以将 HOC 视为参数化容器组件。

# 一些约定
## 将不相关的 props 传递给被包裹的组件
- HOC 应该透传与自身无关的 props。
- 好处：这种约定保证了 HOC 的灵活性以及可复用性。

## 最大化可组合性
并不是所有的 HOC 都一样。有时候它仅接受一个参数，也就是被包裹的组件，不过更多时候HOC接收多个参数，这取决于它需要什么。

有时，利用柯里化手法，可以进一步复用参数。

## 包装显示名称以便轻松调试
HOC 创建的容器组件会与任何其他组件一样，会显示在 React Developer Tools 中。为了方便调试，请选择一个显示名称，以表明它是 HOC 的产物。

最常见的方式是用 HOC 包住被包装组件的显示名称。比如高阶组件名为 withSubscription，并且被包装组件的显示名称为 CommentList，显示名称应该为 WithSubscription(CommentList)：
```jsx
function withSubscription(WrappedComponent) {
  class WithSubscription extends React.Component {/* ... */}
  WithSubscription.displayName = `WithSubscription(${getDisplayName(WrappedComponent)})`;
  return WithSubscription;
}

function getDisplayName(WrappedComponent) {
  return WrappedComponent.displayName || WrappedComponent.name || 'Component';
}
```

# 注意事项
## 不要在 render 方法中使用 HOC
React 的 diff 算法（称为协调）使用组件标识来确定它是应该更新现有子树还是将其丢弃并挂载新子树。 如果从 render 返回的组件与前一个渲染中的组件相同（===），则 React 通过将子树与新子树进行区分来递归更新子树。 如果它们不相等，则完全卸载前一个子树。

通常，你不需要考虑这点。但对 HOC 来说这一点很重要，因为这代表着你不应在组件的 render 方法中对一个组件应用 HOC：
```jsx
render() {
  // 每次调用 render 函数都会创建一个新的 EnhancedComponent
  // EnhancedComponent1 !== EnhancedComponent2
  const EnhancedComponent = enhance(MyComponent);
  // 这将导致子树每次渲染都会进行卸载，和重新挂载的操作！
  return <EnhancedComponent />;
}
```

在render中使用HOC存在两个问题：
1. 性能问题：每次渲染都会导致组件重新创建
2. 状态丢失：重新挂载组件会导致该组件及其所有子组件的状态丢失

## 务必复制静态方法
有时在 React 组件上定义静态方法很有用。例如，Relay 容器暴露了一个静态方法 getFragment 以方便组合 GraphQL 片段。

但是，当你将 HOC 应用于组件时，原始组件将使用容器组件进行包装。这意味着新组件没有原始组件的任何静态方法。
```jsx
// 定义静态函数
WrappedComponent.staticMethod = function() {/*...*/}
// 现在使用 HOC
const EnhancedComponent = enhance(WrappedComponent);

// 增强组件没有 staticMethod
typeof EnhancedComponent.staticMethod === 'undefined' // true
```

为了解决这个问题，你可以在返回之前把这些方法拷贝到容器组件上：
```jsx
function enhance(WrappedComponent) {
  class Enhance extends React.Component {/*...*/}
  // 必须准确知道应该拷贝哪些方法 :(
  Enhance.staticMethod = WrappedComponent.staticMethod;
  return Enhance;
}
```

但要这样做，你需要知道哪些方法应该被拷贝。你可以使用 [hoist-non-react-statics](https://github.com/mridgway/hoist-non-react-statics) 自动拷贝所有非 React 静态方法:
```jsx
import hoistNonReactStatic from 'hoist-non-react-statics';
function enhance(WrappedComponent) {
  class Enhance extends React.Component {/*...*/}
  hoistNonReactStatic(Enhance, WrappedComponent);
  return Enhance;
}
```

除了导出组件，另一个可行的方案是再额外导出这个静态方法。
```jsx
// 使用这种方式代替...
MyComponent.someFunction = someFunction;
export default MyComponent;

// ...单独导出该方法...
export { someFunction };

// ...并在要使用的组件中，import 它们
import MyComponent, { someFunction } from './MyComponent.js';
```

## Refs 不会被传递
虽然高阶组件的约定是将所有 props 传递给被包装组件，但这对于 refs 并不适用。那是因为 ref 实际上并不是一个 prop - 就像 key 一样，它是由 React 专门处理的。如果将 ref 添加到 HOC 的返回组件中，则 ref 引用指向容器组件，而不是被包装组件。

这个问题的解决方案是通过使用 React.forwardRef API（React 16.3 中引入）。

[refs转发](https://zh-hans.reactjs.org/docs/forwarding-refs.html#forwarding-refs-to-dom-components)