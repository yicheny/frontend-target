[TOC]

# 问题
1. 为什么有些本地文件合并后就消失了？
2. 什么时候使用`rebase`，什么时候使用`merge`？

# `Tree-Way-Merge`【三向合并】
有两个以上的本地代码才有可能出现冲突。

假设现在我和另一个开发者从远端拉取同一份代码到本地，我们对同一份文件进行了修改，在进行代码合并时，我们发现这份代码在30行出现了不同的修改。

现在代码存在三个版本：
1. `base` 远端原代码
2. `mine` 我的代码
3. `other` 其他人的代码

如果两份代码中有一份第30行和`base`相同，那么会直接采用另一份不相同的代码进行合并。

真正需要手动解决的，出现在三份代码第30行都不相同时。

这里，有一个重要内容：`git`知道两份不同提交的公共`commit`，或者说，`git`知道两份提交文件所对应的“原文件”

在此基础上：
1. 如果只有一份提交和“原文件”不同，那么使用这个提交
2. 如果两份都和“原文件”不同，那么由开发者手动解决冲突，一般有两种选择：
   1. 选择其中一种
   2. 结合两种

现在我们首先探索这两个原理：
1. `git`是如何记录提交历史的
2. `git`的合并策略是怎么推算出公共`commit`的

# `git`是如何记录提交历史的
我们在控制台执行`git init`，`git`会创建一个`.git`目录，这个目录包含了`git`存储和操作需要的内容，结构如下：
1. `config`  项目特有的配置选项
2. `description`  仓库描述信息，主要给`gitweb`等`git`托管系统使用
3. `HEAD` 文件，记录目前正在使用的分支
4. `hooks` 目录，包含git的钩子脚本
5. `info` 目录，包含一个全局性排除文件，用以放置那些不希望被记录在`.gitignore`文件中的忽略模式
6. `objects` 目录，存储所有的数据内容
7. `refs` 目录，存储指向数据的提交对象指针

其中`HEAD`，`refs`和`objects`是`git`能够记录提交历史的关键所在。

## `blob object`
首先`git init`

然后创建文件并提交到缓存去：
```
echo 111 > a.txt
echo 222 > b.txt
git add .
```

然后我们可以发现`objects`目录下多了两个文件，执行`tree /f`可以查看：

