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