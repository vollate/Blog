---
title: Intel-SGX
date: 2024-03-17 14:53:45
tags: Knowledge
categories: TEE
---

[paper link](https://eprint.iacr.org/2016/086.pdf)

## Memory Protection

SGX 设置了一块 Processor Reserved Memory (PRM) 用于存储 enclave 的数据，这块内存对于任何来自 non-enclave 区域的代码是不可见的。

OS需要自行维护 TLB 缓存的一致性，防止攻击。

![privelege switch](privelege-switch.png)

## UEFI

![uefi](uefi.png)

SEC implementation is the root of trust

