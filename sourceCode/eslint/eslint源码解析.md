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
//package.json
{
    "bin": {
        "eslint": "./bin/eslint.js"
    },
}
```

我们发现入口是`bin/eslint.js`文件

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

可以发现，核心内容在`../lib/cli`文件里：

![](https://pic.imgdb.cn/item/61371a3344eaada7398b36a6.jpg)

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

![](https://pic.imgdb.cn/item/6139828a44eaada7395e519d.jpg)

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

![](https://pic.imgdb.cn/item/613984e944eaada73961c9b1.jpg)

接下来进入`CLIEngine`的`constructor`，看看它在初始化的时候做了什么。

# `cli-engine/cli-engine.js`
```js
class CLIEngine {
    /**
     * Creates a new instance of the core CLI engine.
     * @param {CLIEngineOptions} providedOptions The options for this instance.
     */
    constructor(providedOptions) {
        const options = Object.assign(
            Object.create(null),
            defaultOptions,
            { cwd: process.cwd() },
            providedOptions
        );

        if (options.fix === void 0) {
            options.fix = false;
        }

        const additionalPluginPool = new Map();
        const cacheFilePath = getCacheFile(
            options.cacheLocation || options.cacheFile,
            options.cwd
        );
        const configArrayFactory = new CascadingConfigArrayFactory({
            additionalPluginPool,
            baseConfig: options.baseConfig || null,
            cliConfig: createConfigDataFromOptions(options),
            cwd: options.cwd,
            ignorePath: options.ignorePath,
            resolvePluginsRelativeTo: options.resolvePluginsRelativeTo,
            rulePaths: options.rulePaths,
            specificConfigPath: options.configFile,
            useEslintrc: options.useEslintrc,
            builtInRules,
            loadRules,
            eslintRecommendedPath: path.resolve(__dirname, "../../conf/eslint-recommended.js"),
            eslintAllPath: path.resolve(__dirname, "../../conf/eslint-all.js")
        });
        const fileEnumerator = new FileEnumerator({
            configArrayFactory,
            cwd: options.cwd,
            extensions: options.extensions,
            globInputPaths: options.globInputPaths,
            errorOnUnmatchedPattern: options.errorOnUnmatchedPattern,
            ignore: options.ignore
        });
        const lintResultCache =
            options.cache ? new LintResultCache(cacheFilePath, options.cacheStrategy) : null;
        const linter = new Linter({ cwd: options.cwd });

        /** @type {ConfigArray[]} */
        const lastConfigArrays = [configArrayFactory.getConfigArrayForFile()];

        // 存储私有数据
        internalSlotsMap.set(this, {
            additionalPluginPool,
            cacheFilePath,
            configArrayFactory,
            defaultIgnores: IgnorePattern.createDefaultIgnore(options.cwd),
            fileEnumerator,
            lastConfigArrays,
            lintResultCache,
            linter,
            options
        });

        // 为fixes设置特殊过滤器
        if (options.fix && options.fixTypes && options.fixTypes.length > 0) {
            debug(`Using fix types ${options.fixTypes}`);

            // throw an error if any invalid fix types are found
            validateFixTypes(options.fixTypes);

            // convert to Set for faster lookup
            const fixTypes = new Set(options.fixTypes);

            // save original value of options.fix in case it's a function
            const originalFix = (typeof options.fix === "function")
                ? options.fix : () => true;

            options.fix = message => {
                const rule = message.ruleId && getRule(message.ruleId, lastConfigArrays);
                const matches = rule && rule.meta && fixTypes.has(rule.meta.type);

                return matches && originalFix(message);
            };
        }
    }
    
