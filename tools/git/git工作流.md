[TOC]

# 总览
常见的工作流模型及其特点：
1. 集中式工作流：最为基本的一种Git工作流，适合习惯传统版本控制方式的团队。
2. 特性分支工作流：非常重要的一种Git工作流，充分发挥分支模型的优势
3. git-flow工作流：广泛应用的一种Git工作流，适合管理有固定发布周期的大项目、
4. forking-flow：开源项目的标准Git工作流，灵活与约束并存的分布式工作流
5. github-flow：是Git flow的简化版，专门配合"持续发布"。它是 Github.com 使用的工作流程，也同时是参加开源项目的主要合作方式之一。
6. gitlab-flow：Git flow 与 Github flow 的综合。它吸取了两者的优点，既有适应不同开发环境的弹性，又有单一主分支的简单和便利。它是 Gitlab.com 推荐的做法。
   
# 集中式工作流
所谓集中式工作流(Centralized Workflow)，是指所有人共享一个Git库，且只有一个分支(即master)，所有人的变更都统一提交到这个Git库的master上。

这种工作流程非常适合那些从其他传统版本控制工具（如：SVN）迁移过来的团队，因为这就是他们以前熟悉的工作方式。它允许每个人在本地有一个属于自己的隔离环境，能够独立工作，然后在需要的时候把本地的工作同步到远程的共享库。

当然，这样做并没有完全发挥出Git在分布式协作方面的优势。而且，随着团队规模的扩大，大家在维护共享库时产生冲突的机会也会逐渐增多，从而会使共享库成为整个团队开发的瓶颈。

尽管如此，集中式工作流仍然是很重要的一种工作流，因为它是其他Git工作流的基础。本质上讲，其他工作流都是它的某种“衍生”。

注意：在这种模式下，使用rebase的方式进行集成，可以把冲突的范围局限在单个提交里，而不是所有提交都揉在一起。而且，这样做还可以保持每个提交记录都专注于自己的事情，当出现问题时更容易定位引入问题的地方，需要回退提交历史的时候也可以让回退的影响降到最小。

# 特性分支工作流
所谓特性分支工作流(Feature Branching)，是指当开发新特性或者进行bug修复的时候，开发人员从master分支出发，建立新的分支，把开发工作放到这些专门的分支上独立进行。

特性分支工作流作为集中式工作流的一种扩展，依然沿用了统一远程Git库，但是它充分发挥了Git分支模型的优势，让多人协作开发同一个项目变得更加方便。在这种工作模式下，每个特性的开发都有自己的分支，对项目的主干（即master分支）不会构成任何影响。在特性分支上的变更，只有经过确认以后才会被合并到master分支，这样就保证了master分支的稳定性。

从特性分支往master分支合并的时候，我们还可以通过创建Pull Request来发起讨论，允许其他人对涉及变更的代码进行复查，给出反馈意见。虽然Git本身并不直接提供Pull Request功能，但是现在很多Git服务解决方案，比如像GitHub，Bitbucket，GitLab等，都有相应的功能，使用起来非常方便。

此外，特性分支工作流是一种非常重要的工作流，后面我们在学习更高层次的工作流时会发现，它们都是以特性分支工作流为基础的。

