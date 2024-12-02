---
title: Systemd network-online.target 问题排查记录
date: 2024-12-02 20:46:13
tags:
- Linux
- Systemd
categories:
- Journal
---


~~clash verge rev~~ 某不存在的软件更新到 2.0 后，启动时会安装 systemd 服务用于 TUN 网络代理。但升级后每次都需要重新执行 `uninstall-service` 和 `install-service`, 且必定会卡死一段时间。这里记录一下排查过程。

## 检查为何要执行 `uninstall-service` 和 `install-service`

查看[源码](https://github.com/clash-verge-rev/clash-verge-rev/blob/ae5b2cfb79423c7e76a281725209b812774367fa/src-tauri/src/core/service.rs#L76)得知，service 安装由这个函数执行，检查调用他的 [`check_service`](https://github.com/clash-verge-rev/clash-verge-rev/blob/ae5b2cfb79423c7e76a281725209b812774367fa/src-tauri/src/core/service.rs#L163) 函数发现对 "SERVICE_URL/get_clash" 发 GET 请求来检测服务是否运行。 "SERVICE_URL" 是一个常量,为 "<http://127.0.0.1:33211>"

lsof 看看是什么进程

```shell
❯ sudo lsof -i :33211
COMMAND   PID USER FD   TYPE DEVICE SIZE/OFF NODE NAME
clash-ver 738 root 9u  IPv4   5515      0t0  TCP localhost:33211 (LISTEN)
❯ sudo lsof -p 738
lsof: WARNING: can't stat() fuse.portal file system /run/user/1001/doc
      Output information may be incomplete.
COMMAND   PID USER  FD      TYPE             DEVICE SIZE/OFF    NODE NAME
clash-ver 738 root cwd       DIR              259,6     4096       2 /
clash-ver 738 root rtd       DIR              259,6     4096       2 /
clash-ver 738 root txt       REG              259,6  1068272 4667021 /usr/bin/clash-verge-service
clash-ver 738 root mem       REG              259,6  2014520 4590980 /usr/lib/libc.so.6
clash-ver 738 root mem       REG              259,6    14280 4590996 /usr/lib/libdl.so.2
clash-ver 738 root mem       REG              259,6   973144 4591006 /usr/lib/libm.so.6
clash-ver 738 root mem       REG              259,6    14288 4591034 /usr/lib/libpthread.so.0
clash-ver 738 root mem       REG              259,6    14352 4591039 /usr/lib/librt.so.1
clash-ver 738 root mem       REG              259,6   915712 4591699 /usr/lib/libgcc_s.so.1
clash-ver 738 root mem       REG              259,6   228376 4590929 /usr/lib/ld-linux-x86-64.so.2
clash-ver 738 root   0r      CHR                1,3      0t0       4 /dev/null
clash-ver 738 root   1u     unix 0x000000005430ea85      0t0   17938 type=STREAM (CONNECTED)
clash-ver 738 root   2u     unix 0x000000005430ea85      0t0   17938 type=STREAM (CONNECTED)
clash-ver 738 root   3u  a_inode               0,16        0    2087 [eventpoll:4,8,9]
clash-ver 738 root   4u  a_inode               0,16        0    2087 [eventfd:0]
clash-ver 738 root   5u  a_inode               0,16        0    2087 [eventpoll:4,8,9]
clash-ver 738 root   6u     unix 0x00000000cb59cf7b      0t0    2387 type=STREAM (CONNECTED)
clash-ver 738 root   7u     unix 0x00000000093de046      0t0    2388 type=STREAM (CONNECTED)
clash-ver 738 root   8u     unix 0x00000000cb59cf7b      0t0    2387 type=STREAM (CONNECTED)
clash-ver 738 root   9u     IPv4               5515      0t0     TCP localhost:33211 (LISTEN)
```

## 检查服务状态

看看装了哪些服务

```shell
❯ systemctl list-units|grep clash
  clash-verge-service.service
```

使用 `systemctl status clash-verge-service.service` 查看服务状态和文件位置，打开后如下

```systemd
[Unit]
Description=Clash Verge Service helps to launch Clash Core.
After=network-online.target nftables.service iptables.service

[Service]
Type=simple
ExecStart=/usr/bin/clash-verge-service
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```

Systemd serive 定义中，`After=network-online.target nftables.service iptables.service` 表明至少一个服务完成启动后才会启动 clash-verge-service。
我的系统使用 `firewalld` 作为防火墙,它会作为上层服务自动启用 `nftables.service` 和 `iptables.service` 作为底层服务，所以 `nftables.service iptables.service` 两个服务不会启动，因此只需要等待 `network-online.target` 启动完成即可。

## 测试 network-online.target

执行 `systemctl start network-online.target`，发现会卡死将近 2 min。使用 `systemd-analyze critical-chain network-online.target` 检查依赖和耗时，发现实际执行(CPU 时间)正常，但是启动前经过了长时间等待。

接下来查了下 `network-online.target` 的依赖和被依赖，没什么问题

```shell
❯ systemctl list-dependencies network-online.target --after
network-online.target
○ ├─NetworkManager-wait-online.service
● └─network.target
●   ├─dhcpcd.service
●   ├─NetworkManager.service
●   ├─systemd-networkd.service
●   ├─wpa_supplicant.service
●   └─network-pre.target
●     ├─firewalld.service
○     ├─ip6tables.service
○     ├─iptables.service
○     ├─nftables.service
●     └─systemd-network-generator.service

❯ systemctl list-dependencies network-online.target --before
network-online.target
○ ├─archlinux-keyring-wkd-sync.service
● ├─clash-verge-service.service
○ ├─docker.service
○ └─shutdown.target
○   ├─systemd-halt.service
○   ├─systemd-kexec.service
○   ├─systemd-poweroff.service
○   ├─systemd-reboot.service
○   ├─systemd-soft-reboot.service
○   └─final.target
○     ├─systemd-halt.service
○     ├─systemd-kexec.service
○     ├─systemd-poweroff.service
○     ├─systemd-reboot.service
○     └─systemd-soft-reboot.service
```

同样，执行任何依赖于 `network-online.target` 的服务都会卡死将近 2 min（我就说我 docker.service 为什么启动慢）

参考 [systemd.io](https://systemd.io/NETWORK_ONLINE/)，可以知道在使用 systemd 的系统中，`network-online.target` 一般会在网络连接成功后启动,用于确保服务在网络连接成功后启动来。如需要进行远程磁盘挂载、远程数据库连接等操作，可以将服务依赖于 `network-online.target`。
`network-online.target` 主要通过 `NetworkManager-wait-online.service` 或 `systemd-networkd-wait-online.service` 来启动。我使用的是 NetworkManager，所以应该是 `NetworkManager-wait-online.service`。
在 “To verify that the right service is enabled (usually only one should be):” 章节中提到了可以使用如下命令验证 service 正常配置：

```shell
$ systemctl is-enabled NetworkManager-wait-online.service systemd-networkd-wait-online.service
disabled
enabled
```

经排查发现同时启用了 `NetworkManager-wait-online.service` 和 `systemd-networkd-wait-online.service`，导致了问题。禁用 `systemd-networkd-wait-online.service` 后，`network-online.target` 启动正常。

