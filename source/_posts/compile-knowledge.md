---
title: 编译之法
categories:
  - Compile
  - Linux
date: 2023-12-08 01:23:36
tags:
---

以 C/C++ 为例，简单介绍一下 Linux 下编译的相关知识。看完应该能掌握基础的编译能力，处理报错不再抓瞎。 ~~妈妈再也不用担心我乱逛 stackoverflow 了~~ 该逛还得逛

## 编译器

编译器是一种计算机程序，它会将某种编程语言写成的源代码转换成另一种编程语言。主要目的是将便于人编写、阅读、维护的高级计算机语言所写作的源代码程序，翻译为计算机能解读、运行的低阶机器语言的程序(二进制机器码)。将高级语言转化为低级语言的过程称为编译，反过来成为反编译。

### 汇编语言

汇编语言是最接近于机器的语言，和目标机器的指令集架构紧密相关。指令集是一种抽象，它将计算机的物理实现抽象成指令集规定的一系列操作，使得同一架构的不同型号计算机能运算同一个程序，且规定了程序如何操作硬件完成计算。汇编之所以成为最接近于机器码的语言，是在于它只需要进行简单的替换就能生成对应的机器码。

举个 MIPS32 架构汇编的例子：

```mips
add $1,$2,$3
```

这条指令将2和3号寄存器的值相加存储到1号寄存器中，根据 MIPS32 的转换规则，生成的二进制机器码如下:

opcode |rs |rt| rd |shamt |func
-|-|-|-|-|-
000000 |00010 |00011 |00001 |00000 |100000

- 操作码（opcode）：对于 add 操作，它的操作码是 000000。
- 源寄存器2（rs）：$2 对应的寄存器是 00010。
- 源寄存器3（rt）：$3 对应的寄存器是 00011。
- 目标寄存器（rd）：$1 对应的寄存器是 00001。
- 移位量（shamt）：对于 add 操作而言，不涉及移位，因此为 00000。
- 功能码（funct）：add 操作的功能码是 100000。

汇编语言经过简单的处理就可以直接转换为机器码，但是有许多坏处:

1. 可读性差。以下是一个 clang 生成的简单的 hello world 程序的汇编代码

```asm
 .text
 .file "t.c"
 .globl main                            # -- Begin function main
 .p2align 4, 0x90
 .type main,@function
main:                                   # @main
 .cfi_startproc
# %bb.0:
 pushq %rbp
 .cfi_def_cfa_offset 16
 .cfi_offset %rbp, -16
 movq %rsp, %rbp
 .cfi_def_cfa_register %rbp
 subq $16, %rsp
 movl $0, -4(%rbp)
 leaq .L.str(%rip), %rdi
 movb $0, %al
 callq printf@PLT
 xorl %eax, %eax
 addq $16, %rsp
 popq %rbp
 .cfi_def_cfa %rsp, 8
 retq
.Lfunc_end0:
 .size main, .Lfunc_end0-main
 .cfi_endproc
                                        # -- End function
 .type .L.str,@object                  # @.str
 .section .rodata.str1.1,"aMS",@progbits,1
.L.str:
 .asciz "Hello, world!\n"
 .size .L.str, 15

 .ident "clang version 16.0.6"
 .section ".note.GNU-stack","",@progbits
 .addrsig
 .addrsig_sym printf
```

2. 可移植性差，只使用于特定架构的计算机。如果你想要你的汇编程序同时在 arm64 和 x86_64 上运行，对不起，你需要写两份代码。
3. 其他就不一一列举了……

### 编译过程

一般编译生成可执行文件的过程分为如下步骤:

- 预处理: 进行文本替换等其他操作，如 C/C++ 里 "#xxx" 预处理命令的展开以及宏的替换
- 编译：将源代码转换为对应架构的汇编代码，生成`.s`汇编文件
- 汇编：将上一步生成的汇编代码翻译成二进制机器码，生成`.o` object 文件
- 链接：将上一步生成的文件和所需要的静态/动态库链接为一个新的 object 文件


未完待续
