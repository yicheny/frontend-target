[TOC]

# 手写
## bind、call、apply

## Promise

## 深、浅拷贝
浅拷贝实现方案很多，比如`{...o}`、`Object.assign({},o)`、`for循环`都可以

简单的深拷贝可以使用`JSON.stringify`、`JSON.parse`，不过这种方案有局限，只能对可序列化的数据进行拷贝，不能进行序列化的数据会丢失

```js

```

## 懒加载/缓存
```js
function createMemo(){
    //这里需要做的更完整一些，涉及缓存就需要考虑缓存大小
    //另外，从用户体验考虑，可以使用多种数据结构，以更合理的方式控制缓存
    //这里可以单独做成一个管理缓存的类，以便更多缓存方法使用
    //react就封装过缓存类
    const memoMap = new Map();
    //需要注意，缓存必须是纯函数
    return function memo(key){
        if(memoMap.get(key)) return memoMap.get(key);
        const value = calc(key);//根据key进行复杂运算
        memoMap.set(key,value);
        return value;
    }
}
```

# 执行机制

# 常用方法

# 异步

# es6

# 闭包

# 路由监听

# H5 History

# 小数点失真

# 柯里化、偏函数

# 防抖、节流

# Map、Set、WeakMap、WeakSet

# JS底层【？】

# this

# 箭头函数

# 浏览器存储

# 模块化：amd、umd、cmd

# 纯函数、副作用