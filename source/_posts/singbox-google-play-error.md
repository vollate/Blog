---
title: 记 Singbox Google Play 连接问题解决
date: 2025-07-14 23:29:10
tags:
- Singbox
- Android
categories:
- Journal
---

## 问题描述

自从电脑从 clash verge rev 换到 Singbox 并且配置好自己的 doh3 dns server 后，手机上的代理软件也从 Surfboard 换成的 Singbox 的安卓版并导入电脑上配置好的配置文件。导入之后一切安好，除了 Google Play 商店无法下载，永远卡在转圈。开了 global 模式后，问题解决，因此是规则配置的问题。
不得不说，安卓版 UI 非常简陋，单纯做了个壳子。能用，也仅限能用。log 输出非常不友好，也没有暂停按键，开 debug level 后 log 刷新速度非常快，不是人能看的。不过通过 clash api, 我们可以远程获取到 log 信息。

## 获取 log

首先连接手机并启用 usb debug，电脑上执行

```shell
adb forward tcp:<pc-port> tcp:<phone-port>
```

将手机上 clash api 的端口转发到电脑上，然后通过浏览器访问 `http://localhost:<pc-port>` 就可以调用了。
web ui 一开始尝试使用 yacd,但是发现 log 输出~~花里胡哨~~难以复制并分析，换了 Dreamacro/clash-dashboard 后好多了。

## 问题分析

截取了使用 clash `Rule mode` 和 `Global mode` 的 log 信息，对照后问题如下

```text
# ——— Rule mode ———
[3517687682 …] inbound … to services.googleapis.cn:443
[3517687682 …] outbound/direct[🎯 Direct]: outbound connection to services.googleapis.cn:443

# ——— 切到 Global 模式 后 ———
[4166616312 …] inbound … to services.googleapis.cn:443
[4166616312 …] outbound/vless[这是马赛克(]: outbound connection to services.googleapis.cn:443
```

查询后发现是 services.googleapis.cn 的返回导致之后的下载链接到的是 google play cn 的域名，显而易见无法连接(。将 `services.googleapis.cn` 路由规则改为走代理后，问题解决。

## 疑问

那么问题来了，为什么即使开了 global 模式，还是会去走 cn 的域名呢。查询后发现是 GMS 会对手机 SIM 卡的 MCC/MNC 码进行判断。当 MCC 为 460 时，GMS 就将设备标记为“中国大陆用户”。触发 GMS 内部一个写死的（硬编码的）规则。这个规则将标准的 Google 服务域名（如 \*.googleapis.com, \*.gstatic.com）直接在客户端层面就替换为中国专用的域名（\*.googleapis.cn, \*.gstatic.cn）。
~~笔者刚好有一张旅游办的海外电话卡~~，拔掉国内卡并插上后，重新检查 Singbox log，发现请求域名变成了 `firebaselogging.googleapis.com`，验证了上述原因。

> 好久没写~~这种问题记录~~ blog 了，主要感觉没什么技术含量，而且写起来的时间成本太高，比解决问题的时间还长（
> 不过还是有用的，至少写 blog 会让你想去挖一下问题背后的原因而不是解决了就完事(人肉 AI Agent)，不过还是好水
