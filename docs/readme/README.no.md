[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | **Norsk** | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

La AI rydde opp i de rotete mappene dine og finne filer som er verdt å rydde bort. Støtter **macOS og Windows**, og kan kjøre helt uten nettilgang.


---

## ✨ To kjernefunksjoner

### 🗂️ Filorganisering
Velg en mappe: AI-en forstår hver fil (ikke bare filnavnet) og foreslår automatisk kategorier og mer meningsfulle filnavn:

- **Lokale AI-modeller** (Gemma 3 4B osv.): helt frakoblet, gratis, ingen filer lastes opp — personvernet er ivaretatt
- Sky-API-er som ChatGPT / Gemini støttes også (egen nøkkel kreves)
- **Forhåndsvisning og bekreftelse** før organisering, med **prøvekjøringsmodus** (se resultatet uten å røre filene) og **angre**
- Oppretter som standard bare ett nivå med kategorimapper for å unngå mappefragmentering

### 🧹 Filopprydding
Skanner en mappe og rapporterer fire typer filer du «kanskje ikke trenger»:

| Kategori | Beskrivelse |
|------|------|
| Søppel- / hurtigbufferfiler | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` og lignende |
| Dupliserte filer | Filer med helt likt innhold (hash-sammenligning), overflødige kopier merkes |
| Store filer | Filer over 100 MB, sortert etter størrelse |
| Tomme mapper / null-byte-filer | Tomme kataloger og filer med størrelse 0 |

**🛡 Sikkerhetsløfte: verktøyet teller og lokaliserer bare — det sletter eller flytter aldri noe.** Fra hvert element hopper du med ett klikk rett til mappen der det ligger, slik at du selv bekrefter og sletter manuelt.

---

## 📥 Nedlasting og installasjon

### macOS (Apple Silicon)
1. Last ned `.dmg` fra [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)-siden
2. Åpne den og dra appen til «Programmer»-mappen
3. Hvis første oppstart blokkeres: **høyreklikk på appen → Åpne** (appen er ikke notarisert av Apple)

### Windows
- Last ned artefakten `file-butler-windows` fra siste vellykkede bygg på [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)-siden
- Eller bygg fra kildekode etter [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 Kom i gang på tre trinn

1. **Sett opp en modell** (bare første gang): velg en lokal LLM (anbefalt — gratis og frakoblet) og last den ned, eller skriv inn en ChatGPT- / Gemini-API-nøkkel
2. **Organiser**: velg «Filorganisering» i sidefeltet → velg en mappe → klikk «Analyser mappe» → bekreft hver kategori → utfør
3. **Rydd opp**: velg «Filopprydding» i sidefeltet → velg en mappe → skann → gå gjennom de fire resultatgruppene → klikk deg til hver fils plassering og håndter den selv

Du kan bytte grensesnittspråk (15 språk) når som helst nederst til høyre; språket i kategoriene følger automatisk grensesnittspråket.

---

## 🎨 Endringer i denne utgaven sammenlignet med oppstrøms

- Helt ny layout med venstre sidefelt (to sider: Organisering / Opprydding) + lyst hvitt tema
- Helt nytt appikon
- Ny skrivebeskyttet «Filopprydding»-funksjon (finnes ikke oppstrøms)
- Grensesnittspråk og AI-kategorienes utdataspråk slått sammen til én innstilling, standard er forenklet kinesisk
- Optimalisert kategorigranularitet: kategorimapper på ett nivå som standard, promptene tvinger sammenslåing til vanlige hovedkategorier
- Fullstendig lokalisering til forenklet kinesisk

## 🔧 Bygge fra kildekode

Fullstendige byggetrinn for macOS / Windows / Linux finnes i [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Rask versjon for macOS:

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

