### 关于完整性检测

1. 完整性检测目前做了2件事，dll 检测与下载



2. ffmpeg.exe 检测与下载


目前内置 3 个下载链接：

| 平台 | 链接 |
|:-----:|:---------:|
|gitlab|https://gitlab.com/BensonLaur/resource-provider-gitlab/raw/master/beslyric/ffmpeg.exe|
|sourceforge|https://master.dl.sourceforge.net/project/resource-provider-sourceforge/Beslyric/ffmpeg.exe|
|bitbucket|https://bitbucket.org/bensonlaur/resource-provider-bitbucket/raw/1ee69f772fd4819f1b39d953fbf3220c5606b3c8/beslyric/ffmpeg.exe|
	

同时支持获取动态下载链接：
https://files.cnblogs.com/files/BensonLaur/Beslyric-Link.xml 

程序检测ffmpeg前，会获取最新的下载链接，和内置链接一起作为备选链接。为了防止处在前面的链接数据访问过频繁，导致平台限制，链接的使用先后随机打乱。