    //...其他方法
}
```

![](https://pic.imgdb.cn/item/6139851144eaada7396218f9.jpg)

这里创建的私有数据先不深入，在接下来的使用到这些数据的时候，我们再进行解读。

现在我们对`new Eslint`已经有了一个基本的了解，接下来我们去了解下`engine.lintFiles(files)`的流程及其实现：

![](https://pic.imgdb.cn/item/6139832244eaada7395f19ea.jpg)

## `engine.lintFiles(files)`
```js
/**
 * 在文件和目录名称数组上执行当前配置
 * @param {string[]} patterns 文件名和目录名的数组
 * @returns {Promise<LintResult[]>} lint的结果
 */
async lintFiles(patterns) {
    //'patterns' 必须是非空字符串或非空字符串数组
    if (!isNonEmptyString(patterns) && !isArrayOfNonEmptyString(patterns)) {
        throw new Error("'patterns' must be a non-empty string or an array of non-empty strings");
    }

    const { cliEngine } = privateMembersMap.get(this);

    //处理由 CLIEngine linting 报告生成的 linting 结果以匹配 ESLint 类的 API
    return processCLIEngineLintReport(
        cliEngine,
        cliEngine.executeOnFiles(patterns)
    );
}
```

逻辑比较简单，不多做说明，值得关注是`processCLIEngineLintReport`和`cliEngine.executeOnFiles`

首先看一下`processCLIEngineLintReport`的源码

## `processCLIEngineLintReport`
```js
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
实现并不复杂，就是创建了一个`descriptor`对象，然后挂载到每个`result`的`usedDeprecatedRules`属性上。

接下来看一下`cliEngine.executeOnFiles`

## `cliEngine.executeOnFiles`
```js
class CLIEngine{
    //构造器及其他方法...

    /**
     * 在文件和目录名称数组上执行当前配置
     * @param {string[]} patterns 文件名和目录名数组
     * @returns {LintReport} 所有被 linted 文件的结果
     */
    executeOnFiles(patterns) {
        //取出数据
        const {
            cacheFilePath,
            fileEnumerator,
            lastConfigArrays,
            lintResultCache,
            linter,
            options: {
                allowInlineConfig,
                cache,
                cwd,
                fix,
                reportUnusedDisableDirectives
            }
        } = internalSlotsMap.get(this);

        const results = [];
        const startTime = Date.now();//debug输出的时候会用到，不是核心数据

        // 清除上次使用的配置数组
        lastConfigArrays.length = 0;

        //删除缓存文件
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

        // 迭代源代码文件
        // ...这段代码比较重要，代码量也较大，为方便阅读和理解，我放到了单独的子章节内

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

流程图：

![](https://pic.imgdb.cn/item/6139d62844eaada7390532f2.jpg)

有两个地方我比较关心，一个是迭代源代码文件进行的处理，一个是返回结果，首先从迭代源代码文件部分开始看吧

## 迭代源代码文件
```js
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

    if (lintResultCache) {
        //从lintResultCache取出缓存
        const cachedResult =
            lintResultCache.getCachedLintResults(filePath, config);

        //如果有缓存，添加到results
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
        * 将 lint 结果存储在 LintResultCache 中。
        * 注意：LintResultCache 将删除文件源和任何其他难以序列化的属性，并将在以后的 lint 运行中重新吸收这些属性。
        */
    if (lintResultCache) {
        lintResultCache.setCachedLintResults(filePath, config, result);
    }
}
```

![](https://pic.imgdb.cn/item/613ace8344eaada739684d9d.jpg)

这里值得关注的是`do lint`部分，调用了`verifyText`方法，我们看一下。

### `verifyText`
```js
/**
 * 使用 ESLint 处理源代码
 * @param {Object} config config对象
 * @param {string} config.text 要验证的源代码
 * @param {string} config.cwd 当前工作目录的路径
 * @param {string|undefined} config.filePath `text` 文件的路径。 如果未定义，则使用 `<text>`
 * @param {ConfigArray} config.config config数组
 * @param {boolean} config.fix 如果是`true`，则会进行修复
 * @param {boolean} config.allowInlineConfig 如果为`true`，则使用指令注释。
 * @param {boolean} config.reportUnusedDisableDirectives 如果为`true`，则报告未使用的 eslint-disable 注释。
 * @param {FileEnumerator} config.fileEnumerator 用于检查路径是否为目标的文件枚举器
 * @param {Linter} config.linter 要验证的 linter 实例
 * @returns {LintResult} lint结果
 * @private
 */
