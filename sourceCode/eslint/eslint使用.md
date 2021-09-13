[TOC]

# 安装
`$ npm install eslint --save-dev`

# 生成配置文件
`$ eslint --init`

会在文件夹中自动创建`.eslintrc`文件

# 配置
`ESlint` 被设计为完全可配置的，有两种主要的方式来配置 `ESLint`：
1. `Configuration Comments` - 使用 `JavaScript` 注释把配置信息直接嵌入到一个代码源文件中。
1. `Configuration Files` - 使用 `JavaScript、JSON` 或者 `YAML` 文件为整个目录（处理你的主目录）和它的子目录指定配置信息。可以配置一个独立的 `.eslintrc.*` 文件，或者直接在 `package.json` 文件里的 `eslintConfig` 字段指定配置，`ESLint` 会查找和自动读取它们，再者，你可以在命令行运行时指定一个任意的配置文件。

## `parserOptions`
`ESLint` 允许你指定你想要支持的 `JavaScript` 语言选项。默认情况下，`ESLint` 支持 `ECMAScript 5` 语法。你可以覆盖该设置，以启用对 `ECMAScript` 其它版本和 `JSX` 的支持。
```json
{
    "parserOptions": {
        "ecmaVersion": 6,
        "sourceType": "module",
        "ecmaFeatures": {
            "jsx": true
        }
    },
}
```

## `parser`
`ESLint` 默认使用`Espree`作为其解析器，你可以在配置文件中指定一个不同的解析器，只要该解析器符合下列要求：

它必须是一个 `Node` 模块，可以从它出现的配置文件中加载。通常，这意味着应该使用 `npm` 单独安装解析器包。
它必须符合 `parser interface`。

```json
{
    "parser": "esprima",
}
```

## `Processor`
插件可以提供处理器。处理器可以从另一种文件中提取 `JavaScript` 代码，然后让 ESLint 检测 `JavaScript` 代码。或者处理器可以在预处理中转换 `JavaScript` 代码。

若要在配置文件中指定处理器，请使用 `processor` 键，并使用由插件名和处理器名组成的串接字符串加上斜杠。例如，下面的选项启用插件 `a-plugin` 提供的处理器 `a-processor`：
```json
{
    "plugins": ["a-plugin"],
    "processor": "a-plugin/a-processor"
}
```

### 为特定类型的文件指定处理器
要为特定类型的文件指定处理器，请使用 `overrides` 键和 `processor` 键的组合。例如，下面对 `*.md` 文件使用处理器 `a-plugin/markdown`。

```json
{
    "plugins": ["a-plugin"],
    "overrides": [
        {
            "files": ["*.md"],
            "processor": "a-plugin/markdown"
        }
    ]
}
```

### 为生成代码块指定处理器
处理器可以生成代码块，比如说从`a.md`文件提取代码块生成`0.js` 和 `1.js`文件。

`ESLint` 将这样的命名代码块作为原始文件的子文件处理。你可以在配置的 `overrides` 部分为已命名的代码块指定附加配置。例如，下面的命令对以 `.js` 结尾的 `markdown` 文件中的已命名代码块禁用 `strict` 规则。
```json
{
    "plugins": ["a-plugin"],
    "overrides": [
        {
            "files": ["*.md"],
            "processor": "a-plugin/markdown"
        },
        {
            "files": ["**/*.md/*.js"],
            "rules": {
                "strict": "off"
            }
        }
    ]
}
```

## `Environments`
一个环境定义了一组预定义的全局变量。

