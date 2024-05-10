---
title: gRPC 学习
date: 2024-05-10 18:42:59
tags: 
- Notes
- RPC
Cagetories: 
- Knowledge
---

## 阅读须知

本文仅供学习参考，笔者的 protobuf 版本为 3.21.12.0，不保证其他版本的兼容性。
对于参考本文而产生的任何生产上的问题本文作者概不负责。

## proto file

`.proto` 文件是 gRPC 的核心文件，用于定义服务和消息格式。通过 `protobuf` 的 `protoc` 可以生成符合同一通讯协议的对应的代码，以此实现跨语言通讯。

废话不多说，直接上例子

```proto
syntax = "proto3";// proto file version
package package_name;

// define the service
service ChatService {
  rpc PublishRoom(PublishRoomRequest) returns (PublishRoomReply);
  rpc GetRoomPeers(GetRoomPeersRequest) returns (GetRoomPeersReply);
}

// define the status code(enum)
enum StatusCode{
    OK = 0;
    ERROR = 1;
}

// define the request and reply message
message PublishRoomRequest {
    string name = 1;
    string password = 2;
    oneof contact {// like union in c/c++
        string email = 3;
        string phone = 4;
        int64 id = 5;
    }
}

message PublishRoomReply {
    bool success = 1;
    string message = 2;
}

message GetRoomPeersRequest{
    string name = 1;
    string password = 2;
    string id   = 3;
}

message GetRoomPeersReply{
    bool success = 1;
    repeated string peers = 2;
    string message = 3;
}
```

好了，现在我们大概知道了如何编写一个 `.proto` 文件，接下来我们详细讲解细节

### 语法

