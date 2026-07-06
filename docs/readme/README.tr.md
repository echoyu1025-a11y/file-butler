[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | **Türkçe** | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Dağınık klasörlerinizi yapay zekâya düzenletin ve temizlenmeye değer dosyaları bulun. **macOS ve Windows** desteklenir, tamamen çevrimdışı çalışabilir.


---

## ✨ İki Temel Özellik

### 🗂️ Dosya Düzenleme
Bir klasör seçin; yapay zekâ her dosyayı anlar (yalnızca dosya adına bakmaz), otomatik olarak kategoriler ve daha anlamlı dosya adları önerir:

- **Yerel yapay zekâ modelleri** (Gemma 3 4B vb.): tamamen çevrimdışı, ücretsiz, dosya yüklenmez — gizlilik güvencesi
- ChatGPT / Gemini gibi bulut API'leri de kullanılabilir (kendi anahtarınız gerekir)
- Düzenlemeden önce **önizleme ve onay**, **prova modu** (dosyalara dokunmadan yalnızca sonucu görün) ve **geri alma** desteği
- Varsayılan olarak yalnızca tek seviyeli kategori klasörleri oluşturur, klasör parçalanmasını önler

### 🧹 Dosya Temizleme
Bir klasörü tarar ve «belki gereksiz» dosyaları dört kategoride raporlar:

| Kategori | Açıklama |
|------|------|
| Çöp / önbellek dosyaları | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` vb. |
| Yinelenen dosyalar | İçeriği tamamen aynı olan dosyalar (hash karşılaştırması), fazla kopyalar işaretlenir |
| Büyük dosyalar | 100 MB'ı aşan dosyalar, boyuta göre sıralı |
| Boş klasörler / sıfır baytlık dosyalar | Boş dizinler ve boyutu 0 olan dosyalar |

**🛡 Güvenlik taahhüdü: bu araç yalnızca sayar ve konumu gösterir — hiçbir dosyayı asla silmez veya taşımaz.** Her öğeden tek tıkla bulunduğu klasöre gidebilir, kendiniz doğrulayıp elle silebilirsiniz.

---

## 📥 İndirme ve Kurulum

### macOS (Apple Silicon)
1. [Releases](https://github.com/echoyu1025-a11y/file-butler/releases) sayfasından `.dmg` dosyasını indirin
2. Açın ve uygulamayı «Uygulamalar» klasörüne sürükleyin
3. İlk açılışta engellenirse: **uygulamaya sağ tıklayın → Aç** (uygulama Apple noter onayından geçmemiştir)

### Windows
- [Actions](https://github.com/echoyu1025-a11y/file-butler/actions) sayfasındaki en son başarılı derlemeden `file-butler-windows` çıktısını indirin
- Veya [docs/UPSTREAM_README.md](../UPSTREAM_README.md) yönergelerine göre kaynaktan derleyin

---

## 🚀 Üç Adımda Kullanım

1. **Model kurun** (yalnızca ilk seferde): başlattıktan sonra yerel bir LLM seçip indirin (önerilir — ücretsiz ve çevrimdışı) veya ChatGPT / Gemini API anahtarınızı girin
2. **Düzenleyin**: sol taraftan «Dosya Düzenleme»yi seçin → klasör seçin → «Klasörü Analiz Et»e tıklayın → her kategoriyi onaylayın → uygulayın
3. **Temizleyin**: sol taraftan «Dosya Temizleme»yi seçin → klasör seçin → tarayın → dört gruptaki sonuçları inceleyin → tıklayıp dosyanın konumuna giderek kendiniz karar verin

Arayüzün sağ alt köşesinden dili (15 dil) istediğiniz zaman değiştirebilirsiniz; kategori çıktılarının dili otomatik olarak arayüz dilini izler.

---

## 🎨 Bu Sürümün Upstream'e Göre Değişiklikleri

- Yepyeni sol kenar çubuğu düzeni (Dosya Düzenleme / Dosya Temizleme çift sayfa) + açık beyaz tema
- Yepyeni uygulama simgesi
- Yeni salt okunur «Dosya Temizleme» tarama özelliği (upstream'de yok)
- Arayüz dili ile yapay zekâ kategori çıktı dili tek ayarda birleştirildi, varsayılan Basitleştirilmiş Çince
- Kategori ayrıntı düzeyi iyileştirildi: varsayılan tek seviyeli kategori klasörleri, istemler yaygın büyük kategorilere birleşmeyi zorunlu kılar
- Eksiksiz Basitleştirilmiş Çince yerelleştirme

## 🔧 Kaynaktan Derleme

macOS / Windows / Linux için eksiksiz derleme adımları [docs/UPSTREAM_README.md](../UPSTREAM_README.md) dosyasındadır. macOS hızlı sürüm:

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

