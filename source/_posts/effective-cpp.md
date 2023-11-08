---
title: Effective C++
date: 2023-10-05 16:03:48
tags:
- Note
category:
- C++
---
从vimwiki迁移过来的古早笔记,仔细看看发现好多忘了(

    Items from 1 to 55

----

1. 将cpp看作C，面向对象的cpp，模板和STL四个部分；根据各个部分的不同特性选择不同的编程方式
2. 能用const，enum，inline就别#define
3. 能用const就用const
4. 确保类初始化后再使用  
    - 尽可能使用列表初始化(原因:operator= first call class's default consturctor to allocate memory and get address, then do variable assigment)
    - 避免初始化顺序问题（有关static初始化的问题）
5. 知道默认构造，析构，复制构造函数
    - 一旦创建移动构造||移动赋值操作符，不会生成默认复制构造&&复制运算符，反之亦然
6. 小心使用默认生成的类函数
7. 若要使用多态，则应将基类析构函数定义为virtual
8. 不要让异常离开析构函数
9. 不要在构造和析构函数中调用虚函数
    - 也不是不能用，注意用的时候的类型是基类还是派生类（容易出问题所以不建议用）
10. 重载运算符建议返回*this以便连等及其它操作
11. 重载操作符时要考虑自己对自己操作的情况
    - 处理方法：
        1. 先判断地址
        2. 拷贝后操作拷贝
12. 拷贝（复制运算符）要完全复制类的内容（包括基类的）
    - 要复用代码，不要相互调用复制构造函数，而是共同调用一个三方函数
13. RAII: use class to manage resource(`unique_ptr & share_ptr`)
14. RAII object have specified copy function
15. 访问RAII class数据：
    - 隐式：更方便
    - 显示：更安全
16. 成对使用对应的 `new([])` & `delete([])`
17. 单独创建智能指针
    `eg: A(std::unique_ptr<B>(new B), fuc())`
    - 智能指针创建需要接受 `new` 返回的地址，而类构造函数执行顺序由编译器决定，如果`fuc` throw exception,会导致内存泄漏
18. 设计良好的interface
    - 尽可能不让用户写出出错代码
    - 减少用户碰到管理内存的几率
    - shared_ptr总是使用指向类的析构函数，可以防止cross-DLL(object creat in a dynamic link library but delete in a differet DLL)
19. 定义类就是定义类型，关注一些细节
20. 尽量传const&,对基础类型直接传值(iterator实现是指针，也算)
21. 不要返回指向本地临时变量的指针
22. 对用户隐藏数据，protected没比public包装好到哪去
23. 不用访问data的就别搞成成员函数
24. 类型转换需要非成员函数(`friend`)
25. 创建合适的swap函数：
    - 默认的std::swap执行三次拷贝构造->解决方案：构造wapper类使用指针来存储数据，交换仅交换指针，并且提供public的swap成员函数
    - 如果该类不是模板类，需要具体化std::swap给该类;如果该类是模板类则在namespace中重写swap（模板函数）
    - 调用规则：using *+ using std::swap，调用优先级为 模板swap>具体化std::swap>std::swap,因此记得using* 和using std::swap
26. 尽可能延缓变量的定义（看情况而定）好处：
        - 避免无用变量构造消耗时间（throw会导致无用
        - 优化默认构造+复制构造为复制构造
27. 尽可能不用cast，尤其dynamic_cast；必须要用，定义函数来隐藏cast（尽量避免用户自己cast）；少用c类型的转型, c++四种cast
    - const_cast
    - dynamic_cast
    - reinterpret_cast
    - static_cast
        - static_cast子类转成父类时是产生一个子类中父类的临时拷贝，修改不会映射到原存储
    >cast要小心，搞清楚自己的目的
28. 尽可能避免返回句柄[引用和指针]
    - 指向栈中临时变量会寄掉
29. exception safety:
    - No resource leak
    - No data structures become coorupted  

    exception safety function:
    1. Basic guarantee: after throw, everything in program remain valid  
    2. Strong guarantee: after exception, the program status remain as if the fuction is never called  
    3. Nothrow guarantee: never throw exception(hard to guarantee out of C part in C++)
    - Strong guarantee implement: make a copy and modify the copy, then swap them in noexception way(But this need more resource and time, though it's highly recommand, not always need to provide strong guarantee)
    - 一般函数需要提供至少要有weak exception guarantee(at least no resource leak)
30. inline function rules:
    - limit inline function as small, frequently called functions.
    - 内联别用模板
    其它一些东西：
        - 一般构造和析构函数不是内联
        - 能不能内联主要看编译器
31. 最小化编译依赖关系（通过声明）可以使用：
    1. handle class
    2. interface class: increase memory cost for virtual function table
    编译时只需要重新链接函数就行
32. public inheritance means "is a"(noted that only public do this)
33. 继承会隐藏父类（作用域）的变量和函数（当然名称空间也会{使用using解除隐藏
34. pure virtual function; virtual function;non-virtual function; 不要担心虚函数带来的损耗（大多数情况），也不要全是虚函数（一样）  
    - pure virtual function可以在类外提供实现，如此可以强制子类重写（算是强制注意吧）并减少代码重复
    - 80~20rule:80% running time spend on 20% code
35. 使用none virtual interface idiom 在基类以确保多态在何时调用函数
    - 使用函数指针成员代替虚函数（好处：可以更改调用的函数(坏处：缺少对非public变量的访问权限
    - 使用std::function成员代替虚函数
36. 重定义非虚函数可能会使得多态出现意想不到的问题(从设计上来说就不应该重定义非虚函数)
37. 默认参数值为静态绑定，不同于虚函数的动态绑定，不要在继承中改变默认值
38. has a & implemented in terms of-- composition
39. private inheritance当仅必要时
        - both private inheritance and composition mean is-implemented-in-terms-of
            - composition is easy to understand
            - private inheritance can enable EBO
        - empty base optomize(EBO):当仅单继承且父类是空类时，子类大小等于数据大小(编译器优化
40. 多继承尽量别用，用要考虑virtual
    - 虚拟继承有损耗（比起单继承
    - 多继承例子：public interface+private implementation
41. 类和模板都支持多态
    - 对于类，接口是明确的，多态出现在运行时
    - 对于模板参数，接口是明确且基于合法的表达式的，多态出现在编译时（通过模板初始化和函数重载）
42. typename和class可以互换
    - 在不清楚的情况下，C++默认把typename(class)::***看成变量而不是类型，使用 `typename` 告诉他这是个变量类型
43. 访问模板基类函数的三种方法
    1. 使用 `this` 指针调用
    2. `using` 指令
    3. `base<T>::**`直接用
44. 模板隐式具体化出的不依赖于模板参数的函数会导致程序膨胀
    - 若由非类型参数导致，将模板参数换为函数参数或者类成员变量
    - 若由类型参数导致，可以将数据转化为 无类型指针 `eg: static_cast<void*>` 然后统一调用
45. 在模板类中使用模板成员函数来接受所有合法参数
    - eg: 同模板不同具体化类的实例化的复制构造函数和复制运算符
    - 注意：即使声明了模板复制构造函数，也要声明一般的复制构造函数
46. 我不知道他想说明啥
47. 特化模板类使其对特定类型使用特定函数
    - 通过 `tyepid()` 检查---wrong, can't compile
        - 由于不同的类型支持不同的函数，一些不支持的函数无法被编译（即使`if else`永远不会进入)编译过程要求所有代码合法
    - 通过和函数重载(编译时)
48. TMP(template metaprogramming)
    - 将一部分运行时工作放到编译时进行
49. 通过 `set_new_handler(*new_handler)` 自定义new的行为 `typedef void (*new_handler)()` declear in std, use as `std::new_handler`
    - 通过连锁调用在失败后尝试其他 `handler` 分配内存，最后全部失败再throw bad_alloc
    - 自定义类new的行为: modify `void* operator new(std::size)throw(std::bad_alloc)` and `set_new_handler`
    - C++93前new分配失败会返回0，使用 `new(std::nothrow)` 来启用这一行为（只作用于内存分配时，初始化相关对象仍可能throw
50. 重载new/delete操作符的几种情况
    1. 优化操作速度
    2. 收集内存使用信息
    3. 减少开头和末尾的内存占用
    4. 自定义内存对齐
    5. 将类集束摆放
    6. 其他未列出new行为
51. 重载new和delete
    - 重载new需要一个无限循环来分配内存，并在失败时调用 `set_new_handler` 或者分配0空间并throw
    - 对于类类型，应该分配比预期更大空间
    - delete对nullptr什么也不做，对类类型应该删除比预期更大空间
52. 重写new了记得重写delete，并且不要无意间隐藏了默认的new&delete
53. 注意编译警告，也不要过度依赖编译器
54. 熟悉标准库，包括
    - STL(standard template library)
    - Iostream
    - Internationlization(like wchar_t,wstring)
    - numeric provessing(valarray, complex)
    - exception hierarchy
    - C89 standard library
    - tr1(2005,Technical report 1), add smartPointers,function pointers(tr1::function,std::function now)就是std::expermental::xxx
55. Boost organization and Boost library

----
This is just a begining, C++ learning will never end

