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
            // 做其他任何事之前先保证hash路径的正确
            // replaceHashPath之后会再触发hashChange事件
            replaceHashPath(encodedPath);
        } else {
            const location = getDOMLocation();
            const prevLocation = history.location;

            //仅当confirmTransitionTo转换不通过，执行恢复页面操作
            //如果想要返回的location在当前location的历史堆栈之前，则执行返回操作，此时会将其设置为true
            //forceNextPop为true仅当action为POP时会执行一次setState()
            //locationsAreEqual检查上一次位置和当前位置是否相同【pathname、search、hash、key、state】
            if (!forceNextPop && locationsAreEqual(prevLocation, location)) return; // A hashchange doesn't always == location change.

            //说明当前路径是通过push/replace设置的，不用再往下走
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
                    if (ok) {//通过，则直接转换
                        setState({ action, location });
                    } else {//未通过，则恢复
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

        //想要跳转的location【history.location】的index
        let toIndex = allPaths.lastIndexOf(createPath(toLocation));

        if (toIndex === -1) toIndex = 0;

        //源头是getHashPath()，这是导航当前的location
        let fromIndex = allPaths.lastIndexOf(createPath(fromLocation));

        if (fromIndex === -1) fromIndex = 0;

        const delta = toIndex - fromIndex;

        //如果想要返回的location在当前location的历史堆栈之前，则执行返回操作
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
                    //通过history的公开接口push/replace更改路径，会主动设置相应的action【PUSH/REPLACE】
                    //无论任何原因导致路径更改，都会触发hashChange事件，从而执行handleHashChange()方法
                    //此时，如果不是通过`history`公开接口修改的路径，action都会被标志为`POP`【及其他一系列操作】
                    //如果是`push/replace`设置的路径，则不需要这一系列处理
                    //那么如何判定这个路径是通过`push/replace`修改的？
                    //ignorePath是在push/replace中进行设置的，如果ignorePath存在且与当前路径一致，
                    //那么就认为这个路径是通过push / replace修改的，不用再做处理
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
