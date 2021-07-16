[TOC]

`single-page application`（single-page application）单页面应用就是一个网站只有一个页面，切换url实质上是切换组件。

这种做法的好处是我们不需要一次性将所有页面都进行加载，节省了内存和提升了速度。

另一方面，切换页面时，有些组件时可以复用的，可以只切换需要替换的组件，同样也提升了渲染的效率。

比如说：
//这里需要一张示意图

想要实现这种效果，需要一套路由系统，这个路由主要做的事情就是：
1. 监听url变化
2. 根据url变化匹配到对应的Route，渲染对应Route上的组件

我以`react-router-dom`库为例，说明下大致的的做法。

实现这个路由系统，主要涉及三个库：
- `react-router`
- `history`
- `react-react-dom`

其中，实现核心功能的是`history`和`react-router`

# 库源码版本说明
- `react-router-dom` 5.12.0
- `react-router` 5.12.0
- `history` 4.9.0

# 从基本用法谈起
```jsx
<HashRouter>
    <Menu/> {/*菜单部分*/}
    <Switch> {/*切换部分*/}
        <Route component={ViewA} path='/view/a'/>
        <Route component={ViewB} path='/view/b'/>
        <Route component={DefaultView}/>
    </Switch>
</HashRouter>
```
实际运行结果如下：
//需要一张切换页面的gif图

让我们看一下这段代码做了什么
- `HashRouter` 提供上下文信息，监听`location`变化
- `Switch` 会根据当前的`location`渲染第一个匹配的`Route`
- `Route` 会创建上下文信息，渲染组件

我们依次解读下这三个组件的源码。

## `HashRouter`
`HashRouter`就做了一件事：传递`history`。

> 出自`react-router`库，文件位于`packages/react-router-dom/modules/HashRouter.js`

核心代码：
```jsx
//让Router使用window.location.hash的公开API
class HashRouter extends React.Component {
  history = createHistory(this.props);//注：这个createHistory实际上是createHashHistory

  render() {
    return <Router history={this.history} children={this.props.children} />;
  }
}
```

这个源码里有两个关键的内容：
- `createHashHistory`
- `Router`

### `createHashHistory`
简单来说，这个方法返回了一个`history`对象，`history`有一系列属性和方法，比如`push`、`replace`这些

源码内容较多，这里并不展示，之后会详细介绍，这个方法很重要。

> 出自`history`库，文件位于`modules/createHashHistory.js`

### `Router`
核心代码：
```jsx
class Router extends React.Component {
  //...

  render() {
    return (
      <RouterContext.Provider
        value={{
          history: this.props.history,
          location: this.state.location,
          match: Router.computeRootMatch(this.state.location.pathname),
          staticContext: this.props.staticContext
        }}
      >

        <HistoryContext.Provider
          children={this.props.children || null}
          value={this.props.history}
        />
      </RouterContext.Provider>
    );
  }
}
```
可以看到，`Router`提供了重要的上下文信息，分别说一下这些信息：
- `history`：从`props`获取的，不需要说明
- `location`：一般情况下是`props.history.location`，不过针对渲染的一些特殊情况做了`hack`处理，因为在`Router`加载之前，`location`可能已经变化了，所以需要监听`location`的变化：
    - 如果`Router`已被加载，则使用最新的`location`
    - 如果`Router`没有被加载，则将`location`设为`_pendingLoaction`，等加载结束后将其设置为最新的`location`
- `match`：是一个对象，`{ path: "/", url: "/", params: {}, isExact: pathname === "/" }`
- `staticContext`：使用`StaticRouter`才有的上下文信息

### `Switch`
从源码里可以看到Switch主要任务就是匹配子项的`path`，并根据其生成`match`，然后将其传递给子项。

核心源码:
```jsx
class Switch extends React.Component {
  render() {
    return (
      <RouterContext.Consumer>
        {context => {
          invariant(context, "You should not use <Switch> outside a <Router>");

          const location = this.props.location || context.location;

          let element, match;

          //根据第一个匹配到的path，根据其pathname生成match
          React.Children.forEach(this.props.children, child => {
            if (match == null && React.isValidElement(child)) {
              element = child;

              const path = child.props.path || child.props.from;

              match = path
                ? matchPath(location.pathname, { ...child.props, path })
                : context.match;//如果没有设置path，则默认使用context的match
            }
          });

          //如果成功匹配则为其添加location、computeMatch两个props
          return match
            ? React.cloneElement(element, { location, computedMatch: match })
            : null;
        }}
      </RouterContext.Consumer>
    );
  }
}
```

