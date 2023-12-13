---
title: VPS 测试脚本整理
tags:
- Fiddle
Categories:
- VPS
- Server
---
整理自[知乎](https://zhuanlan.zhihu.com/p/117547388)

## 基础测试(IO+网速)

### Bench.sh

```shell
wget -qO- bench.sh | bash
curl -Lso- bench.sh | bash
wget -qO- 86.re/bench.sh | bash
curl -so- 86.re/bench.sh | bash
```

### SuperBench.sh

会安装较多东西，测得也较全，并且网络测速注重回国速度

```shell
wget -qO- --no-check-certificate https://raw.githubusercontent.com/oooldking/script/master/superbench.sh | bash
curl -Lso- -no-check-certificate https://raw.githubusercontent.com/oooldking/script/master/superb
```

