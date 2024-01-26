---
title: 编译之法
categories:
- Compiler
date: 2023-12-15 01:23:36
tags:
- Knowledge
---


以 C/C++ 为例，简单介绍一下 Linux 下编译的相关知识。看完应该能掌握基础的编译能力，处理报错不再抓瞎。 ~~麻麻再也不用担心我遇到问题只能瞎逛 stackoverflow 了~~ 该逛还得逛

## 基础知识

### 编译型语言与解释型语言

- 编译型语言是将代码直接编译成机器可以直接执行的机器码，可以直接在目标机器上执行。代表是 Fortran, C, C++, RUST, Go 等。
- 解释型语言是通过解释器逐行解释每条命令来执行对应的操作，运行不需要编译，但是需要对应的解释器解释成对应架构的指令。代表是 Python, ECMAScript, PHP 等等。
- 也还有一些两者兼具的，如 JAVA，编译成字节码，然后由 JVM 解释执行。

### 汇编语言

汇编语言是最接近于机器码的语言，和目标机器的指令集架构紧密相关也称为低级语言。指令集是一种抽象，它将计算机的物理实现抽象成指令集规定的一系列操作，使得同一架构的不同型号计算机能运算同一个程序，且规定了程序如何操作硬件完成计算。汇编之所以成为最接近于机器码的语言，是在于它只需要进行简单的替换就能生成对应的机器码。

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

汇编中没有我们常用的控制流，需要手写判断跳转的操作
2. 可移植性差，只使用于特定架构的计算机。如果你想要你的汇编程序同时在 arm64 和 x86_64 上运行，你需要写两份代码。
3. 其他就不一一列举了……

### 函数声明和定义

