### 关于完整性检测

1. 完整性检测目前做了2件事，dll 检测与下载



2. ffmpeg.exe 检测与下载

对于 2.3.0 之前的版本，由于 gitee 和 github 等下载链接的失效，最后总是会去下载 博客园 34.84M 的ffmpeg.exe , 其分解后的文件链接分为 (由于博客园单个文件10M的限制)：

* `https://files.cnblogs.com/files/BensonLaur/ffmpeg.ext.zip`
* `https://files.cnblogs.com/files/BensonLaur/ffmpeg.1.zip`
* `https://files.cnblogs.com/files/BensonLaur/ffmpeg.2.zip`
* `https://files.cnblogs.com/files/BensonLaur/ffmpeg.3.zip`
* `https://files.cnblogs.com/files/BensonLaur/ffmpeg.4.zip`

（单个文件一天最多下载 200M, 这种情况下一天最多可以 20 次）


在2.3.0 版本之后，目前内置 2 个下载链接：

| 平台 | 链接 |
|:-----:|:---------:|
|gitlab|https://gitlab.com/BensonLaur/resource-provider-gitlab/-/raw/master/beslyric/ffmpeg-3.4.1.exe|
|bitbucket|https://bitbucket.org/bensonlaur/resource-provider-bitbucket/raw/bcd1a44f30893427a5f78243e4548f9afbb82c9c/beslyric/ffmpeg-3.4.1.ex|
	

同时支持获取动态下载链接：
https://files.cnblogs.com/files/BensonLaur/Beslyric-Link.xml 
http://bensonlaur.com/files/beslyric/Beslyric-Link.xml （备用）

程序检测ffmpeg前，会获取最新的下载链接，和内置链接一起作为备选链接。为了防止处在前面的链接数据访问过频繁，导致平台限制，链接的使用先后随机打乱。

在最后，当所有链接仍然下载失败，将前往 博客园 benson2 账号存储文件中下载（42.41M）
（这次为了能够每天下载50次，将文件分割为每4M一个文件）

* `https://files.cnblogs.com/files/benson2/ffmpeg.ext.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.1.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.2.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.3.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.4.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.5.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.6.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.7.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.8.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.9.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.10.zip`
* `https://files.cnblogs.com/files/benson2/ffmpeg.11.zip`


### 关于登陆信息收集

1. 目前开启软件后，软件会向服务器 http://www.bensonlaur.com 发送登陆信息用户用户使用情况统计

2. 关于 IP 地址的收集

在2.3.0 版本之后，目前内置 1 个获取IP的链接

| 过滤正则表达式 | 链接 |
|:-----:|:---------:|
|`>(\d+\.\d+\.\d+\.\d+)<`|https://whatismyipaddress.com/|

同时支持获取动态下载获取IP的链接：
https://files.cnblogs.com/files/BensonLaur/Beslyric-Link.xml 
http://bensonlaur.com/files/beslyric/Beslyric-Link.xml （备用）


