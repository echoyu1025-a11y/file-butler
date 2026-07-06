[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | **Svenska** | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Låt AI städa upp dina stökiga mappar och hitta filer som är värda att rensa. Stöder **macOS och Windows** och kan köras helt offline.


---

## 📸 Screenshots

| Organize | Clean up |
| --- | --- |
| ![Organize](../screenshots/organize.png) | ![Clean up](../screenshots/cleanup.png) |

## ✨ Två kärnfunktioner

### 🗂️ Filorganisering
Välj en mapp: AI:n förstår varje fil (inte bara filnamnet) och föreslår automatiskt kategorier och mer meningsfulla filnamn:

- **Lokala AI-modeller** (Gemma 3 4B m.fl.): helt offline, gratis, inga filer laddas upp — integriteten är skyddad
- Moln-API:er som ChatGPT / Gemini stöds också (egen nyckel krävs)
- **Förhandsgranskning och bekräftelse** före organisering, med **testläge** (se resultatet utan att röra filerna) och **ångra**
- Skapar som standard bara en nivå av kategorimappar för att undvika mappfragmentering

### 🧹 Filrensning
Skannar en mapp och redovisar fyra typer av filer som «kanske inte behövs»:

| Kategori | Beskrivning |
|------|------|
| Skräp- / cachefiler | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` med flera |
| Dubblettfiler | Filer med exakt samma innehåll (hashjämförelse), överflödiga kopior markeras |
| Stora filer | Filer över 100 MB, sorterade efter storlek |
| Tomma mappar / nollbyte-filer | Tomma kataloger och filer med storlek 0 |

**🛡 Säkerhetslöfte: verktyget räknar och lokaliserar bara — det raderar eller flyttar aldrig någonting.** Från varje post hoppar du med ett klick direkt till mappen där den finns, så att du själv bekräftar och raderar manuellt.

---

## 📥 Hämta & installera

### macOS (Apple Silicon)
1. Hämta `.dmg`-filen från sidan [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)
2. Öppna den och dra appen till mappen «Program»
3. Om första starten blockeras: **högerklicka på appen → Öppna** (appen är inte notariserad av Apple)

### Windows
- Hämta artefakten `file-butler-windows` från det senaste lyckade bygget på sidan [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)
- Eller bygg från källkod enligt [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 Kom igång i tre steg

1. **Ställ in en modell** (endast första gången): välj en lokal LLM (rekommenderas — gratis och offline) och hämta den, eller ange en ChatGPT- / Gemini-API-nyckel
2. **Organisera**: välj «Filorganisering» i sidofältet → välj en mapp → klicka på «Analysera mapp» → bekräfta varje kategori → verkställ
3. **Rensa**: välj «Filrensning» i sidofältet → välj en mapp → skanna → gå igenom de fyra resultatgrupperna → klicka dig fram till varje fils plats och hantera den själv

Du kan byta gränssnittsspråk (15 språk) när som helst längst ned till höger; kategoriernas språk följer automatiskt gränssnittsspråket.

---

## 🎨 Ändringar i denna utgåva jämfört med uppströmsprojektet

- Helt ny layout med vänster sidofält (två sidor: Organisering / Rensning) + ljust vitt tema
- Helt ny appikon
- Ny skrivskyddad «Filrensning»-funktion (finns inte uppströms)
- Gränssnittsspråk och AI-kategoriernas utdataspråk sammanslagna till en inställning, standard är förenklad kinesiska
- Optimerad kategorigranularitet: enkelnivå-kategorimappar som standard, prompterna tvingar ihopslagning till vanliga huvudkategorier
- Fullständig lokalisering till förenklad kinesiska

## 🔧 Bygga från källkod

Fullständiga bygginstruktioner för macOS / Windows / Linux finns i [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Snabbversion för macOS:

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

