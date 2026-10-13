# Windows 11：VS Code与CLion共用工程

目标是生成Windows原生`.exe`。不使用WSL，不生成Visual Studio解决方案。
本教程默认x86-64 Windows；SVE课在ARM Linux服务器运行。

## 1. 安装UCRT64工具

安装[MSYS2](https://www.msys2.org/)，打开 **MSYS2 UCRT64** 终端。
以下两步只有安装阶段在该终端执行，后续统一PowerShell。

```bash
pacman -Syu
# 如果要求退出终端，关闭后重新打开UCRT64并再次执行更新。
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-gdb mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-clang-tools-extra
```

编译器需要完整C++20支持，建议GCC 13及以上；CMake至少3.24。
GoogleTest 1.15.2、KissFFT 131.1.0、srsRAN固定提交及fmt 11.1.3已随工程提供，
构建不需要在线下载第三方库。不要混入MSYS环境的`/usr/bin/g++`或其他CRT的库。

## 2. 在PowerShell声明工具位置

把示例路径改为你实际安装位置。这是本机设置，不写入共享工程。

```powershell
$env:UCRT64_ROOT = 'C:\msys64\ucrt64'
$env:Path = "$env:UCRT64_ROOT\bin;$env:Path"
g++ --version
cmake --version
ninja --version
gdb --version
```

本终端关闭后设置失效。如希望持久化，在Windows用户环境变量中设置
`UCRT64_ROOT`并把其`bin`目录加入用户Path，再重启VS Code/CLion。
从当前终端执行`code .`时，VS Code将继承这些变量。
运行`.exe`也需要此Path，以找到同工具链的运行时DLL。

## 3. 首次构建与自检

在项目根目录：

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug --parallel 4
ctest --preset windows-debug
.\build\windows-debug\pusch_cli.exe link inf
```

默认输出：`TBS=576 bit_errors=0 CRC=PASS`。
SIMD相关测试可能显示Skipped，这不影响前7周。

每课有两个独立目标：

```powershell
cmake --build --preset windows-debug --target lesson_04_solution lesson_04_exercise
.\build\windows-debug\lesson_04_solution.exe
.\build\windows-debug\lesson_04_exercise.exe
```

答案应通过；初始练习会输出`NOT IMPLEMENTED`，退出码1。
**练习默认不加入通过率统计，也不默认构建**，不会因TODO而破坏参考工程。
若只想检查所有骨架能否编译：

```powershell
cmake --build --preset windows-debug --target all_exercises
```

## 4. VS Code

打开根目录文件夹，安装工程推荐的C/C++、CMake Tools扩展。
在CMake Tools选择`windows-debug`配置预设。`Ctrl+Shift+B`运行构建任务。
运行“PUSCH link (GDB)”调试配置，在`src/link.cc`中设置断点。

推荐第一次调试观察：`tb.size()`、`plan`、`lengths`、`coded.size()`。
需要调试某课时，复制launch配置，把program改为该课的exe名。
源码使用UTF-8；如PowerShell中文字显示异常，先检查终端字体及UTF-8配置，
不要把源文件保存成ANSI。控制台固定验收文本使用ASCII，避免编码影响判断。

## 5. CLion

打开同一根目录的`CMakeLists.txt`。在Toolchains添加MinGW工具链，位置选择
`UCRT64_ROOT`，C/C++编译器和GDB均来自该目录的`bin`。
使用Ninja生成器，导入`windows-debug`/`windows-release`预设。
若CLion版本无法自动导入，在CMake profile设置同样的构建类型与目录。
不使用另一个捆绑MinGW来构建共享build目录；更换编译器时新建构建目录。

## 6. 排错表

| 症状 | 检查方法 |
|---|---|
| 找不到g++或Ninja | `Get-Command g++`；检查UCRT64_ROOT和Path |
| 找不到运行时DLL | 从已配置Path的终端启动exe或IDE |
| CMake编译器缓存错误 | 更换构建目录，避免混用不同工具链缓存 |
| 断点空心或变量优化掉 | 选择Debug预设，确认运行的是同目录程序 |
| `NOT IMPLEMENTED` | 正在运行练习骨架，补TODO或先运行solution |
| `SKIP`/退出码77 | 指令内核未编译或机器不支持，见SIMD指南 |

当前交付机器是macOS；Windows CI配置已提供，但本地验证状态以
[验证记录](validation.md)为准。配置存在不等于已在Windows实际跑过。
