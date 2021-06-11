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
这种写法本质和上面一样的，在源码可以看到`CancelToken.source()`实际上就是对这种写法做了一个封装。

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
//107-111行
try {
    promise = dispatchRequest(newConfig);
} catch (error) {
    return Promise.reject(error);
}
```
进入`dispatchRequest`

### `dispatchRequest`
代码在`lib/core/dispatchRequest`文件。
```js
//...其他代码

var adapter = config.adapter || defaults.adapter;

return adapter(config).then(function onAdapterResolution(response) {
     //...
  }, function onAdapterRejection(reason) {
     //...
  });
```
这里的`adapter`【适配器】是取消请求的关键。

`adapters/` 下的模块是处理分派请求并在收到响应后处理返回的 `Promise` 的模块。

适配器官方提供了两种`xhr`和`http`，但是也支持开发自定义适配器。

默认的适配是`xhr`，从源码来说一下。

首先是这一行：`var adapter = config.adapter || defaults.adapter;`

`config.adapter`如果没有经过修改，那么默认其实就是`defaults.adapter`，我们来回顾下相关代码：
```js
//axios文件
var axios = createInstance(defaults);

//createInstance函数
function createInstance(defaultConfig) {
  var context = new Axios(defaultConfig);
  ...
}

//Axios类
function Axios(instanceConfig) {
  this.defaults = instanceConfig;
  ...
}

//Axios文件
Axios.prototype.request = function request(config) {
  ...
  config = mergeConfig(this.defaults, config);
  ...
}
```
我们看到这里`config`和`defaults`被合并了，如果没有设置`config.adapter`，那么会使用`defaults.adapter`

接下来去看一下`defaults`

### `defaults`
> 详见`lib/defaults.js`文件
```js
var defaults = {
  ...
  adapter: getDefaultAdapter(),
  ...
}
```

```js
function getDefaultAdapter() {
  var adapter;
  if (typeof XMLHttpRequest !== 'undefined') {
    // For browsers use XHR adapter
    adapter = require('./adapters/xhr');
  } else if (typeof process !== 'undefined' && Object.prototype.toString.call(process) === '[object process]') {
    // For node use HTTP adapter
    adapter = require('./adapters/http');
  }
  return adapter;
}
```
到这里我们看到`axios`在浏览器环境下使用的适配器是`xhr`，在`node`环境下使用的是`http`

我们项目的运行是浏览器，所以使用的适配器是`xhr`，让我们看一下这个适配器。

### `xhr`适配器
> 完整代码见`lib/adapters`文件，这里展示部分关键代码
```js
module.exports = function xhrAdapter(config) {
  return new Promise(function dispatchXhrRequest(resolve, reject) {
    ...省略代码

    var request = new XMLHttpRequest();

    ...省略代码

    if (config.cancelToken) {
      // Handle cancellation
      config.cancelToken.promise.then(function onCanceled(cancel) {
        if (!request) {
          return;
        }

        request.abort();
        reject(cancel);
        // Clean up request
        request = null;
      });
    }

    ...省略代码
  });
};
```
可以看见，当我们传了`cancelToken`的时候，`xhr`适配器会给`config.cancelToken.promise`加一个成功状态的回调。

我们可以自由选择时机在外部调用`cancel()`，看一下状态变化：
```js
cancel() 
=> resolvePromise调用 
=> token.promise状态变为成功态 
=> onCanceled调用 
=> request.abort调用【对应请求取消】
```
至此，我们已经了解`axios`的手动取消机制了。

# 项目中遇到的问题