[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | **Íslenska**

# File Butler (文件管家)

Láttu gervigreind taka til í óskipulögðum möppum og finna skrár sem vert er að hreinsa. Styður **macOS og Windows** og getur keyrt algjörlega án nettengingar.


---

## 📸 Screenshots

| Organize | Clean up |
| --- | --- |
| ![Organize](../screenshots/organize.png) | ![Clean up](../screenshots/cleanup.png) |

## ✨ Tveir megineiginleikar

### 🗂️ Skráaskipulagning
Veldu möppu: gervigreindin skilur hverja skrá (horfir ekki bara á skráarnafnið) og stingur sjálfkrafa upp á flokkum og merkingarbærari skráarnöfnum:

- **Staðbundin gervigreindarlíkön** (Gemma 3 4B o.fl.): algjörlega ónettengd, ókeypis, engar skrár sendar — persónuvernd tryggð
- Einnig má nota ský-API eins og ChatGPT / Gemini (eigin lykill nauðsynlegur)
- **Forskoðun og staðfesting** áður en skipulagt er, með **æfingaham** (sjá niðurstöðuna án þess að snerta skrár) og **afturkalla**
- Býr sjálfgefið aðeins til eitt stig flokkamappa til að forðast möppusundrun

### 🧹 Skráahreinsun
Skannar möppu og telur fram fjórar tegundir skráa sem «gætu verið óþarfar»:

| Flokkur | Lýsing |
|------|------|
| Rusl- / skyndiminnisskrár | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` o.s.frv. |
| Tvíteknar skrár | Skrár með nákvæmlega sama innihald (hash-samanburður), umframafrit merkt |
| Stórar skrár | Skrár yfir 100 MB, raðað eftir stærð |
| Tómar möppur / núll-bæta skrár | Tómar möppur og skrár af stærðinni 0 |

**🛡 Öryggisloforð: þetta verkfæri telur aðeins og staðsetur — það eyðir aldrei né færir neitt.** Frá hverjum lið er hægt að hoppa með einum smelli beint í möppuna þar sem hann er, svo þú staðfestir sjálf(ur) og eyðir handvirkt.

---

## 📥 Niðurhal og uppsetning

### macOS (Apple Silicon)
1. Sæktu `.dmg` af [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)-síðunni
2. Opnaðu hana og dragðu forritið í «Forrit»-möppuna
3. Ef fyrsta opnun er stöðvuð: **hægrismelltu á forritið → Opna** (forritið er ekki vottað af Apple)

### Windows
- Sæktu `file-butler-windows`-skrána úr nýjustu vel heppnuðu byggingunni á [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)-síðunni

---

## 🚀 Þrjú skref til að byrja

1. **Settu upp líkan** (aðeins í fyrsta skipti): veldu staðbundið LLM (mælt með — ókeypis og ónettengt) og sæktu það, eða sláðu inn ChatGPT- / Gemini-API-lykil
2. **Skipuleggðu**: veldu «Skráaskipulagning» í hliðarstikunni → veldu möppu → smelltu á «Greina möppu» → staðfestu hvern flokk → framkvæmdu
3. **Hreinsaðu**: veldu «Skráahreinsun» í hliðarstikunni → veldu möppu → skannaðu → farðu yfir niðurstöðuhópana fjóra → smelltu til að fara á staðsetningu hverrar skráar og sjáðu um hana sjálf(ur)

Hægt er að skipta um viðmótstungumál (15 tungumál) hvenær sem er neðst til hægri; tungumál flokkanna fylgir sjálfkrafa viðmótstungumálinu.

---

## 🎨 Breytingar þessarar útgáfu miðað við upprunalega verkefnið

- Glæný vinstri hliðarstika (tvær síður: Skipulagning / Hreinsun) + ljóst hvítt þema
- Glænýtt forritstákn
- Ný skrifvarin «Skráahreinsun»-skönnun (ekki til í upprunalega verkefninu)
- Viðmótstungumál og úttakstungumál gervigreindarflokka sameinuð í eina stillingu, sjálfgefið einfölduð kínverska
- Flokkunarnákvæmni fínstillt: sjálfgefið eins stigs flokkamöppur, leiðbeiningar þvinga sameiningu í algenga yfirflokka
- Full staðfærsla á einfaldaða kínversku

## 🔧 Bygging frá grunnkóða

Flýtiútgáfa fyrir macOS:

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

