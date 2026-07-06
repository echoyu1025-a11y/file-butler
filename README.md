# 文件管家（File Butler）

用 AI 帮你整理乱糟糟的文件夹，并找出值得清理的文件。支持 **macOS 和 Windows**，可完全离线运行。

> 基于开源项目 [AI File Sorter](https://github.com/hyperfield/ai-file-sorter) 深度定制的中文版。遵循 AGPL-3.0 开源协议。

---

## ✨ 两大功能

### 🗂️ 文件整理
选择一个文件夹，AI 读懂每个文件（不只是看文件名），自动建议分类和更有意义的文件名：

- **本地 AI 模型**（Gemma 3 4B 等）：完全离线、免费、文件不上传，隐私无忧
- 也可选用 ChatGPT / Gemini 等云端 API（需自备 key）
- 整理前有**预览确认**，支持**预演模式**（只看结果不动文件）和**撤销**
- 默认只建一级分类文件夹，避免文件夹碎片化

### 🧹 文件清理
扫描文件夹，统计出四类"可能不需要"的文件：

| 类别 | 说明 |
|------|------|
| 垃圾 / 缓存文件 | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` 等 |
| 重复文件 | 内容完全相同的文件（哈希比对），标出多余副本 |
| 大文件 | 超过 100 MB 的文件，按大小排序 |
| 空文件夹 / 零字节文件 | 空目录和大小为 0 的文件 |

**🛡 安全承诺：本工具只统计、只定位，绝不删除或移动任何文件。** 每一项都能一键跳转到所在文件夹，由你自己确认后手动删除。

---

## 📥 下载安装

### macOS（Apple Silicon）
1. 到 [Releases](../../releases) 页面下载 `.dmg`
2. 双击打开，把应用拖进「应用程序」文件夹
3. 首次打开如被拦截：**右键点击应用 → 打开**（应用未做 Apple 公证）

### Windows
- 到 [Actions](../../actions) 页面最近一次成功的构建里下载 `file-butler-windows` 产物
- 或按 [README_EN.md](README_EN.md) 的说明从源码构建

---

## 🚀 使用三步走

1. **配模型**（仅首次）：启动后选择本地 LLM（推荐，免费离线）并下载，或填入 ChatGPT / Gemini API key
2. **整理**：左侧选「文件整理」→ 选文件夹 → 点「分析文件夹」→ 逐项确认分类 → 执行
3. **清理**：左侧选「文件清理」→ 选文件夹 → 扫描 → 查看四类结果 → 点击跳转到文件位置自行处理

界面右下角可随时切换语言（15 种），分类结果的语言会自动跟随界面语言。

---

## 🎨 本定制版相对上游的改动

- 全新左侧导航栏布局（文件整理 / 文件清理双页面）+ 白色浅色主题
- 全新应用图标
- 新增「文件清理」只读扫描功能（上游没有）
- 界面语言与 AI 分类输出语言合并为一个选择，默认简体中文
- 分类粒度优化：默认单级分类文件夹，提示词强制归并到常见大类
- 完整简体中文本地化

## 🔧 从源码构建

macOS / Windows / Linux 的完整构建步骤见 [README_EN.md](README_EN.md)。macOS 快速版：

```bash
brew install qt curl jsoncpp sqlite openssl fmt spdlog mediainfo cmake git pkgconfig libffi
git clone --recursive https://github.com/echoyu1025-a11y/file-butler.git
cd file-butler
cmake -S external/libzip -B external/libzip/build -DBUILD_SHARED_LIBS=OFF -DBUILD_DOC=OFF \
  -DENABLE_BZIP2=OFF -DENABLE_LZMA=OFF -DENABLE_ZSTD=OFF -DENABLE_OPENSSL=OFF \
  -DENABLE_GNUTLS=OFF -DENABLE_MBEDTLS=OFF -DENABLE_COMMONCRYPTO=OFF -DENABLE_WINDOWS_CRYPTO=OFF
cmake --build external/libzip/build
./app/scripts/build_llama_macos.sh --arm64
cd app && make -j8 && ./bin/aifilesorter
```

## 📄 许可与致谢

- 本项目基于 [hyperfield/ai-file-sorter](https://github.com/hyperfield/ai-file-sorter) 修改，感谢原作者
- 许可协议：[GNU AGPL-3.0](LICENSE) —— 你可以自由使用、修改、分发，衍生版本须同样开源
- 本地推理由 [llama.cpp](https://github.com/ggerganov/llama.cpp) 驱动
