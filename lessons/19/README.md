# 第19课：HARQ软合并与缓冲区生命周期

建议用时：4–6小时。先修：第18课。每周另留1–2小时复盘。

## 目标与协议定位

不同RV发送母码的不同位置，必须先反速率匹配到同一坐标系再累加。这个函数只做合并；调用方负责用HARQ进程号、NDI和编码计划确认TB身份。主线不实现MAC状态机。

精读：**38.212 §5.4.2；HARQ过程管理背景见38.321**。规范版本见[资料索引](../../docs/references.md)。
先按条款找到输入/输出符号，再看本课接口，不需要顺序通读整本规范。

## 手算与变量

旧LLR=[2,-3]、新LLR=[3,1]，同TB合并=[5,-2]；新TB直接变为[3,1]。

接口：

```cpp
std::vector<float> CombineHarq(std::span<const float> previous, std::span<const float> current, bool new_data);
```

比特使用0/1字节，索引从0开始；复数为`std::complex<float>`；LLR正值代表0。
`A/B/K/N/E/G`均以比特计，`Zc`是提升尺寸，RE是复数符号所在的网格位置。

## 跟着实现

打开[exercise.cc](exercise.cc)，按TODO完成：

1. 新数据直接覆盖。
2. 检查重传缓冲长度。
3. 逐元素相加。
4. 用RV0与RV2运行核心HARQ测试。

本课C++知识：**span、状态生命周期、返回值所有权**。遇到新语法先看[语言速查](../../docs/cpp_primer.md)。
保留中文在前、英文紧随的核心注释，解释公式、边界与原因。

## 构建、运行与预期结果

先按[Windows环境说明](../../docs/windows_setup.md)配置现有工具链。
**在VS Code直接打开本课目录**，选择当前GCC Kit，执行CMake: Configure。
选择Build目标`lesson_19_solution`或`lesson_19_exercise`，再选择相同Launch目标运行或调试。
不需要先构建根工程，不使用“编译当前文件”。

完成Configure后，可在本课目录的PowerShell终端运行（build为实际构建目录）：

```powershell
cmake --build build --target lesson_19_solution lesson_19_exercise
ctest --test-dir build --output-on-failure
.\build\lesson_19_solution.exe
.\build\lesson_19_exercise.exe
```

单配置构建使用上述exe路径；CMake Tools Run/Debug会自动识别其他生成器的实际输出目录。
答案通过时输出`PASS lesson 19`，退出码0。骨架初始输出`NOT IMPLEMENTED`并返回1。
SIMD课未启用或硬件不支持时输出SKIP，返回77。
查看[main.cc](main.cc)的固定输入与断言，不要删除断言来使程序通过。
本课的必要模块见[CMakeLists.txt](CMakeLists.txt)末尾；只编译这些模块及其传递依赖。

## 对照答案与工程实现

先独立写，再读[solution.cc](solution.cc)。完整模块：[参考实现](solution.cc)。
主工程参考测试和独立向量位于[tests](../../tests)，根工程Configure后运行`ctest --test-dir build --output-on-failure`。
普通课程函数的参考实现与练习分开链接；可复用已完成前课模块，不需要重写依赖。
第23/24课还需跟写对应`src/qpsk_*.cc`的指令内核，并按[优化指南](../../docs/simd.md)启用。

## 常见错误与自测

即使长度相同，不同TB也不能合并；filler的正无穷语义需保持。

- 先解释输入输出长度，再单步检查第一个不一致的位置。
- 修改一个边界输入，预测结果后运行；浮点比较使用容差。
- 写三句话：本课解决什么问题、哪个条款决定行为、测试如何证明正确。

## 源码阅读与复盘

按[源码导读](../../docs/source_map.md)进入对应模块。记录一个协议符号与C++变量的对应关系，说明一处教学简化。
不要把读完文件当作掌握：应能在不看答案时解释手算例子。
