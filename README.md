---
presentation:
  theme: solarized.css
  mouseWheel: true
  width: 1000
  height: 900
---

<!-- slide -->

# 代码检测工具

<!-- slide -->

## 静态检测

<!-- slide -->

https://en.wikipedia.org/wiki/List_of_tools_for_static_code_analysis#C,_C++

![所有工具](assets/2021-03-30_15-13.png)

<!-- slide -->

### pvs-studio

![pvs-studio](https://www.viva64.com/static/img/logo.png)

下载包链接：https://files.viva64.com/pvs-studio-7.12.46137.116-amd64.deb

官方文档连接：https://www.viva64.com/en/m/0036/

破解license

```
Ru.Board
UX9G-38X9-1HNH-0B0F
```

注册license

```bash
pvs-studio-analyzer credentials Ru.Board UX9G-38X9-1HNH-0B0F
```

操作：

```bash
# 根据构建脚本编译的源文件生成跟踪，自动抓取需要分析的源文件，会自动生成analyse.log文件
pvs-studio-analyzer trace -- ./build.sh
# 使用工具分析源代码
pvs-studio-analyzer analyze -j4 -o analyse.log
# 把分析结果转换成文字报告result.txt
plog-converter -a GA:1,2 -t tasklist -o result.txt analyse.log
```

<!-- slide -->


### clang-tidy

建议安装最新的llvm

官方安装文档链接：https://apt.llvm.org/

执行脚本就会自动添加apt源

```bash
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
```

然后用apt安装

```bash
sudo apt install clang-tidy-12
```

clang-tidy可以和很多IDE集成，包括vscode，vim，Clion，QtCreator等，也可以单独使用命令检测源文件

比较推荐的是用CMake集成，在编译代码时会打印警告信息

```cmake
# 查找clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy-12" DOC "Path to clang-tidy executable")
if (NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
else ()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-alpha.*")
endif ()

# 生成目标
add_executable(test_clang_tidy test_breakpad.cpp func.cpp func.hpp)
target_link_libraries(test_clang_tidy
        ${BREAKPAD_CLIENT_LIBRARIES}
        pthread)
# 集成clang-tidy
if (CLANG_TIDY_EXE)
    set_target_properties(test_clang_tidy PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif ()
```

clang-tidy可以设置很多检查配置参数，详细参考：

https://clang.llvm.org/extra/clang-tidy/#id2

https://clang.llvm.org/extra/clang-tidy/checks/list.html

### cppcheck

<img src="https://tse2-mm.cn.bing.net/th/id/OIP.OT6nfJ39hPhLCFpySCZNwQHaGh?pid=ImgDet&rs=1" width="300"/>

下载源码编译

```bash
cd cppcheck-2.4.1
mkdir build
cd build
cmake .. -DCLANG_TIDY=/usr/bin/clang-tidy-12 -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=ON -DWITH_QCHART=ON -USE_Z3=ON -DHAVE_RULES=ON
make -j
sudo make install
```

检测过程（gui示例）

在CMake文件中加入

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

或者执行CMake命令的时候加上`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`保证生成`compile_commands.json`文件，该文件中包含每个源文件编译所需要的头文件依赖和编译参数（flags）

`compile_commands.json`文件默认会出现在`CMAKE_BINARY_DIR`目录下面，也就是执行CMake 的目录下面

打开cppcheck-gui，新建项目然后保存在项目根目录，保存后导入compile database也就是`compile_commands.json`就能够检查出问题了

更多使用文档和命令行操作参考：

http://cppcheck.net/manual.pdf

### Infer



## 内存检测

### AddressSanitizer

官方说明

> AddressSanitizer is a fast memory error detector. It consists of a compiler instrumentation module and a run-time library. The tool can detect the following types of bugs:
>
> - Out-of-bounds accesses to heap, stack and globals
> - Use-after-free
> - Use-after-return (runtime flag ASAN_OPTIONS=detect_stack_use_after_return=1)
> - Use-after-scope (clang flag -fsanitize-address-use-after-scope)
> - Double-free, invalid free
> - Memory leaks (experimental)

总的来说AddressSanitizer是通过编译时替代链接标准的内存管理库然后对内存分配释放使用进行额外的标记管理来检测内存的错误，会带来额外的开销

cmake示例代码

```cmake
add_executable(test_server test_breakpad_server.cpp func.cpp func.hpp)
target_link_libraries(test_server ${BREAKPAD_CLIENT_LIBRARIES} pthread)
add_executable(test_memory_asan test_memory.cpp)
set_target_properties(test_memory_asan PROPERTIES
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize-recover=address"
        LINK_FLAGS "-fsanitize=address -static-libasan"
        )
```

加入`-fsanitize-recover=address`可以使得检查到了内存问题不会中断程序运行，但是也可能会由于该内存问题导致其他问题

该功能在很多小内存分配的情况下效率很低，由于对于每块内存信息都要分配额外的空间进行记录，分配小内存越多开销越大

### ThreadSanitizer

llvm用来检测线程数据竞争的工具，官方文档：

https://clang.llvm.org/docs/ThreadSanitizer.html

需要编译器支持通过宏判断是否支持（测试gcc-7不支持，clang-12支持）

```cpp
#if defined(__has_feature)
#  if __has_feature(thread_sanitizer)
    std::cout << "__has_feature(thread_sanitizer)" << std::endl;
#  endif
#endif
```

由于数据竞争情况不一定会触发，不触发的情况是不会报错的，需要反复测试，

根据给的例子不断运行最后的结果

```
==================
WARNING: ThreadSanitizer: data race (pid=31946)
  Write of size 4 at 0x00000111659c by thread T1:
    #0 Thread1(void*) /home/jiang/code/code_security/test_race.cpp:12:12 (test_race+0x4b5ceb)

  Previous write of size 4 at 0x00000111659c by main thread:
    #0 main /home/jiang/code/code_security/test_race.cpp:25:12 (test_race+0x4b5d7c)

  Location is global 'Global' of size 4 at 0x00000111659c (test_race+0x00000111659c)

  Thread T1 (tid=31948, running) created by main thread at:
    #0 pthread_create <null> (test_race+0x42497b)
    #1 main /home/jiang/code/code_security/test_race.cpp:24:5 (test_race+0x4b5d72)

SUMMARY: ThreadSanitizer: data race /home/jiang/code/code_security/test_race.cpp:12:12 in Thread1(void*)
==================
ThreadSanitizer: reported 1 warnings

```

### MemorySanitizer

llvm用来检测对未初始化内存使用的工具，官方文档：

https://clang.llvm.org/docs/MemorySanitizer.html

和ThreadSanitizer一样链接的时候需要用clang才能支持

在main函数中调用未初始化的值，编译运行得到

```
==1733==WARNING: MemorySanitizer: use-of-uninitialized-value
    #0 0x49909b in main /home/jiang/code/code_security/test_memory.cpp:67:9
    #1 0x7fcf34f7fbf6 in __libc_start_main /build/glibc-S9d2JN/glibc-2.27/csu/../csu/libc-start.c:310
    #2 0x41c2c9 in _start (/home/jiang/code/code_security/build/test_memory+0x41c2c9)

  Uninitialized value was created by an allocation of 'global' in the stack frame of function 'main'
    #0 0x498e30 in main /home/jiang/code/code_security/test_memory.cpp:64

SUMMARY: MemorySanitizer: use-of-uninitialized-value /home/jiang/code/code_security/test_memory.cpp:67:9 in main
Exiting

```

### UndefinedBehaviorSanitizer

可以用来检测一些未定义的行为，比如

- 未对齐或者空的指针
- 有符号数溢出
- 浮点数转换之间的溢出

官方文档：https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html

具体检测类型

| 标志 -fsanitize=                                             | 检测内容                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| alignment                                                    | 使用未[对齐的指针](https://stackoverflow.com/questions/20183094/what-is-a-misaligned-pointer)，创建未对齐的引用(根据那个指针创建)，也检查假设对齐的属性 |
| bool                                                         | 既不用true也不用false加载一个bool值                          |
| builtin                                                      | 传递无效值给编译器内建类型                                   |
| bounds                                                       | 检查数组索引正确性，包括array-bounds和local-bounds。local-bounds不算未定义 |
| enum                                                         | 给枚举赋值时值不在枚举范围内                                 |
| float-cast-overflow                                          | 检查浮点和其他类型转换导致溢出的情况，也就是浮点和整形       |
| float-divide-by-zero                                         | 除以0错误，clang的结果是NaN或者无穷大，不算未定义            |
| function                                                     | 使用函数指针调用了错误的函数类型                             |
| implicit-unsigned-integer-truncation<br>implicit-signed-integer-truncation | 整数隐式转换从宽到窄，如果重新转换回去数据不一样了           |
| implicit-integer-sign-change                                 | 整数隐式转换导致数的符号变了                                 |
| integer-divide-by-zero                                       | 整数除以0                                                    |
| nonnull-attribute                                            | 传递空指针给不允许为空的指针类型函数参数                     |
| null                                                         | 用空指针创建空的引用                                         |
| nullability-arg                                              | 传递空给_Nonnull声明的函数参数（objective-c）                |
| nullability-assign                                           | 传递空给_Nonnull值（objective-c）                            |
| nullability-return                                           | 传递空给_Nonnull返回值（objective-c）                        |
| objc-cast                                                    | 把ObjC对象指针转为不完全类型                                 |
| object-size                                                  |                                                              |
| pointer-overflow                                             | 指针计算溢出，或者新的或旧的值为空指针                       |
| return                                                       | C++在最后没有返回值                                          |
| returns-nonnull-attribute                                    | 函数返回了空指针但是定义是不能返回空                         |
| shift                                                        | 移位操作符移位数小于0或大于等于左边数的位宽，或者左边数为负<br>对于有符号数左移c和c++也检测，可以用-fsanitize=shift-base或-fsanitize=shift-exponent检测左右操作数 |
| unsigned-shift-base                                          | 确保无符号左移不溢出                                         |
| signed-integer-overflow                                      | 有符号数运算后其类型不能代表运算后的类型，被-ftrapv检测，也检测除法溢出(INT_MIN/-1)，不检查隐式转换 |
| unreachable                                                  | if控制流到达了不能到达的程序点，（条件永远不能生效）         |
| unsigned-integer-overflow                                    | 无符号数运算后其类型不能代表运算后的类型，但是不算未定义行为，不检查隐式转换 |
| vla-bound                                                    | 变长数组的长度不为正数                                       |
| vptr                                                         | 使用了一个对象的虚指针是错误的动态类型，或者对象的声明周期还没开始或者已经结束了 |

object-size: An attempt to potentially use bytes which the optimizer can determine are not part of the object being accessed. This will also detect some types of undefined behavior that may not directly access memory, but are provably incorrect given the size of the objects involved, such as invalid downcasts and calling methods on invalid pointers. These checks are made in terms of `__builtin_object_size`, and consequently may be able to detect more problems at higher optimization levels.

也可以用以下检测组：

- `-fsanitize=undefined`: All of the checks listed above other than `float-divide-by-zero`, `unsigned-integer-overflow`, `implicit-conversion`, `local-bounds` and the `nullability-*` group of checks.
- `-fsanitize=undefined-trap`: Deprecated alias of `-fsanitize=undefined`.
- `-fsanitize=implicit-integer-truncation`: Catches lossy integral conversions. Enables `implicit-signed-integer-truncation` and `implicit-unsigned-integer-truncation`.
- `-fsanitize=implicit-integer-arithmetic-value-change`: Catches implicit conversions that change the arithmetic value of the integer. Enables `implicit-signed-integer-truncation` and `implicit-integer-sign-change`.
- `-fsanitize=implicit-conversion`: Checks for suspicious behavior of implicit conversions. Enables `implicit-unsigned-integer-truncation`, `implicit-signed-integer-truncation`, and `implicit-integer-sign-change`.
- `-fsanitize=integer`: Checks for undefined or suspicious integer behavior (e.g. unsigned integer overflow). Enables `signed-integer-overflow`, `unsigned-integer-overflow`, `shift`, `integer-divide-by-zero`, `implicit-unsigned-integer-truncation`, `implicit-signed-integer-truncation`, and `implicit-integer-sign-change`.
- `-fsanitize=nullability`: Enables `nullability-arg`, `nullability-assign`, and `nullability-return`. While violating nullability does not have undefined behavior, it is often unintentional, so UBSan offers to catch it.

对于部分检测类型volatile类型不能使用

在生成环境下可以使用最小运行环境

```
-fsanitize-minimal-runtime
```

支持打印堆栈跟踪和符号

支持把日志写到文件或打印

### DataFlowSanitizer

数据流分析的工具

```c
#include <sanitizer/dfsan_interface.h>
#include <assert.h>

int main(void) {
  int i = 1;
  dfsan_label i_label = dfsan_create_label("i", 0);
  dfsan_set_label(i_label, &i, sizeof(i));

  int j = 2;
  dfsan_label j_label = dfsan_create_label("j", 0);
  dfsan_set_label(j_label, &j, sizeof(j));

  int k = 3;
  dfsan_label k_label = dfsan_create_label("k", 0);
  dfsan_set_label(k_label, &k, sizeof(k));

  dfsan_label ij_label = dfsan_get_label(i + j);
  assert(dfsan_has_label(ij_label, i_label));
  assert(dfsan_has_label(ij_label, j_label));
  assert(!dfsan_has_label(ij_label, k_label));

  dfsan_label ijk_label = dfsan_get_label(i + j + k);
  assert(dfsan_has_label(ijk_label, i_label));
  assert(dfsan_has_label(ijk_label, j_label));
  assert(dfsan_has_label(ijk_label, k_label));

  return 0;
}
```

### LeakSanitizer

运行时检测内存泄露的工具，可以和AddressSanitizer一起使用

一起使用时加上`ASAN_OPTIONS=detect_leaks=1`，单独使用时加上`-fsanitize=leak`

### Valgrind

![valgrind](https://www.valgrind.org/images/valgrind-100.png)

官网：https://www.valgrind.org/

文档：https://www.valgrind.org/docs/manual/manual.html

Valgrind和AddressSanitizer不一样，是建立了一个虚拟的cpu环境作为程序和真实cpu交互

![valgrind_structure](https://img-blog.csdn.net/20170523212045374)

Valgrind安装

```bash
sudo apt install valgrind
```

Valgrind支持用GUI配置分析程序，具体可以使用命令安装

```bash
sudo apt install valkyrie
```

程序如果想在使用Valgrind工具是能够定位到函数符号，尽量用`-g`参数编译

Valgrind有几个组件可以做不同的事

#### memcheck

文档：https://www.valgrind.org/docs/manual/mc-manual.html

memcheck主要用来检测内存错误，支持如下类型：

- 非法读写错误
- 使用未初始化的值
- 在系统调用中使用未初始化或者不可寻址的值
- 非法释放
- 堆没有正常释放（比如new的用free，new数组用delete不带方括号等等）
- （内存拷贝字符串拷贝等）源和目的块重叠
- 可疑参数值（申请内存大小为负或者过大，以至于超过2^63）
- 内存泄露检测

memcheck可以生成xtree文件使用gui更方便查看报告

memcheck使用到达时候需要编译器尽可能的降低优化等级，最好使用-O0，使用-O1会造成一些符号错误和行号对不上的情况，-O2和以上就不推荐用来测试了

命令示例：

```bash
valgrind --tool=memcheck ./test_memory_none
```

使用xtree 查看内存分配数量统计

```bash
valgrind --xtree-memory=full ./test_memory_none
callgrind_annotate xtmemory.kcg.13825 #名字是自动生成的，可以通过命令指定生成的名字
```

配置文件可以写到`.valgrindrc`或者`~/.valgrindrc`下面，就不用每次都输入配置参数

其他具体的使用和参数可以参考文档链接

#### Cachegrind

可以模拟程序和CPU一级二级缓存交互的情况

#### Callgrind

https://www.valgrind.org/docs/manual/cl-manual.html

可以生成程序的函数调用图，以文件形式保存或者边运行程序边查看，图形化可以用KCachegrind

#### Helgrind

和ThreadSanitizer类似，可以检查POSIX pthreads线程之间的同步问题

==相比ThreadSanitizer，该工具检测效率高很多，测试使用ThreadSanitizer需要运行很多遍才能检测出数据竞争==

测试除了pthread，std::thread的问题也能检测出来，包括多个线程同时使用打印函数也会出现竞争情况，也能检测出来，QThread没有测试，文档里面说实验性的支持Qt4的QMutex

使用方法：加上`--tool=helgrind`参数

包括以下一些点

- 滥用POSIX pthreads线程API
  - 对无效的互斥量解锁
  - 对没锁的互斥量解锁
  - 对其他线程持有的互斥量解锁
  - 销毁无效或者锁住的互斥量
  - 对于非循环互斥量进行循环加锁
  - 销毁一块含有锁上的互斥量的内存
  - 把互斥量传递到需要读写锁的函数参数中
  - POSIX pthreads返回失败错误码必须处理
  - 线程退出的同时持有锁住的锁
  - 调用pthread_cond_wait传入没锁住的互斥量，无效互斥量，其他线程锁住的互斥量
  - 条件变量及其关联的互斥体之间的绑定不一致
  - pthread屏障的初始化无效或重复
  - 线程仍在等待的pthread屏障的初始化
  - 销毁从未初始化或仍在等待线程的pthread屏障对象
  - 等待未初始化的pthread屏障
  - 对于Helgrind拦截的所有pthreads函数，即使系统线程库例程返回错误代码（即使Helgrind本身未检测到错误），也会报告错误以及堆栈跟踪
- 锁顺序不一致
- 数据竞争

#### DRD

DRD也是Valgrind里面的检测线程错误的一个工具

多线程编程介绍

- 多线程编程范式
  - 为并发活动建模
  - 使用CPU多核心加速计算
  - 锁
  - 消息传递
  - 自动并行化
  - 软件事务存储
- POSIX线程编程模型
  - 共享地址空间
  - 加载和存储操作
  - 原子存储和读-修改-写操作
  - 线程
  - 同步对象和同步对象的操作

多线程编程出现的问题

- 数据竞争
- 锁争用
- 不正确的使用POSIX线程API
- 死锁
- 虚假共享，两个线程在不同核心运行，在同一块缓存频繁获取不同的变量，频繁交换高速缓存行会导致线程性能下降

使用方法设置`--tool=drd`就可以了

#### Massif

https://www.valgrind.org/docs/manual/ms-manual.html

堆优化工具，堆空间包括了有用的空间和用来对齐和簿记的空间，该工具可以测量程序使用了多少堆空间，也可以测量用了多少栈空间

堆剖析可以减少程序对内存的使用

- 可以加速程序运行——小的程序可以更好的和机器的缓存交互，避免分页
- 如果程序用了很多内存，可以减少程序用完交换分区

使用方法：

```bash
valgrind --tool=massif prog
ms_print massif.out.21185
```

图形化可以借助第三方软件或者自己用Python绘图工具绘制图像曲线

https://github.com/KDE/massif-visualizer

#### DHAT

https://www.valgrind.org/docs/manual/dh-manual.html

动态堆分析工具

用法

```bash
valgrind --tool=dhat prog
```

打开提示的超链接，file:///usr/local/libexec/valgrind/dh_view.html

在网页里面选择生成的dhat.out.xxxx文件打开就可以查看

## 性能分析

### gperftools

最快的内存分配（tcmalloc），同时提供线程友好的堆检测，堆探查，CPU探查

https://github.com/gperftools/gperftools

下载release包编译安装

```bash
sudo apt instal libunwind-dev
cd 代码目录
./configure
make -j
sudo make install
```

如果运行时提示找不到tcmalloc，需要把/usr/local/bin加入到LD_LIBRARY_PATH环境变量中，临时或者永久都可以

#### 堆检测(Heap Checker)

编译时链接tcmalloc

运行

```bash
HEAPCHECK=1 ./a.out [binary args]
```

#### 堆探查(Heap Profiler)

编译时链接tcmalloc和profiler库

方式1：

运行程序时指定存放堆文件的文件名

```bash
HEAPPROFILE=文件名 ./a.out [binary args]
pprof ./a.out /tmp/heapprof.0045.heap
```

可以选择web和pdf查看，pdf需要添加`--pdf`然后重定向到文件

方式2：

在程序开始加上`HeapProfilerStart("文件名");`

程序返回前加上`HeapProfilerStop();`

不需要指定环境变量就可以保存堆文件

使用pprof导出方式和上面一致

#### CPU探查(CPU Profiler)

编译时链接tcmalloc和profiler库

指定的环境变量变为CPUPROFILE

如果用代码则为`ProfilerStart("文件名");`和`ProfilerStop();`



==对于非编译时链接的可执行文件，可以采用设置LD_PRELOAD变量预加载动态库实现调试==

### 内核工具

在命令行输入perf，按照提示安装包

#### top方式查看指定进程最耗资源的函数

```bash
sudo perf top -p PID
```

PID可以用`$(pidof name)`来查找

#### 记录到文件

```bash
sudo perf record -e cpu-clock -g -p PID -o FILE_NAME
```

#### 查看记录结果

```bash
sudo perf report -i FILE_NAME
```

#### 导出文本结果

```bash
sudo perf script -i FILE_NAME > OUTPUT
```

#### 绘制火焰图

https://github.com/brendangregg/FlameGraph

## 栈回溯

主要用来获取当前的调用堆栈信息，当前线程函数调用栈，函数名称逐级信息

为什么仅仅打印当前函数名称并不好，因为同一个函数可能在多个地方被调用到，光打印函数位置并不能准确的反应当前程序运行状态

gcc编译参数`-fno-omit-frame-pointer`可以保留函数调用的堆栈栈顶指针

### libunwind

该文章解释了elf文件格式组成，函数调用过程和

https://zhuanlan.zhihu.com/p/302726082

### libbacktrace

https://github.com/ianlancetaylor/libbacktrace

## 覆盖测试

通常对于单元测试来说，一个函数或接口是否测试充分除了输入数据的覆盖面，还有就是函数内部的分支条件跳转都能覆盖到

对于没有写单元测试的代码，覆盖测试也可以用来测试一些条件语句的发生概率，代码的执行次数等等

gcc和clang都自带了覆盖率测试插桩工具，原理是记录函数开始、控制语句开始和结束、函数结束位置插入汇编代码记录执行次数，详细参考该文章：

https://blog.csdn.net/yanxiangyfg/article/details/80989680

使用：

编译的时候编译参数加入`-fprofile-arcs` 和`-ftest-coverage`

编译玩后编译二进制文件所在的目录会生成两个文件，`xxx.gcda`和`xxx.gcno`

然后运行编译的文件，每次运行都会将`.gcno`里面的数据累加更新

然后执行下面脚本，参数为二进制文件所在目录，即可导出html的报告

```bash
#!/usr/bin/env bash

BINARY_DIR=$1
COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report
echo "二进制目录""${BINARY_DIR}"
lcov --rc lcov_branch_coverage=1 -c -d "${BINARY_DIR}" -o "${BINARY_DIR}/${COVERAGE_FILE}"
genhtml --rc genhtml_branch_coverage=1 "${BINARY_DIR}/${COVERAGE_FILE}" -o ${REPORT_FOLDER}
```

## 崩溃抓取

程序崩溃一般是发生致命性错误，有时候这种问题很难通过调试找到，所以需要在程序崩溃时生成函数调用信息，同时保存部分局部变量用来分析，Windows下面可以用系统自带的`dbghelp.dll`来生成minidump，然后用visual stdio调试，Linux下面可以breakpad库来生成minidump文件

### breakpad

编译，参考



#### 进程内捕获

main函数添加好handler就可以了

```c++
MinidumpDescriptor descriptor("./");
ExceptionHandler eh(descriptor, nullptr, dumpCallback, nullptr, true,
                        -1);
```

#### 进程外捕获

server和client之间使用socketpair连接，`CrashGenerationServer::CreateReportChannel`会自动生成，然后用server进程把client进程调起来

```c++
int main(int argc, char *argv[])
{
    std::setlocale(LC_MESSAGES, "zh_CN.utf8");
    const std::string dumpPath = "./";
    // 创建
    CrashGenerationServer::CreateReportChannel(&server_fd, &client_fd);
    CrashGenerationServer crash_server(
            server_fd,
            &showClientDumpRequest,
            nullptr,
            &showClientExited,
            nullptr,
            true,
            &dumpPath);
    crash_server.Start();

    pid_t child_pid, wpid;
    int status = -1;

    std::cout << "父进程 pid = " << getpid() << std::endl;

    for (int i = f_use_nullptr; i <= f_memory_out_of_bounds; ++i) {
        char cmd[50];
        sprintf(cmd, "./test_breakpad %d 2", client_fd);

        child_pid = fork();
        if (child_pid == 0) {
            std::cout << "创建子进程 " << i << std::endl;
            std::cout << "子进程 pid = " << getpid() << std::endl;
            system(cmd);
            exit(5);
        } else if (child_pid > 0) {
        } else {
            std::cerr << "fork错误: " << std::strerror(errno) << std::endl;
        }
    }

    std::cout << "父进程挂起" << std::endl;

    while ((wpid = wait(&status)) > 0) {
        std::cout << "子进程 " << wpid << " 退出状态为 " << WEXITSTATUS(status) << std::endl;
    }

    return 0;
}
```

#### minidump文件分析

可以用minidump-2-core转成coredump文件用gdb调试

也可以用dump_stackwalk分析调用栈，但是需要用dump_syms导出符号文件

# 其他链接

> https://prajankya.me/valgrind-on-linux/
>
> https://milianw.de/blog/heaptrack-a-heap-memory-profiler-for-linux.html
>
> https://blog.mengy.org/how-valgrind-work/
>
> https://blog.mengy.org/extend-gdb-with-python/
>
> https://zhuanlan.zhihu.com/p/302726082
>
> https://zhuanlan.zhihu.com/p/290689333