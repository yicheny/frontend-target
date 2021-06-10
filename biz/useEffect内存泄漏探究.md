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
## `Axios`
> 注：源码看的是`0.21.1`版本

### 1. `axios.CancelToken.source()`
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

接下来我们看一下`token`是怎么在`axios`的请求中使用的。

# 项目中遇到的问题