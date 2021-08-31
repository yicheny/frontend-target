[TOC]

# 目标
## 参数
解析两个常用的命令行参数：
- `--fix`
- `--init`

了解其他`eslint`参数：...

## 细节
- `eslint`是怎么读取`eslintrc.*`文件的？
- `eslint`是怎么使用插件并运行规则的？
- `eslint`是怎么转换`AST`的？
- `eslint`是怎么遍历文件的？

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

        //...将args解析成对象形式并赋值给变量options

        //...检查一些参数使用错误的情况并退出

        //translateOptions：将 CLI 选项转换为 CLIEngine 预期的选项。
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

# `eslint/eslint.js`
```js
class ESLint {
    constructor(options = {}) {
        const processedOptions = processOptions(options);//验证和规范化包装的 CLIEngine 实例的选项。
        const cliEngine = new CLIEngine(processedOptions);//这一步非常关键，注意！
        const { additionalPluginPool,  configArrayFactory, lastConfigArrays } = getCLIEngineInternalSlots(cliEngine);

        let updated = false;

        // 使用additionalPluginPool的原因是避免使用`addPlugin(id, plugin)`导致缓存被重置
        if (options.plugins) {
            for (const [id, plugin] of Object.entries(options.plugins)) {
                additionalPluginPool.set(id, plugin);
                updated = true;
            }
        }

        // 直接操作 `configArrayFactory` 内部插槽，因为此功能【overrideConfig】不作为 CLIEngine 的公共 API 存在。
        if (hasDefinedProperty(options.overrideConfig)) {
            configArrayFactory.setOverrideConfig(options.overrideConfig);
            updated = true;
        }

        // 更新缓存
        if (updated) {
            configArrayFactory.clearCache();
            lastConfigArrays[0] = configArrayFactory.getConfigArrayForFile();
        }

        // 初始化私有属性
        privateMembersMap.set(this, {
            cliEngine,
            options: processedOptions
        });
    }

    //...其他方法
}
```

# `cli-engine/cli-engine.js`
```js
class CLIEngine {
    constructor(providedOptions) {
        const options = Object.assign( Object.create(null), defaultOptions,{ cwd: process.cwd() }, providedOptions );

        if (options.fix === void 0) {
            options.fix = false;
        }

        const additionalPluginPool = new Map();
        const cacheFilePath = getCacheFile( options.cacheLocation || options.cacheFile, options.cwd);
        const configArrayFactory = new CascadingConfigArrayFactory({
            //...一系列参数配置
        });
        const fileEnumerator = new FileEnumerator({
            //...一系列参数配置
        });
        const lintResultCache =
            options.cache ? new LintResultCache(cacheFilePath, options.cacheStrategy) : null;
        const linter = new Linter({ cwd: options.cwd });

        /** @type {ConfigArray[]} */
        const lastConfigArrays = [configArrayFactory.getConfigArrayForFile()];

        // 存储私有数据
        internalSlotsMap.set(this, {
            //...一系列数据
        });

        // 为修复设置特殊过滤器
        if (options.fix && options.fixTypes && options.fixTypes.length > 0) {
            //...
        }
    }
    
    //...其他方法
}
```
初始化`CLIEngine`创建了很多内容，看一下这些被创建的内容：
- `additionalPluginPool` 附加插件池
- `cacheFilePath` 缓存文件路径，如果没有被设置，那么默认是`.eslintcache`
- `configArrayFactory`
- `fileEnumerator`
- `lintResultCache`
- `linter`
- `lastConfigArrays`
- `internalSlotsMap` 这个是全局数据（`WeakMap`类型），以`CLIEngine`实例为键存储数据

## `executeOnFiiles`方法
```js
class CLIEngine{
    //构造器及其他方法...

    /**
     * Executes the current configuration on an array of file and directory names.
     * @param {string[]} patterns An array of file and directory names.
     * @returns {LintReport} The results for all files that were linted.
     */
    executeOnFiles(patterns) {
        const {
            //...取出数据
        } = internalSlotsMap.get(this);
        const results = [];
        const startTime = Date.now();

        // Clear the last used config arrays.
        lastConfigArrays.length = 0;

        // Delete cache file; should this do here?
        if (!cache) {
            try {
                fs.unlinkSync(cacheFilePath);
            } catch (error) {
                const errorCode = error && error.code;

                // Ignore errors when no such file exists or file system is read only (and cache file does not exist)
                if (errorCode !== "ENOENT" && !(errorCode === "EROFS" && !fs.existsSync(cacheFilePath))) {
                    throw error;
                }
            }
        }

        // Iterate source code files.
        for (const { config, filePath, ignored } of fileEnumerator.iterateFiles(patterns)) {
            if (ignored) {
                results.push(createIgnoreResult(filePath, cwd));
                continue;
            }

            /*
             * Store used configs for:
             * - this method uses to collect used deprecated rules.
             * - `getRules()` method uses to collect all loaded rules.
             * - `--fix-type` option uses to get the loaded rule's meta data.
             */
            if (!lastConfigArrays.includes(config)) {
                lastConfigArrays.push(config);
            }

            // Skip if there is cached result.
            if (lintResultCache) {
                const cachedResult =
                    lintResultCache.getCachedLintResults(filePath, config);

                if (cachedResult) {
                    const hadMessages =
                        cachedResult.messages &&
                        cachedResult.messages.length > 0;

                    if (hadMessages && fix) {
                        debug(`Reprocessing cached file to allow autofix: ${filePath}`);
                    } else {
                        debug(`Skipping file since it hasn't changed: ${filePath}`);
                        results.push(cachedResult);
                        continue;
                    }
                }
            }

            // Do lint.
            const result = verifyText({
                text: fs.readFileSync(filePath, "utf8"),
                filePath,
                config,
                cwd,
                fix,
                allowInlineConfig,
                reportUnusedDisableDirectives,
                fileEnumerator,
                linter
            });

            results.push(result);

            /*
             * Store the lint result in the LintResultCache.
             * NOTE: The LintResultCache will remove the file source and any
             * other properties that are difficult to serialize, and will
             * hydrate those properties back in on future lint runs.
             */
            if (lintResultCache) {
                lintResultCache.setCachedLintResults(filePath, config, result);
            }
        }

        // Persist the cache to disk.
        if (lintResultCache) {
            lintResultCache.reconcile();
        }

        debug(`Linting complete in: ${Date.now() - startTime}ms`);
        let usedDeprecatedRules;

        return {
            results,
            ...calculateStatsPerRun(results),

            // Initialize it lazily because CLI and `ESLint` API don't use it.
            get usedDeprecatedRules() {
                if (!usedDeprecatedRules) {
                    usedDeprecatedRules = Array.from(
                        iterateRuleDeprecationWarnings(lastConfigArrays)
                    );
                }
                return usedDeprecatedRules;
            }
        };
    }
}

# 问题
1. 测试无法运行
2. `package.json`中依赖项为`file:*`的内容无法安装

# 涉及依赖库
- `v8-complie-cache` 使用 V8 的代码缓存加快实例化时间
- `debug` 代码调试
- `optionator` 解析命令行参数
- `@eslint/eslintrc`

# 资料
- [`eslint v7.32.0`](https://github.com/eslint/eslint/tree/v7.32.0#installation-and-usage)