function verifyText({
    text,
    cwd,
    filePath: providedFilePath,
    config,
    fix,
    allowInlineConfig,
    reportUnusedDisableDirectives,
    fileEnumerator,
    linter
}) {
    const filePath = providedFilePath || "<text>";

    debug(`Lint ${filePath}`);

    /*
     * Verify.
     * `config.extractConfig(filePath)` 需要一个绝对路径
     * 但是 `linter` 不知道 cwd 的情况，所以通过 path.join 保证给 linter 的路径总是绝对路径。
     */
    const filePathToVerify = filePath === "<text>" ? path.join(cwd, filePath) : filePath;

    const { fixed, messages, output } = linter.verifyAndFix(
        text,
        config,
        {
            allowInlineConfig,
            filename: filePathToVerify,
            fix,
            reportUnusedDisableDirectives,

            /**
             * 检查 linter 是否应该采用给定的代码块。
             * @param {string} blockFilename 代码块的虚拟文件名。
             * @returns {boolean} 如果 linter 应该采用代码块则返回`true`
             */
            filterCodeBlock(blockFilename) {
                return fileEnumerator.isTargetPath(blockFilename);
            }
        }
    );

    // 调整并返回。
    const result = {
        filePath,
        messages,
        ...calculateStatsPerFile(messages) //计算每个文件的统计数据
    };

    if (fixed) {
        result.output = output;
    }
    if (
        //如果有错误且无输出
        result.errorCount + result.warningCount > 0 &&
        typeof result.output === "undefined"
    ) {
        result.source = text;
    }

    return result;
}
```

![](https://pic.imgdb.cn/item/613ecafd44eaada739b8e077.jpg)

接下来我们看一下`linter.verifyAndFix`这个方法

# `linter/linter.js`
## `linter.verifyAndFix`
```js
/**
 * 对文本执行多次自动修复，直到应用了尽可能多的修复
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
     * 此循环一直持续到以下情况之一为true：
     *
     * 1. 没有应用更多的修复程序。
     * 2. 已经通过了十次。
     *
     * 这意味着无论何时成功应用修复，都会有另一次通过。本质上，保证至少两次通过。
     */
    do {
        passNumber++;

        debug(`Linting code for ${debugTextDescription} (pass ${passNumber})`);
        messages = this.verify(currentText, config, options);

        debug(`Generating fixed text for ${debugTextDescription} (pass ${passNumber})`);
        fixedResult = SourceCodeFixer.applyFixes(currentText, messages, shouldFix);

        /*
            * 如果有任何语法错误，都会停止。
            * 'fixedResult.output' 会是一个空字符串。
            */
        if (messages.length === 1 && messages[0].fatal) {
            break;
        }

        // 确认是否已经被修复过 - 对返回值很重要
        fixed = fixed || fixedResult.fixed;

        // 更新：使用output而不是原始文本
        currentText = fixedResult.output;

    } while (
        fixedResult.fixed &&
        passNumber < MAX_AUTOFIX_PASSES
    );

    /*
        * 如果最后的结果被修复，我们需要再次检查以确保我们拥有最新的信息。
        */
    if (fixedResult.fixed) {
        fixedResult.messages = this.verify(currentText, config, options);
    }

    // 确保最后的结果正确反映修复是否完成
    fixedResult.fixed = fixed;
    fixedResult.output = currentText;

    return fixedResult;
}
```
这里可以看到`fixedResult`有三个值，稍微介绍下：
- `fixed` 是否修复
- `output` 修复后的结果
- `lintMessage[]` 由`LintMessage`组成的数组，`LintMessage`是对象类型，可以认为是报错信息（或者说Lint信息）

```js
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

