# PETools API 与数据布局

## 缓冲区约定

FileBuffer 保持磁盘布局，节数据通过 `PointerToRawData` 定位。ImageBuffer 按 `VirtualAddress` 展开到 `SizeOfImage` 大小。传入错误布局会让目录解析定位错误。

| 接口 | 输入/输出与所有权 |
| --- | --- |
| `ReadPEFile(LPCTSTR, LPVOID*)` | Unicode 构建传宽字符路径；返回读取字节数，输出由 malloc 分配，调用者 free |
| `ValidatePEFile` | 检查 MZ/PE 签名；不等于完整文件验证 |
| `ParsePEHeaders` | FileBuffer → 调用者提供的 PEHeaderInfo |
| `RvaToFileOffset` | FileBuffer + RVA → FOA；0 同时可能表示起始偏移或失败 |
| `CopyFileBufferToImageBuffer` | FileBuffer → malloc 分配的 ImageBuffer，返回大小，调用者 free |
| `CopyImageBufferToFileBuffer` | ImageBuffer → malloc 分配的 FileBuffer，返回大小，调用者 free |
| `QueryCodeSection` | ImageBuffer → 第一处代码节在该缓冲区内的借用指针和 VirtualSize；不得单独 free |
| `Get*TableStrings` | 把目录的文字表示写入调用者缓冲区；当前没有容量参数 |
| `Print*Table` | 向 stdout 打印对应目录 |

## 示例

```c
#include "PETools.h"
#include <stdlib.h>
int main(void) {
    LPVOID file = NULL;
    PEHeaderInfo headers = {0};
    if (!ReadPEFile(_T("sample.exe"), &file)) return 1;
    if (!ValidatePEFile(file)) { free(file); return 1; }
    ParsePEHeaders(file, &headers);
    printf("entry RVA = %08lX\n", headers.dwEntryPoint);
    free(file);
    return 0;
}
```

## 使用限制

实现按 Windows x86 和 PE32 学习模型编写；部分函数把宿主指针转换为 DWORD。未对截断、溢出、无效节数与目录越界作完整检查。不要把签名检查成功理解为任意输入都可安全解析。

`APPEND` 宏使用共享 `tempBuf[512]` 和无容量参数的输出缓冲区，因此当前不支持并发输出；消费者必须自行保证输入与缓冲区容量。后续适合引入带长度的 PEView 和输出接口，这次迁移保留既有 API 行为。
