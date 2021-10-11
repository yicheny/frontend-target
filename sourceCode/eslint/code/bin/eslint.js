#!/usr/bin/env node

/**
 * @fileoverview Main CLI that is run via the eslint command.
 * @author Nicholas C. Zakas
 */

/* eslint no-console:off */

"use strict";

// to use V8's code cache to speed up instantiation time
require("v8-compile-cache");

// must do this initialization *before* other requires in order to work
if (process.argv.includes("--debug")) {
    require("debug").enable("eslint:*,-eslint:code-path,eslintrc:*");
}

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

/**
 * Read data from stdin til the end.
 *
 * Note: See
 * - https://github.com/nodejs/node/blob/master/doc/api/process.md#processstdin
 * - https://github.com/nodejs/node/blob/master/doc/api/process.md#a-note-on-process-io
 * - https://lists.gnu.org/archive/html/bug-gnu-emacs/2016-01/msg00419.html
 * - https://github.com/nodejs/node/issues/7439 (historical)
 * 
 * 在 Windows 上使用 `fs.readFileSync(STDIN_FILE_DESCRIPTOR, "utf8")` 似乎在阻塞之前读取 4096 字节，
 * 并且永远不会耗尽以读取更多数据。

 * 对 Emacs 线程的调查表明：
 * MS-Windows 上的 Emacs 使用管道与子进程通信；
 * Windows 上的管道具有 4K 缓冲区。 
 * 因此，一旦 Emacs 向管道写入超过 4096 个字节，管道就会变满，
 * 然后 Emacs 等待子进程读取管道的末尾，此时 Emacs 将写入其余的内容。
 * @returns {Promise<string>} The read text.
 */
function readStdin() {
    return new Promise((resolve, reject) => {
        let content = "";
        let chunk = "";

        process.stdin
            .setEncoding("utf8")
            .on("readable", () => {
                while ((chunk = process.stdin.read()) !== null) {
                    content += chunk;
                }
            })
            .on("end", () => resolve(content))
            .on("error", reject);
    });
}

/**
 * Get the error message of a given value.
 * @param {any} error The value to get.
 * @returns {string} The error message.
 */
function getErrorMessage(error) {

    // Lazy loading because this is used only if an error happened.
    const util = require("util");

    // Foolproof -- thirdparty module might throw non-object.
    if (typeof error !== "object" || error === null) {
        return String(error);
    }

    // Use templates if `error.messageTemplate` is present.
    if (typeof error.messageTemplate === "string") {
        try {
            const template = require(`../messages/${error.messageTemplate}.js`);

            return template(error.messageData || {});
        } catch {

            // Ignore template error then fallback to use `error.stack`.
        }
    }

    // Use the stacktrace if it's an error object.
    if (typeof error.stack === "string") {
        return error.stack;
    }

    // Otherwise, dump the object.
    return util.format("%o", error);
}

/**
 * Catch and report unexpected error.
 * @param {any} error The thrown error object.
 * @returns {void}
 */
function onFatalError(error) {
    process.exitCode = 2;

    const { version } = require("../package.json");
    const message = getErrorMessage(error);

    console.error(`
Oops! Something went wrong! :(

ESLint: ${version}

${message}`);
}

//------------------------------------------------------------------------------
// Execution
//------------------------------------------------------------------------------

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
