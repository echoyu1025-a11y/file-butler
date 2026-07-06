[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | **Nederlands** | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Laat AI je rommelige mappen opruimen en bestanden vinden die het opschonen waard zijn. Ondersteunt **macOS en Windows** en kan volledig offline draaien.


---

## 📸 Screenshots

| Organize | Clean up |
| --- | --- |
| ![Organize](../screenshots/organize.png) | ![Clean up](../screenshots/cleanup.png) |

## ✨ Twee kernfuncties

### 🗂️ Bestanden organiseren
Kies een map: de AI begrijpt elk bestand (niet alleen de bestandsnaam) en stelt automatisch categorieën en betekenisvollere bestandsnamen voor:

- **Lokale AI-modellen** (Gemma 3 4B, enz.): volledig offline, gratis, geen bestanden geüpload — privacy gegarandeerd
- Cloud-API's zoals ChatGPT / Gemini worden ook ondersteund (eigen sleutel vereist)
- **Voorbeeld en bevestiging** vóór het organiseren, met een **proefdraaimodus** (bekijk het resultaat zonder bestanden aan te raken) en **ongedaan maken**
- Maakt standaard slechts één niveau categoriemappen aan, om versnippering te voorkomen

### 🧹 Bestanden opschonen
Scant een map en rapporteert vier soorten «mogelijk overbodige» bestanden:

| Categorie | Beschrijving |
|------|------|
| Rommel- / cachebestanden | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db`, enz. |
| Dubbele bestanden | Bestanden met identieke inhoud (hash-vergelijking), overbodige kopieën worden gemarkeerd |
| Grote bestanden | Bestanden groter dan 100 MB, gesorteerd op grootte |
| Lege mappen / nul-byte-bestanden | Lege directories en bestanden van 0 bytes |

**🛡 Veiligheidsbelofte: dit hulpmiddel telt en lokaliseert alleen — het verwijdert of verplaatst nooit iets.** Vanaf elk item spring je met één klik naar de bijbehorende map, zodat je zelf bevestigt en handmatig verwijdert.

---

## 📥 Downloaden & installeren

### macOS (Apple Silicon)
1. Download de `.dmg` van de [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)-pagina
2. Open deze en sleep de app naar de map «Apps»
3. Wordt de eerste start geblokkeerd: **rechtsklik op de app → Open** (de app is niet door Apple genotariseerd)

### Windows
- Download het artefact `file-butler-windows` van de laatste geslaagde build op de [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)-pagina

---

## 🚀 In drie stappen aan de slag

1. **Model instellen** (alleen de eerste keer): kies een lokale LLM (aanbevolen — gratis en offline) en download deze, of voer een ChatGPT- / Gemini-API-sleutel in
2. **Organiseren**: kies «Bestanden organiseren» in de zijbalk → kies een map → klik op «Map analyseren» → bevestig elke categorie → voer uit
3. **Opschonen**: kies «Bestanden opschonen» in de zijbalk → kies een map → scan → bekijk de vier resultaatgroepen → klik door naar de locatie van elk bestand en handel het zelf af

Je kunt de interfacetaal (15 talen) op elk moment rechtsonder wisselen; de taal van de categorieën volgt automatisch de interfacetaal.

---

## 🎨 Wijzigingen in deze editie ten opzichte van upstream

- Gloednieuwe lay-out met linkerzijbalk (twee pagina's: Organiseren / Opschonen) + licht wit thema
- Gloednieuw app-pictogram
- Nieuwe alleen-lezen functie «Bestanden opschonen» (niet aanwezig in upstream)
- Interfacetaal en uitvoertaal van AI-categorieën samengevoegd tot één instelling, standaard vereenvoudigd Chinees
- Categoriegranulariteit geoptimaliseerd: standaard categoriemappen van één niveau, prompts dwingen samenvoeging in gangbare hoofdcategorieën af
- Volledige lokalisatie in vereenvoudigd Chinees

## 🔧 Bouwen vanaf de broncode

Snelle versie voor macOS:

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

