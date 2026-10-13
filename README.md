# 用C++读懂PUSCH：8周实战教程

面向有GNU Radio/卫星通信经验、初入4G/5G物理层岗位的学习者。
**Windows 11 + VS Code为主，CLion可使用同一CMake/Ninja工程。**
前7周以可移植标量C++学习协议，第8周再做AVX-512与SVE。

## 从这里开始

1. [安装与运行：Windows / VS Code / CLion](docs/windows_setup.md)。
2. [第01课：构建你的第一个物理层程序](lessons/01/README.md)。
3. 遇到C++语法问题时查[语言速查](docs/cpp_primer.md)。
4. 每周按[检查点与复盘](docs/checkpoints.md)验收。

每课都有：讲义、协议条款、手算例子、分步TODO、可编译骨架、完整答案、固定输入测试。
核心代码使用中文在前、英文紧随的双语注释，采用Google C++风格。

## 先运行，再阅读

在配置好UCRT64_ROOT和Path的PowerShell中：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug --parallel 4
ctest --preset windows-debug
.\build\windows-debug\pusch_cli.exe link inf
```

默认无噪声链路输出：`TBS=576 bit_errors=0 CRC=PASS`。
运行一课：

```powershell
cmake --build --preset windows-debug --target lesson_04_solution lesson_04_exercise
.\build\windows-debug\lesson_04_solution.exe
.\build\windows-debug\lesson_04_exercise.exe
```

练习初始会明确返回`NOT IMPLEMENTED`。在`exercise.cc`中完成TODO，再运行相同固定输入。
不要通过删除测试或直接调用同名参考函数来“完成”练习。
参考程序与练习程序是两个目标，后续课程可以复用已经提供的前课模块。

## 8周路线

每周15–20小时：概念/协议4小时、编程8–10小时、源码阅读2–3小时、复盘1–3小时。
24课建议每课4–6小时；遇到LDPC编码需要时间时，优先保障正确性，把性能拓展留到最后。

| 周 | 课程 | 本周里程碑 |
|---|---|---|
| 1 | [01 构建你的第一个物理层程序](lessons/01/README.md)；[02 资源网格与索引](lessons/02/README.md)；[03 可重复输入与测试](lessons/03/README.md) | 能构建、断点调试并解释网格索引 |
| 2 | [04 Gold序列与PUSCH加扰](lessons/04/README.md)；[05 QPSK与16QAM映射](lessons/05/README.md)；[06 软解调与复噪声](lessons/06/README.md) | 逐比特理解加扰与QAM，明确LLR和噪声定义 |
| 3 | [07 CRC与位序](lessons/07/README.md)；[08 基图选择与分段边界](lessons/08/README.md)；[09 提升尺寸与长度跟踪](lessons/09/README.md) | 计算A/B/C/K′/Zc/K/F |
| 4 | [10 QC-LDPC标量编码](lessons/10/README.md)；[11 速率匹配与交织](lessons/11/README.md)；[12 译码适配与反速率匹配](lessons/12/README.md) | 亲写编码并通过独立向量与译码闭环 |
| 5 | [13 MCS、TBS与资源预算](lessons/13/README.md)；[14 DM-RS与数据RE映射](lessons/14/README.md)；[15 OFDM与循环前缀](lessons/15/README.md) | 得到可解调的PUSCH基带波形 |
| 6 | [16 从理想信道到多径](lessons/16/README.md)；[17 信道估计与均衡](lessons/17/README.md)；[18 BLER与统计可信度](lessons/18/README.md) | 理解基站估计、均衡和BLER |
| 7 | [19 HARQ软合并与缓冲区生命周期](lessons/19/README.md)；[20 UCI复用索引与DFT扩频](lessons/20/README.md)；[21 功控、测量与端到端复盘](lessons/21/README.md) | Windows标量链路集成及协议边界复盘 |
| 8 | [22 性能基线与标量内核](lessons/22/README.md)；[23 AVX-512与掩码尾部](lessons/23/README.md)；[24 SVE与向量长度无关循环](lessons/24/README.md) | 同机比较标量、AVX-512或SVE |

## 代码地图

| 位置 | 内容 |
|---|---|
| `lessons/01`—`lessons/24` | 各课讲义、骨架、答案和验收程序 |
| `include/pusch` | 公共类型和接口 |
| `src/modulation.cc` | Gold、加扰、调制解调、噪声 |
| `src/coding.cc` | CRC、分段、LDPC编码、速率匹配 |
| `src/decoder_adapter.cc` | srsRAN通用译码器适配 |
| `src/waveform.cc` | TBS、DM-RS、映射、OFDM、信道和均衡 |
| `src/link.cc` | TB到波形再到TB的集成和BLER统计 |
| `src/qpsk_avx512.cc`、`src/qpsk_sve.cc` | 第8周指令内核 |
| `tests/data` | 独立参考向量与来源记录 |
| `.vscode`、`CMakePresets.json` | 编辑、构建和调试配置 |

## 范围与验证状态

这是受限配置的NR教学链路，非完整基站或一致性认证。
亲写LDPC编码，复用srsRAN通用译码。无UCI主线、单层、type1 DM-RS、理想同步。
第20课UCI为明确标记的人为索引示例，不作为真实NR实现。

- [接口、数值约定和支持范围](docs/design.md)
- [固定规范版本与资料](docs/references.md)
- [srsRAN源码阅读地图](docs/source_map.md)
- [AVX-512 / SVE指南](docs/simd.md)
- [实际验证记录与未验证项](docs/validation.md)
- [依赖、移植改动与许可](third_party/README.md)

当前交付环境为macOS。Windows配置与CI工作流已提供，但**不把macOS通过写成Windows已验证**；
AVX-512/SVE硬件验证也独立记录。工程按AGPL-3.0-or-later提供，第三方保留各自许可。
