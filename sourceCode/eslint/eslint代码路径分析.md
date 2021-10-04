[TOC]

ESLint 的规则可以使用代码路径，代码路径是程序的执行路径，它在诸如 `if` 语句中 分叉(fork)或添加(join)

```js
if (a && b) {
    foo();
}
bar();
```
![](https://cn.eslint.org/docs/developer-guide/code-path-analysis/helo.svg)

# `Objects`
程序用几个代码路径表示。代码路径用两种对象表示：`CodePath` 和 `CodePathSegment`。

# `CodePath`
`CodePath` 表示整个代码路径。这个对象存在于每个函数和全局中。`CodePath`具有代码路径的初始段和最终段的引用。

`CodePath` 具有以下属性：
- `id` (`string`) - 一个独特的字符串。各个规则可以使用 `id` 来保存每个代码路径的附加信息。
- `initialSegment` (`CodePathSegment`) - 此代码路径的初始段。
- `finalSegments` (`CodePathSegment[]`) - 包括返回和抛出的最终段。
- `returnedSegments` (`CodePathSegment[]`) - 仅包含返回的最终段。
- `thrownSegments` (`CodePathSegment[]`) - 仅包含抛出的最终段。
- `currentSegments` (`CodePathSegment[]`) - 当前位置的分段
- `upper` (`CodePath|null`) - 上层函数/全局作用域的代码路径。
- `childCodePaths` (`CodePath[]`) - 此代码路径包含的函数的代码路径。

# `CodePathSegment`
`CodePathSegment` 是代码路径的一部分。一个代码路径用多个 `CodePathSegment` 对象表示，类似于双向链表。与双向链表的区别在于有分叉【`forking`】和合并【`merging`】（next/prev 是复数）。

`CodePathSegment` 具有以下属性：
- `id` (`string`) - 一个唯一的字符串。各个规则可以使用 id 来保存每个段的附加信息。
- `nextSegments` (`CodePathSegment[]`) - 下一段。如果是分叉，则有两个或更多。如果是最终段，那就什么都没有
- `prevSegments` (`CodePathSegment[]`) - 上一段。如果是合并，则有两个或更多。如果是初始段，则什么都没有
- `reachable`  (`boolean`) - 一个标志，显示它是否可达。当前面有 `return`、`throw`、`break` 或 `continue` 时，这将变为 `false`。

# `Events`
有5个与代码路径相关的事件，您可以在规则中定义事件处理程序。
```js
module.exports = function(context) {
    return {
        /**
         * 在分析代码路径开始时被调用。
         * 在这个时候，代码路径对象只有初始段。
         *
         * @param {CodePath} codePath - 新的代码路径。
         * @param {ASTNode} node - 当前节点。
         * @returns {void}
         */
        "onCodePathStart": function(codePath, node) {
            // 用 codePath 做一些事情
        },

        /**
         * 在分析代码路径的结束时被调用。
         * 在这个时候，代码路径对象就完成了。【怎么理解“完成”这个概念？】
         *
         * @param {CodePath} codePath - 完成的代码路径。
         * @param {ASTNode} node - 当前节点。
         * @returns {void}
         */
        "onCodePathEnd": function(codePath, node) {
            // 用 codePath 做一些事情
        },

        /**
         * 在创建代码路径段时调用此方法。
         * 这说明代码路径被分叉或合并。
         * 这个时候，可以确定该段之前的段是否可到达。
         *
         * @param {CodePathSegment} segment - 新的代码路径段。
         * @param {ASTNode} node - 当前节点。
         * @returns {void}
         */
        "onCodePathSegmentStart": function(segment, node) {
            // 用 segment 做一些事情
        },

        /**
         * 在离开代码路径段时调用此方法。
         * 这个时候，已经没有下一段了（如果有，会执行onCodePathSegmentLoop）
         *
         * @param {CodePathSegment} segment - 离开的代码路径段。
         * @param {ASTNode} node - 当前节点。
         * @returns {void}
         */
        "onCodePathSegmentEnd": function(segment, node) {
            // 用 segment 做一些事情
        },

        /**
         * 当代码路径段循环时调用。
         * 通常段在创建时具有每个先前的段，但是当循环时，段作为新的先前段添加到现有段中。
         *
         * @param {CodePathSegment} fromSegment - 源代码路径段。
         * @param {CodePathSegment} toSegment - 目标代码路径段。
         * @param {ASTNode} node - 当前节点。
         * @returns {void}
         */
        "onCodePathSegmentLoop": function(fromSegment, toSegment, node) {
            // 用 segment 做一些事情
        }
    };
};
```

# 关于`onCodePathSegmentLoop`
当下一个段存在时，总是会触发此事件，直到循环结束

示例 1：
```js
while (a) {
    a = foo();
}
bar();
```

# 资料
- [code-path-analysis](https://cn.eslint.org/docs/developer-guide/code-path-analysis)