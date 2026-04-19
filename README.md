# BmpVsJpg

![C++](https://img.shields.io/badge/language-C%2B%2B-blue)
![MFC](https://img.shields.io/badge/framework-MFC-orange)
![License: MIT](https://img.shields.io/badge/license-MIT-green)
![Platform: Windows](https://img.shields.io/badge/platform-Windows-lightgrey)

MFC-based image conversion toolkit demonstrating JPEG compression and decompression between BMP and JPG formats. Educational project based on the book *"Visual C++ 实现 MPEG-JPEG 编解码技术"*.

基于 MFC 的图像格式转换工具集，演示 BMP 与 JPG 之间的 JPEG 压缩与解压缩技术。教学项目，参考书目：《Visual C++ 实现 MPEG-JPEG 编解码技术》。

## Sub-projects / 子项目

| Project | Direction | Description |
| --- | --- | --- |
| **BmpVsJpg** | BMP ↔ JPG | Main dual-direction converter with Huffman coding |
| **BmpToJpg** | BMP → JPG | Single-direction BMP to JPG |
| **JpgToBmp** | JPG → BMP | Single-direction JPG to BMP |
| **Change_Bmp_Jpg** | BMP ↔ JPG | Alternative dual-direction implementation |

## Requirements / 环境要求

- Windows
- Visual Studio 2010 or later
- C++ with MFC support enabled

## Build / 编译

Open each sub-project's `.vcxproj` or `.sln` file in Visual Studio and build in **Debug** or **Release** configuration.

用 Visual Studio 打开各子项目的 `.vcxproj` 或 `.sln` 文件，选择 Debug 或 Release 配置编译即可。

## Note / 说明

Only `JpgToBmp` is single-direction (JPG to BMP); the other three projects all support bidirectional conversion. The project showcases manual implementation of JPEG Huffman tables and DCT (Discrete Cosine Transform).

仅 `JpgToBmp` 为单向转换（JPG → BMP），其余项目均支持双向转换。项目展示了 JPEG 哈夫曼表和 DCT（离散余弦变换）的手动实现。

## References / 参考

- Reference book / 推荐书目：《Visual C++ 实现 MPEG-JPEG 编解码技术》
- Book PDF and source code / 书目 PDF 及随书源代码：[百度网盘](http://pan.baidu.com/s/1pLEpIzT) 密码：dgpt
- Additional source code and libraries / 相关源代码及函数库：[百度网盘](http://pan.baidu.com/s/1nv0MGhr) 密码：yw2v

## License

[MIT](LICENSE)