#### `matchPath`
`matchPath`主要就是利用`options.path`生成正则表达式，再根据正则表达式去匹配`pathname`。

如果一切正常，返回的结果会包含以下内容（属性名已经表述的很清楚了，不再解释）：
- `path`
- `url`
- `isExact`
- `params`

核心源码如下：
```js
function matchPath(pathname, options = {}) {
    if (typeof options === "string" || Array.isArray(options)) {
        options = { path: options };
    }

    const { path, exact = false, strict = false, sensitive = false } = options;

    const paths = [].concat(path);

    return paths.reduce((matched, path) => {
        if (!path && path !== "") return null;
        if (matched) return matched;

        //regexp是生成的正则表达式，keys是从路径中匹配出来的键组成的数组
        const { regexp, keys } = compilePath(path, {
            end: exact, //为 true 时，regexp 将匹配到字符串的末尾
            strict, //为 true 时，regexp 不允许可选的尾随定界符匹配
            sensitive //为 true 时，regexp 区分大小写
        });
        const match = regexp.exec(pathname);

        if (!match) return null;

        const [url, ...values] = match;
        const isExact = pathname === url;

        if (exact && !isExact) return null;

        return {
            path, // 用于匹配的path
            url: path === "/" && url === "" ? "/" : url, // URL的匹配部分
            isExact, // 是否完全匹配
            params: keys.reduce((memo, key, index) => {
                memo[key.name] = values[index];
                return memo;
            }, {})
        };
    }, null);
}
```

另外说一下，这里的`compilePath`核心功能借助了[path-to-regexp](https://github.com/pillarjs/path-to-regexp)库，然后在此之外还做了缓存，缓存的原理并不复杂，感兴趣可以阅读源码。

### `Route`
`Route`需要注意的有三部分：
1. `match`选择的优先度
1. `Route`会创建`RouterContext`【与第1点有关】
1. `children`、`component`、`render`渲染的优先度及其类型【正常只会传递其中的一种】

核心源码如下：
```jsx
class Route extends React.Component {
    render() {
        return (
            <RouterContext.Consumer>
                {context => {
                    invariant(context, "You should not use <Route> outside a <Router>");

                    const location = this.props.location || context.location;

                    //有computedMatch，最优先使用computedMatch
                    //有path，次优先自己生成match
                    //无path，使用上下文的match【稍微需要注意的，这里未必是Router传递的上下文，因为Route也会创建RouterContext】
                    const match = this.props.computedMatch
                        ? this.props.computedMatch // <Switch> already computed the match for us
                        : this.props.path
                            ? matchPath(location.pathname, this.props)
                            : context.match;

                    //context提供了history、staticContext、location、match
                    const props = { ...context, location, match };

                    let { children, component, render } = this.props;

                    if (Array.isArray(children) && children.length === 0) {
                        children = null;
                    }

                    //创建一个RouterContext
                    //有props.children，最优先渲染children
                    //有props.component，次优先渲染component
                    //props.render优先度在以上两种情况之后
                    return (
                        <RouterContext.Provider value={props}>
                            {props.match
                                ? children
                                    ? typeof children === "function"
                                        ? __DEV__
                                            ? evalChildrenDev(children, props, this.props.path)
                                            : children(props)
                                        : children
                                    : component
                                        ? React.createElement(component, props)
                                        : render
                                            ? render(props)
                                            : null
                                : typeof children === "function"
                                    ? __DEV__
                                        ? evalChildrenDev(children, props, this.props.path)
                                        : children(props)
                                    : null}
                        </RouterContext.Provider>
                    );
                }}
            </RouterContext.Consumer>
        );
    }
}
```

## `createHashHistory`使用及其源码
先看下返回的`history`对象有哪些内容：
```js
const history = {
    length: globalHistory.length,
    action: 'POP',
    location: initialLocation,
    createHref,
    push,
    replace,
    go,
    goBack,
    goForward,
    block,
    listen
};
```

### `history`属性
首先说`history`的属性部分
- `length` 其实就是`window.history.length`
- `action` 
- `location`

### `history`方法
- `createHref`
- `push`
- `replace`
- `go` 底层是`window.locatio.go`，不过在使用前做了可用性检查
- `goBack` 相当于`go(-1)`
- `goForward` 相当于`go(1)`
- `block`
- `listen`

# 参考资料
- [MDN-Location](https://developer.mozilla.org/zh-CN/docs/Web/API/Location)
- [path-to-regexp](https://github.com/pillarjs/path-to-regexp)
- [react-router v5.2.0](https://github.com/ReactTraining/react-router/tree/v5.2.0)
- [history v4.9.0](https://github.com/ReactTraining/history/tree/v4.9.0)