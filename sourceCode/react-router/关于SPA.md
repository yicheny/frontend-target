[TOC]

`single-page application`（single-page application）单页面应用就是一个网站只有一个页面，切换url实质上是切换组件。

这种做法的好处是我们不需要一次性将所有页面都进行加载，节省了内存和提升了速度。

另一方面，切换页面时，有些组件时可以复用的，可以只切换需要替换的组件，同样也提升了渲染的效率。

页面渲染效果是这样的：
![示意图](https://blog.pusher.com/wp-content/uploads/2017/12/getting-started-react-router-v4-app-preview-2.gif)

想要实现这种效果，需要一套路由系统，这个路由主要做的事情就是：
1. 监听`url`变化
2. 根据`url`变化匹配到对应的`Route`，渲染对应`Route`上的组件

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

除此之外，这里还有`HistoryContext`，作用只有一个：提供`history`信息。

`History`在整个`JSX`结构中有且只有一个，只用于提供`history`，而`RouterContext`并不只有`Router`会提供，每个`Route`也会提供`RouterContext`，主要是因为每个上下文提供的`match`是不同的，`match`为什么会不同？下面会进行描述。

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
- `length` 其实就是`window.history.length`，历史堆栈中的条目数
- `action` 当前导航操作
- `location` 当前位置

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

这里我以一个官方测试案例演示用法：
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

源码：
```js
function createHref(location) {
    return '#' + encodePath(basename + createPath(location));
}
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
1. 设置`action`为`PUSH`
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
        //通过history的公开接口push/replace更改路径，会主动设置相应的action【PUSH/REPLACE】
        //无论任何原因导致路径更改，都会触发hashChange事件，从而执行handleHashChange()方法
        //此时，如果不是通过`history`公开接口修改的路径，action都会被标志为`POP`【及其他一系列操作】
        //如果是`push/replace`设置的路径，则不需要这一系列处理
        //那么如何判定这个路径是通过`push/replace`修改的？
        //ignorePath是在push/replace中进行设置的，如果ignorePath存在且与当前路径一致，
        //那么就认为这个路径是通过push / replace修改的，不用再做处理
        ignorePath = path;
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

首先是`result`，它的来源是`prompt`，`prompt`的来源是`history.block(prompt)`，通过参数传入，稍微值得注意的是，`prompt`也可以是函数类型。

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
通过`block`可以注册一条提示消息，该消息将在通知`location listeners`之前向用户显示。这样可以确保用户在确认之前不能直接离开页面。
- 参数：`prompt`
  - `string`类型，提示文案
  - `function`类型，返回结果是提示文案，可执行一些副作用
- 返回值：`function`类型，一个用于清理的方法

用法示例：
```js
//冻结
const unblock = history.block('您确定要离开此页面吗？');

//或者可以这么冻结
history.block((location, action) => {
  if (input.value !== '') return '您确定要离开此页面吗？';
});

unblock();
```

相关源码：
```js
let isBlocked = false;

function block(prompt = false) {
    //设置transitionManager的内部变量prompt，并返回一个类似prompt=null的清理方法
    const unblock = transitionManager.setPrompt(prompt);

    if (!isBlocked) {
        checkDOMListeners(1);
        isBlocked = true;
    }

    //用于清理的方法
    return () => {
        if (isBlocked) {
            isBlocked = false;
            checkDOMListeners(-1);
        }

        return unblock();
    };
}
```
关键思路描述：
- 设置`prompt`
- 看是否已经有页面被冻结了【以保证只能冻结一个页面】  
  - 如果是则执行`checkDOMListeners`【用于监听`hashChange`事件】

##### `checkDOMListeners`
简单说一下`checkDOMListeners`，执行这个方法，是用于添加`hashChange`事件监听，无论有多少需要监听的地方【`block`和`listen`都会执行这个方法】，这个监听只需要添加一次。【在第一次的时候进行添加】

而如果所有与监听相关的方法都被清理了，当`listenerCount`变为0的时候，移除`hashChange`的监听。

`checkDOMListeners`相关源码：
```js
function checkDOMListeners(delta) {
  listenerCount += delta;

  if (listenerCount === 1 && delta === 1) {
    window.addEventListener(HashChangeEvent, handleHashChange);
  } else if (listenerCount === 0) {
    window.removeEventListener(HashChangeEvent, handleHashChange);
  }
}
```

##### `handleHashChange`
`history`的`action`分为三种：`PUSH`、`REPLACE`、`POP`，我们知道通过`history.push`修改`location`动作是`PUSH`、通过`history.replace`修改`location`动作是`REPLACE`，那么什么情况下修改`location`动作会是`POP`呢？

答案是除了`history.push`和`history.replace`之外的其他任何方式导致路由修改，都会将其记录成`POP`。

相关源码：
```js
function handleHashChange() {
    const path = getHashPath();
    const encodedPath = encodePath(path);

    if (path !== encodedPath) {
        // 做其他任何事之前先保证hash路径的正确
        // replaceHashPath之后会再触发hashChange事件
        replaceHashPath(encodedPath);
    } else {
        const location = getDOMLocation();
        const prevLocation = history.location;

        //仅当confirmTransitionTo转换不通过，执行恢复页面操作
        //如果想要返回的location在当前location的历史堆栈之前，则执行返回操作，此时会将其设置为true
        //forceNextPop为true仅当action为POP时会执行一次setState()
        //locationsAreEqual检查上一次位置和当前位置是否相同【pathname、search、hash、key、state】
        if (!forceNextPop && locationsAreEqual(prevLocation, location)) return; // A hashchange doesn't always == location change.

        //说明当前路径是通过push/replace设置的，不用再往下走
        if (ignorePath === createPath(location)) return; // Ignore this change; we already setState in push/replace.

        ignorePath = null;

        handlePop(location);
    }
}

function handlePop(location) {
    if (forceNextPop) {
        forceNextPop = false;
        setState();
    } else {
        const action = 'POP';

        transitionManager.confirmTransitionTo(
            location,
            action,
            getUserConfirmation,
            ok => {
                if (ok) {//通过，则直接转换
                    setState({ action, location });
                } else {//未通过，则恢复
                    revertPop(location);
                }
            }
        );
    }
}

function revertPop(fromLocation) {
    const toLocation = history.location;

    //想要跳转的location【history.location】的index
    let toIndex = allPaths.lastIndexOf(createPath(toLocation));

    if (toIndex === -1) toIndex = 0;

    //源头是getHashPath()，这是导航当前的location
    let fromIndex = allPaths.lastIndexOf(createPath(fromLocation));

    if (fromIndex === -1) fromIndex = 0;

    const delta = toIndex - fromIndex;

    //如果想要返回的location在当前location的历史堆栈之前，则执行返回操作
    if (delta) {
        forceNextPop = true;
        go(delta);
    }
}
```

#### `listen`
相关源码：
```js
function listen(listener) {
  const unlisten = transitionManager.appendListener(listener);
  checkDOMListeners(1);

  return () => {
    checkDOMListeners(-1);
    unlisten();
  };
}
```
思路描述：
1. 通过`transitionManager.appendListener(listener)`添加监听回调
2. 执行`checkDOMListeners`【详见上文描述】
3. 返回一个清理方法

这里其实是一个发布-订阅模式，每一个监听器`listener`都是订阅者，通过`transitionManager.appendListener(listener);`添加订阅

而每次`setState`都会触发发布，通过`transitionManager.notifyListeners(history.location, history.action);`进行发布

什么时候会执行`setState`？执行`PUSH`、`REPLACE`、`POP`操作时，可以简单认为当前位置改变时会通知所有监听者。

# 更多用法
## `hisotry`
`history`提供了三种方法来创建`history`：
- `createBrowerHistorty` 用于支持 `HTML5 history API` 的现代 `Web` 浏览器
- `createMemoryHistory` 用于参考实现，也可用于非 `DOM` 环境，如 `React Native` 或测试
- `createHashHistory` 用于旧版网络浏览器
 
# 相关资料
- [react-router v5.2.0](https://github.com/ReactTraining/react-router/tree/v5.2.0)
- [history v4.9.0](https://github.com/ReactTraining/history/tree/v4.9.0)
- [path-to-regexp](https://github.com/pillarjs/path-to-regexp)
- [MDN-Location](https://developer.mozilla.org/zh-CN/docs/Web/API/Location)
- [MDN-window.confim](https://developer.mozilla.org/zh-CN/docs/Web/API/Window/confirm)