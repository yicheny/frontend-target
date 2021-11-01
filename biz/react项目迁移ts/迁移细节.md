[TOC]

# 迁移流程
## `config-overrides.js`
```js
module.exports = function override(config, env) {
  const rules = config.module.rules[1].oneOf
  rules.push({
    test: /\.tsx?$/,
    loader: 'ts-loader',
  })

  return config
}
```

# 踩坑
## 报错：`Error: Cannot find module 'D:\Code-Git\otcd frontend/config-overrides'`
我翻了下源码，里面是这样的：
```js
var config_overrides = customPath
  ? `${ projectDir }/${ customPath }`
  : `${ projectDir }/config-overrides`;
```

我复制这里的名称然后对比发现是因为我的文件名开头多输入了一个`' '`，但是看不出来，重命名之后读取正常了。

## `TypeError: loaderContext.getOptions is not a function`
原因：`ts-loader 9` 不支持 `webpack 4`

解决：换一个版本 `npm install ts-loader@8.2.0`

## 报错：`Typescript emitted no output`
原写法：
```js
const rules = config.module.rules
rules.push({
    test: /\.tsx?$/,
    loader: 'ts-loader',
})
```

修改后：
```js
const rules = config.module.rules[1].oneOf
rules.push({
    test: /\.tsx?$/,
    loader: 'ts-loader',
})
```


# 参考资料
- [React项目从Javascript到Typescript的迁移经验总结](https://segmentfault.com/a/1190000019075274)
- [vue: 使用ts-loader引入ts文件](https://www.codeleading.com/article/24715588292/)