## `linter.verify`
```js
/**
 * 根据第二个参数指定的规则验证文本.
 * @param {string|SourceCode} textOrSourceCode 要解析的文本或 SourceCode 对象。
 * @param {ConfigData|ConfigArray} config 一个 ESLintConfig 实例来配置一切。
 * @param {(string|(VerifyOptions&ProcessorOptions))} [filenameOrOptions] 被检查文件的可选文件名。
 *      如果未设置，则文件名将在rule上下文中默认为 '<input>'。 
 *      如果是对象，则它具有"filename", "allowInlineConfig"和一些属性。
 * @returns {LintMessage[]} 结果为消息数组或空数组（如果没有消息）。
 */
verify(textOrSourceCode, config, filenameOrOptions) {
    debug("Verify");
    const options = typeof filenameOrOptions === "string"
        ? { filename: filenameOrOptions }
        : filenameOrOptions || {};

    // 如果是 CLIEngine 传递一个 `ConfigArray` 实例，走这里
    if (config && typeof config.extractConfig === "function") {
        return this._verifyWithConfigArray(textOrSourceCode, config, options);
    }

    /*
    * `Linter` 不支持配置中的 `overrides` 属性。 
    * 所以我们不能应用多个处理器。
    */
    if (options.preprocess || options.postprocess) {
        return this._verifyWithProcessor(textOrSourceCode, config, options);
    }

    return this._verifyWithoutProcessors(textOrSourceCode, config, options);
}
```
`eslint`源码使用了`js-doc`规则进行注释，这里我们关注下`verify`的参数和返回值的类型。

### `@param {ConfigData|ConfigArray}`
#### `ConfigData`
```js
//定义在/shared/types.js

/**
 * @typedef {Object} ConfigData
 * @property {Record<string, boolean>} [env] 环境设置
 * @property {string | string[]} [extends] 其他配置文件的路径或可共享配置的包名称
 * @property {Record<string, GlobalConf>} [globals] 全局变量设置
 * @property {string | string[]} [ignorePatterns] 忽略 lint 的 glob 模式
 * @property {boolean} [noInlineConfig] 禁用指令注释的标志
 * @property {OverrideConfigData[]} [overrides] 每种文件的覆盖设置
 * @property {string} [parser] 解析器的路径或解析器的包名
 * @property {ParserOptions} [parserOptions] 解析器选项
 * @property {string[]} [plugins] 插件说明符
 * @property {string} [processor] 处理器说明符
 * @property {boolean} [reportUnusedDisableDirectives] 报告未使用的 `eslint-disable` 注释的标志
 * @property {boolean} [root] The root flag.
 * @property {Record<string, RuleConf>} [rules] The rule settings.
 * @property {Object} [settings] The shared settings.
 */
```