![objects-多两份文件](https://pic.imgdb.cn/item/6130cf3944eaada73956ae29.jpg)

查看文件内容，可以使用以下两种方式：
1. `type`命令
2. 进入`git bash`，使用`cat`命令

内容输出如下：

![objects-直接查看文件内容](https://pic.imgdb.cn/item/6130d14944eaada7395bd621.jpg)

发现输出了一串乱码，这是因为`git`将信息压缩成二进制文件。

不过`git`有提供`cat-file`命令来取回转码前的数据：
- `-t` 可以查看`object`文件的类型
- `-p`可以查看`object`文件的具体内容

![查看文件类型](https://pic.imgdb.cn/item/6130d8ea44eaada739703a0a.jpg)

![查看文件真实内容](https://pic.imgdb.cn/item/6130d91944eaada73970b38b.jpg)

关注这个`blob`文件类型，这是第一个`git object:blob`，它只存储一个文件的内容，不包含文件名等其他信息。

该内容加上特定头部信息一起的`SHA-1`校验和为文件命名，作为这个`object`在`git`仓库中的唯一身份证。 

校验和的前两个字符用于命名子目录，余下的38个字符则用作文件名。

这是现在的`git`仓库：

![](https://pic.imgdb.cn/item/6131f3d444eaada73994c909.jpg)

## `tree object`
现在我们做一次提交：`git commit -m '第一次提交'`

可以看到新增了两个文件：

![新增文件](https://pic.imgdb.cn/item/6130e55c44eaada739930bae.jpg)

![输出类型和内容](https://pic.imgdb.cn/item/6130e5c644eaada73994372a.jpg)

这个文件类型是`tree`，用以记录当前目录结构。

我们分别解释下输出内容：
- `100644` `100`代表`regular file`【常规文件】，`644`代表文件权限
- `blob` 文件类型
- `ce6ad5f153111fe8dfc03ec69ea87ce1ecb1b1e8` `hash`值
- `a.txt` 文件名

![](https://pic.imgdb.cn/item/6131f52c44eaada73996e4ea.jpg)

## `commit object`
再看看另一份文件存了什么：

![提交信息](https://pic.imgdb.cn/item/6130e7d244eaada7399a12a0.jpg)

这个文件类型是`commit`，是更上层的记录，说明下记录内容：
- `tree` `tree`文件的`hash`值
- `author` 提交者/作者
- `committer` 提交信息

![](https://pic.imgdb.cn/item/6131f86544eaada7399c0dbe.jpg)

## `HEAD and refs`
![HEAD and refs](https://pic.imgdb.cn/item/6130f1ba44eaada739b65495.jpg)

解释下输出：
- `refs`下存储着你所有分支的当前`commit object Hash`，
- `HEAD`相当于一个指针，指向`refs`中你当前的分支。

概括来说整个引用关系就是：
1. **`HEAD`里面的内容是当前分支的`ref`**
1. **而当前`ref`的内容是`commit hash`**
1. **`commit object`内容是 `tree hash`**
1. **`tree object`的内容是`blob hash`**
1. **`blob`存储着文件的具体内容**
   
![](https://pic.imgdb.cn/item/6131fa8b44eaada7399f6f8b.jpg)

## 切换`dev`分支
现在我们切出`dev`分支，并进行修改和提交：
```
git checkout -b dev
echo 333 > a.txt
git add .
git commit -m '在dev的第一次提交'
```

然后查看`objects`，发现多出3个文件：

![查看objects](https://pic.imgdb.cn/item/6131c2f944eaada739190f4a.jpg)

查看文件内容【根据内容可以看出类型，类型就不截图表示了】：

![查看objects内容](https://pic.imgdb.cn/item/6131c44a44eaada7391c1e5d.jpg)

这里可以发现：
1. `a`被改变了，所以会再创建一个关于`a`的新文件并记录其`hash`
2. `b`没有改变，所以记录的是原有的文件`hash`

我们看一下这个时候`HEAD`和`refs`里的内容：

![`HEAD`和`refs`里的内容](https://pic.imgdb.cn/item/6131c61044eaada739216341.jpg)

![](https://pic.imgdb.cn/item/61321e4144eaada739e09a9e.jpg)

## 多次`commit`
至此，我们已经了解以下知识：
1. `git`是怎么保存文件的
2. `git`是怎么区分不同分支下的文件的

接下来我们学习在一个分支下多个`commit`是怎么关联的

首先，我们再做一次提交：
```
git checkout master
echo 444 > a.txt
git add .
git commit -m '在master的第二次提交'
```

![](https://pic.imgdb.cn/item/6131c93944eaada7392e392d.jpg)

又多了三个文件，查看内容：

![](https://pic.imgdb.cn/item/6131cc3244eaada73939921b.jpg)

重点关注这个`commit`文件，我们发现这里多了一个`parent`记录。

`git`记录`commit`时，如果是第一次提交没有这个属性，如果是第二次及之后的提交，会记录下上一次`commit`的`hash`

![](https://pic.imgdb.cn/item/6132219a44eaada739e846dd.jpg)

# 合并策略
在了解`git`的记录原理之后，我们了解下git的一些常见的合并策略：
1. `Fast-forward`
1. `Recursive`
1. `Xours`
1. `Xtheirs`
1. `Ours`
1. `Octopus`

然后再了解一些使用较少的合并策略：
1. `Resolve`
2. `squash `

默认`git`会视情况选择合适的策略，如果有需要可以通过`git merge -s 策略名`指定合并策略

## `Fast-forward`
最简单的合并策略，这种合并没有合并记录，只是将`ref`指向改变，下面演示下合并。

这是在`dev`分支做开发：<br/>
![](https://pic.imgdb.cn/item/6133216d44eaada739f61a4f.jpg)

现在合并到`master`分支：<br/>
![](https://pic.imgdb.cn/item/613321f944eaada739f78ddb.jpg)

`Fast-forward`是`git`在合并两个没有分叉的分支时的默认行为，如果你想禁用掉这种行为，明确拥有一次合并的`commit`记录，可以使用`git merge --no-ff`命令来禁用掉。

如果使用`--no-ff`则会多创建一次合并提交，此时合并策略会变成`Three-Way Merge`

## **`Recursive`**
`git`合并策略中最重要也最常用的一种策略。

简单描述下原理就是：通过算法找到两个分支的最近公共祖先节点，并将其公共祖先节点作为`base`节点使用三相合并策略进行合并

### 场景1
举个例子:(圆圈里面文字标识其内容)

1是初始分支，然后切出分支2、3，进行分支合并，首先会找到最近公共节点1，然后对比2、3中文件内容，可以发现分支3对文件没有修改，而2将文件内容改为B，所以会最后合并的结果是`B`

![](https://pic.imgdb.cn/item/613372d344eaada739aeb65e.jpg)

### 场景2 `Criss-Cross`
我们来看更复杂的场景，几个分支相互交叉的情况【出现多个满足条件的共同祖先的现象】，也被叫做`Criss-Cross`现象

![](https://pic.imgdb.cn/item/61338e0244eaada739f77b8f.jpg)

在这个场景中，最近公共祖先节点是哪个提交？

可以找到两个公共祖先节点，将`2`或`3`当作最近公共祖先节点都是合理且可以理解的。

> 查看两个分支的最近公共祖先可以使用命令：`git merge-base --all branch_1 branch_2`

这里先不揭露`git`实际是如何处理的，我们这两个节点分别作为公共祖先节点，并以之前的思路进行分析，看看会是什么情况

如果我们以节点2为`base`节点，结果如下图：

![](https://pic.imgdb.cn/item/61338e4444eaada739f836d3.jpg)

此时公共节点内容是`A`，两个待合并节点内容是`B`、`C`，此时无法自动进行合并，需要手动进行合并。

我们再看一下如果使用节点3作为`base`节点，结果如何：

![](https://pic.imgdb.cn/item/6133935f44eaada739056c10.jpg)

此时公共节点是`B`，待合并节点是`B`、`C`，通过三项合并策略，最终合并的结果是`C`

作为人类思考，将结果设为`C`是合理的做法，而`git`三向合并的结果也会是`C`，`git`是怎么处理的？

`git`在进行三向合并时，如果发现满足条件的公共节点不唯一，会首先合并公共节点【同样使用三向合并策略】，并将合并后的结果作为`base`节点参与之后的合并。

处理过程类似下图：

![](https://pic.imgdb.cn/item/6133930e44eaada739049d3e.jpg)

## `Xours`和`Xtheirs`
默认情况下，当 `Git` 看到两个分支合并中的冲突时，它会将合并冲突标记添加到你的代码中并标记文件为冲突状态来让你解决。

如果你希望 `Git` 简单地选择特定的一边并忽略另外一边而不是让你手动合并冲突，你可以传递给 `merge` 命令一个 `-Xours` 或 `-Xtheirs` 参数。

比如说想要将分支B合并到分支A，如果使用`-Xours`则使用A分支提交，如果使用`-Xtheirs`则使用B分支提交

## `Ours`
`Ours`策略和`Xours`策略有些相似，不同之处在于：`Xours`策略中，如果没有出现冲突，那么会自动合并；如果出现冲突，则会丢弃被合并分支的内容使用自己的分支内容

而`Ours`策略则是无论有没有冲突，`git`都会完全丢弃被合并分支上的内容，只保留本地分支上的修改，但是两个分支的`commit`记录都会被保留。

这种策略的应用场景一般是为了实现某个功能，同时尝试了两种方案，最终决定选择其中一个方案，而又希望把另一个方案的`commit`记录合进主分支里方便日后的查看。

### 为什么没有`Theirs`策略？
既然合并的时候即有`-Xtheirs`参数又有`-Xours`参数，所以下意识的觉得`git`既然有 `Ours` 策略也会有 `Theirs` 策略，实际上`git`曾经有过这个策略后来舍弃了，因为`Theirs`会完全丢掉当前分支的更改，是一个十分危险的操作，如果你真的想丢弃掉自己的修改，可以使用`reset`命令来代替它。

## `Octopus`
`Octopus` 策略可以让我们优雅的合并多个分支。

前面我们介绍的策略都是针对两个分支的，如果现在有多个分支需要合并，使用`Recursive`策略进行两两合并会产生大量的合并记录：每合并其中两个分支就会产生一个新的记录，过多的合并提交出现在提交历史里会成为一种“杂音“，对提交历史造成不必要的”污染“。

`Octopus`在合并多个分支时只会生成一个合并记录，这也是`git`合并多个分支的默认策略。

在`master`分支下执行`git merge dev1 dev2`，示意图：
//原理已经理解，有时间补张图

## `Resolve`
和`Recursive`有些类似，是`Recursive`策略出现之前的三向合并默认策略，区别在于会在最近公共祖先节点中选择其中一个作为`base`节点进行合并

## `Squash`
所谓Squash Merge，是指Git在做两个分支间的合并时，会把被合并分支（通常被称为topic分支）上的所有变更“压缩（squash）”成一个提交，追加到当前分支的后面，作为“合并提交”（merge commit）。

从参与合并的文件变更上来说，Squash Merge和普通Merge并没有任何区别，效果完全一样。唯一的区别体现在提交历史上：正如我们前面提到的，对于普通的Merge而言，在当前分支上的合并提交通常会有两个parent；而Squash Merge却只有一个。

Squash Merge不会像普通Merge那样在合并后的提交历史里保留被合并分支的分叉，被合并分支上的提交记录也不会出现在合并后的提交历史里，所有被合并分支上的变更都被“压缩”成了一个合并提交。

如果在被合并分支上，完整的提交历史里包含了很多中间提交（intermediate commit），比如：改正一个小小的拼写错误可能也会成为一个独立的提交，而我们并不希望在合并时把这些细节都反应在当前分支的提交历史里。这时，我们就可以选择Squash Merge。

另外，后面我们还会看到，如果在合并时想去除被合并分支上的那些中间提交，我们还可选择Rebase。

# 变基
看完`git merge` 的策略后，再看看另一个合并代码时常用的命令`git rebase`。`git rebase`和`merge`最大的不同是它会改变提交的历史。

关于变基我们会学习以下几点：
1. 什么是变基？
2. 怎么使用变基？
3. 变基有什么出彩之处？
4. 在什么情况下应该避免使用变基？

## 变基的基本操作
我们从`c3`开始切出`dev`分支，在`dev`上进行`c4`提交，在`master`分支进行`c5`提交

![](https://pic.imgdb.cn/item/613473e344eaada73959d9b8.jpg)

我们想将`dev`分支的提交合并到`master`分支上，如果不使用变基，直接`merge`会创建一份新的合并提交【就是三方合并策略】，如图：

![](https://pic.imgdb.cn/item/613475e544eaada7395ddd2e.jpg)

退回到之前，现在我们先使用变基操作。先明确一点，变基并不是真正的合并，可以认为它是整合分叉的提交历史，以便再一步进行合并。
```
$ git checkout dev
$ git rebase master
```

变基的原理是：
1. 首先找到这两个分支（即`dev`、`master`）的最近共同祖先`c3`
2. 然后对比`dev`分支相对于该祖先的历次提交，提取相应的修改并存到临时文件
3. 然后重置`dev`提交历史，使其和`master`提交历史一致
4. 追加`dev`分支上的提交记录（将之前存到临时文件的修改依序应用）

如图：

![](https://pic.imgdb.cn/item/613478a844eaada7396369f2.jpg)

`Rebase`本质上就是丢弃已有的提交记录，创建新的提交记录，从内容上看是完全一样的，但提交历史却改变了。

变基之后再进行合并
```
$ git checkout master
$ git merge dev
```

![](https://pic.imgdb.cn/item/61347a7744eaada739671687.jpg)

这个时候合并，因为没有分叉，所以会直接使用`Fast-forward`策略进行合并，不会创建新的提交。

无论是使用三方合并直接合并，还是先变基再合并，最终得到的快照结果都是一样的，不一样的是提交历史记录。

使用变基是一条串行记录，没有分叉。

一般我们这样做的目的是为了确保在向远程分支推送时能保持提交历史的整洁——例如向某个其他人维护的项目贡献代码时。 

在这种情况下，你首先在自己的分支里进行开发，当开发完成时你需要先将你的代码变基到 `origin/master` 上，然后再向主项目提交修改。 这样的话，该项目的维护者就不再需要进行整合工作，只需要快进合并便可。

请注意，无论是通过变基，还是通过三方合并，整合的最终结果所指向的快照始终是一样的，只不过提交历史不同罢了。 变基是将一系列提交按照原有次序依次应用到另一分支上，而合并是把最终结果合在一起。

## 更有趣的变基例子
在对两个分支进行变基时，所生成的“重放”并不一定要在目标分支上应用，你也可以指定另外的一个分支进行应用。

首先我们看一个场景：

![](https://pic.imgdb.cn/item/6134874844eaada7397e4c5f.jpg)

假设你希望将 `client` 中的修改合并到主分支并发布，但暂时并不想合并 `server` 中的修改，因为它们还需要经过更全面的测试。 

这时，你就可以使用 `git rebase` 命令的 `--onto` 选项

```
$ git rebase --onto master server client
```

以上命令的意思是：“取出 `client` 分支，找出处于 `client` 分支和 `server` 分支的共同祖先之后的修改，然后把它们在 `master` 分支上重放一遍”。 这理解起来有一点复杂，效果如图所示：

![](https://pic.imgdb.cn/item/613489ae44eaada739828c2a.jpg)

现在可以快进合并 `master` 分支了
```
$ git checkout master
$ git merge client
```

![](https://pic.imgdb.cn/item/61348aa244eaada7398435c1.jpg)

接下来你决定将 server 分支中的修改也整合进来。 使用 `git rebase [basebranch] [topicbranch]` 命令可以直接将特性分支（即本例中的 server）变基到目标分支（即 master）上。这样做能省去你先切换到 server 分支，再对其执行变基命令的多个步骤。

```
$ git rebase master server
```

![](https://pic.imgdb.cn/item/61348bf344eaada739867593.jpg)

变基后可以进行快速合并并删除合并分支
```
$ git checkout master
$ git merge server
$ git branch -d client
$ git branch -d server
```

![](https://pic.imgdb.cn/item/61348c7644eaada7398752b7.jpg)

### 变基的风险
呃，奇妙的变基也并非完美无缺，要用它得遵守一条准则：

**`Do not rebase commits that you have pushed to a public repository.
If you follow that guideline, you’ll be fine. If you don’t, people will hate you, and you’ll be scorned by friends and family.`**

（永远不要对那些已经推送到公共仓库的更新执行变基。如果你遵循这条金科玉律，就不会出差错。否则，人民群众会仇恨你，你的朋友和家人也会嘲笑你，唾弃你。）

变基操作的实质是丢弃一些现有的提交，然后相应地新建一些内容一样但实际上不同的提交。

如果你已经将提交推送至某个仓库，而其他人也已经从该仓库拉取提交并进行了后续工作，此时，如果你用 `git rebase` 命令重新整理了提交并再次推送，你的同伴因此将不得不再次将他们手头的工作与你的提交进行整合，如果接下来你还要拉取并整合他们修改过的提交，事情就会变得一团糟。

现在有一个远端：

![](https://pic.imgdb.cn/item/61349b4b44eaada7399f18cd.jpg)

我们在本地克隆远端仓库，并在其基础上进行修改：

![](https://pic.imgdb.cn/item/61349d0e44eaada739a1bffe.jpg)

此时，有另一个人向远端进行了提交，其中包括一次合并：

![](https://pic.imgdb.cn/item/6134a18244eaada739a83897.jpg)

然后，我们在本地拉取了远端代码，并将其合并到本地开发分支：

![](https://pic.imgdb.cn/item/6134a02444eaada739a644d7.jpg)

接下来，这个人又决定把合并操作回滚，改用变基；继而又用 `git push --force` 命令覆盖了服务器上的提交历史。 之后你从服务器抓取更新，会发现多出来一些新的提交。

![](https://pic.imgdb.cn/item/6134a21544eaada739a90f5e.jpg)

如果使用`git pull`拉取将会合并两条提交历史的内容，生成一个新的合并提交

![](https://pic.imgdb.cn/item/6134b9e044eaada739cf8f53.jpg)

此时如果你执行 `git log` 命令，你会发现有两个提交的作者、日期、日志居然是一样的，这会令人感到混乱。 此外，如果你将这一堆又推送到服务器上，你实际上是将那些已经被变基抛弃的提交又找了回来，这会令人感到更加混乱。 很明显对方并不想在提交历史中看到 `C4` 和 `C6`，因为之前就是他把这两个提交通过变基丢弃的。

### 以变基解决变基
如果你 真的 遭遇了类似的处境，Git 还有一些高级魔法可以帮到你。 如果团队中的某人强制推送并覆盖了一些你所基于的提交，你需要做的就是检查你做了哪些修改，以及他们覆盖了哪些修改。

实际上，Git 除了对整个提交计算 SHA-1 校验和以外，也对本次提交所引入的修改计算了校验和—— 即 “patch-id”。

如果你拉取被覆盖过的更新并将你手头的工作基于此进行变基的话，一般情况下 Git 都能成功分辨出哪些是你的修改，并把它们应用到新分支上。

还是以刚刚的例子，如果我们不是直接合并，而是执行`git rebase team/master`，`Git`会进行如下处理：
1. 哪些是这条分支上独有的：C2、C3（C7、C8是被生成的提交）
2. 哪些是目标分支上特有的：C5、C4'（C4'等同于C6，而C6包含C4）
3. 把查到的这些提交应用在 `team/master` 上面

![](https://pic.imgdb.cn/item/6134c08f44eaada739dc26d9.jpg)

要想上述方案有效，还需要对方在变基时确保 C4' 和 C4 是几乎一样的。 否则变基操作将无法识别，并新建另一个类似 C4 的补丁（而这个补丁很可能无法整洁的整合入历史，因为补丁中的修改已经存在于某个地方了）。

在本例中另一种简单的方法是使用 `git pull --rebase` 命令而不是直接 `git pull`。 又或者你可以自己手动完成这个过程，先 `git fetch`，再 `git rebase team/master`。

如果你习惯使用 `git pull` ，同时又希望默认使用选项 `--rebase`，你可以执行这条语句 `git config --global pull.rebase true` 来更改 `pull.rebase` 的默认配置。

只要你把变基命令当作是在推送前清理提交使之整洁的工具，并且只在从未推送至共用仓库的提交上执行变基命令，就不会有事。 假如在那些已经被推送至共用仓库的提交上执行变基命令，并因此丢弃了一些别人的开发所基于的提交，那你就有大麻烦了，你的同事也会因此鄙视你。

如果你或你的同事在某些情形下决意要这么做，请一定要通知每个人执行 `git pull --rebase `命令，这样尽管**不能避免伤痛**，但能有所缓解。

## 变基 vs. 合并
总的原则是，**只对尚未推送或分享给别人的本地修改执行变基操作清理历史，从不对已推送至别处的提交执行变基操作**，这样，你才能享受到两种方式带来的便利

多人协同工作应该用 merge，一个人写代码可以用 rebase。

merge会产生分支，然而从版本管理的角度，多人的工作本来就应该位于不同的分支。

单纯为了线条好看而扭曲了实际开发过程中的事实，并不是可取的行为。如果需要merge，本来就是因为在你提交之前有别人修改了代码，那么别人的代码事实上确实就是与你并行修改。

从流程上讲，别人的代码与你并行修改，并且同时都基于某个早先的基线版本，那么这样的两组修改就确实应该位于不同的分支。分支归并正确的显示了多人协同开发过程中实际发生的客观事实。

因此显然应该选择merge，版本管理软件的职责就是准确的记录开发历史上发生过的所有事情，merge能确保你基于修改的基点忠实的反应了情况，这种情况下merge肯定是更准确的。

但如果是你自己一个人写的代码，多余出来的分支确实是不必要的，本来就应该把线整理成线性。那么确实可以考虑使用rebase。——这种情况下一般发生于自己一个人使用了多台电脑，多台电脑各有不同的未提交代码的情形，建议考虑rebase。

结论重复一下：归并目标是他人代码，用来解决两个不同开发者开发的代码冲突的时候，用merge，归并目标是自己代码，用来解决自己在两台不同电脑上修改代码的冲突的时候，用rebase。



rebase，合并的结果好看，一条线，但合并过程中出现冲突的话，比较麻烦
1. rebase过程中，一个commit出现冲突，下一个commit也极有可能出现冲突，一次rebase可能要解决多次冲突；
2. 合并的历史脉络（冲突）被物理消灭了
3. merge，合并结果不好看，一堆线交错，但合并有冲突的话，只要解一次就行了；



为了追求Git的线好看，在团队合作中使用rebase说轻点是舍本逐末，说重了是对团队不负责任。个人以及本地项目无所谓。

# 资料
- [git合并原理](https://www.tripod.fun/2020/06/09/2020/git%E5%90%88%E5%B9%B6%E5%8E%9F%E7%90%861/)
- [pro-git在线阅读](https://www.progit.cn/#_pro_git)
- [git官网：pro-git 可下载](https://git-scm.com/book/zh/v2)
- [这才是真正的Git——Git内部原理揭秘！](https://zhuanlan.zhihu.com/p/96631135)
- [Git合并那些事——Merge策略（上）](https://morningspace.github.io/tech/git-merge-stories-2/#%E5%85%B3%E4%BA%8Emerge%E7%AD%96%E7%95%A5)
- [Git合并那些事——Merge策略（下）](https://morningspace.github.io/tech/git-merge-stories-1/)
- [Git subtree教程](https://segmentfault.com/a/1190000012002151)
- [骚年，听说你写代码从来不用 git rebase [视频]](https://zhuanlan.zhihu.com/p/47905032)
- [Why you should stop using Git rebase](https://medium.com/@fredrikmorken/why-you-should-stop-using-git-rebase-5552bee4fed1)
- [代码合并：Merge、Rebase 的选择](https://github.com/geeeeeeeeek/git-recipes/wiki/5.1-%E4%BB%A3%E7%A0%81%E5%90%88%E5%B9%B6%EF%BC%9AMerge%E3%80%81Rebase-%E7%9A%84%E9%80%89%E6%8B%A9)
- [git rebase 还是 merge的使用场景最通俗的解释](https://zhuanlan.zhihu.com/p/350187205)
- [Git合并那些事——神奇的Rebase](https://morningspace.github.io/tech/git-merge-stories-6/)