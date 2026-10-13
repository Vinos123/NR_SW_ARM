# 固定依赖与许可 / Pinned dependencies and licenses

所有运行必需源码已随工程提供，不需要学习者构建完整srsRAN或在线下载依赖。

| 依赖 | 版本/提交 | 许可 | 用途 |
|---|---|---|---|
| GoogleTest | v1.15.2 | BSD-3-Clause | 核心测试 |
| KissFFT | 131.1.0 | BSD-3-Clause | 单精度复FFT |
| srsRAN_Project | 4bf1543936d062686d64c10724d2f27a9854f065 | AGPL-3.0-or-later | 通用LDPC译码、基图表及最小依赖 |
| fmt | 11.1.3，取自上述srsRAN提交 | MIT | srsRAN格式化类型定义 |

工程整体按根目录LICENSE中的AGPL-3.0-or-later提供；第三方保留各自许可。
集成到其他工程前应审查这些依赖的许可。代码不包含公司内部源码。

## srsRAN提取范围

- `ldpc_decoder_impl`、`ldpc_decoder_generic`、`ldpc_graph_impl`、`ldpc_luts_impl`的源码和头文件。
- `log_likelihood_ratio.cpp`及其最小递归头文件依赖。
- 上游捆绑fmt头文件，使用FMT_HEADER_ONLY。

## 本地修改

1. `support/srsran_assert.h`使用始终有效的可移植断言替代日志与平台相关实现。
2. `adt/span.h`移除未使用的srslog include及copy_loggable_type特化。
3. `src/decoder_adapter.cc`以std::find提供比较辅助符号，不引入额外SIMD库。
4. `ldpc_decoder_impl.cpp`将长度表达式中的unsigned long改为size_t显式运算，兼容Windows LLP64。
5. `support/math/math_utils.h`用C++20的`std::numbers::pi_v<float>`替代非标准`M_PI`宏，兼容Windows GCC严格模式。
6. `adt/bounded_bitset.h`显式包含`<functional>`，为`std::function`提供声明，避免依赖其他头文件的间接包含。

通用译码核心算法、LLR类型语义和标准基图数据保留上游实现。
这些修改与Windows运行验证是两件事，实际验证状态见docs/validation.md。

## 参考向量

`tests/data`由`py3gpp==0.6.0`生成；运行C++不依赖Python。
生成脚本保留在`tools/generate_vectors.py`，文件SHA-256在tests/data/manifest.json。
LDPC采用其独立的稀疏矩阵编码路径；教学C++采用循环移位求解，避免同一实现自证。
py3gpp小BG2的高层包装选iLS存在限制，生成器从合法Zc显式选择iLS后调用稀疏矩阵路径。
