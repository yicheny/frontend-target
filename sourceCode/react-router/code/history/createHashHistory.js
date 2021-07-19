import warning from 'tiny-warning';
import invariant from 'tiny-invariant';

import { createLocation, locationsAreEqual } from './LocationUtils';
import {
    addLeadingSlash,
    stripLeadingSlash,
    stripTrailingSlash,
    hasBasename,
    stripBasename,
    createPath
} from './PathUtils';
import createTransitionManager from './createTransitionManager';
import {
    canUseDOM,
    getConfirmation,
    supportsGoWithoutReloadUsingHash
} from './DOMUtils';

const HashChangeEvent = 'hashchange';

const HashPathCoders = {
    hashbang: {
        encodePath: path =>
            path.charAt(0) === '!' ? path : '!/' + stripLeadingSlash(path),
        decodePath: path => (path.charAt(0) === '!' ? path.substr(1) : path)
    },
    noslash: {
        encodePath: stripLeadingSlash,
        decodePath: addLeadingSlash
    },
    slash: {
        encodePath: addLeadingSlash,
        decodePath: addLeadingSlash
    }
};

function getHashPath() {
    //不能在这里使用window.location.hash 因为在浏览器之间不一致——firefox会预解码它
    const href = window.location.href;
    const hashIndex = href.indexOf('#');
    return hashIndex === -1 ? '' : href.substring(hashIndex + 1);
}

function pushHashPath(path) {
    window.location.hash = path;
}

function replaceHashPath(path) {
    const hashIndex = window.location.href.indexOf('#');
    window.location.replace(
        window.location.href.slice(0, hashIndex >= 0 ? hashIndex : 0) + '#' + path
    );
}

