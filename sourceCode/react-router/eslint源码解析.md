[TOC]

# 目标
解析两个常用的命令行参数：
- `--fix`
- `--int`

了解其他`eslint`参数：...

# 入口
```json5
{
    "bin": {
        "eslint": "./bin/eslint.js"
    },
}
```

# `bin/eslint.js`
```js
//helper和引用...

(async function main() {
    process.on("uncaughtException", onFatalError);
    process.on("unhandledRejection", onFatalError);

    // Call the config initializer if `--init` is present.
    if (process.argv.includes("--init")) {
        await require("../lib/init/config-initializer").initializeConfig();
        return;
    }

    // Otherwise, call the CLI.
    process.exitCode = await require("../lib/cli").execute(
        process.argv,
        process.argv.includes("--stdin") ? await readStdin() : null
    );
}()).catch(onFatalError);
```
其他部分先不用管，我们直接进入`../lib/cli`文件中。

# `lib/cli.js`
```js
//helper和引用...

const cli = {
   async execute(args, text) {
        if (Array.isArray(args)) {
            debug("CLI args: %o", args.slice(2));
        }

        //...将args解析成对象形式并赋值成变量options

        //...检查一些参数使用错误的情况并退出

        const engine = new ESLint(translateOptions(options));
        let results;

        if (useStdin) {
            //...
        } else {
            results = await engine.lintFiles(files);
        }

        if (options.fix) {
            debug("Fix mode enabled - applying fixes");
            await ESLint.outputFixes(results); //将修复后的结果写入文件
        }

        //...

        return 2;
    } 
}
```

# 涉及依赖库
- `v8-complie-cache` 使用 V8 的代码缓存加快实例化时间
- `debug` 代码调试
- `optionator` 解析命令行参数

# 资料
- [`eslint v7.32.0`](https://github.com/eslint/eslint/tree/v7.32.0#installation-and-usage)