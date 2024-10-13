---
title: CMake 从入门到能用就行
tags:
  - CMake
categories:
  - Knowledge
date: 2024-10-13 01:49:55
---


## 构建系统发展历史

1. 手工编译 (1950s - 1960s): 编程语言刚刚诞生，程序员需要手工编译程序，将源代码转换为机器码。
2. 批处理脚本 (1960s - 1970s)： 使用批处理脚本或 shell 脚本自动化编译命令，减少了手工输入。
3. Make 系统 (1970s)： Make 引入了增量编译的概念，只重新编译修改过的文件，大幅提升效率。
4. 配置和生成工具 (1980s - 1990s)： Autotools 等工具通过自动生成跨平台的构建脚本，支持多平台开发，避免了手写 Makefile 的情况。
5. 现代构建系统 (2000s - 2010s)： CMake 和 Meson 等工具采用声明式语法，简化了复杂项目的构建过程。
6. 专用构建系统和构建工具链 (2010s - Present)：Ninja、Gradle、Bazel、Buck 等专注于加速大型项目的构建，提供高度的自定义功能。
7. 云原生和分布式构建 (2010s - Now)： 支持云原生和分布式开发场景的构建系统，集成了容器化和 CI/CD 工具。

<br/>

**增量编译**是指每次只编译修改过的编译单元，避免了无效编译。
例如，在 C/C++ 中，每个源文件对应一个编译单元，只有修改过的源文件才需要重新编译。同时由于预处理的原因，头文件的变更会递归影响到所有直接或间接引用该头文件的源文件。

## GNU Make

Make 是一个构建工具，通过 Makefile 文件定义项目的构建规则。Makefile 文件包含了一系列规则，每个规则由一个目标、依赖和命令组成。

```makefile
# Variable
CC = clang
CFLAGS = -Wall -Wextra -Werror

# Target
all: program

foo.o: foo.c
  $(CC) $(CFLAGS) -c foo.c -o $@
# $@ refers to the target
# $< refers to the first dependency
# $^ refers to all dependencies

main.o: main.c
  $(CC) $(CFLAGS) -c main.c -o $@

program: main.o foo.o
  $(CC) $(CFLAGS) main.o foo.o -o program

include subdir/Makefile
```

make 是一个过程化的构建工具，需要手动书写构建命令，灵活度高但可维护性差。

## CMake

CMake 作为元构建系统，可以生成 Make、Ninja、Visual Studio 等构建系统所需的配置文件，方便的实现项目的跨平台。CMake 的配置文件是 CMakeLists.txt，通过编写 CMakeLists.txt 文件，可以定义项目的构建规则。

### Version

cmake 不同版本之间的命令可能有所不同，新版可能会添加新的命令，并且会废弃一些旧的命令（但大部分情况仍然可用）, 所以在编写 CMakeLists.txt 文件时，需要限定 CMake 的最低版本。

```cmake
cmake_minimum_required(VERSION 3.16) # minimum cmake version
# This is a line comment in cmake
#[[ This is a block comment in cmake ]]
```

### Project

Project 命令用于定义项目的名称和支持的语言。对于支持的语言，可以使用 C、CXX、CUDA、Fortran、ASM 等。默认情况下，CMake 会自动检测项目中的源文件，并根据文件后缀名推断语言类型。

Project 支持设置项目的版本号，可以通过 VERSION 选项指定项目的版本号。

```cmake
project(hello_world) # project name
project(hello_wrold VERSION 5.14 LANGUAGES CXX CUDA) # project name with languages
```

### Variable

CMake 中的变量分为如下几种：普通变量，缓存变量，环境变量。
普通变量仅在本次运行cmake期间有效；缓存变量会被存储到生成项目目录的 CMakeCache.txt 文件中，用途主要有以下几点：

- 避免用户再次运行 cmake 重新设置变量
- 存储某些运行时信息，对用户不可见。如系统库的路径、编译器的路径等。

环境变量取决于运行 cmake 的环境

```cmake
set(SRC_FILES main.cpp lib.cpp) # set Variable
set(CMAKE_CXX_STANDARD 17) # set Variable
set(CMAKE_CXX_STANDARD_REQUIRED ON cache) # set Cache Variable
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}") # print message
message(WARNING "$ENV{USER}") # print warning message
message(ERROR "Error message") # print error message
```

CMAKE 中，所有变量都是字符串，访问不存在的变量会返回空（不是空字符串），如果无法正常处理会产生问题。因此常用的方法是使用 `if` 判断变量是否存在。

```cmake
if(DEFINED CMAKE_CXX_STANDARD)
  message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
else()
  message(WARNING "CMAKE_CXX_STANDARD is not defined")
endif()
```