#### `ConfigArray`
`ConfigArray`类型定义比较特殊，在`1a9f17151a4e93eb17c8a2bf4f0a5320cce616de`提交中被`Nicholas C. Zakas`移除了，原因是`eslint`团队预期在未来实现平面配置，逐渐替代`eslintrc`进行配置，这是计划进程的一部分[#13481](https://github.com/eslint/eslint/issues/13481)

目前在[`7.30.0`](https://github.com/eslint/eslint/releases/tag/v7.30.0)版本中开始支持`FlatConfigArray`，需要使用[@humanwhocodes/config-array](https://www.npmjs.com/package/@humanwhocodes/config-array)库

这是移除文件里的`ConfigArray`定义
```js
/**
 * @fileoverview `ConfigArray` class.
 *
 *
 * `ConfigArray` 类表达了一个配置的全部内容。 
 * 它具有入口配置文件、扩展的基本配置文件、加载的解析器和加载的插件 
 *
 * `ConfigArray` 类提供了三个属性和两个方法。
 *
 * - `pluginEnvironments` 插件环境【Map类型，且不可修改】
 * - `pluginProcessors` 插件处理器【Map类型，且不可修改】
 * - `pluginRules`
 *      是`Map`类型，包含配置数组里所有插件的成员
 *      这个`Map`类型没有突变方法【？我理解是没有扩展新方法】
 *      使用的key是成员ID，比如`pluginId/memberName`
 * - `isRoot()` 如果是`true`，则它的配置属性里包含`root:true`
 * - `extractConfig(filePath)`
 *      `filePath` 参数必须是绝对路径。
 *      提取给定文件的最终配置. 
 *      会合并所有通过`criteria`属性匹配的配置数组元素（element.criteria是文件测试器）
 *
 * `ConfigArrayFactory` 提供配置文件的加载逻辑
 *
 * @author Toru Nagashima <https://github.com/mysticatea>
 */
```

### `@param {(string|(VerifyOptions&ProcessorOptions))}`
#### `VerifyOptions`
```js
/**
 * @typedef {Object} VerifyOptions
 * @property {boolean} [allowInlineConfig] 
 *      允许/禁止内联注释在设置后更改配置的能力
 *      如果未提供，则默认为 true
 *      如果想在没有注释覆盖规则的情况下验证 JS，则很有用.
 * @property {boolean} [disableFixes] 如果 `true` 则 linter 不会将 `fix` 属性放入 lint 结果中。
 * @property {string} [filename] 源代码的文件名
 * @property {boolean | "off" | "warn" | "error"} [reportUnusedDisableDirectives] 为未使用的 `eslint-disable` 指令添加报告的错误。
 */
```

#### `ProcessorOptions`
```js
/**
 * @typedef {Object} ProcessorOptions
 * @property {(filename:string, text:string) => boolean} [filterCodeBlock]  用于选择采用代码块的函数
 * @property {Processor.postprocess} [postprocess] 报告消息的后处理器
 *      如果提供，它应该接受从预处理器返回的每个代码块的消息列表数组，根据需要对消息应用映射，
 *      并返回一维消息数组。
 * @property {Processor.preprocess} [preprocess] 源文本预处理器.
 *      如果提供，它应该接受一串源文本，并将代码块数组返回给 lint。
 */
```

## `_verifyWithConfigArray`
```js
/**
 * 使用 `ConfigArray` 验证给定的代码。
 * @param {string|SourceCode} textOrSourceCode The source code.
 * @param {ConfigArray} configArray The config array.
 * @param {VerifyOptions&ProcessorOptions} options The options.
 * @returns {LintMessage[]} The found problems.
 */
_verifyWithConfigArray(textOrSourceCode, configArray, options) {
    debug("With ConfigArray: %s", options.filename);

    // 存储配置数组以便稍后获取插件环境和规则。
    internalSlotsMap.get(this).lastConfigArray = configArray;

    // 提取此文件的最终配置。
    const config = configArray.extractConfig(options.filename);
    const processor =
        config.processor &&
        configArray.pluginProcessors.get(config.processor);

    // Verify.
    if (processor) {
        debug("Apply the processor: %o", config.processor);
        const { preprocess, postprocess, supportsAutofix } = processor;
        const disableFixes = options.disableFixes || !supportsAutofix;

        return this._verifyWithProcessor(
            textOrSourceCode,
            config,
            { ...options, disableFixes, postprocess, preprocess },
            configArray
        );
    }
    return this._verifyWithoutProcessors(textOrSourceCode, config, options);
}
```

## `_verifyWithProcessor`
```js
/**
 * Verify with a processor.
 * @param {string|SourceCode} textOrSourceCode The source code.
 * @param {ConfigData|ExtractedConfig} config The config array.
 * @param {VerifyOptions&ProcessorOptions} options The options.
 * @param {ConfigArray} [configForRecursive] `ConfigArray` 对象递归地应用多个处理器。
 * @returns {LintMessage[]} The found problems.
 */
_verifyWithProcessor(textOrSourceCode, config, options, configForRecursive) {
    const filename = options.filename || "<input>";
    const filenameToExpose = normalizeFilename(filename);
    const physicalFilename = options.physicalFilename || filenameToExpose;
    const text = ensureText(textOrSourceCode);
    const preprocess = options.preprocess || (rawText => [rawText]);

    const postprocess = options.postprocess || (messagesList => messagesList.flat());
    const filterCodeBlock =
        options.filterCodeBlock ||
        (blockFilename => blockFilename.endsWith(".js"));
    const originalExtname = path.extname(filename);
    const messageLists = preprocess(text, filenameToExpose).map((block, i) => {
        debug("A code block was found: %o", block.filename || "(unnamed)");

        // Keep the legacy behavior.
        if (typeof block === "string") {
            return this._verifyWithoutProcessors(block, config, options);
        }

        const blockText = block.text;
        const blockName = path.join(filename, `${i}_${block.filename}`);

        // Skip this block if filtered.
        if (!filterCodeBlock(blockName, blockText)) {
            debug("This code block was skipped.");
            return [];
        }

        // Resolve configuration again if the file content or extension was changed.
        if (configForRecursive && (text !== blockText || path.extname(blockName) !== originalExtname)) {
            debug("Resolving configuration again because the file content or extension was changed.");
            return this._verifyWithConfigArray(
                blockText,
                configForRecursive,
                { ...options, filename: blockName, physicalFilename }
            );
        }

        // Does lint.
        return this._verifyWithoutProcessors(
            blockText,
            config,
            { ...options, filename: blockName, physicalFilename }
        );
    });

    return postprocess(messageLists, filenameToExpose);
}
```
可以看到这里两个核心处理函数`preprocess`、`postprocess`的默认来源都是`options`，那么`options`是什么时候设置的？

其实就在上一步`_verifyWithConfigArray`里，相关代码：
```js
const processor =
    config.processor &&
    configArray.pluginProcessors.get(config.processor);

if (processor) {
    debug("Apply the processor: %o", config.processor);
    const { preprocess, postprocess, supportsAutofix } = processor;
    const disableFixes = options.disableFixes || !supportsAutofix;

    return this._verifyWithProcessor(
        textOrSourceCode,
        config,
        { ...options, disableFixes, postprocess, preprocess },
        configArray
    );
}
```
可以看到，是从`configArray.pluginProcessors`取出来的。

> 传递链条比较长，我不一个个列举源码了，链条如下：

- `lint#_verifyWithProcessor` 
- -> `lint#_verifyWithConfigArray` 
- -> `lint#verify` 
- -> `cli-engine#executeOnFiles` 
- -> `fileEnumerator`
- -> `{ config, filePath, ignored }`（从迭代器里取出来）

## `_verifyWithoutProcessors`
```js
/**
 * 与 linter.verify 相同，但不支持处理器。
 * @param {string|SourceCode} textOrSourceCode The text to parse or a SourceCode object.
 * @param {ConfigData} providedConfig An ESLintConfig instance to configure everything.
 * @param {VerifyOptions} [providedOptions] 被检查文件的可选文件名。
 * @returns {LintMessage[]} The results as an array of messages or an empty array if no messages.
 */
_verifyWithoutProcessors(textOrSourceCode, providedConfig, providedOptions) {
    const slots = internalSlotsMap.get(this);
    const config = providedConfig || {};
    const options = normalizeVerifyOptions(providedOptions, config);//规范和验证options
    let text;

    // 设置slots.lastSourceCode、text
    if (typeof textOrSourceCode === "string") {
        slots.lastSourceCode = null;
        text = textOrSourceCode;
    } else {
        slots.lastSourceCode = textOrSourceCode;
        text = textOrSourceCode.text;
    }

    // 初始化解析器【默认】
    let parserName = DEFAULT_PARSER_NAME;
    let parser = espree;

    // 更换解析器
    if (typeof config.parser === "object" && config.parser !== null) {
        parserName = config.parser.filePath;
        parser = config.parser.definition;
    } else if (typeof config.parser === "string") {
        if (!slots.parserMap.has(config.parser)) {
            return [{
                ruleId: null,
                fatal: true,
                severity: 2,
                message: `Configured parser '${config.parser}' was not found.`,
                line: 0,
                column: 0
            }];
        }
        parserName = config.parser;
        parser = slots.parserMap.get(config.parser);
    }

    // 搜索并应用"eslint-env *".
    const envInFile = options.allowInlineConfig && !options.warnInlineConfig
        ? findEslintEnv(text)
        : {};
    const resolvedEnvConfig = Object.assign({ builtin: true }, config.env, envInFile);
    const enabledEnvs = Object.keys(resolvedEnvConfig)
        .filter(envName => resolvedEnvConfig[envName])
        .map(envName => getEnv(slots, envName))
        .filter(env => env);

    const parserOptions = resolveParserOptions(parser, config.parserOptions || {}, enabledEnvs);
    const configuredGlobals = resolveGlobals(config.globals || {}, enabledEnvs);
    const settings = config.settings || {};

    if (!slots.lastSourceCode) {
        const parseResult = parse(
            text,
            parser,
            parserOptions,
            options.filename
        );

        if (!parseResult.success) {
            return [parseResult.error];
        }

        slots.lastSourceCode = parseResult.sourceCode;
    } else {

        /*
            * If the given source code object as the first argument does not have scopeManager, analyze the scope.
            * This is for backward compatibility (SourceCode is frozen so it cannot rebind).
            */
        if (!slots.lastSourceCode.scopeManager) {
            slots.lastSourceCode = new SourceCode({
                text: slots.lastSourceCode.text,
                ast: slots.lastSourceCode.ast,
                parserServices: slots.lastSourceCode.parserServices,
                visitorKeys: slots.lastSourceCode.visitorKeys,
                scopeManager: analyzeScope(slots.lastSourceCode.ast, parserOptions)
            });
        }
    }

    const sourceCode = slots.lastSourceCode;
    const commentDirectives = options.allowInlineConfig
        ? getDirectiveComments(options.filename, sourceCode.ast, ruleId => getRule(slots, ruleId), options.warnInlineConfig)
        : { configuredRules: {}, enabledGlobals: {}, exportedVariables: {}, problems: [], disableDirectives: [] };

    // 用声明的全局变量扩大全局作用域
    addDeclaredGlobals(
        sourceCode.scopeManager.scopes[0],
        configuredGlobals,
        { exportedVariables: commentDirectives.exportedVariables, enabledGlobals: commentDirectives.enabledGlobals }
    );

    const configuredRules = Object.assign({}, config.rules, commentDirectives.configuredRules);

    let lintingProblems;

    try {
        lintingProblems = runRules(
            sourceCode,
            configuredRules,
            ruleId => getRule(slots, ruleId),
            parserOptions,
            parserName,
            settings,
            options.filename,
            options.disableFixes,
            slots.cwd,
            providedOptions.physicalFilename
        );
    } catch (err) {
        err.message += `\nOccurred while linting ${options.filename}`;
        debug("An error occurred while traversing");
        debug("Filename:", options.filename);
        if (err.currentNode) {
            const { line } = err.currentNode.loc.start;

            debug("Line:", line);
            err.message += `:${line}`;
        }
        debug("Parser Options:", parserOptions);
        debug("Parser Path:", parserName);
        debug("Settings:", settings);
        throw err;
    }

    return applyDisableDirectives({
        directives: commentDirectives.disableDirectives,
        problems: lintingProblems
            .concat(commentDirectives.problems)
            .sort((problemA, problemB) => problemA.line - problemB.line || problemA.column - problemB.column),
        reportUnusedDisableDirectives: options.reportUnusedDisableDirectives
    });
}
```

## `parse`
```js
/**
 * 将文本解析为 AST。 
 * 移到这里是因为 try-catch 阻止了函数的优化，所以最好保持 try-catch 尽可能孤立
 * @param {string} text The text to parse.
 * @param {Parser} parser The parser to parse.
 * @param {ParserOptions} providedParserOptions Options to pass to the parser
 * @param {string} filePath The path to the file being parsed.
 * @returns {{success: false, error: Problem}|{success: true, sourceCode: SourceCode}}
 * An object containing the AST and parser services if parsing was successful, or the error if parsing failed
 * @private
 */
function parse(text, parser, providedParserOptions, filePath) {
    const textToParse = stripUnicodeBOM(text).replace(astUtils.shebangPattern, (match, captured) => `//${captured}`);
    const parserOptions = Object.assign({}, providedParserOptions, {
        loc: true,
        range: true,
        raw: true,
        tokens: true,
        comment: true,
        eslintVisitorKeys: true,
        eslintScopeManager: true,
        filePath
    });

    /*
     * Check for parsing errors first. If there's a parsing error, nothing
     * else can happen. However, a parsing error does not throw an error
     * from this method - it's just considered a fatal error message, a
     * problem that ESLint identified just like any other.
     */
    try {
        const parseResult = (typeof parser.parseForESLint === "function")
            ? parser.parseForESLint(textToParse, parserOptions)
            : { ast: parser.parse(textToParse, parserOptions) };
        const ast = parseResult.ast;
        const parserServices = parseResult.services || {};
        const visitorKeys = parseResult.visitorKeys || evk.KEYS;
        const scopeManager = parseResult.scopeManager || analyzeScope(ast, parserOptions, visitorKeys);

        return {
            success: true,

            /*
             * Save all values that `parseForESLint()` returned.
             * If a `SourceCode` object is given as the first parameter instead of source code text,
             * linter skips the parsing process and reuses the source code object.
             * In that case, linter needs all the values that `parseForESLint()` returned.
             */
            sourceCode: new SourceCode({
                text,
                ast,
                parserServices,
                scopeManager,
                visitorKeys
            })
        };
    } catch (ex) {

        // If the message includes a leading line number, strip it:
        const message = `Parsing error: ${ex.message.replace(/^line \d+:/iu, "").trim()}`;

        debug("%s\n%s", message, ex.stack);

        return {
            success: false,
            error: {
                ruleId: null,
                fatal: true,
                severity: 2,
                message,
                line: ex.lineNumber,
                column: ex.column
            }
        };
    }
}
```

# `FileEnumerator`
查找过程略掉，可以发现`FileEnumerator`枚举的`config`是通过`configArrayFactory.getConfigArrayForFile`获得的，相关代码如下：
```js
//FileEnumerator *_iterateFilesRecursive方法中
config = configArrayFactory.getConfigArrayForFile(
    filePath,
    { ignoreNotFoundError: true }
);
```

关于`configArrayFactory`的类型定义：
```js
/**
 * @typedef {Object} FileEnumeratorInternalSlots
 * @property {CascadingConfigArrayFactory} configArrayFactory The factory for config arrays.
 * @property {string} cwd The base directory to start lookup.
 * @property {RegExp|null} extensionRegExp The RegExp to test if a string ends with specific file extensions.
 * @property {boolean} globInputPaths Set to false to skip glob resolution of input file paths to lint (default: true). If false, each input file paths is assumed to be a non-glob path to an existing file.
 * @property {boolean} ignoreFlag The flag to check ignored files.
 * @property {(filePath:string, dot:boolean) => boolean} defaultIgnores The default predicate function to ignore files.
 */
```

来源于：
```js
const {
    Legacy: {
        IgnorePattern,
        CascadingConfigArrayFactory
    }
} = require("@eslint/eslintrc");
```

关于`@eslint/eslintrc`也是之前提到的[#13481](https://github.com/eslint/eslint/issues/13481)计划的一部分。

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
- [`@eslint/eslintrc v0.4.3`](https://github.com/eslint/eslintrc/tree/v0.4.3)