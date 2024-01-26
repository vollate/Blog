---
title: Linux 下系统代理配置
date: 2024-01-09 23:57:33
categories:
- Linux
tags:
- Journal
---
配置 firefox 系统代理时出现了一些问题，然后查了下 Linux 桌面系统代理的原理，记录一下

系统信息:

```text
   OS: Arch Linux x86_64 
   Kernel: 6.6.10-arch1-1 
   Packages: 1369 (pacman) 
   Shell: zsh 5.9 
   DE: Plasma 5.27.10 
   WM: KWin 
   Theme: [Plasma], Canta-dark [GTK2/3] 
   Icons: Tela-circle-manjaro-dark [Plasma], Tela-circle-manjaro-dark [GTK2/3] 
```

使用 `clash-verge` 进行代理，然后 system-proxy 设置为 manual 后 edge 和 chrome 能正常识别。但是 firefox 死活不行，上网查了不支持代理的问题是11年前的。 无奈开始怀疑是 firefox 不支持 KDE 桌面环境的系统代理设置。

![setting](2024-01-10_01-00.png)

## KDE 系统代理配置

如上图，配置好后，系统代理的信息可以在 `~/.config/kioslaverc` 中找到
![rc file](2024-01-10_01-04.png)

## GNOME 系统代理配置

GNOME 桌面同样可以通过 GUI 配置系统代理，具体不展示了因为没 GNOME 桌面(~~全用Arch导致的~~。GNOME 下桌面应用可以通过 `gsettings` 来获取对应的 key 值，如 `gsettings get org.gnome.system.proxy mode` 可以获取现在的系统代理模式。

## 解决问题

发现 `clash-verge` 内置的系统代理配置的是 `gsettings` 的选项，而且 firefox 只认 GNOME 的系统代理配置(Mozilla 快把 firefox 玩死了)。因此之前一直没有成功。Ubuntu 用户太多导致软件适配倾向于 GNOME 桌面，~~但是真的好丑啊 GNOME~~。

>小贴士：一般桌面 app 不吃 env variable 这一套
