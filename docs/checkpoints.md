# 每周验收与岗位复盘

## 第1周

能解释一个GNU Radio流图中的采样点如何与NR资源网格关联；在调试器里查看6PRB网格。
独立编译第1–3课答案，并完成练习。说明编译错误、链接错误、运行错误的区别。

## 第2周

不看答案写出QPSK四个点，解释16QAM的sqrt(10)。能说清为什么加扰两次恢复原比特，
但对LLR解扰要翻转符号。把复噪声方差拆成两个实分量方差，检查量纲。

## 第3周

手算A=24、A=3824、A=9000三例，记录A/B/C/K′/Kb/Zc/K/F。
说清TB CRC24A与CB CRC24B的区别，以及BG2中Kb为什么可能不等于10。

## 第4周

先通过全码字H*c=0，再通过独立编码向量；不能只做自发自收。
画出完整码字→穿孔→循环选择→交织的索引，解释RV和filler。
阅读适配层，指出哪些转换可能影响性能而不改变协议本身。

## 第5周

默认6PRB配置必须得到：TBS=576，DM-RS RE=36，数据RE=972，G=1944，时域3840样本。
解释TBS资源上限156与实际数据资源162/PRB的差异。
检查资源网格图或打印l=2的RE，确认没有导频/数据覆盖。

## 第6周

对单位信道、平坦复增益、短多径分别测试。
比较解析信道频响和DM-RS估计，不预设二者必须给出完全相同BLER。
执行：

```powershell
.\build\windows-debug\pusch_cli.exe bler 20 bler_smoke.csv
```

20块仅为快速冒烟；正式统计增加样本数并保留置信区间。
说明为何“本次0错误”不能推出“真实BLER=0”。

## 第7周

使用RV0、RV2的恢复LLR在同一母码坐标系合并，说明新TB必须重置缓冲。
读UCI实际条款，列出玩具索引示例缺失的编码、预算和占位步骤。
区分测量量：

| 指标 | 对象与用途 | 本教程状态 |
|---|---|---|
| RSRP | 指定参考信号RE的平均功率 | 阅读38.215定义，未实现完整标准测量 |
| SINR | 指定信号对象相对干扰加噪声的功率比 | 解释；不把CLI Es/N0改名为标准SINR |
| EVM | 接收星座相对参考星座的误差 | 可由均衡结果与已知符号计算，非BLER |
| BER | 错误比特/总比特 | 链路返回bit_errors |
| BLER | CRC失败TB/总TB | CLI统计并给Wilson区间 |

## 第8周

依次测量标量、优化内核；所有尾部测试通过后才记录加速。
没有相关硬件时保留“待验证”，不得用编译通过冒充运行通过。
提交一个中文技术报告，以及如下英文摘要：

> I implemented a constrained single-layer NR PUSCH link in C++.
> The reference profile uses ... .
> I validated the bit-processing stages against ... .
> The main discrepancy I debugged was ... .
> On the tested platform, the optimized kernel ... .
> The following features remain outside the supported profile: ... .

## 五类故障定位练习

在单独复制的源码或临时分支中改动，每次只引入一类错误，完成后恢复：

1. **位序**：反转CRC附加顺序，观察最早失败的参考测试。
2. **长度**：把BG2的K错误写成Kb×Zc，观察小块测试与filler位置。
3. **映射**：把DM-RS放到奇数k，逐RE比对而非只看频谱形状。
4. **LLR符号**：反转解调输出，检查无噪声CRC为何也失败。
5. **噪声归一化**：把N0/2改成N0，测实际复噪声功率并解释BLER偏移。

不要提交带故障的参考答案。每次记录“故障→首个不一致位置→定位依据→修复验证”。
