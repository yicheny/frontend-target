//用于debug的追踪函数
const trace = _.curry(function (tag, x) {
    console.log(tag, x);
    return x;
});

//组合
const compose = function (f, g) {
    return function (x) {
        return f(g(x));
    };
};

//
const match = _.curry((what, str) => str.match(whate));
const replace = _.curry((what, replacement, str) => str.replace(what, replacement));
const filter = _.curry((f, ary) => ary.filter(f));
const map = _.curry((f, ary) => ary.map(f));

const toUpperCase = x => x.toUpperCase();
const toLowerCase = x => x.toLowerCase();
const head = x => x[0];
const reverse = reduce((acc, x) => [x].concat(acc), []);
const last = compose(head, reverse);


//