在比较时，则可以直接加上引号，这样即使变量不存在也不会报错。

```cmake
if("${CMAKE_CXX_STANDARD}" STREQUAL "17")
  message(STATUS "CMAKE_CXX_STANDARD is 17")
else()
  message(WARNING "CMAKE_CXX_STANDARD is not 17")
endif()
```

### Control Flow

同大部分编程语言一样，CMake 也支持条件判断和循环控制。

#### if

if 语句支持 AND、OR、NOT 逻辑运算符，可以使用 PARENT_SCOPE 选项将变量传递到父作用域。

```cmake
if(CMAKE_CXX_STANDARD EQUAL 17 OR ENABLE_CXX17)
  message(STATUS "CMAKE_CXX_STANDARD is 17")
elseif(CMAKE_CXX_STANDARD EQUAL 14 AND CMAKE_CXX_STANDARD_REQUIRED)
  message(STATUS "CMAKE_CXX_STANDARD is 14")
else()
  message(WARNING "CMAKE_CXX_STANDARD is not 17 or 14") 
endif()
```

任何空字符串、0、FALSE、NOTFOUND、OFF、NO、N、IGNORE、无定义的变量都会被视为假，其他值都会被视为真。

#### loop

CMake 支持 FOREACH、WHILE、WHILE、UNTIL 循环，其中 FOREACH 循环最为常用。

```cmake
set(SRC_FILES main.cpp lib.cpp a.cpp b.cpp)
foreach(file ${SRC_FILES})
  message(STATUS "Source file: ${file}")
endforeach()
```

### Subdir

CMake 支持子目录，可以通过 add_subdirectory 命令添加子目录，子目录中的 CMakeLists.txt 文件会被执行。

eg：假设项目目录结构如下

```txt
.
├── 1
│   └── CMakeLists.txt
├── 2
│   └── CMakeLists.txt
└── CMakeLists.txt
```

根目录的 CMakeLists.txt 文件如下

```cmake
project(foo)
set(useless "root")
message(STATUS "Root: ${useless}")
add_subdirectory(1)
message(STATUS "Root: ${useless}")
add_subdirectory(2)
message(STATUS "Root: ${useless}")
message(STATUS "Root: ${useless2}")
```

1/CMakeLists.txt 文件如下

```cmake
message(STATUS "Dir1: ${useless}")
set(useless "dir1")
message(STATUS "Dir1: ${useless}")
```

2/CMakeLists.txt 文件如下

```cmake
set(useless "dir2" PARENT_SCOPE)
set(useless2 "dir2")
message(STATUS "Dir2: ${useless2}")
```

运行`cmake -Bbuild` 输出如下

```txt
// something
-- Root: root
-- Dir1: root
-- Dir1: dir1
-- Root: root
-- Dir2: dir2
-- Root: dir2
-- Root:
// something
```

可以看到，子目录中的变量不会影响到父目录，但是可以通过 PARENT_SCOPE 选项将变量传递到父目录。子目录调用完成后，返回到父目录，继续执行。

CMake 支持 include 命令，可以包含其他 CMakeLists.txt 文件。include 命令会将被包含的文件的内容直接插入到当前文件中，因此被包含的文件中定义的变量会影响到包含的文件。和 C/CPP的预处理#include 作用相同。

### Executable/Lib

CMake 中支持三种类型的构建目标：可执行文件（add_executable）、静态库（add_library STATIC）、动态库（add_library SHARED）。

```cmake
add_executable(hello_world main.cpp) # add executable target
add_library(hello STATIC lib.cpp) # add static library target
add_library(hello SHARED lib.cpp) # add shared library target
```

### include_directories

include_directories 命令用于添加头文件搜索路径，可以添加多个路径。使用 BEFORE 选项可以将路径添加到已有路径的前面，放置被其他路径覆盖。

```cmake
include_directories(include/foo) # add include path
include_directories(BEFORE include/bar) # add include path before
```

### target_xxx

target_xxx 系列命令用于设置目标的属性，如编译选项、链接选项、依赖等。分为三种属性：PRIVATE, PUBLIC, INTERFACE。

- PRIVATE 属性只对当前目标有效, 不会传递给依赖目标
- PUBLIC 属性对当前目标和依赖目标都有效
- INTERFACE 属性只对依赖目标有效,不会对当前目标有效

```cmake
target_include_directories(hello PUBLIC include) # add include path for target hello and any target that depends on hello
target_compile_definitions(hello PRIVATE DEBUG) # add compile definition for target hello
target_link_libraries(hello INTERFACE foo) # link library foo to targets who depends on hello
```

