# LiquidBattle

基于 [Google Liquidfun](https://github.com/google/liquidfun) 的 OOP 大作业 **`LiquidBattle`**

作业与文档编写时使用环境为：`Ubuntu 20.04.2 LTS` + `cmake version 3.16.3` + `g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0`

经测试兼容的环境：`Win10 21H1 Build 19043.1052 家庭中文版` + `cmake version 3.20.2` + `g++ (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0`

### 编译运行指南

在文件目录打开命令行，输入以下命令
```
mkdir build
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j LiquidBattle
./LiquidBattle
```
即可编译运行 `Release` 版本

将命令中的 `Release` 替换为 `Debug` 即可编译运行 `Debug` 版本
