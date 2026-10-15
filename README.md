# PETools

滴水逆向 PE 学习过程中编写的 C 静态库，供 TinyX86、反汇编引擎演示程序和 JiaquanPEViewer 共同使用。代码入口为 `PETools.h` / `PETools.c`，Visual Studio 库项目为 `PETools.vcxproj`。

提供 PE32 文件读取、头与节信息解析、RVA/FOA 换算、FileBuffer/ImageBuffer 转换、导入/导出/重定位/资源表显示，以及节和目录编辑练习接口。C++ 调用通过头文件的 `extern "C"` 兼容。

## 下载与构建

```powershell
git clone --recurse-submodules https://github.com/582835293/PETools.git
cd PETools
# 在 Visual Studio Developer PowerShell 中运行；也可直接打开仓库内 .slnx
msbuild "PETools.vcxproj" /m /p:Configuration=Debug /p:Platform=Win32
# VS 2022 工具集可追加 /p:PlatformToolset=v143
```

安装 Visual Studio C++ 桌面开发组件和 Windows 10 SDK，选择 x86/Win32。CMake 构建与验证：

```powershell
cmake -S . -B build -A Win32
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

完整应用当前只支持 Windows x86/PE32。[共享依赖、升级方法和项目关系](docs/project-relationships.md)。

## 接入其他项目

添加此仓库为 `deps/PETools` 子模块；在应用 `.vcxproj` 添加对 `deps/PETools/PETools.vcxproj` 的 `ProjectReference`，并把 `deps/PETools` 加入头文件搜索路径。应用中 `#include "PETools.h"`，不要再次编译一份 PETools.c。

所有消费者与库统一 `UNICODE` / `_UNICODE` 和运行库（Debug `/MDd`、Release `/MD`）。CMake 的 `petools` 目标会传播头文件目录和 Unicode 定义。

## 测试与边界

测试用内存中构造的单节 PE32 验证头字段、RVA/FOA、文件/映像往返和代码节定位，并检查 C++ 链接。接口说明、缓冲区所有权和限制见 [API 文档](docs/api.md)。

这是学习代码。现有接口多不带缓冲区长度，未完成畸形文件边界检查、PE32+ 支持和线程安全改造。公开展示不等同于增加开源许可证；本仓库尚未指定许可证。

[构建与测试记录](docs/validation.md) · [GitHub Actions](https://github.com/582835293/PETools/actions)
