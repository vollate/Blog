---
title: Google GN CheatSheet
date: 2024-07-14 14:44:21
tags:
---
## 基础语法

1. 目标(Target)

    - executable：定义一个可执行目标

    ```gn
    executable("my_app") {
        sources = [ "main.cc"  ]
    }
    ```

    - shared_library：定义一个共享库目标

    ```gn
    shared_library("my_shared_lib") {
        sources = [ "shared_lib.cc"  ]
    }
    ```

    - static_library：定义一个静态库目标

    ```gn
    static_library("my_static_lib") {
        sources = [ "static_lib.cc"  ]
    }
    ```

    - source_set：定义一组源文件，通常用于中间构建

    ```gn
    source_set("my_sources") {
        sources = [ "file1.cc", "file2.cc"  ]
    }
    ```

1. 变量
    - sources：定义要包含在目标中的源文件列表

    ```gn
    sources = [ "main.cc", "util.cc"  ]
    ```

    - deps：定义目标的依赖项列表

    ```gn
    deps = [ "//:my_static_lib"  ]
    ```

    - public_deps：类似于 deps，但会将依赖项公开给依赖此目标的其他目标

    ```gn
    public_deps = [ "//:my_shared_lib"  ]
    ```

1. 函数
    - import：导入其他 GN 文件中的内容，只能在文件的顶部使用，用于配置变量和默认值

    ```gn
    import("//build/config.gni")
    ```

    - set_defaults：设置默认值，通常用于设置全局配置

    ```gn
    set_defaults("executable") {
      output_name = "default_executable"
    }
    ```

    - template：定义模板，便于复用构建规则(就是模板函数)

    ```gn
    # definition
    template("foo") {
        assert(defined(invoker.var1), "var1 must be defined")
        print(invoker.var1)
        print(invoker.var2)
        # something
    }

    # useage
    foo("useless"){
        var1 = "foo"
        var2 = "bar"
    }
    ```

1. 配置
    - config：定义一组编译或链接选项，可以在多个目标中复用

    ```gn
    config("default_cflags") {
        cflags = [ "-Wall", "-Werror" ]
    }
    ```

1. 工具链
    - toolchain：定义一个工具链，用于指定编译和链接的工具及其选项

    ```gn
    toolchain("my_toolchain") {
        tool("cc") {
            command = "gcc"
        }
        tool("cxx") {
            command = "g++"
        }
        tool("link") {
            command = "g++"
        }
    }
    ```

1. 过滤器(Filter)
    - sources_assignment_filter：用于过滤 sources 列表中的文件

    ```gn
        sources_assignment_filter = [ "*.cc", "*.h"  ]
    ```

1. 分组
    - group：定义一个构建目标的分组

    ```gn
    group("all") {
          deps = [ ":target1", ":target2"  ]
    }
    ```

1. 外部脚本执行
    - exec_script：运行外部脚本，并将结果导入 GN 配置中

    ```gn
    result = exec_script("find_sources.py", [], "string_list")
    sources = result
    ```

1. 控制流(control flow)
    - if：条件语句，用于在 GN 文件中进行条件构建

    ```gn
    if (is_win) {
        # win config
    } else if (is_linux){
        # linux config
    } else {
        # other config
    }
    ```

    - foreach:编译列表

    ```gn
    var_list=[
        "a",
        "b",
        "c"
    ]
    foreach(var, var_list) {
        rubbish += var
    }
    ```

1. 作用域

    - set_defaults：设置默认值，通常用于设置全局配置

    ```gn
    set_defaults("executable") {
        output_name = "default_executable"
    }
    ```

    - scope：定义一个作用域，可以在 GN 文件中组织和管理变量和目标

    ```gn
    scope("my_scope") {
        sources = [ "file1.cc", "file2.cc"  ]
    }
    ```

## 生成过程

和 CMake 类似，GN从根目录的 `BUILD.gn` 开始解析，通过 dependence 确定依赖子目录，然后在各个依赖的子目录中寻找 `BUILD.gn` 来构建依赖。解析文件的部分依赖脚本语言而不是 CMake 那样可以直接调用内置函数，有更高的灵活性，但是也更复杂。

## 常用命令/参数

- `gn gen out/Default --args='...'`: 在 out/Default 目录生成 ninja 构建文件
- `gn desc out/Default //some_path:target --${additional}`: 显示当前生成项目目录中某个 target 的详细信息
  - `--tree`: 显示生成 target 过程所有依赖调用顺序和参数
  - `--blame`: 显示生成 target 过程的编译参数中每个参数的来源
- `gn check out/Default`: 检查当前项目的配置是否正确
- `gn format`: 格式化所有 BUILD.gn 文件
- `gn gen --ide=${x} out/Default` 生成 IDE 项目文件
  - `--ide=json` 生成 JSON 格式的 IDE 项目文件,用于 clangd 等
  - `--ide=vs` 生成 Visual Studio 项目文件(只支持x64 win32 两种target plateform)
  - `--ide=xcode` 生成 Xcode 项目文件


