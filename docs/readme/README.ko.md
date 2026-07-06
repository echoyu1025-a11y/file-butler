[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | **한국어** | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

AI가 어지러운 폴더를 정리하고 정리할 가치가 있는 파일을 찾아 드립니다. **macOS와 Windows**를 지원하며 완전히 오프라인으로 실행할 수 있습니다.


---

## ✨ 두 가지 핵심 기능

### 🗂️ 파일 정리
폴더를 선택하면 AI가 각 파일을 이해하고(파일 이름만 보는 것이 아님) 분류와 더 의미 있는 파일 이름을 자동으로 제안합니다:

- **로컬 AI 모델**(Gemma 3 4B 등): 완전 오프라인, 무료, 파일 업로드 없음 — 개인정보 걱정 없음
- ChatGPT / Gemini 같은 클라우드 API도 사용 가능(API 키 필요)
- 정리 전 **미리보기 확인**, **시뮬레이션 모드**(파일을 건드리지 않고 결과만 확인) 및 **실행 취소** 지원
- 기본적으로 1단계 분류 폴더만 생성하여 폴더 파편화를 방지

### 🧹 파일 청소
폴더를 스캔하여 "필요 없을 수도 있는" 파일을 네 가지로 분류해 보여 줍니다:

| 분류 | 설명 |
|------|------|
| 정크 / 캐시 파일 | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` 등 |
| 중복 파일 | 내용이 완전히 동일한 파일(해시 비교), 불필요한 사본을 표시 |
| 대용량 파일 | 100 MB를 초과하는 파일, 크기순 정렬 |
| 빈 폴더 / 0바이트 파일 | 빈 디렉터리와 크기가 0인 파일 |

**🛡 안전 약속: 이 도구는 집계와 위치 확인만 할 뿐, 어떤 파일도 삭제하거나 이동하지 않습니다.** 각 항목에서 한 번의 클릭으로 해당 폴더로 이동할 수 있으며, 직접 확인한 후 수동으로 삭제하면 됩니다.

---

## 📥 다운로드 및 설치

### macOS (Apple Silicon)
1. [Releases](https://github.com/echoyu1025-a11y/file-butler/releases) 페이지에서 `.dmg`를 다운로드
2. 열어서 앱을 「응용 프로그램」 폴더로 드래그
3. 처음 실행 시 차단되면: **앱을 마우스 오른쪽 버튼으로 클릭 → 열기**(앱은 Apple 공증을 받지 않았습니다)

### Windows
- [Actions](https://github.com/echoyu1025-a11y/file-butler/actions) 페이지에서 최근 성공한 빌드의 `file-butler-windows` 아티팩트를 다운로드
- 또는 [docs/UPSTREAM_README.md](../UPSTREAM_README.md)의 설명에 따라 소스에서 빌드

---

## 🚀 사용 3단계

1. **모델 설정**(처음 한 번만): 실행 후 로컬 LLM(권장 — 무료·오프라인)을 선택해 다운로드하거나, ChatGPT / Gemini API 키를 입력
2. **정리**: 왼쪽에서 「파일 정리」 선택 → 폴더 선택 → 「폴더 분석」 클릭 → 분류를 하나씩 확인 → 실행
3. **청소**: 왼쪽에서 「파일 청소」 선택 → 폴더 선택 → 스캔 → 네 가지 결과 확인 → 클릭해서 파일 위치로 이동한 뒤 직접 처리

인터페이스 오른쪽 아래에서 언제든지 언어(15개)를 전환할 수 있으며, 분류 결과의 언어는 인터페이스 언어를 자동으로 따릅니다.

---

## 🎨 업스트림 대비 이 커스터마이징판의 변경 사항

- 완전히 새로운 왼쪽 사이드바 레이아웃(파일 정리 / 파일 청소 이중 페이지) + 밝은 화이트 테마
- 완전히 새로운 앱 아이콘
- 읽기 전용 「파일 청소」 스캔 기능 추가(업스트림에는 없음)
- 인터페이스 언어와 AI 분류 출력 언어를 하나의 설정으로 통합, 기본값은 중국어 간체
- 분류 세분화 최적화: 기본 1단계 분류 폴더, 프롬프트가 일반적인 대분류로의 병합을 강제
- 완전한 중국어 간체 현지화

## 🔧 소스에서 빌드하기

macOS / Windows / Linux의 전체 빌드 단계는 [docs/UPSTREAM_README.md](../UPSTREAM_README.md)를 참조하세요. macOS 빠른 버전:

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

