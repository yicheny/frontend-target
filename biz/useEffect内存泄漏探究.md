[TOC]

# 基础知识
## Axios取消请求的两种方式
### 1. 使用`CancelToken.source`
```js
const source = axios.CancelToken.source();

axios.get('/user/12345', {
  cancelToken: source.token
});

source.cancel('Operation canceled by the user.');
```
1. 通过`axios.CancelToken.source()`生成`source`实例
2. 请求接口时将`source.token`作为`cancelToken`传递
3. 调用`source.cancel`可以将相关请求取消

### 2. 使用 `executor` 函数
```js
const CancelToken = axios.CancelToken;
let cancel;

axios.get('/user/12345', {
  cancelToken: new CancelToken(function executor(c) {
    cancel = c;
  })
});

cancel();
```
1. 当`cancelToken`是`CancelToken`实例时，会将对应的取消方法作为回调的参数传递
2. 将取消方法传递给外部变量`cancel`
3. 调用`cancel`，取消对应接口请求

## `useEffect`清除副作用
常见做法是类似这样的，可能出现的情况较多，这里我举一个例子，理解含义即可：
```js
useEffect(()=>{
    const timeId = setInterval(executor,1000);//包括定时器、请求、事件监听、视窗监听等...
    return ()=>clearInterval(timeId); //卸载时清除，避免内存泄漏
},[])
```

# 深入原理
## `CancelToken相关`
> 注：源码看的是`0.21.1`版本

### `axios.CancelToken.source()`
因为我们项目里用的就是这种方式，所以我想先了解`axios.CancelToken.source()`相关的原理。 

直接上源码：
```js
CancelToken.source = function source() {
  var cancel;
  var token = new CancelToken(function executor(c) {
    cancel = c;
  });
  return {
    token: token,
    cancel: cancel
  };
};
```
代码很简单，看到我们发现这里就是对`new CancelToken`的使用做了一个简单封装。

下面我们看一下`new CancelToken`会得到什么。

### `CancelToken`
> `CancelToken`是用于取消请求的对象。
```js
function CancelToken(executor) {
  if (typeof executor !== 'function') {
    throw new TypeError('executor must be a function.');
  }

  var resolvePromise;
  this.promise = new Promise(function promiseExecutor(resolve) {
    resolvePromise = resolve;
  });

  var token = this;
  executor(function cancel(message) {
    if (token.reason) {
      // Cancellation has already been requested
      return;
    }

    token.reason = new Cancel(message);
    resolvePromise(token.reason);
  });
}
```
`new CancelToken`会得到一个实例`token`，实例有一个属性`promise`。实例化时会传递一个`cancel`回调给`executor`，这个就是我们在外部接收到的。

这里使用了一个闭包，`resolvePromise`执行，`token.promise`状态改变，`resolvePromise`的参数是`token.reason`【`Cancel`实例】

稍微看一下`Cancel`。

### `Cancel`
> `Cancel`是在取消操作时抛出的对象。
```js
function Cancel(message) {
  this.message = message;
}

Cancel.prototype.toString = function toString() {
  return 'Cancel' + (this.message ? ': ' + this.message : '');
};

Cancel.prototype.__CANCEL__ = true;
```

## `axios`实例相关
看下源码中`axios`实例的创建：
```js
var axios = createInstance(defaults);
```

继续深入：
```js
function createInstance(defaultConfig) {
  var context = new Axios(defaultConfig);
  var instance = bind(Axios.prototype.request, context);

  // Copy axios.prototype to instance
  utils.extend(instance, Axios.prototype, context);

  // Copy context to instance
  utils.extend(instance, context);

  return instance;
}
```
这里比较清晰，没什么可说的，之后会回来说一下这里的`defaultConfig`

### 实现`get`等方法
代码位于`lib/core/Axios`文件的`125-135`行。
```js
// Provide aliases for supported request methods
utils.forEach(['delete', 'get', 'head', 'options'], function forEachMethodNoData(method) {
  /*eslint func-names:0*/
  Axios.prototype[method] = function(url, config) {
    return this.request(mergeConfig(config || {}, {
      method: method,
      url: url,
      data: (config || {}).data
    }));
  };
});
```
所以实际我们执行`axios.get()`等同于执行`axios.request({method:'get',...})`

### `Axios.prototype.request`
代码位于`lib/core/Axios`文件的`24-118`行，代码较多，我就不展示了。
```js

```

# 项目中遇到的问题