在编程中，我们用函数来表示一种特定的操作，它能够处理我们的输入并且产生对应的输出(副作用)。想深入了解可以看看[λ演算](https://zh.wikipedia.org/wiki/%CE%9B%E6%BC%94%E7%AE%97)，这里就不过多介绍了。函数能够简化我们的代码，对于需要在多处重复执行的操作，我们通过将他包装成函数来简化我们的代码。
函数的声明就是告诉编译器我们有这么一个叫xxx的函数，它的输入和输出都是什么类型。定义则是具体描述了函数如何处理输入最终得到输出。

在编程中，函数往往是声明和定义分开，来将逻辑和函数解耦。在 C/C++ 中，我们通常使用 `#include` 预处理命令来引用头文件，头文件中包含了函数声明。预处理器会直接将被 include 的文件内容复制到预处理后的文件对应位置中。

### 编译过程

编译器是一种计算机程序，它会将某种编程语言写成的源代码转换成另一种编程语言。主要用途是将便于人编写、阅读、维护的高级计算机语言所写作的源代码程序，翻译为目标机器能解读、运行的低阶机器语言的程序。将高级语言转化为低级语言的过程称为编译，反过来成为反编译。

一般编译的过程分为如下步骤:

- 预处理: 进行文本替换等其他操作，如 C/C++ 里 "#xxx" 预处理命令的展开以及宏的替换
- 编译：将源代码转换为对应架构的汇编代码，生成`.s`汇编文件
- 汇编：将上一步生成的汇编代码翻译成二进制机器码，生成`.o` object 文件
- 链接：将上一步生成的文件和所需要的静态/动态库链接为一个新的 object 文件

注意不是所有语言的编译都有上述的几个部分，如 JAVA 。

### 静态链接和动态链接

我们可以通过声明让在多个地方使用函数而只需提供一次定义，同理，多个文件可以共用同一个函数，只需要链接包含了函数定义的 library 即可，无需重复编译。链接方式有如下两种:

- 静态链接：将被链接的库的所有代码嵌入到新的可执行文件或目标文件中。
  - 优点：生成的可执行文件不需要依赖外部库
  - 缺点：
    1. 生成文件变大
    1. 如果被链接的库更新，需要重新编译
    1. 无法重复利用共享库
- 动态链接：在程序运行时或运行前的装载阶段将外部库代码加载到内存中的过程。可执行文件只包含对这些库的引用，不包含实际的库代码
  - 优点：
    1. 可执行文件可以更小
    1. 多个可执行文件可以复用同一个 lib
    1. 如果更新不改变函数接口，则无需重新编译
  - 缺点：
    1. 会依赖外部库，如 linux 的 glibc。如果版本不兼容，运行时会报错。
    1. 启动时会略微增加时间

动态链接库 `.dll` (Dynamic link library)是 Windows 下的的称呼，在 Unix/Linux 下称为 Shared Object `.so`，本文会统一叫动态链接库(叫顺口了)。

Linux 下，静态链接库一般以 `.a` 结尾 (archive)

---

## gcc/clang 使用

本章主要介绍在命令行中使用 gcc/clang 进行编译的的过程和注意事项，以下命令中的 clang 都可以换成 gcc。但是注意，不同编译器所支持的编译参数并不完全相同，因此项目中需要对不同的编译器进行不同参数的适配。

- 获取帮助 `man clang`，man 是 Linux 下的一个 reference 页面，安装的程序可以以一定格式提供 manual 供 man 命令调用。man 使用 vim 的键位，按 / 建开始搜索，n 建下一个，N 键上一个，按 q 键退出。你可以使用 `man man` 来查看 man 的 manual，如果你只想获得简单的帮助，请使用 `clang --help`
- 编译一个文件，生成可执行文件
[test.c](test.c)

```shell
clang test.c -o test
```

注意，在 clang/gcc 默认使用 “main” 作为主函数的入点，如果你的文件中没有 main 函数，编译会报错，因为编译器没有找到主函数入口。这里我们使用 “-o” 指定输出文件名称，如果不指定，默认为 "a.out"

- 编译一个库
[mBool.c](mBool.c)

```shell
# static lib
clang -c mBool.c -o mBool.o # 这里不写默认生成同名后缀为.o的对应文件
ar r mBool.a mBool.o        # for GNU toolchain
llvm-ar r libmBool.a mBool.o   # for llvm toolchain
# 其实静态连接库就是编译出的 object 文件改个名字，因为 linux 下静态链接库以 .a 结尾. 上面的指令只是把 .o 改个名存起来

# shared lib
clang -shared mBool.c -o libmBool.so -fPIC
```

上述动态链接库的编译过程中使用了 `-fPIC` 这个参数，意味着生成 position independent code，这样我们的动态链接库就可以在任意地址被装载。

- 链接一个库
[mBoolTest.c](mBoolTest.c)  这里我懒得写头文件了，直接前置声明了

```shell
# link the static lib
clang -o mTest.out mBoolTest.c `pwd`/mBool.a
clang -o mTest.out -static mBoolTest.c -L`pwd` -lmBool      # if no shared lib, "-static" is uneeded

# link the shared lib
clang -o mTest.out mBoolTest.c -L`pwd` -lmBool

# ref:
#  -L${target_lib_path}
#  -l${lib_name}            # compiler will try to find lib${lib_name}.a/so
#  -l:${custom_lib_name}
#  -static                  # use static link
```

对于静态链接库，我们直接将其作为输入即可链接，因为其本质上就是一个编译后生成的二进制文件 `.o`

对于动态链接库，编译器会有一些系统默认的动态库目录，它会进入目录下查找。显然我们当前的文件夹并不在此之列，因此为了让编译器进入我们当前的目录下查找链接库，我们使用 `-L` 来加入我们库所在的目录来让编译器查找。我们之前将编译出来的库命名为 "libmBool.*"，因此可以直接使用 `-l` 来链接我们的库。因为我们的目录下同时存在静态和动态链接库，因此编译器默认使用动态链接。我们可以使用 `-static` 来让编译器使用静态链接库。

- 使用 -rpath
现在我们尝试运行我们刚刚链接完动态链接库的可执行文件，然后就报错了

```text
./mTest.out: error while loading shared libraries: libmBool.so: cannot open shared object file: No such file or directory
```

系统提示我们找不到动态链接库。因为我们没有存储动态链接库的位置，动态链接器(如 ld-linux\.so)不知道去哪加载我们的动态链接库我们使用 `-rpath` 来存储动态链接库的目录信息。可以使用绝对路径或相对路径，一般使用相对路径，这样程序只要保持文件内的结构不变即可在各处执行。

```shell
# use relative path
clang -o mTest.out  mBoolTest.c -L`pwd` -lmBool -rpath .

# use absolute path
clang -o mTest.out  mBoolTest.c -L`pwd` -lmBool -rpath `pwd`
```

- 引用头文件
现在我们不偷懒了，引入一个头文件 [head.h](head.h)，创建一个 [nMBoolTest.c](nMBoolTest.c)，假设头文件存储在目录 `/tmp` 下，我们要让编译器知道我们头文件的位置，使用 `-I` 选项

```shell
clang -o nTest.out nMBoolTest.c -L`pwd` -lmBool -rpath `pwd` -I/tmp
```

- 使用环境变量
假设我们没有 make，cmake 这样的自动化构建工具，或者有一个复杂无比的 Makefile/CMakeLists.txt，无法轻易更改。我们需要让编译器知道我们自定义的 header 和 lib 的位置，这时候就需要我们的环境变量登场了。下面介绍一些常用的，更多的看对应编译器的文档，[GCC](https://gcc.gnu.org/onlinedocs/gcc/Environment-Variables.html)

- C_INCLUDE_PATH：阅读理解
- C_PATH：same as above
- CPLUS_INCLUDE_PATH：阅读理解
- LIBRARY_PATH：告诉连接器库的目录

写法同 `PATH`，用 `:` 分隔

- 常用编译选项:
  - `-g`: generate debug info
  - `-O0, -O1, -O3, -Ofast`: 设置编译器优化等级，等级越高优化越多。O0无优化，Ofast 启用O3的同时使用一些不符合 ISO 标准的优化
  - `-v`：输出执行的命令
  - `-x`：显示指定语言类型
  - `-std=`：指定语言标准
  - `-Wall`：将所有 warning 当作 error
  - `-Wno-deprecated`：如果使用了编译器被弃用的功能，不要产生 warning
  - `-Wno-deprecated-declarations`：不要对使用 C++ 中的 [[deprecated]] 修饰的对象产生 warning
  - `-mx32`：生成32位x86代码
  - `-stdlib=`：指定使用的标准库
  - `-E`：只进行预处理
  - `-S`：生成汇编代码后停止
  - `-c`：编译或汇编对象文件，但不链接
  - more: read the manual

## 常见问题解决

- 运行时装了但找不到动态链接库: 把库目录写进 `LD_LIBRARY_PATH`，这个环境变量告诉动态链接器除了默认目录外该去哪里找库
- 学会看报错，报错是帮你解决问题的
- 心态崩了的话缓上个半天就好了

![fix_all](fix_all.jpg)

## 完

