# 第12课：译码适配与反速率匹配

建议用时：4–6小时。先修：第11课。每周另留1–2小时复盘。

## 目标与协议定位

反交织后，重复位置的LLR相加，未发送位置为0，表示没有信息。filler表示已知的0，应有强正可靠度。适配层把float转换到srsRAN的整数LLR，明确区分有限饱和值和已知位。

精读：**38.212 §5.4.2的逆过程；译码算法为接收机选择**。规范版本见[资料索引](../../docs/references.md)。
先按条款找到输入/输出符号，再看本课接口，不需要顺序通读整本规范。

## 手算与变量

同一比特两次观测LLR为2和3，合并为5；一次为2另一次为-3，合并为-1。

接口：

```cpp
std::vector<float> RateRecover(std::span<const float> llrs, const CodingPlan& p, int rv, int qm);
```

比特使用0/1字节，索引从0开始；复数为`std::complex<float>`；LLR正值代表0。
`A/B/K/N/E/G`均以比特计，`Zc`是提升尺寸，RE是复数符号所在的网格位置。

## 跟着实现

打开[exercise.cc](exercise.cc)，按TODO完成：

1. 逆交织。
2. 按选择索引累加。
3. 标记已知filler。
4. 阅读适配层量化与外部CRC检查。

本课C++知识：**适配层、堆内存、unique_ptr、量化**。遇到新语法先看[语言速查](../../docs/cpp_primer.md)。
保留中文在前、英文紧随的核心注释，解释公式、边界与原因。

## 构建、运行与预期结果

先按[Windows环境说明](../../docs/windows_setup.md)配置UCRT64。仓库根目录PowerShell：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug --target lesson_12_solution lesson_12_exercise
.\build\windows-debug\lesson_12_solution.exe
.\build\windows-debug\lesson_12_exercise.exe
```

参考答案通过时输出`PASS lesson 12`，退出码0。练习初始输出`NOT IMPLEMENTED`并返回1；完成后应满足相同验收。SIMD课在未启用或不支持的机器上输出SKIP，返回77，不算完成硬件验证。
查看[main.cc](main.cc)中的固定输入与断言；不要删除断言来使程序通过。

## 对照答案与工程实现

先独立写，再读[solution.cc](solution.cc)。完整模块：[参考实现](../../src/coding.cc)。
主工程参考测试和独立向量位于[tests](../../tests)，运行`ctest --preset windows-debug`。
普通课程函数的参考实现与练习分开链接；可复用已完成前课模块，不需要重写依赖。
第23/24课还需跟写对应`src/qpsk_*.cc`的指令内核，并按[优化指南](../../docs/simd.md)启用。

## 常见错误与自测

srsRAN在CRC指针为nullptr时返回nullopt，不代表输出一定失败；最终检查CRC。

- 先解释输入输出长度，再单步检查第一个不一致的位置。
- 修改一个边界输入，预测结果后运行；浮点比较使用容差。
- 写三句话：本课解决什么问题、哪个条款决定行为、测试如何证明正确。

## 源码阅读与复盘

按[源码导读](../../docs/source_map.md)进入对应模块。记录一个协议符号与C++变量的对应关系，说明一处教学简化。
不要把读完文件当作掌握：应能在不看答案时解释手算例子。
