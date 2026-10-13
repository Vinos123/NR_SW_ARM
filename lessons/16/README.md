# 第16课：从理想信道到多径

建议用时：4–6小时。先修：第15课。每周另留1–2小时复盘。

## 目标与协议定位

信道使一个发送采样影响多个接收采样。先实现因果线性卷积，输出与输入等长。只要信道记忆小于CP并且同步正确，去CP后可用每子载波复增益描述。

精读：**信号处理实验；不是38.901数字孪生模型**。规范版本见[资料索引](../../docs/references.md)。
先按条款找到输入/输出符号，再看本课接口，不需要顺序通读整本规范。

## 手算与变量

x=[1,2,3]，h=[1,0.5]，输出截取前三点为[1,2.5,4]。

接口：

```cpp
std::vector<Complex> ApplyChannel(std::span<const Complex> input, std::span<const Complex> taps);
```

比特使用0/1字节，索引从0开始；复数为`std::complex<float>`；LLR正值代表0。
`A/B/K/N/E/G`均以比特计，`Zc`是提升尺寸，RE是复数符号所在的网格位置。

## 跟着实现

打开[exercise.cc](exercise.cc)，按TODO完成：

1. 初始化零输出。
2. 遍历有效延迟抽头。
3. 累加h[j]*x[i-j]。

本课C++知识：**双层循环、size_t下溢检查、复数乘法**。遇到新语法先看[语言速查](../../docs/cpp_primer.md)。
保留中文在前、英文紧随的核心注释，解释公式、边界与原因。

## 构建、运行与预期结果

先按[Windows环境说明](../../docs/windows_setup.md)配置现有工具链。
**在VS Code直接打开本课目录**，选择当前GCC Kit，执行CMake: Configure。
选择Build目标`lesson_16_solution`或`lesson_16_exercise`，再选择相同Launch目标运行或调试。
不需要先构建根工程，不使用“编译当前文件”。

完成Configure后，可在本课目录的PowerShell终端运行（build为实际构建目录）：

```powershell
cmake --build build --target lesson_16_solution lesson_16_exercise
ctest --test-dir build --output-on-failure
.\build\lesson_16_solution.exe
.\build\lesson_16_exercise.exe
```

单配置构建使用上述exe路径；CMake Tools Run/Debug会自动识别其他生成器的实际输出目录。
答案通过时输出`PASS lesson 16`，退出码0。骨架初始输出`NOT IMPLEMENTED`并返回1。
SIMD课未启用或硬件不支持时输出SKIP，返回77。
查看[main.cc](main.cc)的固定输入与断言，不要删除断言来使程序通过。
本课的必要模块见[CMakeLists.txt](CMakeLists.txt)末尾；只编译这些模块及其传递依赖。

## 对照答案与工程实现

先独立写，再读[solution.cc](solution.cc)。完整模块：[参考实现](../../src/channel_model.cc)。
主工程参考测试和独立向量位于[tests](../../tests)，根工程Configure后运行`ctest --test-dir build --output-on-failure`。
普通课程函数的参考实现与练习分开链接；可复用已完成前课模块，不需要重写依赖。
第23/24课还需跟写对应`src/qpsk_*.cc`的指令内核，并按[优化指南](../../docs/simd.md)启用。

## 常见错误与自测

有限长度线性卷积与循环卷积不同；本函数截断末尾，不自动补发尾部。

- 先解释输入输出长度，再单步检查第一个不一致的位置。
- 修改一个边界输入，预测结果后运行；浮点比较使用容差。
- 写三句话：本课解决什么问题、哪个条款决定行为、测试如何证明正确。

## 源码阅读与复盘

按[源码导读](../../docs/source_map.md)进入对应模块。记录一个协议符号与C++变量的对应关系，说明一处教学简化。
不要把读完文件当作掌握：应能在不看答案时解释手算例子。
