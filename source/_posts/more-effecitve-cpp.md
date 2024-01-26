---
title: More Effecitve C++
date: 2023-11-08 16:21:57
tags:
- Notes
category:
- C++
---

- 不能直接往数组类容器塞多态类，使用指针；一定要引用的情况下（需要`operator()`）用std::reference
- 直接创建数组或用vector,array等容器预先分配大小时调用类的无参(默认)构造函数，因此如果用了explict记得声明无参构造函数

```c++
//没有无参构造函数解法（别用）
class Ex;
void* ptr=operator new[](10*sizeof(Ex));
for(int i=0;i!=10;++i)
    new (&ptr[i]) Ex(...);//call constructor
//Free memory
operator delete(ptr);
```

- custom `i++&++i` for class

```c++
class foo;
int foo operator++();//++i
const int foo operator++(int);//i++
```

- 尽可能不重载 `&&`,`||` and `,`，因为不满足短路逻辑
- 有关`new`, `new[]`,`delete`,`delete[]`
  - new时，先调用`void* operator new(size_t size)`分配内存,然后初始化对象并且将`void*`转型为对应类型指针
  - operator new 的一个重载:`void* operator new(size_t size, void* loaction)` 定位new实现
  - delete时，先调用析构函数然后`operator delete` 释放内存
  - 对数组:
    - new []调用`operator new[]()`分配内存,然后为每个对象调用构造函数
    - delete[]同理
- 构造函数异常处理(未完全构造函数不会栈解退自动析构):
  - 使用try catch进行清理
  - 防止成员列表初始化const pointer异常导致资源泄漏的方法(未完全构造指针无法使用delete):
    - 调用一个basic exception guarantee的函数
    - 使用RAII类
- 异常
  - 异常栈解退时调用析构函数再抛出异常将会导致程序直接终止(call terminate directly kill program)
  - 异常拷贝时按照静态类型拷贝(派生类的基类引用拷贝为基类)
  - 异常捕获的支持隐式转型(按指):
    - 任何指针to `void*`(极其不建议用指针)
    - 派生类转基类
  - 总是按catch先后顺序捕获
  - 按引用捕获可以实现多态
- Improve proformance:
  - lazy evaluation: 返回一个结果的代理类，只有在需要时才进行计算
  - lazy copy： 写时复制
  - lazy fetch：涉及到数据库的部分
  - over-eager evaluation：提前计算并存入缓存，或constexpr编译期计算
- 临时对象: 仅在按值传递对象或者传递常量引用参数时产生。返回对象时，若直接返回则会拷贝局部对象
- 返回值优化: 返回构造函数的参数，这样返回的对象会在返回的作用域构造(编译器优化)
- 判断类是否在堆中(为了确定能否调用`delete this`)：重载`operator new,operator new[]`,记录返回的指针到一个图中，每次删除前进行查找是否在堆上
- 适当使用代理可以简化流程，但是会在隐式转型的地方出错
- 类型擦除:使用虚基类(接口)
- double dispatch:(针对不同动态类型的多个object产生不同副作用)
  - 虚函数 + RTTI：无封装，可维护性为0
  - 多重虚函数调用: 类似模板递归展开的手动实现，每次确定一个类型，然后调用下一个虚函数
  - 手动实现虚函数表

