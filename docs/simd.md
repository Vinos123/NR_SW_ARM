# 第8周：最后再做AVX-512与SVE

先完成第1–21课。优化对象仅为QPSK软解调内核，布局是交织I/Q float数组。
公式：`LLR = 2*sqrt(2)*component/N0`，正值偏向0，N0为复噪声方差。

## AVX-512：Windows x86-64

```powershell
cmake --preset windows-release
cmake --build --preset windows-release
.\build\windows-release\pusch_cli.exe bench scalar
cmake --preset windows-avx512
cmake --build --preset windows-avx512
ctest --preset windows-avx512
.\build\windows-avx512\pusch_cli.exe bench scalar
.\build\windows-avx512\pusch_cli.exe bench avx512
```

比较同一个Release构建中的scalar和avx512。`src/qpsk_avx512.cc`单独带
`-mavx512f`，分派和其他协议代码不带此选项。GCC CPU检测包含CPU/OS状态支持。
不支持时返回77并打印SKIP，不能记录为性能验证通过。

## SVE：ARM Linux服务器

将同一源码目录同步到服务器，安装GCC/G++（建议13+）、CMake和Ninja。
无需复制Windows的build目录。确认`lscpu`及`/proc/cpuinfo`，再构建：

```bash
cmake --preset linux-sve
cmake --build --preset linux-sve -j 4
ctest --preset linux-sve
./build/linux-sve/pusch_cli bench scalar
./build/linux-sve/pusch_cli bench sve
```

只有SVE内核目标启用`-march=armv8-a+sve`，运行前用Linux HWCAP检查。
Mac ARM支持NEON不代表支持SVE，不在Mac上运行SVE实验。

## 跟写内核

先在第23/24课补全分派接口，再完成本课`kernel_exercise.cc`中的实际内核。
`kernel_solution.cc`为独立答案，不会链接到练习目标。
AVX-512需要广播、未对齐加载、乘法、存储以及尾部掩码。
SVE使用`svcntw`、`svwhilelt_b32`、`svld1_f32`、`svmul_n_f32_x`、`svst1_f32`。
修改后运行全部SIMD测试，尤其0、1、15、16、17、33、257长度和偏移指针。

## 性能报告模板

| 字段 | 记录 |
|---|---|
| CPU型号/核心/频率模式 | 实际机器信息 |
| OS/编译器/构建类型 | Release；保留编译选项 |
| SVE向量长度或AVX-512能力 | 实际硬件值 |
| 样本数与循环次数 | 默认65539 float，预热100次，7轮×1000次 |
| 标量ns/float | 实测中位数 |
| 优化ns/float | 实测中位数 |
| 相对加速 | 同机标量时间/优化时间 |
| 正确性 | 与标量、通用解调器的容差结果 |

小内核可能受内存、分派和调用开销限制，不预设加速倍数。不同机器的绝对耗时
不能单独证明某一种指令集更好。每次测量保留原始终端输出。
