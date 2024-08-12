---
title: 使用 pass 管理密码
tags: Password
categories: Journal
date: 2024-08-10 19:48:05
---

[zx2c4 pass](https://www.passwordstore.org/) 是一个 unix 下的密码管理工具，使用 gpg 加密密码，配合 git 可以方便的实现密码同步，来彻底摆脱 browser 和其他各式各样非自由密码管理工具。
下面介绍 pass 的配置，密码迁移，以及各个客户端的使用方法。

## 配置

### 安装 pass

UNIX like 系统直接使用包管理器安装即可，我的系统是 Arch, 使用 pacman 安装:

```bash
# 已有 gpg 和 git 的忽略这步
sudo pacman -S gnupg git

# pass 本体为 cli 工具，可以配合 qtpass 来使用
sudo pacman -S pass qtpass
```

如果是windows，可以尝试以下几个平替:

- [prs](https://github.com/timvisee/prs): rust 实现的 pass，但不支持 extension
- [pass-winmenu](https://github.com/geluk/pass-winmenu): 可以实现 pass 的基本功能，但为 GUI 应用
- [passpy](https://github.com/bfrascher/passpy): python 实现的 pass，功能补全且年久失修
- [pass4win](https://github.com/mbos/Pass4Win): 年久失修且已经 archiving ，不建议使用

### 初始化 pass

在使用 pass 之前，先生成 gpg 密钥对。已有 gpg 密钥对可以跳过这一步。

```bash
gpg --full-generate-key # 然后根据提示输入对应信息和你要设置的密码

gpg --list-secret-keys --keyid-format LONG 
# 列出密钥的 gpg-id, 大概的输出长这样:
# /home/foo/.gnupg/pubring.kbx
# ----------------------------------
# sec   ed25519/xxx 20xx-xx-xx [SC] [expires: 20xx-xx-xx]
#       <gpg-id>
# uid                 [ unknown] vollate <uint44t@gmail.com>
# ssb   cv25519/xxxxxxxxxxxxxxxx 20xx-xx-xx [E] [expires: 20xx-xx-xx]

pass init <gpg-id> # 使用 gpg-id 初始化 pass
```

经过初始化，会生成 `~/.password-store` 目录。目录下会生成一个 `.gpg-id` 文件，存放了用来加密密码的 gpg key 的 id。

如果你想要导入或导出 gpg key,使用下面的命令

```bash
# export
gpg --export-secret-keys -a <gpg-id> > private.key

# import
gpg --import private.key
```

注意，使用 `--export-secret-keys` 导出私钥，否则行为为导出公钥。

### 同步密码

使用 git 来同步密码，这里使用 github 作为远程仓库。推荐使用 ssh-key 进行认证，因为github很早就停止了使用密码认证。

首先创建你的 private repo，不用添加 gitignore 或 readme，然后初始化你的 .password-store 并 ref 到远程仓库。~~或者你可以跟我一样偷懒 clone 下来然后 把 .git 扔进 .password-store 里。~~ 之后就可以使用git 来管理密码了。

## 从浏览器迁移

firefox,chrome,edge都支持导出密码为 csv 文件，首先导出密码到 csv 文件，然后安装 [pass-import](https://github.com/roddhjav/pass-import)。 使用下面的命令导入密码:

```bash
pass import <csv-file>
```

更多用法见 [useage](https://github.com/roddhjav/pass-import?tab=readme-ov-file#usage)

导出完后记得清空浏览器保存的密码。如果开启了云同步，需要在 history 中删除密码后同步到云端。
- Chome 直接历史记录清除密码后会自动同步
- Edge 需要在历史记录中删除密码后手动强制同步![fucking microsoft](fucking-mc.png)
- firefox 在 password manager 右上角可以直接删除所有密码。

## 客户端

下面推荐几个个人使用的客户端:

- [qtpass](https://qtpass.org/): pass 的 qt 客户端，支持图形化操作和一定 extension
- [passff](https://codeberg.org/PassFF/passff): firefox 插件，可以在浏览器中使用 pass 来填充密码，基本可以做到完全替代浏览器自带的密码管理器(但是 windows 上支持稀烂)
- [Android-Password-Store](https://github.com/android-password-store/Android-Password-Store): Android 客户端，支持加密解密和 git 同步，但是自动填充稀烂

> 目前 windows 上的 passff 有一些问题，暂时没找到好的解决方案~~不用win就行了~~

## 扩展

pass 支持 extension，具体参考[这个合集](https://github.com/tijn/awesome-password-store)，找自己需要的用。

