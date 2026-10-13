# Windows 11：使用VS Code当前工具链

工程使用普通CMake，没有构建预设，也没有编译器、生成器或安装路径绑定。
使用你现有的MSYS/MSYS2环境中、由VS Code检测到或已经选中的GCC工具链即可。
CMake至少3.24，GCC需要支持C++20；CMake Tools配置阶段会检查编译能力。

## 1. 打开一个课目录

例如在VS Code选择“打开文件夹”，打开仓库内的`lessons/04`。
也可在已配置环境的终端运行：

```powershell
code .\lessons\04
```

保持课目录位于完整仓库内：它会引用`../../include`、公共源码和必要第三方依赖。
每课有自己的`CMakeLists.txt`，无需先配置根工程，也不依赖前课的build目录。

## 2. 选择工具链并配置

安装推荐的C/C++、CMake Tools扩展。使用 **CMake: Select a Kit** 选择当前环境检测到的GCC工具链；已有正确选择时继续使用它。
选择Debug构建类型，执行 **CMake: Configure**。

“MSYS”是环境名称，具体编译器以CMake Tools显示的gcc/g++路径为准。
工程不替你切换到UCRT64或指定Ninja；CMake Tools按照当前工具链及可用工具选择生成器。
程序运行也使用同一开发环境，确保该工具链的运行时DLL可被找到。

## 3. 选择目标后构建

在CMake Tools的Build目标中选择：

- `lesson_04_solution`：参考答案。
- `lesson_04_exercise`：需要你实现的练习。

执行Build，只编译该目标及其必要模块。独立课工程的默认构建也会生成这两个目标。
初始练习可以编译，但运行会输出`NOT IMPLEMENTED`并返回1，这是预期行为。

**不要使用“编译当前文件”或Code Runner直接编译main.cc。** 一课通常包含多个源码文件，
应由CMake完成编译和链接。修改exercise.cc后仍构建exercise目标。

## 4. 运行和调试

在CMake Tools的Launch目标中选择同一个答案或练习目标：

- Run按钮运行当前目标。
- Debug按钮或 **CMake: Debug** 启动调试。
- 在该课solution.cc或exercise.cc设置断点。

CMake Tools从已配置的工程中识别程序路径和调试器，不需要固定的launch.json。
构建目标和Launch目标是两个选择，切换答案/练习时同时确认，避免运行了旧目标。

这一机制见[CMake Tools官方运行/调试说明](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/debug-launch.md)。

## 5. 在终端使用已配置工程

以下命令以CMake Tools常用的`build`目录为例；若IDE选择了其他目录，替换为实际路径。
执行前已在VS Code完成Configure，因此无需在终端重新指定编译器或生成器。

```powershell
cmake --build build --target lesson_04_solution lesson_04_exercise
ctest --test-dir build --output-on-failure
.\build\lesson_04_solution.exe
.\build\lesson_04_exercise.exe
```

单配置GCC构建的exe通常位于build目录；多配置生成器可能放在Debug等子目录。
CMake Tools的Run/Debug会使用实际路径，无需猜测。
CTest仅运行参考答案；它不会把未完成练习计入通过率。

## 6. 打开根工程

需要完整链路和全部测试时，打开仓库根目录，使用相同的Configure流程。
根工程仍能选择单课目标，只编译该课的模块，例如：

```powershell
cmake --build build --target lesson_04_solution
```

根工程中该课程序通常位于`build/lessons/04`；建议仍使用CMake Tools Run/Debug。
完整链路目标为`pusch_cli`，默认不传参数即可运行无噪声链路。
要编译全部练习骨架，选择`all_exercises`；它不会执行未完成的练习。

## 7. CLion

打开根目录或某一课目录中的CMakeLists.txt，选择你现有的GCC工具链。
选择相应的lesson solution/exercise运行配置。根工程和独立课工程使用同一套源码与依赖声明，
不需要单独维护CLion项目文件。

## 8. 迁移与排错

| 情况 | 处理 |
|---|---|
| 仍提示旧预设或旧编译器 | 重新打开文件夹，使用CMake Tools清理缓存并重新Configure |
| 更换编译器或生成器 | 使用新build目录；不要复用旧工具链的CMakeCache |
| 已构建练习却运行答案 | 同时检查Build目标与Launch目标 |
| 程序找不到DLL | 从当前MSYS开发环境启动VS Code，检查其继承的Path |
| 单文件编译出现undefined reference | 改用CMake目标构建 |
| `NOT IMPLEMENTED` | 练习尚未实现，按TODO补全 |
| SIMD输出SKIP | 尚未启用该内核或缺少硬件，见优化指南 |

实际平台验证状态见[验证记录](validation.md)。当前交付环境是macOS，Windows IDE操作仍需在你的电脑上确认。
