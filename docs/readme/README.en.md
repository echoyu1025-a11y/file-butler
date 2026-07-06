[简体中文](../../README.md) | **English** | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Let AI organize your messy folders and find files worth cleaning up. Supports **macOS and Windows**, and can run fully offline.


---

## ✨ Two Core Features

### 🗂️ File Organizing
Pick a folder and let AI understand every file (not just its name), then suggest categories and more meaningful file names:

- **Local AI models** (Gemma 3 4B, etc.): fully offline, free, no files uploaded — privacy guaranteed
- Cloud APIs such as ChatGPT / Gemini are also supported (bring your own key)
- **Preview and confirm** before organizing, with a **dry-run mode** (see the results without touching files) and **undo**
- Creates only one level of category folders by default, avoiding folder fragmentation

### 🧹 File Cleanup
Scans a folder and reports four kinds of files you may not need:

| Category | Description |
|------|------|
| Junk / cache files | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` and the like |
| Duplicate files | Files with identical content (hash comparison), redundant copies flagged |
| Large files | Files over 100 MB, sorted by size |
| Empty folders / zero-byte files | Empty directories and files of size 0 |

**🛡 Safety promise: this tool only counts and locates files — it never deletes or moves anything.** Every item can jump straight to its containing folder, so you confirm and delete manually.

---

## 📥 Download & Install

### macOS (Apple Silicon)
1. Download the `.dmg` from the [Releases](https://github.com/echoyu1025-a11y/file-butler/releases) page
2. Open it and drag the app into the "Applications" folder
3. If blocked on first launch: **right-click the app → Open** (the app is not notarized by Apple)

### Windows
- Download the `file-butler-windows` artifact from the latest successful build on the [Actions](https://github.com/echoyu1025-a11y/file-butler/actions) page
- Or build from source following [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 Three Steps to Use

1. **Set up a model** (first launch only): pick a local LLM (recommended — free and offline) and download it, or enter a ChatGPT / Gemini API key
2. **Organize**: choose "Organize Files" in the sidebar → pick a folder → click "Analyze Folder" → confirm each category → apply
3. **Clean up**: choose "Clean Up Files" in the sidebar → pick a folder → scan → review the four result groups → click through to each file's location and handle it yourself

You can switch the interface language (15 languages) at the bottom right at any time; the category output language follows the interface language automatically.

---

## 🎨 Changes in This Edition Compared to Upstream

- Brand-new left sidebar layout (dual pages: File Organizing / File Cleanup) + light white theme
- Brand-new app icon
- New read-only "File Cleanup" scanner (not in upstream)
- Interface language and AI category output language merged into one setting, defaulting to Simplified Chinese
- Category granularity tuned: single-level category folders by default, prompts force merging into common top-level categories
- Complete Simplified Chinese localization

## 🔧 Building from Source

Full build steps for macOS / Windows / Linux are in [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Quick version for macOS:

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

