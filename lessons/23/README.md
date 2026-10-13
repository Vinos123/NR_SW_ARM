# 第23课：AVX-512与掩码尾部

建议用时：4–6小时。先修：第22课。每周另留1–2小时复盘。

## 目标与协议定位

AVX-512一次处理16个float。先处理完整向量，再用掩码处理尾部。未对齐加载允许普通vector内存。基线分派代码不能整体使用AVX-512编译，否则检测前也可能执行不支持的指令。

精读：**Intel Intrinsics Guide / Arm ACLE；非3GPP规定算法**。规范版本见[资料索引](../../docs/references.md)。
先按条款找到输入/输出符号，再看本课接口，不需要顺序通读整本规范。

## 手算与变量

n=17：一次16元素向量，剩余1元素，尾掩码为0x0001。

接口：

```cpp
bool OptimizedLlr(std::span<const float> iq, float variance, std::span<float> output);
```

比特使用0/1字节，索引从0开始；复数为`std::complex<float>`；LLR正值代表0。
`A/B/K/N/E/G`均以比特计，`Zc`是提升尺寸，RE是复数符号所在的网格位置。

## 跟着实现

打开[exercise.cc](exercise.cc)，按TODO完成：

1. 实现能力检测后的分派。
2. 阅读并跟写对应src指令内核。
3. 用偏移地址和短尾测试。
4. 记录同机标量与优化结果。

本课C++知识：**intrinsics、掩码/谓词、单独编译目标、运行时检测**。遇到新语法先看[语言速查](../../docs/cpp_primer.md)。
保留中文在前、英文紧随的核心注释，解释公式、边界与原因。

## 构建、运行与预期结果

先按[Windows环境说明](../../docs/windows_setup.md)配置UCRT64。仓库根目录PowerShell：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug --target lesson_23_solution lesson_23_exercise
.\build\windows-debug\lesson_23_solution.exe
.\build\windows-debug\lesson_23_exercise.exe
```

参考答案通过时输出`PASS lesson 23`，退出码0。练习初始输出`NOT IMPLEMENTED`并返回1；完成后应满足相同验收。SIMD课在未启用或不支持的机器上输出SKIP，返回77，不算完成硬件验证。
查看[main.cc](main.cc)中的固定输入与断言；不要删除断言来使程序通过。

## 对照答案与工程实现

先独立写，再读[solution.cc](solution.cc)。完整模块：[参考实现](../../src/qpsk_avx512.cc)。
主工程参考测试和独立向量位于[tests](../../tests)，运行`ctest --preset windows-debug`。
普通课程函数的参考实现与练习分开链接；可复用已完成前课模块，不需要重写依赖。
本课还需完成[kernel_exercise.cc](kernel_exercise.cc)，对照[kernel_solution.cc](kernel_solution.cc)的指令内核；并按[优化指南](../../docs/simd.md)启用。

## 常见错误与自测

不支持的硬件必须报告SKIP；不能用标量结果冒充SIMD验证。

- 先解释输入输出长度，再单步检查第一个不一致的位置。
- 修改一个边界输入，预测结果后运行；浮点比较使用容差。
- 写三句话：本课解决什么问题、哪个条款决定行为、测试如何证明正确。

## 源码阅读与复盘

按[源码导读](../../docs/source_map.md)进入对应模块。记录一个协议符号与C++变量的对应关系，说明一处教学简化。
不要把读完文件当作掌握：应能在不看答案时解释手算例子。
