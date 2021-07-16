import pathToRegexp from "path-to-regexp";

const cache = {};
const cacheLimit = 10000;
let cacheCount = 0;

//这里使用了path-to-regexp库，作用是将诸如 /user/:name 之类的路径字符串转换为正则表达式。
//详见：https://github.com/pillarjs/path-to-regexp
function compilePath(path, options) {
    const cacheKey = `${options.end}${options.strict}${options.sensitive}`;
    const pathCache = cache[cacheKey] || (cache[cacheKey] = {});

    if (pathCache[path]) return pathCache[path];

    const keys = [];
    const regexp = pathToRegexp(path, keys, options);
    const result = { regexp, keys };

    if (cacheCount < cacheLimit) {
        pathCache[path] = result;
        cacheCount++;
    }

    return result;
}

/**
 * Public API for matching a URL pathname to a path.
 */
function matchPath(pathname, options = {}) {
    if (typeof options === "string" || Array.isArray(options)) {
        options = { path: options };
    }

    const { path, exact = false, strict = false, sensitive = false } = options;

    const paths = [].concat(path);

    return paths.reduce((matched, path) => {
        if (!path && path !== "") return null;
        if (matched) return matched;

        //regexp是生成的正则表达式，keys是从路径中匹配出来的键组成的数组
        const { regexp, keys } = compilePath(path, {
            end: exact, //为 true 时，regexp 将匹配到字符串的末尾
            strict, //为 true 时，regexp 不允许可选的尾随定界符匹配
            sensitive //为 true 时，regexp 区分大小写
        });
        const match = regexp.exec(pathname);

        if (!match) return null;

        const [url, ...values] = match;
        const isExact = pathname === url;

        if (exact && !isExact) return null;

        return {
            path, // 用于匹配的path
            url: path === "/" && url === "" ? "/" : url, // URL的匹配部分
            isExact, // 是否完全匹配
            params: keys.reduce((memo, key, index) => {
                memo[key.name] = values[index];
                return memo;
            }, {})
        };
    }, null);
}

export default matchPath;

//关于可选的尾随定界符，比如^\/app((\/\w+)+|\/?)$，它可以匹配:
// /app
// /app/foo/bar
// /app/*
// ... 

//可以看这个问题：https://stackoverflow.com/questions/8917209/regex-match-for-optional-trailing-slash