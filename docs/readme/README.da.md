[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | **Dansk** | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Lad AI rydde op i dine rodede mapper og finde filer, der er værd at rydde ud. Understøtter **macOS og Windows** og kan køre helt offline.


---

## 📸 Screenshots

| Organize | Clean up |
| --- | --- |
| ![Organize](../screenshots/organize.png) | ![Clean up](../screenshots/cleanup.png) |

## ✨ To kernefunktioner

### 🗂️ Filorganisering
Vælg en mappe: AI'en forstår hver fil (ikke kun filnavnet) og foreslår automatisk kategorier og mere sigende filnavne:

- **Lokale AI-modeller** (Gemma 3 4B m.fl.): helt offline, gratis, ingen filer uploades — privatlivet er beskyttet
- Cloud-API'er som ChatGPT / Gemini understøttes også (egen nøgle kræves)
- **Forhåndsvisning og bekræftelse** før organisering, med **prøvekørselstilstand** (se resultatet uden at røre filerne) og **fortryd**
- Opretter som standard kun ét niveau af kategorimapper for at undgå mappefragmentering

### 🧹 Filoprydning
Scanner en mappe og opgør fire slags filer, du «måske ikke har brug for»:

| Kategori | Beskrivelse |
|------|------|
| Skrald- / cachefiler | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` osv. |
| Duplikerede filer | Filer med fuldstændig ens indhold (hash-sammenligning), overflødige kopier markeres |
| Store filer | Filer over 100 MB, sorteret efter størrelse |
| Tomme mapper / nul-byte-filer | Tomme mapper og filer med størrelse 0 |

**🛡 Sikkerhedsløfte: værktøjet tæller og lokaliserer kun — det sletter eller flytter aldrig noget.** Fra hvert element hopper du med ét klik direkte til mappen, hvor det ligger, så du selv bekræfter og sletter manuelt.

---

## 📥 Download & installation

### macOS (Apple Silicon)
1. Hent `.dmg`-filen fra [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)-siden
2. Åbn den, og træk appen ind i mappen «Programmer»
3. Hvis første start blokeres: **højreklik på appen → Åbn** (appen er ikke notariseret af Apple)

### Windows
- Hent artefaktet `file-butler-windows` fra det seneste vellykkede build på [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)-siden

---

## 🚀 Kom i gang i tre trin

1. **Opsæt en model** (kun første gang): vælg en lokal LLM (anbefales — gratis og offline) og hent den, eller indtast en ChatGPT- / Gemini-API-nøgle
2. **Organisér**: vælg «Filorganisering» i sidepanelet → vælg en mappe → klik på «Analysér mappe» → bekræft hver kategori → udfør
3. **Ryd op**: vælg «Filoprydning» i sidepanelet → vælg en mappe → scan → gennemgå de fire resultatgrupper → klik dig frem til hver fils placering og håndtér den selv

Du kan når som helst skifte grænsefladesprog (15 sprog) nederst til højre; kategoriernes sprog følger automatisk grænsefladesproget.

---

## 🎨 Ændringer i denne udgave i forhold til upstream

- Helt nyt layout med venstre sidepanel (to sider: Organisering / Oprydning) + lyst hvidt tema
- Helt nyt appikon
- Ny skrivebeskyttet «Filoprydning»-funktion (findes ikke upstream)
- Grænsefladesprog og AI-kategoriernes outputsprog samlet i én indstilling, standard er forenklet kinesisk
- Optimeret kategorigranularitet: kategorimapper i ét niveau som standard, prompterne tvinger sammenlægning i almindelige hovedkategorier
- Fuldstændig lokalisering til forenklet kinesisk

## 🔧 Byg fra kildekode

Hurtig version til macOS:

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

