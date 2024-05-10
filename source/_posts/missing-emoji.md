---
title: Arch Linux 下 firefox 无法显示 emoji
date: 2024-02-21 00:11:59
tags:
- Linux
- KDE Plasma
categories: 
- Journal
---

## 问题描述

firefox 无法显示任何 emoji，但 chrome，edge 均显示正常

## 折腾过程

首先怀疑是 firefox 什么地方配置炸了。搜了几篇提问，解决方案都无效，包括 Arch wiki 上的[这个](https://wiki.archlinuxcn.org/wiki/Firefox#%E5%AD%97%E4%BD%93%E9%97%AE%E9%A2%98)。
然后我开了下自己另一台电脑上的 Arch 发现显示正常，于是推断可能是缺字体。具体是什么不清楚，但是包名中大概带 "emoji"。搜了半天没找到个能用的解决方案，noto-font-emoji 装了也没用。最后不抱希望搜了下[package](https://archlinux.org/packages/)，原以为会出现一堆带 emoji 的，结果就十几个。看了看，觉得缺少 "unicode-emoji" 的可能性最高，装了，好了。
然后发现 vim 中的 warning 字体突然变成 unicode 里面那个黄色 warning（丑），于是又把包卸了。重启后发现 firefox 中 emoji 正常显示，大概是本地哪个配置文件炸了一开始才显示不了。

## 总结

人生 -1h
强迫症满足++
折腾 Linux 经验++

