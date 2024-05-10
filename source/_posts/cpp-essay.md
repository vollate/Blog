---
title: CPP 随笔
date: 2023-11-03 11:20:31
tags: 
- Notes
- C++
categories: 
- Knowledge
---
记录cpp编程过程中的一些总结

## clang + libstdc++ debug 问题

众所周知 LLVM 的 `libc++` 和 GNU 的 `libstdc++` 是两个不同的标准库实现。虽然使用 clang + libstdc++ 编译也可以使用 LLDB 对程序进行 debug，但是字符串以及一些数据结构如 `unordered_map` 无法正确显示值。解决方式:

1. 使用编译选项 "-fstandalone-debug"  
1. 在编译和链接时换为 `libc++`, cmake 中可以使用 `set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")` 来解决。

cheatsheet: cmake 中切换到 llvm 全套 C++ 工具链

```cmake
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
endif ()
```