# git-flow工作流
git-flow工作流(git-flow Workflow)是`2010`年由`Vincent Driessen`在他的一篇[博客](https://nvie.com/posts/a-successful-git-branching-model/)里提出来的。它定义了一整套完善的基于Git分支模型的框架，结合了版本发布的研发流程，适合管理具有固定发布周期的大型项目。

和特性分支工作流相比，git-flow工作流并没有引入任何新的概念。不同的地方在于，它强化了对Git分支模型的使用，结合产品或项目发布周期的特定需求，定义了各种不同类型的分支，每一种分支都有它自己特定的职责，并且分支之间什么时候、以什么样的方式交互，也都有相应的规则。下面我们就具体来看一下。

## 分支
### `Master`分支
Master分支作为唯一一个正式对外发布的分支，是所有分支里最稳定的。这是因为，只有经过了严格审核和测试，并且在当前发布计划里的特性，才会被合并到master分支。当某个版本发布的时候，我们通常还会为master分支加上带有相应版本号的tag。

### `Develop`分支
Develop分支是根据master分支创建出来的，它作为一种集成分支(Integration Branch)，是专门用来集成开发完成的各种特性的。Develop分支通常具有更加详细和完整的提交历史，包括一些很细节的提交记录。而master分支则因为是面向版本发布的，所以它的提交历史会略去这些细节，显得比较精简。

### `Feature`分支
Feature分支是根据develop分支创建出来的，git-flow工作流里的每个新特性都有自己的feature分支，这一点和特性分支工作流是一样的。这些分支除了在开发人员的本地存在以外，也可以被推送到共享的远程Git库，作为工作备份，以及与其他人协同工作的基础。当特性开发结束以后，这些分支上的工作会被合并到develop分支。但feature分支从来不会直接和master分支打交道。

### `Release`分支
当积累了足够多的已完成特性，或者预定的系统发布周期临近的时候，我们就会从develop分支创建出一个release分支，专门用来做和当前版本发布有关的工作。Release分支一旦开出来以后，就不允许再有新的特性被加入到这个分支了，只有bug修复或者文档编辑之类的工作才允许进入该分支。

Release分支上的内容最终会被合并到master分支，等版本发布的时候，我们通常还会为master分支加上带有相应版本号的tag。同时，release分支也会被合并到develop分支。在release分支活跃其间，develop分支也一直处于Open状态。Release分支上的内容代表当前版本在发布之前的准备工作，develop分支上的内容则代表下一个版本的开发工作，两者是可以并行展开的。

### `Hotfix`分支
Hotfix分支不从是develop分支创建出来的，而是直接根据master分支创建得到的，其目的是为了给运行在生产环境中的系统快速提供补丁，同时确保不会给正在其他上分支进行的工作造成影响。当hotfix分支上的工作完成以后，可以合并到master分支和develop分支，以及当前的release分支。如果有版本的更新，也可以为master分支打上相应的tag。

上面提到的所有分支，从稳定性的角度来说，每一种分支都处在各自不同的层次。如果当前分支的代码达到了更加稳定的水平，那它就可以向更稳定的分支进行合并了。

## 工作方式 
Vincent Driessen不仅定义了git-flow的工作流程，还提供了一个相应的命令行工具git-flow，可以简化我们在执行git-flow工作流时，对每一个分支的各种繁琐的操作。它实际上是对Git命令行的封装。接下来，我们就利用这个工具演示一下整个git-flow工作流。

使用方式：[git-flow文档](https://www.git-tower.com/learn/git/ebook/cn/command-line/advanced-topics/git-flow/)

# `Forking-flow`
这种工作流允许每个开发人员都拥有属于自己的远程Git库，而不是所有人集中共享一个远程库。每一个采用Forking工作流的项目都有一个唯一的官方远程库，其他人则从这个官方远程库fork出自己的个人远程库。

对于这两种远程库，我们在本地使用Git时，通常会为它们起相应的名字，比如：个人远程库习惯叫做origin，官方远程库习惯叫做upstream。

# `github-flow`工作流
`git-flow`适合多版本共存的管理，在web项目中这个需求并不强烈，此时使用`github-flow`更适合持续交付

# `gitlab-flow`
Gitlab flow 是 Git flow 与 Github flow 的综合。它吸取了两者的优点，既有适应不同开发环境的弹性，又有单一主分支的简单和便利。它是 Gitlab.com 推荐的做法。

# 资料
- [Git工作流面面观——集中式工作流](https://morningspace.github.io/tech/git-workflow-2/)
- [Git工作流面面观——特性分支工作流](https://morningspace.github.io/tech/git-workflow-3/)
- [Git工作流面面观——git-flow工作流](https://morningspace.github.io/tech/git-workflow-4/)
- [Git工作流面面观——Forking工作流](https://morningspace.github.io/tech/git-workflow-5/)
- [git-flow文档](https://www.git-tower.com/learn/git/ebook/cn/command-line/advanced-topics/git-flow/)
- [git-flow vs github-flow](https://lucamezzalira.com/2014/03/10/git-flow-vs-github-flow/)
- [github-flow](http://scottchacon.com/2011/08/31/github-flow.html)
- [gitlab-flow](https://docs.gitlab.com/ee/topics/gitlab_flow.html)
- [知乎-如何看待 Git flow 发明人称其不适用于持续交付？](https://www.zhihu.com/question/379545619)
- [知乎-gitlab flow 大家是怎么优秀使用的，相关实践资料也可以？](https://www.zhihu.com/question/322574581/answer/671281377)
- [知乎-三大工作流](https://zhuanlan.zhihu.com/p/140553749)
- [阮一峰-workflow](https://www.ruanyifeng.com/blog/2015/12/git-workflow.html)