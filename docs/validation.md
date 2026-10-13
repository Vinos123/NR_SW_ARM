# 实际验证记录

记录日期：2026-10-13（Asia/Shanghai）。

## 已执行

环境：macOS ARM64、AppleClang 21.0.0、CMake/Ninja、C++20。
clang-format 23.1.3采用工程Google预设。构建包含随附的srsRAN通用译码器。

| 检查 | 结果 |
|---|---|
| Debug构建 | 通过 |
| Release构建 | 通过 |
| Debug CTest | 43项：39通过，4跳过，0失败 |
| Release CTest | 43项：39通过，4跳过，0失败 |
| 24课练习骨架构建 | 通过；初始TODO不应运行成功 |
| 自编C++格式检查 | 通过 |
| AVX-512/SVE指令源文件交叉编译 | AppleClang成功生成目标文件，仅编译检查，不是硬件运行 |
| 无噪声QPSK链路 | TBS=576，bit_errors=0，CRC=PASS |
| 无噪声16QAM链路 | MCS10，TBS=1256，bit_errors=0，CRC=PASS |
| 两抽头多径 | 已知信道与DM-RS估计模式均恢复TB |
| 20 TB/SNR冒烟扫描 | 已运行，见results/bler_smoke.csv |

跳过项：`Simd.Avx512`、`Simd.Sve`、第23课和第24课硬件运行。
“不失败”不代表这些项目已验证。

## 独立参考与覆盖

`tests/data/manifest.json`记录参考向量生成方式和SHA-256。
生成器为py3gpp 0.6.0，**没有调用被测C++来生成期望值**。

- Gold序列、CRC16/24A/24B、完整QPSK与16QAM星座。
- 单块与多块分段，包含filler与CB CRC。
- BG1/BG2，各取8组提升集合的代表Zc，并额外测试Zc=384。
- 18个码字案例，每个比较完整编码输出及RV 0、1、2、3。
- 51组支持范围内的MCS/PRB/TBS组合。
- CRC单比特错误、BG选择阈值、码块边界、HARQ恢复和合并。
- OFDM往返、DM-RS平坦信道估计、噪声统计、均衡与端到端CRC。

LDPC向量使用py3gpp的稀疏矩阵编码路径。它的高层`nrLDPCEncode`在小BG2情况下
选择iLS有局限，维护脚本显式按Zc选择集合后调用底层稀疏矩阵算法，未修改向量以迎合C++。

20块的BLER扫描只检查运行和输出，不用于评估产品性能。每个点保留Wilson 95%区间。
没有对完整空口波形进行外部仪表或MATLAB一致性认证。

## 尚未执行，不能宣称通过

- Windows 11/UCRT64原生构建、运行及VS Code/CLion实际调试。
- Windows CI实际运行（工作流已提供，但当前目录未推送到远端运行）。
- AVX-512硬件执行、SVE服务器执行与性能加速验证。

本机静态检查已修复上游一处Windows LLP64长度类型问题，并排除日志/pthread依赖。
这不替代真实Windows测试。Windows环境准备好后运行：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
cmake --build --preset windows-debug --target all_exercises
ctest --preset windows-debug
cmake --preset windows-release
cmake --build --preset windows-release
ctest --preset windows-release
```

VS Code检查断点能否命中`RunLink`，CLion检查能否使用同一UCRT64工具链。
将实际OS、GCC/GDB版本和测试日志补入此文件，再把Windows状态改为已验证。

## 复现本机验证

```bash
cmake -S . -B build/local -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/local -j 4
cmake --build build/local --target all_exercises -j 4
ctest --test-dir build/local --output-on-failure
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j 4
ctest --test-dir build/release --output-on-failure
python tools/check_format.py
```

`tools/check_format.py`和向量生成器是维护工具；学习者运行C++不需要Python。
