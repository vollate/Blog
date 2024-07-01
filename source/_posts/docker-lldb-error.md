---
title: LLDB 在 Docker 中功能异常
date: 2024-06-27 00:36:35
tags:
- Docker
- LLDB
categories:
- Journal
---
## 记(寄)录

写 15445 的时候发现 clang-14 以上编译 fmt 会报错（arch 包太新了导致的），因此在 debian12 的 docker 中挂载 host 文件夹开发。用 lldb debug 时在出现 exception 或 signal后 lldb 会报错 `A packet returned an error 8` 并退出进程，导致无法 backtrace，但是 gdb 没事。
原因是因为 lldb debug 用的 syscall ptrace 在 docker 中被限制了，解决方法是在 `docker run` 时加上 `--cap-add=SYS_PTRACE --security-opt seccomp=unconfined` 参数。
[Problem on stack overflow](https://stackoverflow.com/questions/69291157/lldb-producing-a-packet-returned-error-8-when-i-run-my-program)

## Docker 小知识

- Linux capabilities: Docker 关于 Linux capabilities 的[章节](https://docs.docker.com/engine/reference/run/#runtime-privilege-and-linux-capabilities)。这个是用来限制容器权限的，可以看到默认情况下 "SYS_PTRACE Trace arbitrary processes using ptrace(2)."是被禁用的，然后 lldb 就寄了（
- Secure computing mode(Seccomp): 这是 Linux 内核的一个 feature,用于控制计算时的行为安全。Docker 默认的 profile 禁用了大约 44 个 syscall, ptrace 位列其中。通过 `--security-opt seccomp=unconfined` 来关闭这个 profile（图省事的不安全做法），正确的是改默认 profile 来自定义一个profile。然而我懒，就这样了。

## ptrace 怎么不安全了

**Docker 官方文档**:

- Blocked in Linux kernel versions before 4.8 to avoid seccomp bypass.
- Tracing/profiling arbitrary processes is already blocked by dropping CAP_SYS_PTRACE, because it could leak a lot of information on the host.

上 NVD 搜了一圈，好多文档都写的禁止使用 ptrace, debugger 和 strace（used to trace syscal)，但是没说为啥。cgroup 和 namespace 不是已经隔离了吗，等哪天有空查到再研究下。
<!--TODO-->

## 为什么 GDB 没事

TODO: 找了半天资料没找到，gdb internals 偏偏这一章是咕咕的，先洗洗睡了。
