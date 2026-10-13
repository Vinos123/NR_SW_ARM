# 资料与版本

## 协议阅读顺序

先用[ShareTechnote PUSCH](https://www.sharetechnote.com/html/5G/5G_PUSCH.html)
理解处理链，再回到规范确认公式和边界。该页面不是规范替代品。
PUSCH是UE发送、gNB接收；分段边界使用B≤Kcb；DFT扩频不等于最终OFDM时域波形。

| 文档 | 固定版本 | 本教程阅读范围 |
|---|---|---|
| [38.211](https://www.etsi.org/deliver/etsi_ts/138200_138299/138211/17.05.00_60/ts_138211v170500p.pdf) | 17.5.0 | §4、§5.1–5.3、§6.3.1、§6.4.1.1 |
| [38.212](https://www.etsi.org/deliver/etsi_ts/138200_138299/138212/17.06.00_60/ts_138212v170600p.pdf) | 17.6.0 | §5.1、5.2.2、5.3.2、5.4.2、§6.2；UCI看§6.3.2 |
| [38.213](https://www.etsi.org/deliver/etsi_ts/138200_138299/138213/17.06.00_60/ts_138213v170600p.pdf) | 17.6.0 | §7.1功控；理解定时和控制过程背景 |
| [38.214](https://www.etsi.org/deliver/etsi_ts/138200_138299/138214/17.06.00_60/ts_138214v170600p.pdf) | 17.6.0 | §6.1资源、MCS/TBS；按其引用读§5.1.3 |
| [38.215](https://www.etsi.org/deliver/etsi_ts/138200_138299/138215/17.01.00_60/ts_138215v170100p.pdf) | 17.1.0 | §5 UE/NG-RAN测量定义，明确测量对象 |

每课先列出输入/输出符号，再判断哪些来自调度、哪些由本模块计算。
不要混入后续Release新增步骤，也不要把接收算法当作发射协议强制规定。

## 工程资料

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [VS Code GCC on Windows](https://code.visualstudio.com/docs/cpp/config-mingw)
- [MSYS2环境](https://www.msys2.org/docs/environments/)
- [CMake Tools运行与调试](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/debug-launch.md)
- [CLion工具链](https://www.jetbrains.com/help/clion/how-to-create-toolchain-in-clion.html)
- [Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)
- [Arm ACLE](https://arm-software.github.io/acle/main/acle.html)
- [MATLAB NR PUSCH Throughput](https://www.mathworks.com/help/5g/ug/nr-pusch-throughput.html)：可选，需5G Toolbox。
- [py3gpp](https://github.com/catkira/py3gpp)：独立离线参考向量生成器，仅维护者需要Python。

标准表格是算法输入，完整标准PDF未重新分发。本教程不是3GPP一致性认证。
