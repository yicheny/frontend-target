[TOC]

# 场景
附带完整源码的解析只关注核心执行流程，有着太多细节，而且没有调整章节顺序，阅读起来相对困难。

简易版做了一些整理，为了方便理解做了如下处理：
1. 精简代码，删减掉不重要的代码
1. 只是阐述思路，以伪代码或无代码表现
1. 添加了对`eslint`配置的说明
1. 添加了豆知识说明（Shebang、BOM、glob pattern等）
1. 添加了专题说明

预期是阅读此文档后至少对`eslint`的流程有个大致思路。

## 一些约定
为方便书写和理解，添加一些约定

1. 如果我在注释里以`//*`开头则表明这部分很重要，在下面有**深入描述**

# 阅读能力要求
1. `javascript`基本语法
1. 掌握`eslint`使用【主要是配置这一块】

如果对`eslint`配置不熟悉也没问题，我在下面写了配置的用法。

## `eslint`基本配置
> `eslint`配置源有两种：一个是文件中代码注释进行配置，一个是通过文件进行配置，这里只以文件配置进行说明，另外支持的配置文件也有很多类型，这里只以`eslintrc.js`进行说明

如果对`eslint`配置不熟悉或者没有使用过的人，可以看一下这部分，了解下`eslint`的常用配置项，这里大概说一下每个配置项的作用，但是不深入细节。

如果对`eslint`非常熟悉的人可以跳过这部分。

