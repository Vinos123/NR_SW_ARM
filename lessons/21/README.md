# 第21课：功控、测量与端到端复盘

建议用时：4–6小时。先修：第20课。每周另留1–2小时复盘。

## 目标与协议定位

功控决定发射功率目标；归一化链路里的Es/N0不是直接的dBm。RSRP是指定参考信号RE上的功率测量；SINR、EVM、BER、BLER的对象也不同。本课建立量纲与适用对象的对应，不把自行统计的PUSCH功率冒称标准RSRP。

精读：**38.213 §7.1.1；38.215 §5.1、§5.2测量定义**。规范版本见[资料索引](../../docs/references.md)。
先按条款找到输入/输出符号，再看本课接口，不需要顺序通读整本规范。

## 手算与变量

P0=-80dBm、PL=120dB、alpha=1、M=6、mu=1时未限幅功率约50.79dBm；PCMAX=23dBm后输出23。

接口：

```cpp
float PuschPowerDbm(float p_max, float p0, float alpha, float path_loss, int prbs, int mu, float delta_tf, float adjustment);
```

比特使用0/1字节，索引从0开始；复数为`std::complex<float>`；LLR正值代表0。
`A/B/K/N/E/G`均以比特计，`Zc`是提升尺寸，RE是复数符号所在的网格位置。

## 跟着实现

打开[exercise.cc](exercise.cc)，按TODO完成：

1. 计算带宽项10log10(2^mu*M)。
2. 加入路径损耗补偿。
3. 执行PCMAX限幅。
4. 逐层跟踪完整链路长度。

本课C++知识：**dB与线性单位、std::min、接口整合**。遇到新语法先看[语言速查](../../docs/cpp_primer.md)。
保留中文在前、英文紧随的核心注释，解释公式、边界与原因。

## 构建、运行与预期结果

先按[Windows环境说明](../../docs/windows_setup.md)配置UCRT64。仓库根目录PowerShell：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug --target lesson_21_solution lesson_21_exercise
.\build\windows-debug\lesson_21_solution.exe
.\build\windows-debug\lesson_21_exercise.exe
```

参考答案通过时输出`PASS lesson 21`，退出码0。练习初始输出`NOT IMPLEMENTED`并返回1；完成后应满足相同验收。SIMD课在未启用或不支持的机器上输出SKIP，返回77，不算完成硬件验证。
查看[main.cc](main.cc)中的固定输入与断言；不要删除断言来使程序通过。

## 对照答案与工程实现

先独立写，再读[solution.cc](solution.cc)。完整模块：[参考实现](../../src/waveform.cc)。
主工程参考测试和独立向量位于[tests](../../tests)，运行`ctest --preset windows-debug`。
普通课程函数的参考实现与练习分开链接；可复用已完成前课模块，不需要重写依赖。
第23/24课还需跟写对应`src/qpsk_*.cc`的指令内核，并按[优化指南](../../docs/simd.md)启用。

## 常见错误与自测

38.215不是通用BLER/EVM测试标准；本课不实现完整标准测量流程。

- 先解释输入输出长度，再单步检查第一个不一致的位置。
- 修改一个边界输入，预测结果后运行；浮点比较使用容差。
- 写三句话：本课解决什么问题、哪个条款决定行为、测试如何证明正确。

## 源码阅读与复盘

按[源码导读](../../docs/source_map.md)进入对应模块。记录一个协议符号与C++变量的对应关系，说明一处教学简化。
不要把读完文件当作掌握：应能在不看答案时解释手算例子。
