# 第8周：AVX-512与SVE

先完成第1–21课。数学定义、内核布局和验收标准保持不变。
不再提供SIMD构建预设，通过普通CMake选项启用相应内核。

## AVX-512：第23课

在VS Code打开`lessons/23`，选择现有x86-64 GCC工具链和Release构建类型。
在本机CMake配置参数中添加`-DPUSCH_ENABLE_AVX512=ON`，重新Configure。
选择`lesson_23_solution`或`lesson_23_exercise`构建运行。
也可以在配置好当前工具链的终端显式配置新的目录：

```powershell
cmake -S lessons/23 -B build/lesson23 -DCMAKE_BUILD_TYPE=Release -DPUSCH_ENABLE_AVX512=ON
cmake --build build/lesson23
ctest --test-dir build/lesson23 --output-on-failure
```

终端首次配置沿用当前环境可用的生成器；若与你的VS Code生成器不同，
在IDE中Configure后仅执行build/test命令即可，不要混用同一个缓存目录。
只有指令内核源码带`-mavx512f`，能力检测代码不带高级指令选项。
未启用或硬件不支持时明确返回77，不能算运行验证通过。

## SVE：第24课

将仓库同步到ARM Linux服务器，不复制Windows的build目录：

```bash
cmake -S lessons/24 -B build/lesson24 -DCMAKE_BUILD_TYPE=Release -DPUSCH_ENABLE_SVE=ON
cmake --build build/lesson24 -j 4
ctest --test-dir build/lesson24 --output-on-failure
```

仅SVE内核启用`-march=armv8-a+sve`。运行前使用Linux HWCAP检查。
Mac ARM的NEON不能替代SVE硬件验证。

## 跟写与基准

每课先完成exercise.cc中的分派，再完成kernel_exercise.cc。
对应kernel_solution.cc独立链接到答案目标，不会被练习目标调用。
AVX-512学习广播、加载、乘法、存储和尾部掩码；SVE学习运行时向量长度及谓词。
覆盖0、1、15、16、17、33、257元素和未对齐指针。

完整基准入口位于根工程pusch_cli。使用同一工具链、Release模式和相同SIMD选项配置根工程，
然后从其实际输出目录执行：

```powershell
.\build\pusch_cli.exe bench scalar
.\build\pusch_cli.exe bench avx512
```

ARM服务器运行对应的`bench scalar`、`bench sve`。
记录CPU/OS/编译器/构建选项、向量长度、正确性和同机相对加速。
默认基准65539 float、预热100次、7轮×1000次，取中位数。
不同机器绝对耗时不用于单独判断指令集优劣。