可用全局变量见[`environments`](https://cn.eslint.org/docs/user-guide/configuring#specifying-environments)

这些环境并不是互斥的，所以你可以同时定义多个。

可以在源文件里、在配置文件中或使用 命令行 的 --env 选项来指定环境。

要在你的 `JavaScript` 文件中使用注释来指定环境，格式如下：
```js
/* eslint-env node, mocha */
```
该设置启用了 `Node.js` 和 `Mocha` 环境。

配置文件启用【配置文件有很多种，这里只以`json`类型举例】：
```json
{
    "env": {
        "browser": true,
        "node": true
    }
}
```

## `Globals`
当访问当前源文件内未定义的变量时，`no-undef` 规则将发出警告。如果你想在一个源文件里使用全局变量，推荐你在 `ESLint` 中定义这些全局变量，这样 `ESLint` 就不会发出警告了。你可以使用注释或在配置文件中定义全局变量。

要在你的 `JavaScript` 文件中，用注释指定全局变量，格式如下：
```js
/* global var1, var2 */
```

这定义了两个全局变量，`var1` 和 `var2`。如果你想选择性地指定这些全局变量可以被写入(而不是只被读取)，那么你可以用一个 `"writable"` 的标志来设置它们:
```js
/* global var1:writable, var2:writable */
```

文件配置:
```json
{
    "globals": {
        "var1": "writable",
        "var2": "readonly"
    }
}
```

## `Plugins`
`ESLint` 支持使用第三方插件。在使用插件之前，你必须使用 `npm` 安装它。

在配置文件里配置插件时，可以使用 `plugins` 关键字来存放插件名字的列表。插件名称可以省略 `eslint-plugin-` 前缀。
```json
{
    "plugins": [
        "plugin1",
        "eslint-plugin-plugin2"
    ]
}
```

## `Rules`
`ESLint` 附带有大量的规则。你可以使用注释或配置文件修改你项目中要使用的规则。要改变一个规则设置，你必须将规则 `ID` 设置为下列值之一：
- `"off"` 或 `0` - 关闭规则
- `"warn"` 或 `1` - 开启规则，使用警告级别的错误：`warn` (不会导致程序退出)
- `"error"` 或 `2` - 开启规则，使用错误级别的错误：`error` (当被触发的时候，程序会退出)

### 注释配置
为了在文件注释里配置规则，使用以下格式的注释：
```js
/* eslint eqeqeq: "off", curly: "error" */
```
在这个例子里，`eqeqeq` 规则被关闭，`curly` 规则被打开，定义为错误级别。你也可以使用对应的数字定义规则严重程度：
```js
/* eslint eqeqeq: 0, curly: 2 */
```
这个例子和上个例子是一样的，只不过它是用的数字而不是字符串。`eqeqeq` 规则是关闭的，`curly` 规则被设置为错误级别。

如果一个规则有额外的选项，你可以使用数组字面量指定它们，比如：
```js
/* eslint quotes: ["error", "double"], curly: 2 */
```
这条注释为规则 `quotes` 指定了 `"double"` 选项。数组的第一项总是规则的严重程度（数字或字符串）。

在这些配置文件中，规则 `plugin1/rule1` 表示来自插件 `plugin1` 的 `rule1` 规则。你也可以使用这种格式的注释配置，比如：
```js
/* eslint "plugin1/rule1": "error" */
```
注意：当指定来自插件的规则时，确保删除 `eslint-plugin-` 前缀。`ESLint` 在内部只使用没有前缀的名称去定位规则。

### 文件配置
```json5
{
    "rules": {
        "eqeqeq": "off",
        "curly": "error",
        "quotes": ["error", "double"],
        "plugin1/rule1": "error"//配置定义在插件中的一个规则的时候，你必须使用 插件名/规则ID 的形式
    }
}
```

## `settings`

# 关于配置文件
- `JavaScript` - 使用 `.eslintrc.js` 然后输出一个配置对象。
- `YAML` - 使用 `.eslintrc.yaml` 或 `.eslintrc.yml` 去定义配置的结构。
- `JSON` - 使用 `.eslintrc.json` 去定义配置的结构，`ESLint` 的 `JSON` 文件允许 `JavaScript` 风格的注释。
- (**弃用**) - 使用 `.eslintrc`，可以使 `JSON` 也可以是 `YAML`。
- `package.json` - 在 `package.json` 里创建一个 `eslintConfig`属性，在那里定义你的配置。

如果同一个目录下有多个配置文件，`ESLint` 只会使用一个。优先级顺序如下：
1. `.eslintrc.js`
1. `.eslintrc.yaml`
1. `.eslintrc.yml`
1. `.eslintrc.json`
1. `.eslintrc`
1. `package.json`

## 层叠配置

## 继承

# 禁止警告
## 通过注释
可以在你的文件中使用以下格式的块注释来临时禁止规则出现警告：
```js
/* eslint-disable */

alert('foo');

/* eslint-enable */
```

你也可以对指定的规则启用或禁用警告:
```js
/* eslint-disable no-alert, no-console */

alert('foo');
console.log('bar');

/* eslint-enable no-alert, no-console */
```

如果在整个文件范围内禁止规则出现警告，将 `/* eslint-disable */` 块注释放在文件顶部：
```js
/* eslint-disable */

alert('foo');
```

你也可以对整个文件启用或禁用警告:
```js
/* eslint-disable no-alert */

// Disables no-alert for the rest of the file
alert('foo');
```

可以在你的文件中使用以下格式的行注释或块注释在某一特定的行上禁用所有规则：
```js
alert('foo'); // eslint-disable-line

// eslint-disable-next-line
alert('foo');

/* eslint-disable-next-line */
alert('foo');

alert('foo'); /* eslint-disable-line */
```

在某一特定的行上禁用某个指定的规则：
```js
alert('foo'); // eslint-disable-line no-alert

// eslint-disable-next-line no-alert
alert('foo');

alert('foo'); /* eslint-disable-line no-alert */

/* eslint-disable-next-line no-alert */
alert('foo');
```

在某个特定的行上禁用多个规则：
```js
alert('foo'); // eslint-disable-line no-alert, quotes, semi

// eslint-disable-next-line no-alert, quotes, semi
alert('foo');

alert('foo'); /* eslint-disable-line no-alert, quotes, semi */

/* eslint-disable-next-line no-alert, quotes, semi */
alert('foo');
```

上面的所有方法同样适用于插件规则。例如，禁止 eslint-plugin-example 的 rule-name 规则，把插件名（example）和规则名（rule-name）结合为 example/rule-name：
```js
foo(); // eslint-disable-line example/rule-name
foo(); /* eslint-disable-line example/rule-name */
```

注意：为文件的某部分禁用警告的注释，告诉 `ESLint` 不要对禁用的代码报告规则的冲突。`ESLint` 仍解析整个文件，然而，禁用的代码仍需要是有效的 `JavaScript` 语法。

## 通过文件
若要禁用一组文件的配置文件中的规则，请使用 `overrides` 和 `files`。例如:
```json
{
  "rules": {...},
  "overrides": [
    {
      "files": ["*-test.js","*.spec.js"],
      "rules": {
        "no-unused-expressions": "off"
      }
    }
  ]
}
```

# 资料
- [`Configuring ESLint`](https://cn.eslint.org/docs/user-guide/configuring)
- [命令行](https://cn.eslint.org/docs/user-guide/command-line-interface)