# srsRAN源码导读

固定提交：`4bf1543936d062686d64c10724d2f27a9854f065`。不要使用浮动main链接。

先读教学代码，能解释输入输出之后再读工程优化。

| 课程 | 源码入口 | 带着什么问题读 |
|---|---|---|
| 04 | [序列](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/sequence_generators/pseudo_random_generator_impl.cpp) | 寄存器初值、递推和批量生成状态 |
| 05–06 | [星座](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_modulation/modulation_mapper_lut_impl.cpp) | Gray标签与归一化 |
| 07 | [CRC](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_coding/crc_calculator_generic_impl.cpp) | 反馈位和多项式 |
| 08–09 | [分段](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx_impl.cpp) | A/B/C/Zc与filler |
| 10 | [编码](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_coding/ldpc/ldpc_encoder_generic.cpp) | 前四个校验块及循环移位 |
| 11 | [速率匹配](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_coding/ldpc/ldpc_rate_matcher_impl.cpp) | k0、filler、交织 |
| 12、19 | [反速率匹配](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher_impl.cpp) | 软缓冲区累加与量化 |
| 12 | [译码](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_coding/ldpc/ldpc_decoder_generic.cpp) | 变量节点与校验节点消息 |
| 14、17 | [DM-RS估计](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/signal_processors/pusch/dmrs_pusch_estimator_impl.cpp) | 导频提取、频时插值 |
| 15 | [OFDM](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/lower/modulation/ofdm_modulator_impl.cpp) | FFT长度、CP、频率索引 |
| 17 | [均衡](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/equalization/channel_equalizer_generic_impl.cpp) | 矩阵与噪声假设 |
| 18 | [PUSCH解调](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_processors/pusch/pusch_demodulator_impl.cpp) | RE提取→均衡→LLR→解扰 |
| 21 | [处理器](https://github.com/srsran/srsRAN_Project/blob/4bf1543936d062686d64c10724d2f27a9854f065/lib/phy/upper/channel_processors/pusch/pusch_processor_impl.cpp) | 估计、解调、译码模块生命周期 |

## 三遍阅读法

1. 只找接口、配置和数据长度，不看SIMD细节。
2. 画出调用关系，在每条边写数据类型、单位、所有权和生命周期。
3. 对照某个固定输入，追踪协议公式如何变成索引与循环。

教程将分配与计算写得直观；srsRAN面向吞吐、缓存、复用和异步处理。
两者不必有相同类结构，但同一配置下的比特语义必须一致。

## 适配层专题

本工程使用通用译码器的直接构造，绕开CPU工厂与日志。输入不含前2Zc，
输出含filler。CRC指针为nullptr时不启用早停，外部必须检查CB/TB CRC。
查看[依赖说明](../third_party/README.md)了解保留源码和修改点。
