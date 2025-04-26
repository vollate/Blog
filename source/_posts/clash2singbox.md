---
title: Clash 迁移到 Sing-Box
date: 2025-04-26 18:31:17
tags:
  - Singbox
  - Proxy
  - Linux
categories:
  - Journal
---

很早以前就了解到 Sing-Box 了，但是人一直很懒，clash-verge-rev 配置没怎么改就懒得换（不得不提用 Tauri 写的 GUI 真的各种奇奇怪怪的问题）。今天爆了 service 提权漏洞，索性换到 Singbox 了。

Sing-Box 自身在桌面端没有成熟的 GUI，只有 CLI, 这里选用 [GUI for Cores](https://gui-for-cores.github.io/zh/)，支持多种 kernel。

Sing-box 作为重新实现的新内核，支持大多数代理协议，内存占用更低。同时没有 clash-* 那么多历史包袱并且完全开源，安全性更高。

## 安装

笔者的系统是 X64 Arch Linux，内核是 `Linux 6.14.3-arch1-1`,使用 NetworkManager 管理网络,防火墙是 `firewalld`。

作为 Arch 用户，最先尝试的肯定是 AUR 包了，装了发现并不是最新（2025-04-26)，看了下 Github 上的二进制 release 内置滚动更新，直接下载了二进制包。

单独创建一个目录解压后丢进去，配置照着[官方文档](https://gui-for-cores.github.io/zh/guide/gfs/community)就行了。

## 问题&解决

### 防火墙放行 tun 设备

和mihomo(clash-meta)不同，singbox 注册 tun 设备的方式是通过 netlink 的方式注册的，这就导致 NetworkManager 会自动将其注册为一个新的 Connection 而不是 mihomo 的 interface 设备，每次的 uuid 会变化，无法设置 firewalld zone 规则。

解决方法是在 `/etc/NetworkManager/conf.d/` 目录下创建一个"xxx.config"配置文件:

```ini
[keyfile]
unmanaged-devices=interface-name:<Your tun interface name>
```

将 `<Your tun interface name>` 替换为你实际的 tun 设备名称,这个在 Singbox 的配置文件中可以找到，如果是 GUI for singbox 则是在的 profile中配置（记得启用），然后重新加载 NetworkManager:

```bash
systemctl reload NetworkManager
```

然后在 firewalld 中添加规则并重启:

```bash
firewall-cmd --permanent --zone=trusted --add-interface=<Your tun interface name>
firewall-cmd --reload
```

这样 `NetworkManager` 就不会管理这个设备尝试为其创建 Connection，firewalld 便可以根据 interface-name 设置 zone 规则了。