function createHashHistory(props = {}) {
    invariant(canUseDOM, 'Hash history needs a DOM');

    const globalHistory = window.history;
    //如果使用带有散列历史记录的go(n)导致页面重新加载，则返回false
    const canGoWithoutReload = supportsGoWithoutReloadUsingHash();

    const { getUserConfirmation = getConfirmation, hashType = 'slash' } = props;
    //将basename处理成标准格式
    const basename = props.basename
        ? stripTrailingSlash(addLeadingSlash(props.basename)) //处理首尾的斜线
        : '';

    const { encodePath, decodePath } = HashPathCoders[hashType];

    function getDOMLocation() {
        //decode得到原有的path
        let path = decodePath(getHashPath());

        warning(
            !basename || hasBasename(path, basename),
            'You are attempting to use a basename on a page whose URL path does not begin ' +
            'with the basename. Expected path "' +
            path +
            '" to begin with "' +
            basename +
            '".'
        );

        //stripBasename: 如果path以basename开头，则返回basename后面的部分，否则返回path
        if (basename) path = stripBasename(path, basename);

        return createLocation(path);
    }

    const transitionManager = createTransitionManager();

    function setState(nextState) {
        Object.assign(history, nextState);
        history.length = globalHistory.length;
        transitionManager.notifyListeners(history.location, history.action);
    }

    let forceNextPop = false;
    let ignorePath = null;

    function handleHashChange() {
        const path = getHashPath();
        const encodedPath = encodePath(path);

        if (path !== encodedPath) {
            // Ensure we always have a properly-encoded hash.
            replaceHashPath(encodedPath);
        } else {
            const location = getDOMLocation();
            const prevLocation = history.location;

            if (!forceNextPop && locationsAreEqual(prevLocation, location)) return; // A hashchange doesn't always == location change.

            if (ignorePath === createPath(location)) return; // Ignore this change; we already setState in push/replace.

            ignorePath = null;

            handlePop(location);
        }
    }

    function handlePop(location) {
        if (forceNextPop) {
            forceNextPop = false;
            setState();
        } else {
            const action = 'POP';

            transitionManager.confirmTransitionTo(
                location,
                action,
                getUserConfirmation,
                ok => {
                    if (ok) {
                        setState({ action, location });
                    } else {
                        revertPop(location);
                    }
                }
            );
        }
    }

    function revertPop(fromLocation) {
        const toLocation = history.location;

        // TODO: We could probably make this more reliable by
        // keeping a list of paths we've seen in sessionStorage.
        // Instead, we just default to 0 for paths we don't know.

        let toIndex = allPaths.lastIndexOf(createPath(toLocation));

        if (toIndex === -1) toIndex = 0;

        let fromIndex = allPaths.lastIndexOf(createPath(fromLocation));

        if (fromIndex === -1) fromIndex = 0;

        const delta = toIndex - fromIndex;

        if (delta) {
            forceNextPop = true;
            go(delta);
        }
    }

    // 在做其他任何事之前，保证hash被正确编码
    const path = getHashPath();
    const encodedPath = encodePath(path);

    //将window.location.href的hash部分换为encodePath
    if (path !== encodedPath) replaceHashPath(encodedPath);

    const initialLocation = getDOMLocation();
    let allPaths = [createPath(initialLocation)];

    // 以下方法是公开接口 

    function createHref(location) {
        return '#' + encodePath(basename + createPath(location));
    }

    function push(path, state) {
        warning(
            state === undefined,
            'Hash history cannot push state; it is ignored'
        );

        const action = 'PUSH';
        const location = createLocation(
            path,
            undefined,
            undefined,
            history.location
        );

        transitionManager.confirmTransitionTo(
            location,
            action,
            getUserConfirmation,
            ok => {
                if (!ok) return;

                const path = createPath(location);
                const encodedPath = encodePath(basename + path);
                const hashChanged = getHashPath() !== encodedPath;

                if (hashChanged) {
                    //我们无法判断哈希更改是否是由 PUSH 引起的，
                    //因此我们宁愿在此处设置状态并忽略哈希更改。 
                    //这里需要注意的是，页面中的其他哈希历史会将其视为 POP。
                    ignorePath = path;
                    pushHashPath(encodedPath);

                    const prevIndex = allPaths.lastIndexOf(createPath(history.location));
                    const nextPaths = allPaths.slice(
                        0,
                        prevIndex === -1 ? 0 : prevIndex + 1
                    );

                    nextPaths.push(path);
                    allPaths = nextPaths;

                    setState({ action, location });
                } else {
                    warning(
                        false,
                        'Hash history cannot PUSH the same path; a new entry will not be added to the history stack'
                    );

                    setState();
                }
            }
        );
    }

    function replace(path, state) {
        warning(
            state === undefined,
            'Hash history cannot replace state; it is ignored'
        );

        const action = 'REPLACE';
        const location = createLocation(
            path,
            undefined,
            undefined,
            history.location
        );

        transitionManager.confirmTransitionTo(
            location,
            action,
            getUserConfirmation,
            ok => {
                if (!ok) return;

                const path = createPath(location);
                const encodedPath = encodePath(basename + path);
                const hashChanged = getHashPath() !== encodedPath;

                if (hashChanged) {
                    // We cannot tell if a hashchange was caused by a REPLACE, so we'd
                    // rather setState here and ignore the hashchange. The caveat here
                    // is that other hash histories in the page will consider it a POP.
                    ignorePath = path;
                    //调用了window.location.replace，以给定的URL替换当前资源，特点是当前页面不会保存到会话历史中，这样用户点击回退不会回到该页面。
                    replaceHashPath(encodedPath);
                }

                const prevIndex = allPaths.indexOf(createPath(history.location));

                if (prevIndex !== -1) allPaths[prevIndex] = path;

                setState({ action, location });
            }
        );
    }

    function go(n) {
        warning(
            canGoWithoutReload,
            'Hash history go(n) causes a full page reload in this browser'
        );

        globalHistory.go(n);
    }

    function goBack() {
        go(-1);
    }

    function goForward() {
        go(1);
    }

    let listenerCount = 0;

    function checkDOMListeners(delta) {
        listenerCount += delta;

        if (listenerCount === 1 && delta === 1) {
            window.addEventListener(HashChangeEvent, handleHashChange);
        } else if (listenerCount === 0) {
            window.removeEventListener(HashChangeEvent, handleHashChange);
        }
    }

    let isBlocked = false;

    function block(prompt = false) {
        //设置transitionManager的内部变量prompt，并返回一个类似prompt=null的清理方法
        const unblock = transitionManager.setPrompt(prompt);

        if (!isBlocked) {
            checkDOMListeners(1);
            isBlocked = true;
        }

        //用于清理的方法
        return () => {
            if (isBlocked) {
                isBlocked = false;
                checkDOMListeners(-1);
            }

            return unblock();
        };
    }

    function listen(listener) {
        const unlisten = transitionManager.appendListener(listener);
        checkDOMListeners(1);

        return () => {
            checkDOMListeners(-1);
            unlisten();
        };
    }

    const history = {
        length: globalHistory.length,
        action: 'POP',
        location: initialLocation,
        createHref,
        push,
        replace,
        go,
        goBack,
        goForward,
        block,
        listen
    };

    return history;
}

export default createHashHistory;
