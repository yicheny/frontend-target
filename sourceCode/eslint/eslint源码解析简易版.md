[TOC]

# 场景
附带完整源码的解析有着太多细节，而且文档我还也没整理顺序，阅读起来相对困难。

简易版做了一些整理，而且展示的代码为了方便理解也做了处理：
1. 精简代码，删减掉不重要的代码
1. 只是阐述思路，以伪代码或无代码表现

预期是阅读此文档后至少对`eslint`的流程有个大致思路。

# 阅读能力要求
1. 掌握`eslint`使用【主要是配置这一块】
1. `javascript`基本语法

## `eslint`基本配置
> `eslint`配置源有两种：一个是文件中代码注释进行配置，一个是通过文件进行配置，这里只以文件配置进行说明，另外支持的配置文件也有很多类型，这里只以`eslintrc.js`进行说明

如果对`eslint`配置不熟悉或者没有使用过的人，可以看一下这部分，了解下`eslint`的常用配置项，这里大概说一下每个配置项的作用，但是不深入细节。

如果对`eslint`非常熟悉的人可以跳过这部分。

如果想要有更深入的了解，请看[`eslint配置指南`](https://eslint.bootcss.com/docs/user-guide/configuring)

```js
//.eslintrc.js
module.exports = {
    parser:`esprima`,//解析器，目前支持：babel-eslint、@typescripts-eslint/parser、esprima、espree【官方默认】
    plugins:['a-plugin'],//插件，提供processor、rule、configs【通过extends使用】等...
    processor:"a-pulgin/markdown-processor",//处理器：用于从另一种文件中提取代码，然后让`eslint`检测
    overrides:[//覆盖配置
        {
            "files":["*.md"],
            ...
        }
    ],
    env:{//一个环境定义了一组预定义的全局变量，可用环境见配置文档
        browser:true
    },
    globals:{//定义全局变量
        var1:"writable"
    },
    rules:{//开启规则，决定错误级别
        eqeqeq:true,
    }
    settings:{//共享设置，将被提供给每一个被执行的规则
        "sharedData":"hello"
    },
    root:true,//与eslint配置文件的查找相关，有些复杂，之后专门说一下
    extends："eslint:recommended",//继承文件规则（配置文件的路径、可共享配置的名称、字符串数组），这里也在下面单独说一下
}
```

## 配置文件查找规则
### 场景1
首先从最简单的场景说起：
```
your-project
├─┬ lib
│ ├── source.js
│ └── .eslintrc
```
这种情况下`lib/`下的文件会使用`lib/.eslintrc`作为配置文件使用。

### 场景2
现在将`.eslintrc`换个位置，以便可以让更多目录使用
```
your-project
├── .eslintrc
├─┬ lib
│ └── source.js
└─┬ tests
  └── test.js
```
如果是这种目录结构，`lib/`和`tests`目录下的文件都会使用`your-project/.eslintrc`作为配置文件。

`eslint`遍历文件时，会优先使用距离文件最近的`.eslintrc`文件，如果当前目录下没有，会向外查找，直到根目录为止。【其实无论有没有都会向外查找，现在假设没有】

### 场景3
如果`tests/`目录下有一些独立的配置，但是希望复用`your-project/.eslintrc`的配置，那么可以在其目录下再添加一个`.eslintrc`
```
your-project
├── .eslintrc
├─┬ lib
│ └── source.js
└─┬ tests
  ├── test.js
  └── .eslintrc
```
这个时候如果`your-project/.eslintrc`和`tests/.eslintrc`规则冲突，则优先使用`tests/.eslintrc`规则【或者你也可以认为是当前目录规则覆盖了外层目录规则】

### 场景4
在原来的基础上添加`fixs`目录，这个目录的文件只想使用`fixs/.eslintrc`进行检查，不想让`your-project/.eslintrc`的配置生效
```
your-project
├── .eslintrc
├─┬ lib
│ └── source.js
├─┬ tests
│ ├── test.js
│ └── .eslintrc
└─┬ fixs
  ├── fix.js
  └── .eslintrc
```
此时可以在`fixs/.eslintrc`的配置文件中添加`root:true`这一项规则，这样`eslint`会停止向外层查找

## 关于`Plugin`
`Plugin`是`eslint`配置中重要一部分，它涉及`processor`、`rules`、`plugin`、`extends`这几个地方，我们知道`eslintrc.*`是级联配置，但是它并不是最终配置，`eslint`里会将级联配置打平使用。

这里这里涉及到很重要的点，就是我们的`eslintrc.*`并不是最终配置，`eslint`加载`plugin`里的内容并继承，事实上`plugin`是使得配置复杂的一个重要组成部分，`eslint`通过扩展、覆盖、级联合并等方式调整配置，以致于用户很难了解最终输出的配置。——正是因为这一点，官方提供了`--print-config`方法【这是官方核心成员`Nicholas C. Zakas`在`Issue`评论里回复的】

这里稍微介绍下`Nicholas C. Zakas`这个人，看源码和`Issues`的时候发现这个人对`eslint`影响深远，而且依旧在主导着`eslint`未来的发展，这个之后也会很多次提到：
1. 学习过`javascript`的人对他应该很熟悉，他是《`Javascript`高级程序设计》（俗称红宝书）前三版的作者，第四版因为他个人身体原因换作者了。
1. 除此以外，他是`Eslint`项目最初的作者，也是现在的核心开发之一。
1. 另外，`espree`【兼容`esprima`的解析器】也是他写的，目前`eslint`默认支持的解析器就是`espree`。
1. 然后`eslint/eslintrc`这个项目他也是核心开发者，并且是提交量最多的人，目前官方配置解析库使用的就是这个。

当前`eslintrc.*`方案影响配置的点有很多，比如说上面的`eslintrc.*`的位置，`root`、`extends`、`overrides`等等这些配置项。

`plugins`在当下方案【级联配置数组】非常重要，因为它不仅涉及`plugins`键，还影响到`processor`、`extends`、`rules`这些键，具体实现先不提，我们这里先了解`plugin`是什么，有哪些内容，怎么使用。

顺便一提，官方计划推行一种新的配置方案，现有方案会被完全放弃，新方案下`extends`、`overrides`、`.eslintignore`、`plugins`这些不在使用，目前计划进度见[`Implement Flat Config #13481`](https://github.com/eslint/eslint/issues/13481)。

> 关于插件的详细说明请见[插件指南](https://eslint.bootcss.com/docs/developer-guide/working-with-plugins)

首先明确一个概念：`eslint`并没有`plugin API`（这个也是Nicholas C. Zakas说的），我们拥有的是一个插件格式，`Eslint`一直将插件视为命名事物的集合，它提供`rule`、`processor`、`config`这些内容给`Eslint`进行使用，**插件本身什么也不会做，插件只是提供配置，由`Eslint`获取配置进行处理**

现在我们看一下`plugin`格式可以提供哪些内容。

## 关于`extends`

## 关于`.eslintignore`
只会使用**当前目录**下的`.eslintignore`文件

# 问题
在开始之前我会提几个问题，阅读源码的时候带着这些问题去思考，最终问题解答我放到了“解答”这个部分，可以直接在这部分看解答。

1. `eslint`是怎么将`eslintrc.*`作为默认配置的？
1. `extends`是怎么生效的？
1. `overrides`是怎么生效的？
1. 关于`eslint.*`之外的另一种配置方案是怎么回事？

# 入口
```json5
//package.json
{
    "bin": {
        "eslint": "./bin/eslint.js"
    },
}
```

入口是`bin/eslint.js`文件

![](https://pic.imgdb.cn/item/613719e144eaada7398a9c46.jpg)

# `bin/eslint.js`
```js
(async function main() {
    if (process.argv.includes("--init")) {
        await require("../lib/init/config-initializer").initializeConfig();
        return;
    }

    process.exitCode = await require("../lib/cli").execute(...);
}()).catch(onFatalError);
```

可以发现，核心内容在`../lib/cli`文件里：

![](https://pic.imgdb.cn/item/61371a3344eaada7398b36a6.jpg)

# `lib/cli.js`
```js
async execute(args, text) {
    //将args解析成对象形式并赋值给变量options

    //检查一些命令行参数并退出

    //!lint核心处理

    //quite,printResult并退出

    return 2;
}
```

流程示意图：

![](https://pic.imgdb.cn/item/6138268644eaada73959cb1f.jpg)

## `lint`核心处理
```js
const engine = new ESLint(translateOptions(options));//初始化ESLint
let results;

if (useStdin) {//使用--stdin会走这里，先不关注
    ...
} else {
    results = await engine.lintFiles(files);
}

if (options.fix) {
    await ESLint.outputFixes(results); //将修复后的结果写入文件
}
```

![](https://pic.imgdb.cn/item/6139828a44eaada7395e519d.jpg)

这部分代码里有三个点我认为较为重要，我们接下来解读这三个点：
1. 初始化`Eslint`
2. 调用`engine.lintText`
3. 调用`engine.lintFiles`

# `eslint/eslint.js`
这里先给出`constructor`部分的代码：
```js
class ESLint {
    constructor(options = {}) {
        //验证和规范化包装的 CLIEngine 实例的选项。
        const processedOptions = processOptions(options);

        //初始化cliEngine
         const cliEngine = new CLIEngine(processedOptions);

        // 添加到插件池
        if (options.plugins) {...}

        // 覆盖配置
        if (hasDefinedProperty(options.overrideConfig)) {...}

        // 更新缓存
        if (updated) {...}

        // 初始化私有属性
        privateMembersMap.set(this, {...});
    }
}
```

![](https://pic.imgdb.cn/item/613984e944eaada73961c9b1.jpg)

接下来进入`CLIEngine`的`constructor`，看看它在初始化的时候做了什么。

# `cli-engine/cli-engine.js`
```js
constructor(providedOptions) {
    // 整合options
    const options = Object.assign(...)

    // 创建一系列数据
    const additionalPluginPool = new Map();
    const cacheFilePath = getCacheFile(...);
    const configArrayFactory = new CascadingConfigArrayFactory({...});
    const fileEnumerator = new FileEnumerator({...});
    const lintResultCache = ...;
    const linter = new Linter({ cwd: options.cwd });
    const lastConfigArrays = [configArrayFactory.getConfigArrayForFile()];

    // 存储私有数据
    internalSlotsMap.set(this, {...});

    // 为fixes设置特殊过滤器
    if (options.fix && options.fixTypes && options.fixTypes.length > 0) {...}
}
```

![](https://pic.imgdb.cn/item/6139851144eaada7396218f9.jpg)

这里创建的私有数据先不深入，在接下来的使用到这些数据的时候，我们再进行解读。

现在我们对`new Eslint`已经有了一个基本的了解，接下来我们去了解下`engine.lintFiles(files)`的流程及其实现：

![](https://pic.imgdb.cn/item/6139832244eaada7395f19ea.jpg)

## `engine.lintFiles(files)`
```js
async lintFiles(patterns) {
    //...
    return processCLIEngineLintReport(cliEngine,cliEngine.executeOnFiles(patterns));
}
```
关键在`cliEngine.executeOnFiles`

## `cliEngine.executeOnFiles`
```js
executeOnFiles(patterns) {
    //取出数据
    const {...} = internalSlotsMap.get(this);

    // 清除上次使用的配置数组
    lastConfigArrays.length = 0;

    //删除缓存文件
    if (!cache) {...}

    // 迭代源代码文件
    // ...这段代码比较重要，代码量也较大，为方便阅读和理解，我放到了单独的子章节内

    // 将缓存持久化到磁盘
    if (lintResultCache) lintResultCache.reconcile();

    return {...};
}
```

流程图：

![](https://pic.imgdb.cn/item/6139d62844eaada7390532f2.jpg)

有两个地方我比较关心，一个是迭代源代码文件进行的处理，一个是返回结果，首先从迭代源代码文件部分开始看吧

## 迭代源代码文件
```js
for (const { config, filePath, ignored } of fileEnumerator.iterateFiles(patterns)) {
    //是否忽略
    if (ignored) {
        results.push(createIgnoreResult(filePath, cwd));
        continue;
    }

    //将不包含的config放到lasterConfigArrays
    if (!lastConfigArrays.includes(config)) lastConfigArrays.push(config);

    //取出缓存并添加到results
    if (lintResultCache) {...}

    // Do lint.
    const result = verifyText({...});

    //将lint结果添加至results
    results.push(result);

    //将lint结果缓存
    if (lintResultCache) lintResultCache.setCachedLintResults(filePath, config, result);
}
```

![](https://pic.imgdb.cn/item/613ace8344eaada739684d9d.jpg)

这里值得关注的是`do lint`部分，调用了`verifyText`方法，我们看一下。

### `verifyText`
```js
function verifyText({...}) {
    //处理path
    const filePath = providedFilePath || "<text>";
    const filePathToVerify = filePath === "<text>" ? path.join(cwd, filePath) : filePath;//保证是绝对路径

    const { fixed, messages, output } = linter.verifyAndFix(...);

    // 调整结果
    const result = {...};
    if (fixed) result.output = output;
    if (/*如果有错误且无输出*/) result.source = text;

    return result;
}
```

![](https://pic.imgdb.cn/item/613ecafd44eaada739b8e077.jpg)

接下来我们看一下`linter.verifyAndFix`这个方法

# `linter/linter.js`
## `linter.verifyAndFix`
```js
verifyAndFix(text, config, options) {
    //循环修复文件
    do{...}while{...}

    // 如果文件已修复，则生成LintMessage[]信息并赋值到message属性上【这里最重要的作用还是保证信息是最新的】
    if (fixedResult.fixed) fixedResult.messages = this.verify(currentText, config, options);

    // 更新结果
    fixedResult.fixed = fixed;
    fixedResult.output = currentText;

    return fixedResult;
}
```
这里比较重要的是循环修复和生成`LintMessage[]`信息这两个部分。

我们先看一下循环修复文件的代码

### 循环修复文件
```js
do {
    passNumber++;//通过次数，最大10次，超过即使有未修复内容也不再处理

    //校验并生成LintMessage[]
    messages = this.verify(currentText, config, options);

    //修复错误【根据shouldFix】
    fixedResult = SourceCodeFixer.applyFixes(currentText, messages, shouldFix);

    //检查到有语法错误就break
    if (messages.length === 1 && messages[0].fatal) break;

    //更新fixed
    fixed = fixed || fixedResult.fixed;

    //更新currentText
    currentText = fixedResult.output;
} while (
    已修复 && 通过次数小于最大值【10】
);
```
这里思路也不是很复杂，也有两个地方我想看一下：`this.verify`、`SourceCoceFixer.applyFixes`

我们先看`verify`方法.

## `linter.verify`
```js
verify(textOrSourceCode, config, filenameOrOptions) {
    //处理options
    const options = typeof filenameOrOptions === "string"
        ? { filename: filenameOrOptions }
        : filenameOrOptions || {};

    //如果config类型是`ConfigArray`
    if (config && typeof config.extractConfig === "function") {
        return this._verifyWithConfigArray(textOrSourceCode, config, options);
    }

    //如果有Processor
    if (options.preprocess || options.postprocess) {
        return this._verifyWithProcessor(textOrSourceCode, config, options);
    }

    return this._verifyWithoutProcessors(textOrSourceCode, config, options);
}
```
这里三个方法都挺重要的，我们先看看`this._verifyWithoutProcessors`方法，这个方法非常非常重要。

## `_verifyWithoutProcessors`
```js
_verifyWithoutProcessors(textOrSourceCode, providedConfig, providedOptions) {
    //获取一些需要使用的数据
    const slots = internalSlotsMap.get(this);
    const config = providedConfig || {};
    const options = normalizeVerifyOptions(providedOptions, config);//规范和验证options

    // 设置slots.lastSourceCode、text
    if (typeof textOrSourceCode === "string") { ... } else { ... }

    // 初始化解析器【默认】
    let parserName = DEFAULT_PARSER_NAME;
    let parser = espree;

    // 更换解析器
    if (typeof config.parser === "object" && config.parser !== null) {...}
    else if (typeof config.parser === "string") {...}

    // 搜索并应用"eslint-env *".
    ...

    // 设置slots.lastSourceCode【这一步会解析源码】
    ...
    
    // 将声明的变量添加到全局作用域
    addDeclaredGlobals(...);

    lintingProblems = runRules(...);//其实这里有try-catch包裹

    return applyDisableDirectives({...});//应用禁用指令
}
```
这个方法做了很多事情，而且都挺重要，我们会依次了解这些内容，首先从“设置`slots.lastSourceCode`”这部分内容开始

### 设置`slots.lastSourceCode`
```js
//如果没有slots.lastSourceCode
if (!slots.lastSourceCode) {
    //使用parse解析【这里是AST转换】
    const parseResult = parse(
        text,
        parser,
        parserOptions,
        options.filename
    );

    //这说明转换失败
    if (!parseResult.success) {
        return [parseResult.error];
    }

    //赋值slots.lastSourceCode
    slots.lastSourceCode = parseResult.sourceCode;
} else {
    //如果有slots.lastSourceCode，但是没有slots.lastSourceCode.scopeManager
    if (!slots.lastSourceCode.scopeManager) {
        //使用new SourceCode生成slots.lastSourceCode
        slots.lastSourceCode = new SourceCode({...});
    }
}
```
可以看到有两个点值得关注：`parse`、`SourceCode`，我们先看`parse`

### `parse`
```js
function parse(text, parser, providedParserOptions, filePath) {
    //1. 去除BOM; 2. 注释Shebang
    const textToParse = stripUnicodeBOM(text).replace(astUtils.shebangPattern, (match, captured) => `//${captured}`);

    const parserOptions = Object.assign({}, providedParserOptions, {...});

    try {
        //这部分是SourceCode初始化需要用到的参数
        const parseResult = (typeof parser.parseForESLint === "function")
            ? parser.parseForESLint(textToParse, parserOptions)
            : { ast: parser.parse(textToParse, parserOptions) };
        const ast = parseResult.ast;
        const parserServices = parseResult.services || {};
        const visitorKeys = parseResult.visitorKeys || evk.KEYS;
        const scopeManager = parseResult.scopeManager || analyzeScope(ast, parserOptions, visitorKeys);

        return {
            success: true,
            sourceCode: new SourceCode({...})
        };
    } catch (ex) {
        //如果出现解析错误，不会直接抛错，而是作为一个错误信息返回
        return {
            success: false,
            error: {...}
        };
    }
}
```
可以看到，即使是通过`parse`方法，`slots.lastSourceCode`得到的依旧是`SourceCode`实例。

### `runRules`
```js
function runRules(sourceCode, configuredRules, ruleMapper, parserOptions, parserName, settings, filename, disableFixes, cwd, physicalFilename) {
    const emitter = createEmitter();
    const nodeQueue = [];
    let currentNode = sourceCode.ast;

    Traverser.traverse(sourceCode.ast, {...});

    //规则上下文都继承这个对象，相比复制属性性能更好
    const sharedTraversalContext = Object.freeze(Object.assign(Object.create(BASE_TRAVERSAL_CONTEXT),{...}));

    const lintingProblems = [];

    Object.keys(configuredRules).forEach(...));

    //如果顶级节点是"Program"，则仅运行代码路径分析器，否则跳过
    const eventGenerator = nodeQueue[0].node.type === "Program"? new CodePathAnalyzer(...) : new NodeEventGenerator(...);

    nodeQueue.forEach(...);

    return lintingProblems;
}
```

# 解答
## 1. `eslint`是怎么默认读取`eslintrc.*`文件配置的？
涉及引用库：
1. `optionator` 是`js`选项解析和帮助生成库 
1. `@eslint/eslintrc` 此存储库包含 `ESLint` 的旧 `ESLintRC` 配置文件格式
1. `import-fresh` 当需要全新导入模块时，可用于测试目的

核心流程相关：
1. 命令入口文件是`bin/eslint.js`
1. `eslint.js`正常流程走的是`cli/cli.js`的`cli.execute`方法（不是初始化，就走这条流程）
1. 在`cli.execute`中，有两个处理`options`的部分，一个是`CLIOptions.parse`，一个是`translateOptions`
    1. `CLIOptions.parse`底层使用的是`optionator`库，其配置中默认`eslintrc`是`true`
    2. `translateOptions`返回值中`useEslintrc`就是`eslintrc`
1. 在`cli.execute`中初始化`Eslint`
    1. 在`Eslint`中初始化`CliEngine`
    1. 在`CliEngine`中初始化`CascadingConfigArrayFactory`
1. 执行`engine.lintFile`【这部分有很多分支，然是最终都会是`configArrayFactory`提供的`API`】
    1. 走`cliEngine.executeOnFiles`
    1. 走`fileEnumerator.iterateFiles`
    1. 走`fileEnumerator._iterateFiles`
    1. 走`fileEnumerator._iterateFilesWithFile`
    1. `const config = configArrayFactory.getConfigArrayForFile(filePath)`

现在说一下`CascadingConfigArrayFactory`方法，这个方法已经被抽取到单独的库`@eslint/eslintrc`中了，看`git`提交记录注释，这和`eslint`团队未来的计划有关，他们未来想要使用一种新的配置方式，目前计划正在逐渐进行中。

1. 进`CascadingConfigArrayFactory.getConfigArrayForFile`
1. 进`CascadingConfigArrayFactory._finalizeConfigArray`
1. 如果`useEslintrc`为`true`，则进`configArrayFactory.loadInDirectory`
1. 迭代`configFilenames`

进入这个变量：
```js
const configFilenames = [
    ".eslintrc.js",
    ".eslintrc.cjs",
    ".eslintrc.yaml",
    ".eslintrc.yml",
    ".eslintrc.json",
    ".eslintrc",
    "package.json"
];
```
可以发现这个正是`eslint`使用文档里所介绍的配置文件优先度可以对应上

然后就是迭代这个`configFilenames`
1. 得到`pathname`
1. `path.join(pathname)`得到`filePath`
1. 判断`filePath`是否存在且是否是文件类型
1. 调用`loadConfigFile`
1. 根据不同文件后缀调用不同`load`方法，调用`loadJSConfigFile`【假设是`.js`文件】
1. `importFresh(filePath)`

## 关于`eslint.*`之外的另一种配置方案是怎么回事？
[Config File Simplification](https://github.com/eslint/rfcs/pull/9)这是来自`eslint/rfcs`下的一个`RFC`（`Request for Comments` 请求意见）

该提议提供了一种通过新的配置文件格式来简化 `ESLint` 配置的方法。配置文件格式是用 `JavaScript` 编写的，并删除了几个现有的配置键，以支持用户手动创建它们。

提议该方案人是`Nicholas C. Zakas`，稍微介绍下这个人，学习过`javascript`的人对他应该很熟悉，他是《`Javascript`高级程序设计》（俗称红宝书）前三版的作者，第四版因为他个人身体原因换作者了。

除此以外，他是`Eslint`项目最初的作者，也是现在的核心开发之一。

另外，`espree`【兼容`esprima`的解析器】也是他写的，目前`eslint`默认支持的解析器就是`espree`。

然后`eslint/eslintrc`这个项目他也是核心开发者，并且是提交量最多的人，目前官方配置解析库使用的就是这个。

关于目前`eslintrc.*`方法存在的问题可以看这里[Config File Simplification](https://github.com/eslint/rfcs/pull/9)

新的配置库见[`@humanwhocodes/config-array`](https://github.com/humanwhocodes/config-array)

目前计划进度见[`Implement Flat Config #13481`](https://github.com/eslint/eslint/issues/13481)

这个计划目前依旧在实现中，等待后续发展。

# 资料
- [配置指南](https://eslint.bootcss.com/docs/user-guide/configuring)
- [插件指南](https://eslint.bootcss.com/docs/developer-guide/working-with-plugins)