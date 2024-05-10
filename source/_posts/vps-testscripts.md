---
title: VPS 测试脚本整理
tags:
  - Server  
categories:
  - Journal

date: 2023-12-13 15:44:50
---

整理自[知乎](https://zhuanlan.zhihu.com/p/117547388)

## 基础测试(IO+网速)

### Bench

```shell
wget -qO- bench.sh | bash
curl -Lso- bench.sh | bash
wget -qO- 86.re/bench.sh | bash
curl -so- 86.re/bench.sh | bash
```

### SuperBench

会安装较多东西，测得也较全，并且网络测速注重回国速度

```shell
wget -qO- --no-check-certificate https://raw.githubusercontent.com/oooldking/script/master/superbench.sh | bash
curl -Lso- -no-check-certificate https://raw.githubusercontent.com/oooldking/script/master/superb
```

### Superseed

提供测试全国各地三大运营商速度

```shell
wget https://raw.githubusercontent.com/oooldking/script/master/superspeed.sh && chmod +x superspeed.sh && ./superspeed.sh
```

### UnixBench

主要测试项目有：系统调用、读写、进程、图形化测试、2D、3D、管道、运算、C库等系统基准性能提供测试数据，[发布页](https://github.com/teddysun/across?tab=readme-ov-file)

```shell
wget --no-check-certificate https://github.com/teddysun/across/raw/master/unixbench.sh
chmod +x unixbench.sh
./unixbench.sh
```

### Memory Test

```shell
#CentOS / RHEL
yum install wget -y
yum groupinstall "Development Tools" -y
wget https://raw.githubusercontent.com/FunctionClub/Memtester/master/memtester.cpp
gcc -l stdc++ memtester.cpp
./a.out
#Ubuntu / Debian
apt-get update
apt-get install wget build-essential -y
wget https://raw.githubusercontent.com/FunctionClub/Memtester/master/memtester.cpp
gcc -l stdc++ memtester.cpp
./a.out
```

