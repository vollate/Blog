---
title: 编译之法
categories:
  - Compile
  - Linux
date: 2023-12-08 01:23:36
tags:
---

以 C/C++ 为例，简单介绍一下 Linux 下编译的相关知识。看完应该能掌握基础的编译能力，处理报错不再抓瞎。 ~~麻麻再也不用担心我遇到问题只能瞎逛 stackoverflow 了~~ 该逛还得逛

## 基础知识

### 编译型语言与解释型语言

- 编译型语言是将代码直接编译成机器可以直接执行的机器码，可以直接在目标机器上执行。代表是 Fortran, C, C++, RUST, Go 等。
- 解释型语言是通过解释器逐行解释每条命令来执行对应的操作，运行不需要编译，但是需要对应的解释器解释成对应架构的指令。代表是 Python, ECMAScript, PHP 等等。
- 也还有一些两者兼具的，如 JAVA，编译成字节码，然后由 JVM 解释执行。

### 汇编语言

汇编语言是最接近于机器的语言，和目标机器的指令集架构紧密相关也称为低级语言。指令集是一种抽象，它将计算机的物理实现抽象成指令集规定的一系列操作，使得同一架构的不同型号计算机能运算同一个程序，且规定了程序如何操作硬件完成计算。汇编之所以成为最接近于机器码的语言，是在于它只需要进行简单的替换就能生成对应的机器码。

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
    .file   "test.c"
    .globl  main                            # -- Begin function main
    .p2align    4, 0x90
    .type   main,@function
main:                                   # @main
    .cfi_startproc
# %bb.0:
    pushq   %rbp
    .cfi_def_cfa_offset 16
    .cfi_offset %rbp, -16
    movq    %rsp, %rbp
    .cfi_def_cfa_register %rbp
    subq    $16, %rsp
    movl    $0, -4(%rbp)
    leaq    .L.str(%rip), %rdi
    movb    $0, %al
    callq   printf@PLT
    xorl    %eax, %eax
    addq    $16, %rsp
    popq    %rbp
    .cfi_def_cfa %rsp, 8
    retq
.Lfunc_end0:
    .size   main, .Lfunc_end0-main
    .cfi_endproc
                                        # -- End function
    .type   .L.str,@object                  # @.str
    .section    .rodata.str1.1,"aMS",@progbits,1
.L.str:
    .asciz  "Hello, World!\n"
    .size   .L.str, 15

    .ident  "clang version 16.0.6"
    .section    ".note.GNU-stack","",@progbits
    .addrsig
    .addrsig_sym printf
```

汇编中没有我们平时常用的循环控制流，需要手写 jump 操作，一个简单的 for 循环的汇编如下:

```text
Source:
int main() {
  int a;
  for (int i = 0; i < 100; ++i) {
    a += i;
  }
}

Assembly:
    .text
    .file   "test.c"
    .globl  main                            # -- Begin function main
    .p2align    4, 0x90
    .type   main,@function
main:                                   # @main
    .cfi_startproc
# %bb.0:
    pushq   %rbp
    .cfi_def_cfa_offset 16
    .cfi_offset %rbp, -16
    movq    %rsp, %rbp
    .cfi_def_cfa_register %rbp
    movl    $0, -4(%rbp)
    movl    $0, -12(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
    cmpl    $100, -12(%rbp)
    jge .LBB0_4
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
    movl    -12(%rbp), %eax
    addl    -8(%rbp), %eax
    movl    %eax, -8(%rbp)
# %bb.3:                                #   in Loop: Header=BB0_1 Depth=1
    movl    -12(%rbp), %eax
    addl    $1, %eax
    movl    %eax, -12(%rbp)
    jmp .LBB0_1
.LBB0_4:
    movl    -4(%rbp), %eax
    popq    %rbp
    .cfi_def_cfa %rsp, 8
    retq
.Lfunc_end0:
    .size   main, .Lfunc_end0-main
    .cfi_endproc
                                        # -- End function
    .ident  "clang version 16.0.6"
    .section    ".note.GNU-stack","",@progbits
    .addrsig
```

2. 可移植性差，只使用于特定架构的计算机。如果你想要你的汇编程序同时在 arm64 和 x86_64 上运行，你需要写两份代码。
3. 其他就不一一列举了……

### 函数声明和定义

根据并不是高中的数学知识，函数是数学描述对应关系的一种特殊集合。在编程中，我们用函数来表示一种特定的操作，它能够处理我们的输入并且产生对应的输出(副作用)。想深入了解可以看看[λ演算](https://zh.wikipedia.org/wiki/%CE%9B%E6%BC%94%E7%AE%97)，这里就不过多介绍了。函数能够简化我们的代码，对于需要在多处重复执行的操作，我们通过将他包装成函数来简化我们的代码。
函数的声明就是告诉编译器我们有这么一个叫xxx的函数，它的输入和输出都是什么类型。定义则是具体描述了函数如何处理输入最终得到输出。

在编程中，函数往往是声明和定义分开，来将逻辑和函数解耦。在 C/C++ 中，我们通常使用 `#include` 预处理命令来引用头文件，头文件中包含了函数声明。预处理器会直接将被 include 的文件内容复制到预处理后的文件中。


## 编译器

编译器是一种计算机程序，它会将某种编程语言写成的源代码转换成另一种编程语言。主要目的是将便于人编写、阅读、维护的高级计算机语言所写作的源代码程序，翻译为计算机能解读、运行的低阶机器语言的程序(二进制机器码)。将高级语言转化为低级语言的过程称为编译，反过来成为反编译。

### 编译过程

一般编译生成可执行文件的过程分为如下步骤:

- 预处理: 进行文本替换等其他操作，如 C/C++ 里 "#xxx" 预处理命令的展开以及宏的替换
- 编译：将源代码转换为对应架构的汇编代码，生成`.s`汇编文件
- 汇编：将上一步生成的汇编代码翻译成二进制机器码，生成`.o` object 文件
- 链接：将上一步生成的文件和所需要的静态/动态库链接为一个新的 object 文件

注意不是所有语言的编译都有上述的几个部分，如 JAVA

### gcc/clang 使用

本章主要介绍在命令行中使用 gcc/clang 进行编译的的过程和注意事项

#### 基础操作

- 获取帮助 `man clang`，man 是 Linux 下的一个 reference 页面，安装的程序可以以一定格式提供 manual 供 man 命令调用。man 使用 vim 的键位，按 / 建开始搜索，按 q 键退出。~~你可以使用 `man man` 来查看 man 的 manual~~，如果你只想获得简单的帮助，请使用 `clang --help`
- 编译一个文件，生成可执行文件

```shell
clang test.c -o test
```

注意，在 clang/gcc 默认使用 “main” 作为主函数的入点，如果你的文件中没有 main 函数，编译会报错，因为编译器没有找到主函数入口。这里我们使用 “-o” 指定输出文件名称，如果不指定，默认为 "a.out"

- 编译一个静态链接库