首先附上[**官方文档**](https://protobuf.dev/programming-guides/)

proto file 的语法分为两个版本，分别是 `proto2` 和 `proto3`，两个版语法本非常相似，但`proto3` 取消了 `require/optional` field，因此不建议给自己找事跨版本通讯。本文仅介绍 `proto3` 的语法。

- 在 `.proto` 文件的开头，我们需要声明使用的版本 `syntax = "proto${x}";`，`${x}` 为版本号，一定要在开头声明，否则默认会使用 `proto2` 的语法。
- 接下来我们需要声明包名 `package package_name;`，包名用于区分不同的服务，避免命名冲突，实际编码中会转换成各个语言的包名或名称空间，如 C++ 会转换成 `namespace package_name;`， Rust 会转换成 `mod package_name;`
- `service` 用于定义服务，服务中包含多个 `rpc` 方法，`rpc` 方法包含请求和响应消息，具体会在后面讲解
- 之后是各类复合类型定义，如 `enum`、`message` 等，该模块和 service 定义没有顺序要求，只要 `service` 中使用的复合类型在本文件中定义即可

#### Scalar Value Types

gRPC 中共用如下几种基础类型，在编译时会转换成对应语言的基础类型:

- 变长编码（Varint）：
  - int32（有符号整数）
  - int64（有符号整数）
  - uint32（无符号整数）
  - uint64（无符号整数）
  - sint32（有符号整数，使用 ZigZag 编码）
  - sint64（有符号整数，使用 ZigZag 编码）
  - bool（布尔值，存储为 0 或 1）
  - enum（枚举，通常作为 Varint 编码的整数）

- 固定长度编码：
  - fixed32（32 位有符号整数，固定 4 字节）
  - fixed64（64 位有符号整数，固定 8 字节）
  - sfixed32（32 位有符号整数，固定 4 字节）
  - sfixed64（64 位有符号整数，固定 8 字节）
  - float（32 位浮点数，固定 4 字节）
  - double（64 位浮点数，固定 8 字节）

- 长度前缀编码：
  - string（字符串）
  - bytes（二进制数据）
  - message（嵌套消息）

这种多种编码方式允许 Protocol Buffers 以高效和紧凑的方式进行序列化。

<details>
  <summary>各语言具体转换，点我展开</summary>
  <table style="width:100%"><tbody><tr><th>.proto Type</th><th>Notes</th><th>C++ Type</th><th>Java/Kotlin Type<sup>[1]</sup></th><th>Python Type<sup>[3]</sup></th><th>Go Type</th><th>Ruby Type</th><th>C# Type</th><th>PHP Type</th><th>Dart Type</th></tr><tr><td>double</td><td></td><td>double</td><td>double</td><td>float</td><td>float64</td><td>Float</td><td>double</td><td>float</td><td>double</td></tr><tr><td>float</td><td></td><td>float</td><td>float</td><td>float</td><td>float32</td><td>Float</td><td>float</td><td>float</td><td>double</td></tr><tr><td>int32</td><td>Uses variable-length encoding. Inefficient for encoding negative
numbers – if your field is likely to have negative values, use sint32
instead.</td><td>int32</td><td>int</td><td>int</td><td>int32</td><td>Fixnum or Bignum (as required)</td><td>int</td><td>integer</td><td>int</td></tr><tr><td>int64</td><td>Uses variable-length encoding. Inefficient for encoding negative
numbers – if your field is likely to have negative values, use sint64
instead.</td><td>int64</td><td>long</td><td>int/long<sup>[4]</sup></td><td>int64</td><td>Bignum</td><td>long</td><td>integer/string<sup>[6]</sup></td><td>Int64</td></tr><tr><td>uint32</td><td>Uses variable-length encoding.</td><td>uint32</td><td>int<sup>[2]</sup></td><td>int/long<sup>[4]</sup></td><td>uint32</td><td>Fixnum or Bignum (as required)</td><td>uint</td><td>integer</td><td>int</td></tr><tr><td>uint64</td><td>Uses variable-length encoding.</td><td>uint64</td><td>long<sup>[2]</sup></td><td>int/long<sup>[4]</sup></td><td>uint64</td><td>Bignum</td><td>ulong</td><td>integer/string<sup>[6]</sup></td><td>Int64</td></tr><tr><td>sint32</td><td>Uses variable-length encoding. Signed int value. These more
efficiently encode negative numbers than regular int32s.</td><td>int32</td><td>int</td><td>int</td><td>int32</td><td>Fixnum or Bignum (as required)</td><td>int</td><td>integer</td><td>int</td></tr><tr><td>sint64</td><td>Uses variable-length encoding. Signed int value. These more
efficiently encode negative numbers than regular int64s.</td><td>int64</td><td>long</td><td>int/long<sup>[4]</sup></td><td>int64</td><td>Bignum</td><td>long</td><td>integer/string<sup>[6]</sup></td><td>Int64</td></tr><tr><td>fixed32</td><td>Always four bytes. More efficient than uint32 if values are often
greater than 2<sup>28</sup>.</td><td>uint32</td><td>int<sup>[2]</sup></td><td>int/long<sup>[4]</sup></td><td>uint32</td><td>Fixnum or Bignum (as required)</td><td>uint</td><td>integer</td><td>int</td></tr><tr><td>fixed64</td><td>Always eight bytes. More efficient than uint64 if values are often
greater than 2<sup>56</sup>.</td><td>uint64</td><td>long<sup>[2]</sup></td><td>int/long<sup>[4]</sup></td><td>uint64</td><td>Bignum</td><td>ulong</td><td>integer/string<sup>[6]</sup></td><td>Int64</td></tr><tr><td>sfixed32</td><td>Always four bytes.</td><td>int32</td><td>int</td><td>int</td><td>int32</td><td>Fixnum or Bignum (as required)</td><td>int</td><td>integer</td><td>int</td></tr><tr><td>sfixed64</td><td>Always eight bytes.</td><td>int64</td><td>long</td><td>int/long<sup>[4]</sup></td><td>int64</td><td>Bignum</td><td>long</td><td>integer/string<sup>[6]</sup></td><td>Int64</td></tr><tr><td>bool</td><td></td><td>bool</td><td>boolean</td><td>bool</td><td>bool</td><td>TrueClass/FalseClass</td><td>bool</td><td>boolean</td><td>bool</td></tr><tr><td>string</td><td>A string must always contain UTF-8 encoded or 7-bit ASCII text, and cannot
be longer than 2<sup>32</sup>.</td><td>string</td><td>String</td><td>str/unicode<sup>[5]</sup></td><td>string</td><td>String (UTF-8)</td><td>string</td><td>string</td><td>String</td></tr><tr><td>bytes</td><td>May contain any arbitrary sequence of bytes no longer than 2<sup>32</sup>.</td><td>string</td><td>ByteString</td><td>str (Python 2)<br>bytes (Python 3)</td><td>[]byte</td><td>String (ASCII-8BIT)</td><td>ByteString</td><td>string</td><td>List<int></int></td></tr></tbody></table>
</details>

这是官网的表格照抄结果，仅供学习使用，生产请以官方文档为准以避免时效性问题。[官方文档链接](https://protobuf.dev/programming-guides/proto3/#scalar)

#### Enumerations

#### Service 定义

## 题外话

### 可变长度编码和 ZigZag 编码

具体过程可以参考[ZigZag例子](https://gist.github.com/mfuerstenau/ba870a29e16536fdbaba)

