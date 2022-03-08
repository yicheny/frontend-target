[TOC]

# 问题

1. 新项目想要使用`V6`版本的`react-router-dom`
2. 公共组件库`rootnet-ui`使用的是`V5`版本的`react-router-dom`
3. V5和V6不兼容

# 解决方案

1. 首先保证项目所需的`react-router-dom`正常下载
2. 如果项目下载的版本与组件库版本所需相同，则共用`react-router-dom`
3. 如果项目下载的版本和组件库版本所需不同，则主项目的`node_modules`下载一份，组件库的`node_modules`下载一份

# 行动

这是修改前的`rootnet-ui`的`package`，有两个问题

```json
{
  "peerDependencies": {
    "react-router-dom": ">=5.0.0"
  },
  "devDependencies": {
    "react-router-dom": "^5.0.0"
  }
}
```

首先是`react-router-dom`不应该放到`devDependecies`依赖下，调整如下：

```json
{
  "dependencies": {
    "react-router-dom": "^5.0.0"
  }
}
```

其次`peerDependencies`限制区间有问题，大于等于`5.0`都是适配区间，所以`6.0.0`之后的版本也会认为是适配的包，那么此时它会使用公共的`6.0.0`的包，这个时候就会出错。

修改后如下：
```json
{
  "peerDependencies": {
    "react-router-dom": "^5.0.0"
  }
}
```

进行如上修改后，一般会有两种情况。

情况1，以`otcd`这种旧有项目为例，使用的`5.0`版本的`react-router-dom`，所以组件库不会进行单独下载：

![](https://pic.imgdb.cn/item/6226fd185baa1a80ab2169e8.jpg)

情况2，`r-demo`是我创建的新项目，使用了`6.0`版本的`react-router-dom`，这个时候组件库发现依赖包不匹配，会单独下载一份。

![](https://pic.imgdb.cn/item/6226fd825baa1a80ab21ae1d.jpg)

# 新的问题
解决依赖库版本的下载之后，出现了一个新的报错：

![](https://pic.imgdb.cn/item/6226fdda5baa1a80ab21f043.jpg)

在组件库的`Menu`和`Button`组件中使用`withRouter`。

`withRouter`是`5.0`版本的API，必须搭配该版本的`Router`组件使用，然后主项目使用的是`6.0`的`Router`，所以出现了这个报错。

# 行动
## 修改组件库相关组件
```js
// const _Menu = withRouter(Menu);
function _Menu(props){
    return <Menu history={useHistory()} {...props}/>
}
```

## 自定义`useHistory`
> 如果是适配版本的`react-router-dom`则不需要做任何处理
 
使用`6.0`及以上版本的，需要做一些处理以适配`history`接口，不过因为`6.0`版本没有`useHistory`，所以我们使用`6.0`提供的方法自定义一个符合`history`接口的`hook`

```js
function useHistory(){
    const location = useLocation();
    const navigate = useNavigate();

    const listen = useCallback((callback)=>{
        callback && callback(location);
    },[location])

    return {listen,push:navigate,location}
}
```