常用的 target_xxx 命令有：

- target_include_directories 添加头文件搜索路径
- target_compile_definitions 添加编译宏定义
- target_compile_options 添加编译选项
- target_link_libraries 添加链接库

### find_package

find_package 命令用于查找外部库，有如下几种模式：

- Module 模式：在指定的路径下查找 `Find<package>.cmake` 文件，这个文件包含了查找库的规则，如何设置库的头文件路径、库文件路径、链接库等。
- Config 模式：在指定的路径下查找 `<package>Config.cmake` 或 `<lower-case-package>-config.cmake` 文件，这个文件包含了库的配置信息，如头文件路径、库文件路径、链接库等。

其他可选项

- `REQUIRED`: 如果找不到库，CMake 会报错并停止构建。如果 `find_package` 成功，会定义一个 `<package>_FOUND` 变量，表示找到库。
- `OPTIONAL`: 如果找不到库，CMake 会继续构建。
- `QUIET`: 失败时不输出警告。
- `PATHS` & `HINTS`: 指定查找路径，其中 `HINTS` 优先级高于 `PATHS`。
- `<package>_ROOT` 变量或环境变量如果设置，会优先使用。

>具体查找顺序参考 [CMake 官方文档](https://cmake.org/cmake/help/latest/command/find_package.html#find-package) 💩

```cmake
find_package(Boost) # find boost library
if(Boost_FOUND)
  message(STATUS "Boost found")
else()
  message(FATAL_ERROR "Boost not found")
endif()

find_package(OpenCV CONFIG REQUIRED) # find opencv library
find_package(OpenGL REQUIRED HINTS /usr/lib) # find opengl library
```

### ctest

ctest 是 CMake 的测试工具，可以用于运行测试。可以通过 `add_test` 指令添加对应的测试。签名如下: `add_test(NAME <test_name> COMMAND <executable> [arg1 [arg2 ...]])`

```cmake
enable_testing() # enable testing
add_executable(test test.cpp) # add test target
add_test(NAME example-test COMMAND test config.yaml > ${CMAKE_BINARY_DIR}/test.log) # add test
```

### 常用变量

- CMAKE_SOURCE_DIR: 项目根目录
- CMAKE_BINARY_DIR: 项目构建目录
- CMAKE_CURRENT_SOURCE_DIR: 当前处理的 CMakeLists.txt 文件所在的目录
- CMAKE_PREFIX_PATH: 查找库的路径
- CMAKE_MODULE_PATH: 查找模块的路径
- CMAKE_INCLUDE_PATH: 查找头文件的路径
- CMAKE_LIBRARY_PATH: 查找库文件的路径
- CMAKE_INSTALL_PREFIX: 安装目录
- CMAKE_BUILD_TYPE: 构建类型，如 Debug、Release、RelWithDebInfo、MinSizeRel
- CMAKE_C_COMPILER: C 编译器
- CMAKE_CXX_COMPILER: C++ 编译器
- CMAKE_C_FLAGS: C 编译选项
- CMAKE_CXX_FLAGS: C++ 编译选项
- CMAKE_EXE_LINKER_FLAGS: 可执行文件链接选项
- CMAKE_LINKER_FLAGS: 链接选项

### CMake Cli

虽然 CMake 有提供 GUI 工具，但是大部分情况下还是使用命令行工具。CMake 的命令行工具是 cmake，可以通过 cmake --help 查看帮助信息。

常用命令行参数：

- `cmake -B<build-dir>`: 指定构建目录
- `cmake <source-dir>`: 指定源代码目录
- `cmake --build <build-dir>`: 构建项目
- `cmake -D<variable>=<value>`: 设置变量
- `ctest -C <build-dir>`: 在指定的构建目录下运行测试

## 拓展

- [让 CMake install 支持 find_package](https://www.foonathan.net/2016/03/cmake-install/)
- [vcpkg](https://github.com/microsoft/vcpkg)
- [precompiled headers](https://cmake.org/cmake/help/latest/command/target_precompile_headers.html)

## 废话

由于 CMake 语法过于复杂，实际使用时应多去官网查找用法。网址贴到[这里](https://cmake.org/documentation/)，不过官网的文档查找效率低下，个人一般都是找大型 cmake 项目直接抄 CMakeLists.txt （

![😆](Image_1728729096791.jpg)

<!--CMake 语法是依托，历史遗留太多(支持 JAVA 没看懂)。但是由于其存在已久，大量的开源库都使用 CMake 作为构建工具,加上目前比较好用的 C/C++ 包管理区 `vcpkg` 也使用 CMake 作为构建工具，所以学习 CMake 是必要的。-->

