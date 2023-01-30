## Git简明教程

仅列出指令。



clone仓库：

```shell
#通过HTTPS
git clone https://github.com/SHTOG/Smart-home.git
#通过SSH
git clone git@github.com:SHTOG/Smart-home.git
```



切换分支：

```shell
$ git checkout demo1                                                                                     
切换到分支 'demo1'
您的分支与上游分支 'origin/demo1' 一致。
```



更新本地仓库代码：

```shell
$ git fetch                                                                                              
remote: Enumerating objects: 57, done.
remote: Counting objects: 100% (40/40), done.
remote: Compressing objects: 100% (6/6), done.
remote: Total 21 (delta 15), reused 21 (delta 15), pack-reused 0
展开对象中: 100% (21/21), 7.33 KiB | 357.00 KiB/s, 完成.
来自 github.com:SHTOG/Smart-home
   b3e3ca7..bdb2d9e  demo1      -> origin/demo1
```



查看工作区与本地仓库代码差异：

```shell
$ git diff bdb2d9e3fb49a0222bc13201ed6ffc67d9cf0e5b                                                       
diff --git "a/\345\237\272\344\272\216STM32F4\347\232\204\346\231\272\350\203\275\345\256\266\345\261\205/HARDWARE/24C02/24Cxx.c" "b/\345\237\272\344\272\216STM32F4\347\232\204\346\231\272\350\203\275\345\256\266\345\261\205/HARDWARE/24C02/24Cxx.c"
index fc2d001..61c16b4 100644
(下略)
```



合并本地仓库代码到工作区：

```shell
$ git merge                                                                                               
更新 b3e3ca7..bdb2d9e
Fast-forward
 .../HARDWARE/24C02/24Cxx.c"                                           | 169 +++++++----
 .../HARDWARE/24C02/24Cxx.h"                                           |  10 +-
 .../USER/Listings/object.map"                                         | 506 ++++++++++++++++----------------
 .../USER/myList.h"                                                    |   6 +-
 .../USER/object.uvguix.Kirito"                                        |  48 +--
 11 files changed, 457 insertions(+), 350 deletions(-)
```



更新本地仓库并合并代码到工作区(等同于`git fetch && git merge`)：

```shell
$ git pull                                                                                               
已经是最新的。
```



合并分支：

```shell
$ git checkout main #切换到main分支，因为现在我们要将demo1合并到main分支
$ git merge demo1
```

可能出现问题：`致命错误：拒绝合并无关的历史`

改为使用如下指令：

```shell
$ git merge demo1 --allow-unrelated-histories                                                             
Merge made by the 'ort' strategy.
 .../HARDWARE/24C02/24Cxx.c"                                           | 169 +++++++----
 .../HARDWARE/24C02/24Cxx.h"                                           |  10 +-
 .../HARDWARE/TIMER/timer.c"                                           |  31 +-
 .../HARDWARE/TIMER/timer.h"                                           |   1 +
 .../USER/AnalyseAndSend.c"                                            |   7 +-
 .../USER/AnalyseAndSend.h"                                            |   6 +
 .../USER/Listings/object.map"                                         | 506 ++++++++++++++++----------------
 .../USER/main.c"                                                      |  12 +-
 .../USER/myList.c"                                                    |  11 +-
 .../USER/myList.h"                                                    |   6 +-
 .../USER/object.uvguix.Kirito"                                        |  48 +--
 11 files changed, 457 insertions(+), 350 deletions(-)
```



向远程仓库提交更改：

```shell
$ git push                                                                                                
枚举对象中: 4, 完成.
对象计数中: 100% (4/4), 完成.
使用 16 个线程进行压缩压缩对象中: 100% (2/2), 完成.
写入对象中: 100% (2/2), 290 字节 | 290.00 KiB/s, 完成.
总共 2（差异 1），复用 0（差异 0），包复用 0
remote: Resolving deltas: 100% (1/1), completed with 1 local object.
To github.com:SHTOG/Smart-home.git
   c2aa24d..8c61425  main -> main
```





你已经掌握git的基本使用流程了，快去GitHub上试试吧！



