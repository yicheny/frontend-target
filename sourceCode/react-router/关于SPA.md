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
    - 如果`Router`已被加载，则直接将最新的`location`设置到`state`
    - 如果`Router`没有被加载，则最新的`location`设为`_pendingLocation`，等`Router`加载结束后将其设置到`state`
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

关于`history`属性，这里重点说明下`location`属性。

#### `location`
核心源码如下【注：已省略无关代码】：
```js
function createHashHistory(props = {}) {
  const { hashType = 'slash' } = props;
  //将basename处理成标准格式
  const basename = props.basename
    ? stripTrailingSlash(addLeadingSlash(props.basename))
    : '';

  const { encodePath, decodePath } = HashPathCoders[hashType];

  function getDOMLocation() {
    //decode得到原有的path
    let path = decodePath(getHashPath());

    //warn警告...

    //stripBasename: 如果path以basename开头，则返回basename后面的部分，否则返回path
    if (basename) path = stripBasename(path, basename);

    return createLocation(path);
  }

  // 在做其他任何事之前，保证hash被正确编码
  const path = getHashPath();
  const encodedPath = encodePath(path);

  //将window.location.href的hash部分换为encodePath
  if (path !== encodedPath) replaceHashPath(encodedPath);

  const initialLocation = getDOMLocation();

  const history = {
    location: initialLocation,
  };
}
```
描述下代码思路：
1. 得到标准格式的`basename`
2. 得到`hashPath`
3. 对`hashPath`进行`encode`【在做其他任何操作之前，需要先编码`hashPath`】
4. 使用编码后的路径替换原有的`hashPath`
5. 生成`initialLocation`
   1. 解码得到原有`hashPath`【之前编码的路径在其他地方也有用处，所以需要编码和解码，而非直接使用】
   2. 如果有`basename`，且`path`使用`basename`作为前缀，则以`basename`之后的字符串作为`path`
   3. 根据`path`生成`createLocation`

重点是这个`createLocation`方法

##### `createLocation`
`createLocation`是一个基础的工具方法，不仅在`getDOMLocation`里面使用了，还在`push`和`replace`这两个公共接口里使用了。

根据传入参数的不同，`createLocation`的行为也有所差异，这里我省略无关部分，只描述`getDOMLocation`里面会执行到的代码。

相关源码如下：
```js
function createLocation(path, state, key, currentLocation) {
  let location;
  if (typeof path === 'string') {
    location = parsePath(path);
    location.state = state;
  } else {
    //...
  }

  try {
    location.pathname = decodeURI(location.pathname);
  } catch (e) {
    //警告和报错...
  }

  if (key) location.key = key;

  if (currentLocation) {
    //...
  } else {
    if (!location.pathname) {
      location.pathname = '/';
    }
  }

  return location;
}
```
描述下这里相关代码的主要思路：
1. 将`path`解析成对象并赋值给`location`
2. `location.pathname = decodeURI(location.pathname);`【无需解释】
3. 返回`location`

我们再深入了解下细节，看看这里是怎么将`path`字符串解析的

##### `parsePath`
```js
export function parsePath(path) {
  let pathname = path || '/';
  let search = '';
  let hash = '';

  const hashIndex = pathname.indexOf('#');
  if (hashIndex !== -1) {
    hash = pathname.substr(hashIndex);
    pathname = pathname.substr(0, hashIndex);
  }

  const searchIndex = pathname.indexOf('?');
  if (searchIndex !== -1) {
    search = pathname.substr(searchIndex);
    pathname = pathname.substr(0, searchIndex);
  }

  return {
    pathname,
    search: search === '?' ? '' : search,
    hash: hash === '#' ? '' : hash
  };
}
```
关于`parsePath()`，需要了解以下内容：
- 参数：一个`path`字符串
- 返回值：一个对象，包含属性`pathname`、`search`、`hash`

这里不再描述代码思路，因为这里写的很漂亮，代码干净利落，命名也很准确，完整看下来思路就出来了，再解释反而会丧失许多细节。

### `history`方法
- `createHref`
- `push`
- `replace`
- `go` 底层是`window.location.go`，不过在使用前做了可用性检查
- `goBack` 相当于`go(-1)`
- `goForward` 相当于`go(1)`
- `block`
- `listen`

接下来会依次描述`createHref`、`push`、`replace`、`block`、`listen`这几个公开接口。

#### `createHref`
作用是将`location`还原成`path`【如果有`basename`，会将`basename`也加上】。

源码：
```js
function createHref(location) {
    return '#' + encodePath(basename + createPath(location));
}
```

这里我以官方测试库的一个单元测试为例说明用法：
```js
describe('with a bad basename', () => {
    let history;
    beforeEach(() => {
      history = createBrowserHistory({ basename: '/the/bad/base/' });
    });

    it('knows how to create hrefs', () => {
      const href = history.createHref({
        pathname: '/the/path',
        search: '?the=query',
        hash: '#the-hash'
      });

      expect(href).toEqual('/the/bad/base/the/path?the=query#the-hash');
});
```

#### `push`
相关源码：
```js
function push(path, state) {
    warning(
        state === undefined,
        'Hash history cannot push state; it is ignored'
    );

    const action = 'PUSH';
    const location = createLocation(
        path,
        undefined,
        undefined,
        history.location
    );

    transitionManager.confirmTransitionTo(
        location,
        action,
        getUserConfirmation,
        ok => {
            //这部分会在下面描述...
        }
    );
}
```
描述下思路：
1. 将`action`改为`push`
2. 根据`path`和`history.location`生成新的`location`
3. 调用`transitionManager.confirmTransitionTo(location,action,getUserConfirmation,callback)`

