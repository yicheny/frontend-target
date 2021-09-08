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

现在我们知道的：

![](https://pic.imgdb.cn/item/613719e144eaada7398a9c46.jpg)

# `bin/eslint.js`
```js
//helper和引用...

(async function main() {
    process.on("uncaughtException", onFatalError);
    process.on("unhandledRejection", onFatalError);

    // 如果存在`--init`，则调用配置初始值设定项。
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
到这里我们知道：

![](https://pic.imgdb.cn/item/61371a3344eaada7398b36a6.jpg)


其他部分先不管，优先关注`../lib/cli`文件。

# `lib/cli.js`
```js
//helper和引用...

const cli = {
    /**
     * 根据传入的参数数组执行 CLI
     * @param {string|Array|Object} args 处理的参数
     * @param {string} [text] 要 lint 的文本（用于 TTY）
     * @returns {Promise<number>} 操作的退出代码
     */
   async execute(args, text) {
        if (Array.isArray(args)) {
            debug("CLI args: %o", args.slice(2));
        }

        /** @type {ParsedCLIOptions} */
        let options;

        try {
            options = CLIOptions.parse(args);
        } catch (error) {
            log.error(error.message);
            return 2;
        }

        const files = options._;
        const useStdin = typeof text === "string";

        if (options.help) {
            log.info(CLIOptions.generateHelp());
            return 0;
        }
        if (options.version) {
            log.info(RuntimeInfo.version());
            return 0;
        }
        if (options.envInfo) {
            try {
                log.info(RuntimeInfo.environment());
                return 0;
            } catch (err) {
                log.error(err.message);
                return 2;
            }
        }

        if (options.printConfig) {
            if (files.length) {
                log.error("The --print-config option must be used with exactly one file name.");
                return 2;
            }
            if (useStdin) {
                log.error("The --print-config option is not available for piped-in code.");
                return 2;
            }

            const engine = new ESLint(translateOptions(options));
            const fileConfig =
                await engine.calculateConfigForFile(options.printConfig);

            log.info(JSON.stringify(fileConfig, null, "  "));
            return 0;
        }

        debug(`Running on ${useStdin ? "text" : "files"}`);

        if (options.fix && options.fixDryRun) {
            log.error("The --fix option and the --fix-dry-run option cannot be used together.");
            return 2;
        }
        if (useStdin && options.fix) {
            log.error("The --fix option is not available for piped-in code; use --fix-dry-run instead.");
            return 2;
        }
        if (options.fixType && !options.fix && !options.fixDryRun) {
            log.error("The --fix-type option requires either --fix or --fix-dry-run.");
            return 2;
        }

        //translateOptions将options转换为Eslint初始化需要的数据
        const engine = new ESLint(translateOptions(options));
        let results;

        if (useStdin) {
            results = await engine.lintText(text, {
                filePath: options.stdinFilename,
                warnIgnored: true
            });
        } else {
            results = await engine.lintFiles(files);
        }

        if (options.fix) {
            debug("Fix mode enabled - applying fixes");
            await ESLint.outputFixes(results);
        }

        let resultsToPrint = results;

        if (options.quiet) {
            debug("Quiet mode enabled - filtering out warnings");
            resultsToPrint = ESLint.getErrorResults(resultsToPrint);
        }

        if (await printResults(engine, resultsToPrint, options.format, options.outputFile)) {

            // Errors and warnings from the original unfiltered results should determine the exit code
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

        return 2;
    }
}
```

流程示意图：

![](https://pic.imgdb.cn/item/6138268644eaada73959cb1f.jpg)

## `lint-core`
重点查看`lint`部分的核心代码：
```js
const engine = new ESLint(translateOptions(options));
let results;

if (useStdin) {
    results = await engine.lintText(text, {
        filePath: options.stdinFilename,
        warnIgnored: true
    });
} else {
    results = await engine.lintFiles(files);
}

if (options.fix) {
    debug("Fix mode enabled - applying fixes");
    await ESLint.outputFixes(results);
}
```

## 核心处理
```js
//cli.execute方法中

//translateOptions：将 CLI 选项转换为 CLIEngine 预期的选项。
const engine = new ESLint(translateOptions(options));
let results;

if (useStdin) {//使用--stdin会走这里，先不关注
    results = await engine.lintText(text, {
        filePath: options.stdinFilename,
        warnIgnored: true
    }); 
} else {
    results = await engine.lintFiles(files);
}

if (options.fix) {
    debug("Fix mode enabled - applying fixes");
    await ESLint.outputFixes(results); //将修复后的结果写入文件
}
```

![](https://pic.imgdb.cn/item/6138949e44eaada7391b1201.jpg)

这部分代码里有三个点我认为较为重要，我们接下来深入解读这三个点：
1. 初始化`Eslint`
2. 调用`engine.lintText`
3. 调用`engine.lintFiles`

# `eslint/eslint.js`
这里先给出`constructor`部分的代码：
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

![](https://pic.imgdb.cn/item/6138954b44eaada7391be0de.jpg)

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

        // 清除上次使用的配置数组。
        lastConfigArrays.length = 0;

        if (!cache) {
            //...删除缓存文件
        }

        // 迭代源代码文件
        for (const { config, filePath, ignored } of fileEnumerator.iterateFiles(patterns)) {
            if (ignored) {
                results.push(createIgnoreResult(filePath, cwd));
                continue;
            }

            /*
             * Store used configs for:
             * - 此方法用于收集已使用的已弃用规则
             * - `getRules()` 方法用于收集所有加载的规则
             * - `--fix-type` 选项用于获取加载规则的元数据
             */
            if (!lastConfigArrays.includes(config)) {
                lastConfigArrays.push(config);
            }

            // 如果有缓存结果则跳过
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
                //...一系列参数
            });

            results.push(result);

            /*
             * 将 lint 结果存储在 LintResultCache 中。
             * 注意：LintResultCache 将删除文件源和任何其他难以序列化的属性，并将在以后的 lint 运行中重新吸收这些属性。
             */
            if (lintResultCache) {
                lintResultCache.setCachedLintResults(filePath, config, result);
            }
        }

        // 将缓存持久化到磁盘
        if (lintResultCache) {
            lintResultCache.reconcile();
        }

        debug(`Linting complete in: ${Date.now() - startTime}ms`);
        let usedDeprecatedRules;

        return {
            results,
            ...calculateStatsPerRun(results),

            // 惰性初始化，因为 CLI 和 `ESLint` API 不使用它
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
```

可以看到`lint`是通过`verifyText`处理的，这个方法底层使用的是`linter.verifyAndFix`，我们直接看`linter.verifyAndFix`方法

# `linter/linter.js`
## `linter.verifyAndFix`
```js
class Linter{
    /**
     * Performs multiple autofix passes over the text until as many fixes as possible
     * have been applied.
     * @param {string} text The source text to apply fixes to.
     * @param {ConfigData|ConfigArray} config The ESLint config object to use.
     * @param {VerifyOptions&ProcessorOptions&FixOptions} options The ESLint options object to use.
     * @returns {{fixed:boolean,messages:LintMessage[],output:string}} The result of the fix operation as returned from the
     *      SourceCodeFixer.
     */
    verifyAndFix(text, config, options) {
        let messages = [],
            fixedResult,
            fixed = false,
            passNumber = 0,
            currentText = text;
        const debugTextDescription = options && options.filename || `${text.slice(0, 10)}...`;
        const shouldFix = options && typeof options.fix !== "undefined" ? options.fix : true;

        /**
         * This loop continues until one of the following is true:
         *
         * 1. No more fixes have been applied.
         * 2. Ten passes have been made.
         *
         * That means anytime a fix is successfully applied, there will be another pass.
         * Essentially, guaranteeing a minimum of two passes.
         */
        do {
            passNumber++;

            debug(`Linting code for ${debugTextDescription} (pass ${passNumber})`);
            messages = this.verify(currentText, config, options);

            debug(`Generating fixed text for ${debugTextDescription} (pass ${passNumber})`);
            fixedResult = SourceCodeFixer.applyFixes(currentText, messages, shouldFix);

            /*
             * stop if there are any syntax errors.
             * 'fixedResult.output' is a empty string.
             */
            if (messages.length === 1 && messages[0].fatal) {
                break;
            }

            // keep track if any fixes were ever applied - important for return value
            fixed = fixed || fixedResult.fixed;

            // update to use the fixed output instead of the original text
            currentText = fixedResult.output;

        } while (
            fixedResult.fixed &&
            passNumber < MAX_AUTOFIX_PASSES
        );

        /*
         * If the last result had fixes, we need to lint again to be sure we have
         * the most up-to-date information.
         */
        if (fixedResult.fixed) {
            fixedResult.messages = this.verify(currentText, config, options);
        }

        // ensure the last result properly reflects if fixes were done
        fixedResult.fixed = fixed;
        fixedResult.output = currentText;

        return fixedResult;
    }
}
```

## `SourceCodeFixer.applyFixes`

## `linter.verify`

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