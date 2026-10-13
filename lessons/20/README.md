# 第20课：UCI复用索引与DFT扩频

建议用时：4–6小时。先修：第19课。每周另留1–2小时复盘。

## 目标与协议定位

真实UCI复用涉及HARQ-ACK/CSI长度、编码、资源预算、占位符和映射次序，本课只建立索引追踪方法。DFT扩频是频域调制符号上的变换，之后还需资源映射和OFDM；不应说它直接生成最终时域波形。

精读：**38.212 §6.2.7、§6.3.2；38.211 §6.3.1.4**。规范版本见[资料索引](../../docs/references.md)。
先按条款找到输入/输出符号，再看本课接口，不需要顺序通读整本规范。

## 手算与变量

人为6槽位预算：D0,U0,D1,D2,U1,D3。另一个独立实验对12个QPSK符号做酉DFT再IDFT恢复。

接口：

```cpp
Bits MultiplexToy(std::span<const uint8_t> data, std::span<const uint8_t> uci);
```

比特使用0/1字节，索引从0开始；复数为`std::complex<float>`；LLR正值代表0。
`A/B/K/N/E/G`均以比特计，`Zc`是提升尺寸，RE是复数符号所在的网格位置。

## 跟着实现

打开[exercise.cc](exercise.cc)，按TODO完成：

1. 显式声明教学映射。
2. 保证数据和UCI无重叠。
3. 比较DFT往返误差。
4. 阅读真实复用流程列出缺失步骤。

本课C++知识：**索引表、容量预算、实验范围声明**。遇到新语法先看[语言速查](../../docs/cpp_primer.md)。
保留中文在前、英文紧随的核心注释，解释公式、边界与原因。

## 构建、运行与预期结果

先按[Windows环境说明](../../docs/windows_setup.md)配置UCRT64。仓库根目录PowerShell：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug --target lesson_20_solution lesson_20_exercise
.\build\windows-debug\lesson_20_solution.exe
.\build\windows-debug\lesson_20_exercise.exe
```

参考答案通过时输出`PASS lesson 20`，退出码0。练习初始输出`NOT IMPLEMENTED`并返回1；完成后应满足相同验收。SIMD课在未启用或不支持的机器上输出SKIP，返回77，不算完成硬件验证。
查看[main.cc](main.cc)中的固定输入与断言；不要删除断言来使程序通过。

## 对照答案与工程实现

先独立写，再读[solution.cc](solution.cc)。完整模块：[参考实现](solution.cc)。
主工程参考测试和独立向量位于[tests](../../tests)，运行`ctest --preset windows-debug`。
普通课程函数的参考实现与练习分开链接；可复用已完成前课模块，不需要重写依赖。
第23/24课还需跟写对应`src/qpsk_*.cc`的指令内核，并按[优化指南](../../docs/simd.md)启用。

## 常见错误与自测

不能把本课MultiplexToy当作38.212合规UCI实现。

- 先解释输入输出长度，再单步检查第一个不一致的位置。
- 修改一个边界输入，预测结果后运行；浮点比较使用容差。
- 写三句话：本课解决什么问题、哪个条款决定行为、测试如何证明正确。

## 源码阅读与复盘

按[源码导读](../../docs/source_map.md)进入对应模块。记录一个协议符号与C++变量的对应关系，说明一处教学简化。
不要把读完文件当作掌握：应能在不看答案时解释手算例子。