现在这里有两个重要的点，一个是`transitionManager`，一个是`callback`【就是`ok => ...`这个回调函数】

##### `createTransitionManager`
`transitionManager`是通过`createTransitionManager`创建的。

关于`transitionManager.confirmTransitionTo()`，对于有没有`promat`处理上是不同的，这里我先说一下没有`promat`的情况。

相关源码：
```js
function createTransitionManager() {
    let prompt = null;

    //...

    function confirmTransitionTo(
        location,
        action,
        getUserConfirmation,
        callback
    ) {
        if (prompt != null) {
            //... prompt可以被setPrompt方法修改，history.block里面调用了setPrompt
        } else {
            callback(true);
        }
    }

    //...

    return {
        confirmTransitionTo,
        //...
    };
}
```
很直接，调用`callback(true)`

现在让我们回到`ok => ...`这个回调：
```js
ok => {
    if (!ok) return;
    const path = createPath(location);
    const encodedPath = encodePath(basename + path);
    //getHashPath会获得最新的hash，而encodePath是根据history.location生成的hash
    const hashChanged = getHashPath() !== encodedPath;

    if (hashChanged) {
        ignorePath = path;//在handleHashChange()方法会用到
        pushHashPath(encodedPath);//window.location.hash = path;

        const prevIndex = allPaths.lastIndexOf(createPath(history.location));
        const nextPaths = allPaths.slice(
            0,
            prevIndex === -1 ? 0 : prevIndex + 1
        );

        nextPaths.push(path);
        allPaths = nextPaths;

        setState({ action, location });
    } else {
        warning(
            false,
            'Hash history cannot PUSH the same path; a new entry will not be added to the history stack'
        );

        setState();
    }
}
```
描述下思路：
1. 根据`history.location`经过一系列处理得到`encodePath`
2. 获取当前最新的`hashPath`和`encodePath`进行对比
3. 如果发现`hashPath`改变了，则进行一系列相关状态的更新，调用`setState`

继续去看一下`setState`：
```js
function setState(nextState) {
  Object.assign(history, nextState);
  history.length = globalHistory.length;
  transitionManager.notifyListeners(history.location, history.action);
}
```
主要做了两件事：
1. 更新`history`属性
2. 触发发布事件【这个发布订阅会在描述`listen`方法时说明，这里暂不深入】

现在回过去，看一下`prompt`不为`null`时怎么处理：
```js
function confirmTransitionTo(
    location,
    action,
    getUserConfirmation,
    callback
) {
    if (prompt != null) {
        const result =
        typeof prompt === 'function' ? prompt(location, action) : prompt;

      if (typeof result === 'string') {
        if (typeof getUserConfirmation === 'function') {
          getUserConfirmation(result, callback);
        } else {
          warning(
            false,
            'A history needs a getUserConfirmation function in order to use a prompt message'
          );

          callback(true);
        }
      } else {
        // 取消转换
        callback(result !== false);
      }
    } else {
        callback(true);//prompt为null时
    }
}
```
描述一下思路：

异常分支这里不做说明了，理想场景下`result`是字符串，`getUserConfirmation`是函数，此时执行`getUserConfirmation(result, callback);`

`result`和`getUserConfirmation`的源头分别在哪里？

首先是`result`，它的来源是`prompt`，`prompt`的修改来源是`history.block`，通过参数传入，稍微值得注意的是，`prompt`也可以是函数类型。

然后`getUserConfirmation`，它的源头是`createHashHistory(props)`的参数`props.getUserConfirmation`，如果没传，则默认是下面这个方法：
```js
export function getConfirmation(message, callback) {
  callback(window.confirm(message)); // eslint-disable-line no-alert
}
```

#### `replace`
```js
function replace(path, state) {
  warning(
    state === undefined,
    'Hash history cannot replace state; it is ignored'
  );

  const action = 'REPLACE';
  const location = createLocation(
    path,
    undefined,
    undefined,
    history.location
  );

  transitionManager.confirmTransitionTo(
    location,
    action,
    getUserConfirmation,
    ok => {
      if (!ok) return;

      const path = createPath(location);
      const encodedPath = encodePath(basename + path);
      const hashChanged = getHashPath() !== encodedPath;

      if (hashChanged) {
        ignorePath = path;
        //调用了window.location.replace，以给定的URL替换当前资源，特点是当前页面不会保存到会话历史中，这样用户点击回退不会回到该页面。
        replaceHashPath(encodedPath);
      }

      const prevIndex = allPaths.indexOf(createPath(history.location));

      if (prevIndex !== -1) allPaths[prevIndex] = path;

      setState({ action, location });
    }
  );
}
```
重复部分不再说明，我们重点关注下`()=>...`部分，这部分代码跟`push`也差不太多，这里是直接将路径换掉了【在`window.location`和`allPaths`中】。

#### `block`
```js
function block(prompt = false) {
  const unblock = transitionManager.setPrompt(prompt);

  if (!isBlocked) {
    checkDOMListeners(1);
    isBlocked = true;
  }

  return () => {
    if (isBlocked) {
      isBlocked = false;
      checkDOMListeners(-1);
    }

    return unblock();
  };
}
```

#### `listen`

# 相关资料
- [react-router v5.2.0](https://github.com/ReactTraining/react-router/tree/v5.2.0)
- [history v4.9.0](https://github.com/ReactTraining/history/tree/v4.9.0)
- [path-to-regexp](https://github.com/pillarjs/path-to-regexp)
- [MDN-Location](https://developer.mozilla.org/zh-CN/docs/Web/API/Location)
- [MDN-window.confim](https://developer.mozilla.org/zh-CN/docs/Web/API/Window/confirm)