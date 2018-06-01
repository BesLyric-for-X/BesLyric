# BesLyric
本项目目的在制作一款 操作简单、功能实用的 专门用于制作网易云音乐滚动歌词的 歌词制作软件。（[更多介绍 ](http://www.cnblogs.com/BensonLaur/p/6262565.html)）

<img src="https://github.com/BensonLaur/image-storage/raw/master/BesLyric%20project/BesLyric%20demo.png" width="60%" height="60%">


<img src="https://github.com/BensonLaur/image-storage/raw/master/BesLyric%20project/BesLyric%20demo3.png" width="60%" height="60%">



BesLyric 最新下载地址：http://beslyric.320.io/BesBlog/beslyric/download.action  (备用下载 [地址1](http://files.cnblogs.com/files/BensonLaur/Beslyric.zip) [地址2](http://beslyric.320.io/BesBlog/Res/Beslyric.zip) [地址3](https://github.com/BensonLaur/image-storage/raw/master/General%20Resource/download/Beslyric.zip) )


1、开发环境:

开发语言：C++

开发IDE：Visual studio 2010 （以及其他兼容版本）

操作系统：window 系统

应用类型：window GUI 程序




2、开发准备

本程序完全基于 SOUI 的框架来搭建，所以运行此程序，你需要先准备好SOUI的开发环境,过程很简单，如下：

1） 下载soui源代码

由于不同版本的soui生成的dll和代码无法兼容，需要区别对待

pre_backward 分支使用 SOUI 版本为 2.2 [下载zip](http://ovfwclhwl.bkt.clouddn.com/soui_2_2.zip)

master 分支使用 SOUI 版本为 2.6.3.1（2017/11/10号获取） [下载zip](http://ovfwclhwl.bkt.clouddn.com/soui_2_6_3_1.zip)

2018-4-23 日已将原先 pre_backward 分支独立开发内容迁移到 master 上， pre_backward 分支将停止维护

2）解压后 直接运行 soui.08.sln (vs2010之后的版本都可以，项目会自动转换)

3）编译源码生成需要的dll组件到 bin 目录下 (重建解决方案 “Rebuild Solution”, 之后跑一下 demo 能跑起来就算准备完毕了)

4）最后是配置开发环境，主要是在设置系统环境变量和将库路径关联到VS开发环境（按照 [SOUI教程第七篇](http://www.cnblogs.com/setoutsoft/p/3928361.html)里面的"认识SOUI应用程序向导"操作完即可）




soui基本介绍：https://www.oschina.net/p/soui

soui系列教程：http://www.cnblogs.com/setoutsoft/category/600691.html

soui 最新地址：https://github.com/SOUI2/soui