如果想要有更深入的了解，请看[`eslint配置指南`](https://eslint.bootcss.com/docs/user-guide/configuring)

```js
//.eslintrc.js
module.exports = {
    parserOptions:{ //指定支持的javascript语言选项
        ecmaVersion : 2020,//es版本，3、5、6、8、9、10或者2015、2016这种
        sourceType : 'module', //'script'或'module'，代码类型
        ecmaFeatures: {
            globalReturn : true, //允许在全局作用域下使用return
            impliedStrict : true, //开启全局严格模式，必须是es5版本及以上
            jsx : true, //开启jsx（注意，开启jsx不等于支持react，如果想要支持react语法官方建议使用rslint-plugin-react
            experimentalObjectRestSpread : true //支持解构语法
        }
    },
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
        eqeqeq:2,
    }
    settings:{//共享设置，将被提供给每一个被执行的规则
        "sharedData":"hello"
    },
    root: true,//与eslint配置文件的查找相关，有些复杂，之后专门说一下
    extends: "eslint:recommended",//继承文件规则（配置文件的路径、可共享配置的名称、字符串数组），这里也在下面单独说一下
    noInlineConfig: false, //是否禁用内联配置
}
```

## 配置文件查找规则
### 场景1
首先从最简单的场景说起：
```
your-project
├─┬ lib
│ ├── source.js
│ └── .eslintrc --> 生效
```
这种情况下`lib/`下的文件会使用`lib/.eslintrc`作为配置文件使用。

### 场景2
现在将`.eslintrc`换个位置，以便可以让更多目录使用
```
your-project
├── .eslintrc  --> 生效
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
├── .eslintrc  --> 生效
├─┬ lib
│ └── source.js
└─┬ tests
  ├── test.js
  └── .eslintrc  --> 生效
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
  └── .eslintrc  --> 生效
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

### 定义
首先明确一个概念：`eslint`并没有`plugin API`（这个也是Nicholas C. Zakas说的），我们拥有的是一个插件格式，`Eslint`一直将插件视为命名事物的集合，它提供`rule`、`processor`、`config`这些内容给`Eslint`进行使用，**插件本身什么也不会做，插件只是提供配置，由`Eslint`获取配置进行处理**

我们看下源码里对于`Plugin`类型的定义：

```js
/**
 * @typedef {Object} Plugin
 * @property {Record<string, ConfigData>} [configs] The definition of plugin configs.
 * @property {Record<string, Environment>} [environments] The definition of plugin environments.
 * @property {Record<string, Processor>} [processors] The definition of plugin processors.
 * @property {Record<string, Function | Rule>} [rules] The definition of plugin rules.
 */
```

`Plugin`类型的定义将在接下来插件例子里得到体现。

### 命名
`eslint`的插件命名格式必须是`eslint-plugin-<plugin-name>`或`@<scope>/eslint-plugin-<plugin-name>`

### 提供内容
> 注意：这些内容插件不需要全部提供，开发者可以根据需要提供其中一项或多项
```js
module.exports = {
    rules: { //可以提供额外的规则，使用可以不加前缀，以`plugin-name/rule-name`的形式使用
        "dollar-sign":{//规则名
            create:function(context){}
        }
    },
    environments: { //可以提供额外的环境，可以定义`globals`、`parserOptions`， 使用时可以不加前缀，以`插件名/环境名`的方式调用
        jquery: { //环境名
            globals: {
                $: false //true允许全局覆盖，false不允许全局覆盖
            },
            parserOptions: { 

            }
        }
    },
    processors: { //可以提供处理器
        //需要符合这种接口
        '.md': { //处理的文件后缀，比如.js、.jsx、.html等
            preprocess: (text,filename) => {//文件内容，文件名
                //将需要字符部分拆离出来 
                return [string] //将字符块数组返回---之所以是数组是因为一份文档可以剥离多份内容，比如说一份`doc.md`可以存在多份代码块
            },
            postprocess: (messages,filename) => {//检测信息（这里是一个二维数组）、文件
                // 这里我特别说一下messages，这里涉及一点源码，直接说原理比较好解释，展示下伪代码：
                // messages = preprocess(text, filename).map(block => verify(block));
                // postprocess(messages,filename);

                // 首先是preprocess返回的是代码块组成的数组，每个代码块我们都进行检测，得到一个一维数组，这个数组包含对这个代码块的所有检测信息
                // 每个检测信息对象是LintMessage类型，每个代码块得到的检测结果是LintMessage[]
                // 然后我们知道preprocess是代码块组成的数组，那么messages就会是LintMessage[][]
                // postprocess返回值需要是LintMessage[]，可以认为是将二维数组打平，具体打平操作由插件开发者决定

                return messages[0] //返回值是一个一维数组
            },
            supportsAutoFix: true, //是否支持自动修复
        }
    },
    configs: { //提供一些配置以供扩展，比如以 extends:["plugin":customPlugin/customConfig] 进行扩展
        customConfig: {
            plugins:['myPlugin'],
            env:["browser"],
            rules:[...],
        }
    },
    peerDependencies: { //依赖
        "eslint": ">=0.8.0"
    }
}
```

### 源码中的定义
```js
/**
 * @typedef {Object} Environment
 * @property {Record<string, GlobalConf>} [globals] The definition of global variables.
 * @property {ParserOptions} [parserOptions] The parser options that will be enabled under this environment.
 */

/**
 * @typedef {Object} Processor
 * @property {(text:string, filename:string) => Array<string | { text:string, filename:string }>} [preprocess] The function to extract code blocks.
 * @property {(messagesList:LintMessage[][], filename:string) => LintMessage[]} [postprocess] The function to merge messages.
 * @property {boolean} [supportsAutofix] If `true` then it means the processor supports autofix.
 */

/**
 * @typedef {Object} LintMessage
 * @property {number|undefined} column The 1-based column number.
 * @property {number} [endColumn] The 1-based column number of the end location.
 * @property {number} [endLine] The 1-based line number of the end location.
 * @property {boolean} fatal If `true` then this is a fatal error.
 * @property {{range:[number,number], text:string}} [fix] Information for autofix.
 * @property {number|undefined} line The 1-based line number.
 * @property {string} message The error message.
 * @property {string|null} ruleId The ID of the rule which makes this message.
 * @property {0|1|2} severity The severity of this message.
 * @property {Array<{desc?: string, messageId?: string, fix: {range: [number, number], text: string}}>} [suggestions] Information for suggestions.
 */
```

## `Rule`
`Rule`是`eslint`的核心组成部分，对于源码的解析重点正是`Rule`

`Rule`的作用大概来说是校验文档中的代码，如果异常则进行提示，有些`Rule`会提供修复函数，可以对异常代码进行修复。

在了解作用之后，还需要的是`Rule`的定义，`Rule`是什么样的？

这样问或许有些抽象，下面我们来写一个`Demo`，看看怎么定义一个`Rule`

### 基本格式
```js
module.exports = {
    meta: {
        //type有三种
        //1. "problem"  问题类规则识别导致错误的代码
        //2. "suggestion" 建议类规则识别**容易**造成问题的代码
        //3. "layout" 识别风格相关代码，比如空格，分号这些
        type: "suggestion",
        docs: {
            description: "disallow unnecessary semicolons",
            //category包含以下几种
            //1. `Possible Errors` 与代码中可能的错误或逻辑错误有关
            //2. `Best Practices` 最佳实践，帮助避免一些问题
            //3. `Strict Mode` 与严格模式相关
            //4. `Variables` 与变量声明相关
            //5. `Node.js and CommonJS` 关于Node.js 或 在浏览器中使用的CommonJS相关规则
            //6. `Stylistic Issues` 代码风格
            //7. `ECMAScript 6` es6【es2015】相关
            //8. `Deprecated` 已弃用规则
            //9. `Removed` 被移除，并提供新的规则替代 
            category: "Possible Errors",
            //在配置文件设置"extends": "eslint:recommended"属性时，是否启用该规则
            recommended: true,
            //url 指定可以访问完整文档的 url。
            url: "https://eslint.org/docs/rules/no-extra-semi"
        },

        //如果不设置fixable即使规则实现了fix功能也不会进行修复
        //源码fixable是这么使用的：
        //if (problem.fix && rule.meta && !rule.meta.fixable) throw new Error(...)
        //对于fixable的定义：
        //@property {"code"|"whitespace"} [fixable] The autofix type.
        //源码中并没有直接使用这两个值
        //我看了一些官方定义的规则，基本"whitespace"是和空格相关的修复，和"code"相关的是和代码相关的修复
        //这里举两个例子，array-element-newline规则就是空格相关的修复，arrow-body-style是和代码相关的修复，感兴趣的可以看一下两者fixable和fix的实现
        //实际上，无论使用哪个字符串运行过程是相同的，不同的是对阅读者的意义
        fixable: "code",

        //用于描述规则的选项，ESLint会使用它验证配置中的选项是否有效
        //遵循JSON schema格式，关于JSON schema它是用于描述及验证JSON数据的规范，
        //通过JSON schema可以先验证数据是否符合指定的格式，避免无效输入
        //如果想要详细的了解JSON schema可以去官网http://json-schema.org/
        //我们启用rule时，第一项决定其错误级别，schema定义的配置项从第二项开始生效
        //以这里为例，使用的时候可以这么写："rule-name":[ 2, "never", { "exceptRange": false } ]
        schema: [
            {
                "enum": ["always", "never"]
            },
            {
                "type": "object",
                "properties": {
                    "exceptRange": {
                        "type": "boolean"
                    }
                },
                "additionalProperties": false
            }
        ],

        //规则是否已弃用，如果没有弃用可以不用定义
        deprecated:false,
        //如果规则不支持的情况下，用于替代的规则
        replacedBy:['indent'],
        //规则报告的消息
        messages:{
            expected: "Expected indentation of {{expected}} but found {{actual}}."
        }
    },

    //create我放到下面独立章节做讲解
    //这里只需要知道这里一个返回对象类型的函数即可
    create: function(context) { 
        return {
            // callback functions
        };
    }
};
```

### `create`
首先我们看一下源码中涉及`create`的部分：
```js
/**
 * 运行rule，获取它提供的listeners
 * @param {Rule} rule 具有create方法的标准rule
 * @param {Context} ruleContext 应该传递给rule的上下文
 * @throws {any} 执行rule.create所出现的任何错误
 * @returns {Object} selectors 和 listeners 组成的映射对象
 */
function createRuleListeners(rule, ruleContext) {
    try {
        return rule.create(ruleContext);
    } catch (ex) {
        ex.message = `Error while loading rule '${ruleContext.id}': ${ex.message}`;
        throw ex;
    }
}
```

### `Context`
`Context`包含规则上下文相关的信息，有以下属性：
- `parserOptions` 解析器选项
- `id`
- `options` 已配置选项
- `settings` 共享设置
- `parserPath` 配置中`parser`的名称
- `parserServices` 解析器中为规则提供服务的对象

`Context`有以下方法：
- `getAncestors()` - 返回当前遍历节点的祖先数组，从 AST 的根节点开始，一直到当前节点的直接父节点。这个数组不包括当前遍历的节点本身。
- `getDeclaredVariables(node)` - 返回由给定节点声明的变量 列表。此信息可用于跟踪对变量的引用。
- `getFilename()` - 返回与源文件关联的文件名。
- `getScope()` - 返回当前遍历节点的 `scope`。此信息可用于跟踪对变量的引用
- `getSourceCode()` - 返回一个`SourceCode`对象，你可以使用该对象处理传递给 `ESLint` 的源代码。
- `markVariableAsUsed(name)` - 在当前作用域内用给定的名称标记一个变量，如果找到一个具有给定名称的变量并将其标记为已使用，则返回 `true`，否则返回 `false`。
- `report(descriptor)` - 报告问题的代码

#### `context.getScope()`
这个方法返回一个作用域。【作用域是什么类型，有什么用？】

该方法返回的作用域具有以下类型：
| AST Node Type | Scope Type | 
| --- | --- | 
| `Program` | `global` | 
| `FunctionDeclaration` | `function` | 
| `FunctionExpression` | `function` | 
| `ArrowFunctionExpression` | `function` |
| `ClassDeclaration` | `class` |
| `ClassExpression` | `class` |
| `BlockStatement` ※1 | `block` | 
| `SwitchStatement` ※1 | `switch` | 
| `ForStatement` ※2 | `for` |
| `ForInStatement` ※2 | `for` | 
| `ForOfStatement` ※2 | `for` |	
| `WithStatement` | `with` | 
| `CatchClause` | `catch` |
| others | ※3 |

※1 仅当配置的解析器提供块作用域特性时才使用，`parserOptions.ecmaVersion`必须要大于等于6才能使用块级作用域特性

※2 只有当 `for` 语句将迭代变量定义为块作用域的变量时 (例如，`for (let i = 0;;) {}`)。

※3 具有自己作用域的最近祖先节点的作用域。如果最近的祖先节点有多个作用域，那么它选择最内部的作用域（例如，如果`Program#sourceType` 是 `"module"`，则 `Program` 节点有一个 `global` 作用域和一个 `module` 作用域。最内层的作用域是 `"module"` 作用域。）。

#### `context.report()`
用来发布警告或错误【由开发者决定】

该方法只接受一个参数，是一个对象，包含以下属性：
- `message`(可选) 有问题的消息
- `messageId`(可选) 通过`messageId`可以使用`meta.messages`定义的消息，而不必自己定义`message`
- `node`(可选) - 与问题有关的 `AST` 节点。如果存在且没有指定 `loc`，那么该节点的开始位置被用来作为问题的位置。
- `loc`(可选) - 用来指定问题位置的一个对象。如果同时指定的了 `loc` 和 `node`，那么位置将从`loc`获取而非`node`
    - `start` - 开始位置
        - `line` - 问题发生的行号，从 `1` 开始。
        - `column` - 问题发生的列号，从 `0` 开始。
    - `end` - 结束位置
        - `line` - 问题发生的行号，从 `1` 开始。
        - `column` - 问题发生的列号，从 `0` 开始。
- `data`(可选) - 提供的数据给`message`的占位符使用【下面有示例】
- `fix`(可选) - 用来解决问题的修复函数

> 请注意，`node` 或 `loc` 至少有一个是必须的；`message`和`messageId`至少有一个是必须的

##### `data`示例
```js
context.report({
    node: node,
    message: "Unexpected identifier: {{ identifier }}",
    data: {
        identifier: node.name
    }
});
```

##### `messageId`示例
```js
// in your rule
module.exports = {
    meta: {
        messages: {
            avoidName: "Avoid using variables named '{{ name }}'"
        }
    },
    create(context) {
        return {
            Identifier(node) {
                if (node.name === "foo") {
                    context.report({
                        node,
                        messageId: "avoidName",
                        data: {
                            name: "foo",
                        }
                    });
                }
            }
        };
    }
};
```

##### `fix`
如果想要支持对问题的修复，需要提供`fix`函数：
```js
context.report({
    node: node,
    fix: function(fixer){
        return fixer.insertTextAfter(node,';)
    }
})
```

###### 参数`fixer`
`fixer`有以下方法：
1. `insertTextAfter(nodeOrToken, text)` 在给定的节点或记号之后插入文本
1. `insertTextAfterRange(range, text)` 在给定的范围之后插入文本
1. `insertTextBefore(noderOrToken, text)`
1. `insertTextBeforeRange(range, text)` 
1. `remove(nodeOrToken)` 删除给定节点或记号
1. `removeRange(range)` 删除给定范围内的文本
1. `replaceText(nodeOrText, text)`
1. `replaceTextRange(range, text)`

这些方法都返回一个`fixing`对象。

###### 返回值
可以返回以下值：
1. 一个`fixing`对象
1. 一个包含`fixing`对象的数组
1. 一个可迭代的对象，用来枚举`fixing`对象。

如果让`fix`函数返回多个`fixing`对象，那么这些对象不能重叠。

修复的最佳实践：
1. 避免任何可能改变代码运行时行为和导致其停止工作的修复。
1. 做尽可能小的修复。那些不必要的修复可能会与其他修复发生冲突，应该避免。
1. 使每条消息只有一个修复。这是强制的，因为你必须从 `fix()` 返回修复操作的结果。
1. 由于所有的规则只第一轮修复之后重新运行，所以规则就没必要去检查一个修复的代码风格是否会导致另一个规则报告错误。

#### `options`
有些规则可能需要输入可选项，可选项可以通过`.eslintrc`、命令行、注释等方式进行输入，比如：
```js
{
    "quotes": ["error", "double"]
}
```

规则使用：
```js
module.exports = {
    create: function(context){
        const isDouble = (context.options[0] === 'double'); //注意，错误级别不属于`context.options`
        //...
    }
}
```

#### `getSourceCode()`

## 关于`.eslintignore`
只会使用**当前目录**下的`.eslintignore`文件

# 问题
在开始之前我会提几个问题，阅读源码的时候带着这些问题去思考，最终问题解答我放到了“解答”这个部分，可以直接在这部分看解答。

`eslintrc`相关
1. [x] `eslint`是怎么将`eslintrc.*`作为默认配置的？
1. [ ] `extends`是怎么生效的？
1. [ ] `overrides`是怎么生效的？
1. [x] 关于`eslint.*`之外的另一种配置方案是怎么回事？

`rule`相关
1. [ ] `eslint`是怎么执行`rule`的
2. [ ] 代码修复的细节

# 入口
```json5
//package.json
{
    "bin": {
        "eslint": "./bin/eslint.js"
    },
}
```

入口是`bin/eslint.js`文件，每次我们执行命令`eslint [options] file.js [file.js] [dir]`就是从这里开始执行的。

![package.json#bin](https://pic.imgdb.cn/item/613719e144eaada7398a9c46.jpg)

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
![bin/eslint#main](https://pic.imgdb.cn/item/61371a3344eaada7398b36a6.jpg)

这里如果我们执行`eslint --init`会走`initializeConfig`这边的代码，这个命令可以初始创建`eslintrc.*`文件，现在我并不想探寻`--init`的具体实现细节。

`require("../lib/cli").execute(...)`这里包含着`lint`处理，我们这次主要目的是了解：
1. 了解`options`的处理流程
2. 了解`rule`是怎么被执行的
3. 了解`eslint`的修复原理

所以我们接下来会进入`lib/cli.js`文件中查看代码。

不过在此之前，我要说一下`process.exitCode`，在接下来的`cli.execute()`会返回几种值：`0`、`1`、`2`，返回值会被赋值给`process.exitCode`。

对于`process.exitCode`的值及其作用我们有了解的必要，这样可以让我们理解`cli.execute()`返回值的作用，不然的话可能有的人会疑惑`cli.execute()`这些数字值的意义。

`node`中退出线程有两种方式`process.exitCode`、`process.exit(exitCode)`，相同点就是`exitCode`为`0`时表示正常退出，大于等于`1`时表示异常退出，这里返回值时`1`，`2`时都属于异常退出，对于程序来说没什么区别，对于开发者来说，如果是`1`则表示警告或者不严重的错误，而`2`则表示致命错误，开发者可以通过退出码快速判定异常级别或者说严重程度。

那么这两种方式有什么区别呢？

`process.exitCode`的退出相对安全，它会等待进程执行结束自然退出，而`process.exit(exitCode)`退出则是强制中断

这里举两个例子，先是`process.exitCode`
```js
console.log(1);
process.exitCode = 2;
console.log(2)
```

执行结果会是这样的：
![process.exitCode](https://pic.imgdb.cn/item/616395312ab3f51d91d4421b.jpg)

然后是`process.exit()`
```js
console.log(1);
process.exit(2)
console.log(2)
```

执行结果如下：
![process.exit](https://pic.imgdb.cn/item/616395862ab3f51d91d4ace8.jpg)

一般来说，如果有正在执行的异步任务【比如I/O读取或者接口通信之类的】，使用`process.exit()`可能会导致问题，在这里`eslint`存在大量的`I/O`操作，所以它选择了`process.exitCode`进行退出。

有些情况下使用`process.exit()`也是合理的选择，了解两者的用法和区别，可以根据情况选中其中适合的一种。

# `lib/cli.js`
```js
const cli = {
    async execute(args, text) {
        //将args解析成对象形式并赋值给变量options

        //检查一些命令行参数并退出，比如help，version等

        //* lint核心处理

        //是否启用quite模式，启用则过滤警告
        
        //printResult并退出

        return 2;
    }   
}
```

流程示意图：

![cli.execute](https://pic.imgdb.cn/item/6163aa712ab3f51d91ec28cf.jpg)

这里特别说明一下，源码里`cli`对象真的只有`execute`一个方法。

下面我说明下`cli.execute`在这里什么，为什么要这么做？

首先，我们知道在执行命令行指令时，我们输入的是字符串，比如说执行类似`order -s=true -b=1`这种指令，通过`process.argv`取到的默认是字符串，很明显这样的原始信息是很不方便的，比如在这里我们可以希望直接得到的参数是这样的：
```js
{
    s:true,
    b:1
}
```
这样用起来肯定方便许多。

然后还有一些情况，比如说我们希望`-h`和`--help`标识同一个属性，又比如我们希望`-s`只能输入布尔类型，如果输入其他类型在使用之前就可以校验并报错。

如果我们自己处理，当然也可以做到，不过相对麻烦一些，而且一些特殊场景可能也没考虑到，我自己写过一些命令行工具，我的经验是如果命令就那么几个，只在内部项目使用，自己处理也行，比较方便。如果对命令行工具要求较高，并且处理场景相对复杂，那么最好使用一些参数解析库。

`eslint`这里使用了`optionator`这个库进行解析，这个库我也用过，一些细节处理先不用关注，它最核心的功能还是将字符串参数解析成对象形式的数据以便在接下来使用。

所以这里，`cli.execute`做的第一件事，是将`argv`的原始参数解析成对象形式数据，便于接下来使用。

然后接下来，解析完之后，拿到`options`的参数就可以校验并执行相关功能了，类似这样：
```js
if (options.help) {
    log.info(CLIOptions.generateHelp());
    return 0;
}
```

有很多命令是和核心`linter`无关的，比如说`eslint --help`这个是输入帮助列表，`eslint --version`是打印当前`eslint`版本，它们都是执行一些功能之后**直接退出**。

所以一开始优先处理这部分命令，会在执行`linter`之前直接退出。

这部分不是说不重要，只是说这里不是我们想要了解的`linter`重心，所以我们还是得往下面得部分走。

然后来到了`linter`这部分，这部分我们可以认为是对`linter`的核心处理，我们在这里解析`eslintrc.*`的配置项，并且执行`rule`。

这部分我会在下一节做详细说明，这里知道它的作用就可以。

接着走到下面，如果有`--quite`，启用`quite`模式会过滤掉警告。

然后接下来到`printResult`这部分，代码类似这样：
```js
if(await printResults(engine, resultsToPrint, options.format, options.outputFile)){
    //检查错误、致命错误、警告
}
```

先说下`printResults`的实现：
```js
/**
 * Outputs the results of the linting.
 * @param {ESLint} engine The ESLint instance to use.
 * @param {LintResult[]} results The results to print.
 * @param {string} format The name of the formatter to use or the path to the formatter.
 * @param {string} outputFile The path for the output file.
 * @returns {Promise<boolean>} True if the printing succeeds, false if not.
 * @private
 */
async function printResults(engine, results, format, outputFile) {
    let formatter;

    try {
        formatter = await engine.loadFormatter(format);
    } catch (e) {
        log.error(e.message);
        return false;
    }

    const output = formatter.format(results);

    if (output) {
        if (outputFile) {
            const filePath = path.resolve(process.cwd(), outputFile);

            if (await isDirectory(filePath)) {
                log.error("Cannot write to output file path, it is a directory: %s", outputFile);
                return false;
            }

            try {
                await mkdir(path.dirname(filePath), { recursive: true });
                await writeFile(filePath, output);
            } catch (ex) {
                log.error("There was a problem writing the output file:\n%s", ex);
                return false;
            }
        } else {
            log.info(output);
        }
    }

    return true;
}
```
代码应该不难理解，这里我还是说明，梳理下逻辑，如果认为自己完全理解源码的可以跳过。

`printResult`的作用就是打印结果

`printResult`的返回值是布尔类型，如果打印成功返回`true`，打印失败返回`false`

关于参数：
- `engine` `Eslint`实例，关于`Eslint`会在下面的部分说明
- `results` 需要打印的结果
- `format` 格式化程序
- `outputFile` 输出路径

运行逻辑：
1. 通过`engine.loadFormatter(format)`取出格式化程序
2. 然后利用格式化程序将结果格式化。
3. 如果没有输出路径，则直接打印结果
4. 如果有输出路径
   1. 我们先判断这个路径是不是目录，如果是则报错退出，
   2. 如果不是，我们尝试为其创建一个目录，然后写入到文件路径中

如果成功打印结果会执行对错误和警告的检查

如果打印失败，我们可以直接认为执行失败。但是呢，即使打印成功，`linter`之后可能依旧存在错误和警告，这个时候需要针对`linter`的结果进行校验。

```js
if (await printResults(engine, resultsToPrint, options.format, options.outputFile)) {
    const { errorCount, fatalErrorCount, warningCount } = countErrors(results);

    const tooManyWarnings =
        options.maxWarnings >= 0 && warningCount > options.maxWarnings;
    const shouldExitForFatalErrors =
        options.exitOnFatalError && fatalErrorCount > 0;

    if (!errorCount && tooManyWarnings) {
        log.error(
            "ESLint found too many warnings (maximum: %s).",
            options.maxWarnings 
        );
    }

    if (shouldExitForFatalErrors) {
        return 2;
    }

    return (errorCount || tooManyWarnings) ? 1 : 0;
}
```
这里逻辑也很简单，就是：
1. 如果有致命错误，返回2
1. 如果警告或一般错误，返回1（如果只有警告会特别进行一个关于警告的打印——如果设置了`--max-warining`）
1. 没有警告、错误、致命错误，返回0

在`printResults`之后是`return 2`，这里返回`2`是因为之前的`printResults`失败，这是严重的失败，所以返回`2`

到这里，我觉得对于`cli.execute()`的执行流程我们应该是清晰了，我们再回顾下流程图：

![cli.execute](https://pic.imgdb.cn/item/6163aa712ab3f51d91ec28cf.jpg)

现在我们进入`lint`核心处理部分，看这里做了什么。

## `lint`核心处理
```js
const engine = new ESLint(translateOptions(options));//初始化ESLint
let results;

if (useStdin) {//使用--stdin会走这里
    result = await engine.lintText(text, {...})
} else {
    results = await engine.lintFiles(files);
}

if (options.fix) {
    await ESLint.outputFixes(results); //将修复后的结果写入文件
}
```

流程示意图：

![lint核心处理](https://pic.imgdb.cn/item/6139828a44eaada7395e519d.jpg)

可以看到这里初始化了一个`Eslint`实例，它接收的参数是`translateOptions(options)`，我们知道`options`是命令行参数解析得到的参数对象，那么这里的`translateOptions()`函数是做什么的？

这里它对`options`做了转换，为什么需要转换？因为`options`和`Eslint`初始化需要的数据是有所差异的，所以这里是将`options`的转换为`CLIEngine`所需要的初始化参数。

这里额外提一句，这里源码注释是`Translates the CLI options into the options expected by the CLIEngine.`（将 `CLI` 选项转换为 `CLIEngine` 预期的选项。）。

这句话其实并不是很合理，因为这个`options`传递给`Eslint`里面的时候，它是经过再处理然后传递给`CLIEngine`的。而`options`在`Eslint`里面不只是传递`CLIEngine`，它本身也需要使用`options`，比如说`options.plugins`，`options.overrideConfig`这些。

的确，这里一部分处理是为了`CLIEngine`，但并不是全部，有两点可以证明：
1. 传递给`CLIEngine`的时候需要再次处理
1. `Eslint`初始化本身也需要用到`Options`的部分数据

所以我的想法是认为`translateOptions`的处理既是为了`CLIEngine`，也是为了`Eslint`，它并不只是为了`CLIEngine`的预期数据，这种想法是比较合理的。

回到`translateOptions`的实现，对`options`具体转换内容不说（因为属性太多了），我这里用伪代码表示下大概的属性分类，大概就这么三种，
```js
function translateOptions({cache,inlineConfig,reportUnusedDisableDirectives}){
    return {
        //属性完全相同的
        cache, 
        //属性名不同的
        extensions: ext,
        //基于一个或多个options属性衍生出需要的值
        reportUnusedDisableDirectives: reportUnusedDisableDirectives ? "error" : void 0,
        cacheLocation: cacheLocation || cacheFile,
        fix: (fix || fixDryRun) && (quiet ? quietFixPredicate : true),
}
```

总之，现在我们先得到了一个`Eslint`的实例`engine`

然后接着往下走，接下来是`linter`的关键，这里有个小分支，如果使用`--stdin`则调用`lintText`，否则调用`lintFiles`

这里如果我们使用`--stdin`则读取得到的是源码本身，可以直接`lint`，所以是`lintText`；而如果常规模式，我们会传递一个文件根目录，根据这个目录递归读取文件得到代码，然后进行`lint`，所以是`lintFiles`

我们这里优先关注`lintFiles`的实现。

最后是`options.fix`这里，其实这个并不属于`lint`的重点，这里只是决定是否将修复结果写入到文件。

现在我们再回顾下流程图：

![lint核心处理](https://pic.imgdb.cn/item/6139828a44eaada7395e519d.jpg)

这次有3个重要的部分，这里`lintText`我放到最后，需要考虑的是先了解`Eslint`的初始化过程，还是先了解`LintFiles`？

从实际执行流程的考虑，应该先了解`Eslint`，然后在了解`LintFiles`，麻烦的地方在于`Eslint`的初始化做了很多事，而且不仅仅是为了`lintFiles`和`lintText`准备数据，它的方法是很多的。

如果先了解`Eslint`，需要花费很长时间，会牵涉到很多非`lint`核心部分的代码，而如果先看`lintFiles`我们可以关注最核心的部分，这样子需要了解的内容会少很多。

综上，先看`lintFiles`的执行过程。

我们先看一下`lintFiles(files)`接受的参数`files`

## `engine.lintFiles(files)`
```js
async lintFiles(patterns) {
    //patterns必须是非空字符串 或 非空字符字符串数组
    if (!isNonEmptyString(patterns) && !isArrayOfNonEmptyString(patterns)) {
        throw new Error("'patterns' must be a non-empty string or an array of non-empty strings");
    }

    const { cliEngine } = privateMembersMap.get(this);

    return processCLIEngineLintReport(
        cliEngine,
        cliEngine.executeOnFiles(patterns)
    );
}
```
这里首先是对参数`patterns`的校验，它必须是非空字符串 或 非空字符字符串数组，否则直接抛错退出。

然后是取出`cliEngine`，这里`privateMembersMap`是`WeakMap`类型，它是一个全局变量，键名是`Eslint`实例，键值是一个对象，这个对象有两个属性`cliEngine`、`options`。

```js
/**
 * @type {WeakMap<ESLint, ESLintPrivateMembers>}
 */
const privateMembersMap = new WeakMap();

/**
 * Private members for the `ESLint` instance.
 * @typedef {Object} ESLintPrivateMembers
 * @property {CLIEngine} cliEngine The wrapped CLIEngine instance.
 * @property {ESLintOptions} options The options used to instantiate the ESLint instance.
 */
```

这里稍微说一下，为什么使用`WeakMap`类型，主要原因是因为它的键名是弱引用，弱引用和强引用定义：
```
在计算机程序设计中，弱引用与强引用相对，是指不能确保其引用的对象不会被垃圾回收器回收的引用。一个对象若只被弱引用所引用，则被认为是不可访问（或弱可访问）的，并因此可能在任何时刻被回收。
```

无论是`Object`还是`Map`都是强引用，举个例子：
```js
const map = new Map();
const key = new Array();

map.set(key,1)
//注意，这里将key设置为null，原来的引用对象不会被回收
key = null;
```

如果想要回收原引用对象，需要这么写：
```js
let map = new Map();
let key = new Array();
map.set(key, 1);
map.delete(key); //注意这里！
key = null;
```

如果使用`WeakMap`
```js
const map = new WeakMap();
const key = new Array();

map.set(key,1)
key = null; //原来的对象会被回收
```

就是这样，另外这里可以发现`privateMembersMap`使用的`Eslint`实例作为键名，这其实也是它的一个重要特点：
1. `Object`只能使用简单类型作为键名，比如`Strting`，`Number`，`Symbol`，且可枚举
1. `Map`的键名可以是任意类型，且可枚举
1. `WeakMap`的键名必须是对象，不可枚举

现在我们取出`engine`，然后执行
```js
return processCLIEngineLintReport(
    cliEngine,
    cliEngine.executeOnFiles(patterns)
);
```

我们看一下`processCLIEngineLintReport`的实现：
```js
/**
 * CLIEngineLintReport的results提取出来返回【以适配Eslint的API】
 * @param {CLIEngine} cliEngine The CLIEngine instance.
 * @param {CLIEngineLintReport} report The CLIEngine linting report to process.
 * @returns {LintResult[]} The processed linting results.
 */
function processCLIEngineLintReport(cliEngine, { results }) {
    const descriptor = {
        configurable: true,
        enumerable: true,
        get() {
            return getOrFindUsedDeprecatedRules(cliEngine, this.filePath);
        }
    };

    for (const result of results) {
        Object.defineProperty(result, "usedDeprecatedRules", descriptor);
    }

    return results;
}
```
这里主要是从参数`report`取出`results`返回，因为我们想要的结果是`LintResult[]`类型。

可以看到，这里为每条`result`添加了一个`usedDeprecatedRules`属性，用于获取已被使用的废弃规则。

然后我们回到`cliEngine.executeOnFiles(patterns)`这里，它非常的关键，它的执行结果会作为参数传递给`processCLIEngineLintReport`，我们进入查看代码。

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

![ESLint#constructor](https://pic.imgdb.cn/item/613984e944eaada73961c9b1.jpg)

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

![CliEngine#constructor](https://pic.imgdb.cn/item/6139851144eaada7396218f9.jpg)

这里创建的私有数据先不深入，在接下来的使用到这些数据的时候，我们再进行解读。

不过这里我要说一下：`CascadingConfigArrayFactory`这个类非常重要，我在下面用了一个专题特别进行说明，这个类的主要目的就是用于处理级联配置，如果对这一块不了解或者感兴趣的，可以看一下这个专题。

现在我们对`new Eslint`已经有了一个基本的了解，接下来我们去了解下`engine.lintFiles(files)`的流程及其实现：

![lint核心处理2](https://pic.imgdb.cn/item/6139832244eaada7395f19ea.jpg)

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

![cliEngine.executeOnFiles](https://pic.imgdb.cn/item/6139d62844eaada7390532f2.jpg)

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

![迭代源代码文件](https://pic.imgdb.cn/item/613ace8344eaada739684d9d.jpg)

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

![verifyText](https://pic.imgdb.cn/item/61454f8f2ab3f51d91fd4a75.jpg)

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

![linter.verifyAndFix](https://pic.imgdb.cn/item/614552772ab3f51d91010fd2.jpg)

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
这里思路也不是很复杂，也有两个地方我想看一下：`this.verify`、`SourceCodeFixer.applyFixes`

我们先看`SourceCodeFixer.applyFixes`方法.

## `SourceCodeFixer.applyFixes`
```js
/**
 * 将消息指定的fixes应用于给定文本。 
 * 尝试智能修复，不会在文本中的同一区域应用fixes。
 * @param {string} sourceText 要应用更改的文本。
 * @param {Message[]} messages ESLint 报告的消息数组。
 * @param {boolean|Function} [shouldFix=true] 确定是否应修复每条消息
 * @returns {Object} 包含固定文本和任何未固定消息的对象。
 */
SourceCodeFixer.applyFixes = function(sourceText, messages, shouldFix) {
    //shouldFix为false，不尝试修复
    if (shouldFix === false) return { fixed: false, messages, output: sourceText };

    // clone the array
    const remainingMessages = [],
        fixes = [],
        bom = sourceText.startsWith(BOM) ? BOM : "",
        text = bom ? sourceText.slice(1) : sourceText;
    let lastPos = Number.NEGATIVE_INFINITY, //初始设为负无穷大
        output = bom;

    //如果problem对象有fix属性，将problem放到fixes数组，否则将其放到remainingMessages数组
    messages.forEach(problem => {...});

    //进行修复
    if (fixes.length) return {...}
    
    //没有可用修复
    return { fixed: false, messages, output: bom + text };
};
```

### 进行修复
```js
let fixesWereApplied = false;//标识尚未修复

//先为message排序，按fix范围从前往后排列，然后进行迭代【这里problem就是message】
for (const problem of fixes.sort(compareMessagesByFixRange)) { 
    //如果shouldFix不是函数 或者 should(problem)结果为true 则进行修复
    if (typeof shouldFix !== "function" || shouldFix(problem)) {
        attemptFix(problem);//应用修复

        //修复失败的原因只会是和之前的修复冲突了，
        //所以无论修复成功还是失败，我们都将其标记为true，表示已经进行修复
        fixesWereApplied = true;
    } else {
        remainingMessages.push(problem); //不进行修复的problem放到remainingMessages数组
    }
}
output += text.slice(Math.max(0, lastPos));

return {
    fixed: fixesWereApplied,
    messages: remainingMessages.sort(compareMessagesByLocation),
    output
};
```

### `attemptFix`
```js
/**
 * 尝试使用problem的"fix"。
 * @param {Message} problem 应用修复的消息对象
 * @returns {boolean} 修复是否成功应用
 */
function attemptFix(problem) {
    const fix = problem.fix;
    const start = fix.range[0];
    const end = fix.range[1];

    // 如果开始修复的位置在上一次修复结束的位置之前，
    // 或者开始修复的位置在结束修复的位置之前
    // 则不进行修复
    if (lastPos >= start || start > end) {
        remainingMessages.push(problem);
        return false;
    }

    // 如果开始修复的位置小于0，且结束位置大于等于0
    // 或者开始修复的位置等于0，且fix.text是以BOM开始的
    // 则设置output = ""; (移除BOM)
    if ((start < 0 && end >= 0) || (start === 0 && fix.text.startsWith(BOM))) {
        output = "";
    }

    // 应用fix到output上
    // 在上一次的基础上，从源码中摘取不需要修复的部分，添加到output
    output += text.slice(Math.max(0, lastPos), Math.max(0, start))
    // 将修复的内容添加到output
    output += fix.text; 
    lastPos = end;//更新修复结束的位置
    return true;
}
```

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

#### `parse`
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

### `SourceCode`--分析中
```js
class SourceCode extends TokenStore {
    /**
     * 表示解析的源代码.
     * @param {string|Object} textOrConfig The source code text or config object.
     * @param {string} textOrConfig.text The source code text.
     * @param {ASTNode} textOrConfig.ast 代表代码的 AST 的 Program 节点。 这个 AST 应该从 BOM 被剥离的文本中创建
     * @param {Object|null} textOrConfig.parserServices The parser services.
     * @param {ScopeManager|null} textOrConfig.scopeManager The scope of this source code.
     * @param {Object|null} textOrConfig.visitorKeys The visitor keys to traverse AST.
     * @param {ASTNode} [astIfNoConfig] 代表代码的 AST 的 Program 节点。 这个 AST 应该从 BOM 被剥离的文本中创建
     */
    constructor(textOrConfig, astIfNoConfig) {
        let text, ast, parserServices, scopeManager, visitorKeys;

        // Process overloading.
        if (typeof textOrConfig === "string") {
            text = textOrConfig;
            ast = astIfNoConfig;
        } else if (typeof textOrConfig === "object" && textOrConfig !== null) {
            text = textOrConfig.text;
            ast = textOrConfig.ast;
            parserServices = textOrConfig.parserServices;
            scopeManager = textOrConfig.scopeManager;
            visitorKeys = textOrConfig.visitorKeys;
        }

        validate(ast);
        super(ast.tokens, ast.comments);

        /**
         * The flag to indicate that the source code has Unicode BOM.
         * @type boolean
         */
        this.hasBOM = (text.charCodeAt(0) === 0xFEFF);

        /**
         * The original text source code.
         * BOM was stripped from this text.
         * @type string
         */
        this.text = (this.hasBOM ? text.slice(1) : text);

        /**
         * The parsed AST for the source code.
         * @type ASTNode
         */
        this.ast = ast;

        /**
         * The parser services of this source code.
         * @type {Object}
         */
        this.parserServices = parserServices || {};

        /**
         * The scope of this source code.
         * @type {ScopeManager|null}
         */
        this.scopeManager = scopeManager || null;

        /**
         * The visitor keys to traverse AST.
         * @type {Object}
         */
        this.visitorKeys = visitorKeys || Traverser.DEFAULT_VISITOR_KEYS;

        // Check the source text for the presence of a shebang since it is parsed as a standard line comment.
        const shebangMatched = this.text.match(astUtils.shebangPattern);
        const hasShebang = shebangMatched && ast.comments.length && ast.comments[0].value === shebangMatched[1];

        if (hasShebang) {
            ast.comments[0].type = "Shebang";
        }

        this.tokensAndComments = sortedMerge(ast.tokens, ast.comments);

        /**
         * 源代码根据 ECMA-262 规范分成几行。
         * 这样做是为了避免每个规则都需要单独执行。
         * @type string[]
         */
        this.lines = [];
        this.lineStartIndices = [0];

        const lineEndingPattern = astUtils.createGlobalLinebreakMatcher();
        let match;

        /*
        以前，这是使用正则表达式实现的，
        该正则表达式匹配一系列非换行符，后跟换行符，然后添加匹配项的长度。 
        但是，当文件末尾包含大量非换行符时，这会导致灾难性的回溯问题。 
        为了避免这种情况，当前的实现只匹配换行符并使用 match.index 来获取正确的行开始索引
        */
        while ((match = lineEndingPattern.exec(this.text))) {
            this.lines.push(this.text.slice(this.lineStartIndices[this.lineStartIndices.length - 1], match.index));
            this.lineStartIndices.push(match.index + match[0].length);
        }
        this.lines.push(this.text.slice(this.lineStartIndices[this.lineStartIndices.length - 1]));

        // Cache for comments found using getComments().
        this._commentCache = new WeakMap();

        // don't allow modification of this object
        Object.freeze(this);
        Object.freeze(this.lines);
    }
    
    //其他方法
}
```

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

    //迭代配置规则
    Object.keys(configuredRules).forEach(...));

    //如果顶级节点是"Program"，则仅运行代码路径分析器，否则跳过
    const eventGenerator = nodeQueue[0].node.type === "Program"? new CodePathAnalyzer(...) : new NodeEventGenerator(...);

    nodeQueue.forEach(...);

    return lintingProblems;
}
```

### 迭代配置规则
```js
Object.keys(configuredRules).forEach(ruleId => {
    const severity = ConfigOps.getRuleSeverity(configuredRules[ruleId]);

    // not load disabled rules
    if (severity === 0) {
        return;
    }

    const rule = ruleMapper(ruleId);

    if (rule === null) {
        lintingProblems.push(createLintingProblem({ ruleId }));
        return;
    }

    const messageIds = rule.meta && rule.meta.messages;
    let reportTranslator = null;
    const ruleContext = Object.freeze(
        Object.assign(
            Object.create(sharedTraversalContext),
            {
                id: ruleId,
                options: getRuleOptions(configuredRules[ruleId]),
                report(...args) {

                    /*
                        * Create a report translator lazily.
                        * In a vast majority of cases, any given rule reports zero errors on a given
                        * piece of code. Creating a translator lazily avoids the performance cost of
                        * creating a new translator function for each rule that usually doesn't get
                        * called.
                        *
                        * Using lazy report translators improves end-to-end performance by about 3%
                        * with Node 8.4.0.
                        */
                    if (reportTranslator === null) {
                        reportTranslator = createReportTranslator({
                            ruleId,
                            severity,
                            sourceCode,
                            messageIds,
                            disableFixes
                        });
                    }
                    const problem = reportTranslator(...args);

                    if (problem.fix && rule.meta && !rule.meta.fixable) {
                        throw new Error("Fixable rules should export a `meta.fixable` property.");
                    }
                    lintingProblems.push(problem);
                }
            }
        )
    );

    const ruleListeners = createRuleListeners(rule, ruleContext);

    // 将规则中的所有选择器添加为侦听器
    Object.keys(ruleListeners).forEach(selector => {
        emitter.on(
            selector,
            timing.enabled
                ? timing.time(ruleId, ruleListeners[selector])
                : ruleListeners[selector]
        );
    });
});
```

#### `Rule`类型定义
```js
/**
 * @typedef {Object} Rule
 * @property {Function} create The factory of the rule.
 * @property {RuleMeta} meta The meta data of the rule.
 */

 /**
 * @typedef {Object} RuleMeta
 * @property {boolean} [deprecated] If `true` then the rule has been deprecated.
 * @property {RuleMetaDocs} docs The document information of the rule.
 * @property {"code"|"whitespace"} [fixable] The autofix type.
 * @property {Record<string,string>} [messages] The messages the rule reports.
 * @property {string[]} [replacedBy] The IDs of the alternative rules.
 * @property {Array|Object} schema The option schema of the rule.
 * @property {"problem"|"suggestion"|"layout"} type The rule type.
 */
 ```

# 专题：`@eslint/eslint`
## `CascadingConfigArrayFactory`
职责：处理配置文件的级联。

描述：这个类提供枚举每个文件的功能，这些文件与给定的`glob`模式和配置匹配

# 豆知识
## `Shebang`
在计算领域中，`Shebang`（也被称为`Hashbang`、`sh-bang`）是一个由井号和叹号构成的字符序列`#!`，其出现在文本文件的第一行的前两个字符。

### 为什么需要注释`Shebang`
在`parse`方法里对`Shebang`进行了替换，`#!` => `//`

在文件中存在`Shebang`的情况下，类`Unix`操作系统的程序加载器会分析`Shebang`后的内容，将这些内容作为解释器指令，并调用该指令，并将载有`Shebang`的文件路径作为该解释器的参数
`
例如，以指令`#!/bin/sh`开头的文件在执行时会实际调用`/bin/sh`程序（通常是`Bourne shell`或兼容的`shell`，例如`bash`、`dash`等）来执行。这行内容也是`shell`脚本的标准起始行。

使用`#!/usr/bin/env` 脚本解释器名称是一种常见的在不同平台上都能正确找到解释器的办法。

`Linux`的操作系统的文件一般是`UTF-8`编码。如果脚本文件是以`UTF-8`的`BOM`（`0xEF 0xBB 0xBF`）开头的，那么`exec`函数将不会启动`shebang`指定的解释器来执行该脚本。因此，`Linux`的脚本文件不应在文件开头包含`UTF-8`的`BOM`。

由于`#`符号在许多脚本语言中都是注释标识符，`Shebang`的内容会被这些脚本解释器自动忽略。 在`#`字符不是注释标识符的语言中，例如`Scheme`，解释器也可能忽略以`#!`开头的首行内容，以提供与`Shebang`的兼容性

然而，并不是每一种解释器都会自动忽略`shebang`行，例如对于下面的脚本，`cat`会把文件中的两行都输出到标准输出中。
```
#!/bin/cat
Hello world!
```

在这里处理文件内容时，可以明确对于`Eslint`脚本来说`#!`是应当被忽略的内容，它不应该被当作解释器指令被执行，而且换成标准注释`//`可以确保不会被输出到编译后的代码文件中。

## `BOM`
`BOM`全称`byte-order mark`（字节顺序标记）

## `glob pattern`【命令行通配符】
在计算机编程中，`glob pattern`用通配符指定文件名集。

具体语法见 [阮一峰 命令行通配符教程](http://www.ruanyifeng.com/blog/2018/09/bash-wildcards.html)

# 解答
## `eslint`是怎么默认读取`eslintrc.*`文件配置的？
涉及引用库：
1. `optionator` 是`js`选项解析和帮助生成库 
1. `@eslint/eslintrc` 此存储库包含 `ESLint` 的旧 `ESLintRC` 配置文件格式
2. `import-fresh` 每次导入模块都是全新的（使用`require`读取相同模块时其数据时共享的）

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

### 为什么需要新方案？
关于目前`eslintrc.*`方法存在的问题可以看这里[Config File Simplification](https://github.com/eslint/rfcs/pull/9)

为什么需要新的配置方案？

现有方案里的配置并不是执行时真正使用的配置数据，现在的逻辑是这样的：初始数据【在开发项目里提供】 => 扩展、覆盖、级联合并【由`Eslint`在内部处理】 => 真正使用的数据

这带来了一些问题：
1. 从提供的配置文件很难直接看出最终使用的配置，这一点可以通过`--print-config`缓解
1. 扩展、覆盖、级联合并的细节在`Eslint`的内部，这使得配置时的负担增加了，因为配置者需要了解`Eslint`背后的原理
1. 现在这种将合并放在内部进行统一处理的方案非常复杂，`Nicolas C. Zakas`将两份配置方案抽离出来成了独立的库，级联配置数组的库的复杂度和代码量都远远超过平铺方案，因为现有方案是集中统一处理，需要考虑所有情况，然后现在的方案还需要处理文件加载，所以非常复杂。而新方案只是提供一些方法，让开发者自己操作，方法之间相对独立，不是一个大流程，只需要保证这些基础方法正常就可以了。
1. 现在这套容易出错，而且一旦出错不容易处理，因为处理配置部分在库上，而不是本地

新的配置方案是什么呢？`Eslint`提供了一个库[`@humanwhocodes/config-array`](https://github.com/humanwhocodes/config-array)，开发者自己用这个库处理好配置数据给`Eslint`，就是这样，将处理的权力给开发者。好处有很多：
1. 可以直接在本地拿到最终配置，这样本地也可以写一些相关测试了
1. 处理的部分在本地，这样就不需要了解`Eslint`的合并原理了，处理逻辑就在本地
1. `Eslint`更纯粹了，因为它不再负责配置的打平处理、插件文件加载等处理了
1. 不容易出错，即使出错了也容易处理，因为逻辑就在本地，无论是调试还是修改都简单很多
1. 数据处理逻辑也变得简单了，在`Eslint`里处理需要考虑所有情况，代码量极大，逻辑也很复杂，而本地则没有这种负担，我们只需要考虑自己项目中的配置合并、扩展和覆盖即可
1. 配置本身也会变得简单，一些用于扩展、合并、覆盖的键，比如`extends`、`plugins`、`overrides`这些会被移除【其实这些本身就不是必要的】

### 新的配置方案
新的配置库见[`@humanwhocodes/config-array`](https://github.com/humanwhocodes/config-array)

目前计划进度见[`Implement Flat Config #13481`](https://github.com/eslint/eslint/issues/13481)

这个计划目前依旧在实现中，关注后续发展。

## `extends`是怎么生效的？

# 涉及一些依赖库
只做简单描述，每个库的具体资料我放到“资料”部分了，想要深入的了解可以进入链接

## `optionator`
简化命令行参数控制

## `debug`
方便控制`debug`

## `import-fresh`
保证每次引入文件都是全新的

## `@eslint/eslintrc`
级联配置方案

## `@humanwhocodes/config-array`
平铺配置方案

# 资料
- [eslint源码](https://github.com/eslint/eslint)
- [配置指南](https://eslint.bootcss.com/docs/user-guide/configuring)
- [插件指南](https://eslint.bootcss.com/docs/developer-guide/working-with-plugins)
- [规则指南](https://cn.eslint.org/docs/developer-guide/working-with-rules)
- [Code Path Analysis Details](https://cn.eslint.org/docs/developer-guide/code-path-analysis)
- [`wiki-Shebang`](https://zh.wikipedia.org/wiki/Shebang)
- [`BOM`](https://zh.wikipedia.org/wiki/%E4%BD%8D%E5%85%83%E7%B5%84%E9%A0%86%E5%BA%8F%E8%A8%98%E8%99%9F)
- [`glob (programming)`](https://en.wikipedia.org/wiki/Glob_(programming))
- [`npm optionator`](https://github.com/gkz/optionator)
- [`npm import-fresh`](https://github.com/sindresorhus/import-fresh)
- [`npm debug`](https://github.com/visionmedia/debug/blob/master/test.js)
- [阮一峰 命令行通配符教程](http://www.ruanyifeng.com/blog/2018/09/bash-wildcards.html)
- [JSON Schema](http://json-schema.org/)