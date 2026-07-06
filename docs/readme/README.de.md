[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | **Deutsch** | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Lass die KI deine unordentlichen Ordner aufräumen und Dateien finden, die sich zum Ausmisten lohnen. Unterstützt **macOS und Windows** und läuft komplett offline.


---

## ✨ Zwei Kernfunktionen

### 🗂️ Dateien organisieren
Wähle einen Ordner: Die KI versteht jede Datei (nicht nur den Dateinamen) und schlägt automatisch Kategorien und aussagekräftigere Dateinamen vor:

- **Lokale KI-Modelle** (Gemma 3 4B usw.): komplett offline, kostenlos, keine Dateien werden hochgeladen — Privatsphäre garantiert
- Cloud-APIs wie ChatGPT / Gemini werden ebenfalls unterstützt (eigener API-Key erforderlich)
- **Vorschau mit Bestätigung** vor dem Organisieren, inklusive **Probelauf-Modus** (Ergebnis ansehen, ohne Dateien anzufassen) und **Rückgängig**
- Standardmäßig wird nur eine Ebene von Kategorieordnern angelegt, um Ordner-Zersplitterung zu vermeiden

### 🧹 Dateien bereinigen
Durchsucht einen Ordner und listet vier Arten von „möglicherweise unnötigen" Dateien auf:

| Kategorie | Beschreibung |
|------|------|
| Müll- / Cache-Dateien | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db` und Ähnliches |
| Doppelte Dateien | Dateien mit identischem Inhalt (Hash-Vergleich), überflüssige Kopien werden markiert |
| Große Dateien | Dateien über 100 MB, nach Größe sortiert |
| Leere Ordner / Null-Byte-Dateien | Leere Verzeichnisse und Dateien mit Größe 0 |

**🛡 Sicherheitsversprechen: Dieses Werkzeug zählt und lokalisiert nur — es löscht oder verschiebt niemals etwas.** Von jedem Eintrag springst du mit einem Klick direkt in den zugehörigen Ordner und löschst selbst, nachdem du es geprüft hast.

---

## 📥 Download & Installation

### macOS (Apple Silicon)
1. Lade die `.dmg` von der [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)-Seite herunter
2. Öffne sie und ziehe die App in den Ordner „Programme"
3. Falls der erste Start blockiert wird: **Rechtsklick auf die App → Öffnen** (die App ist nicht von Apple notariell beglaubigt)

### Windows
- Lade das Artefakt `file-butler-windows` aus dem letzten erfolgreichen Build auf der [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)-Seite herunter
- Oder baue aus dem Quellcode gemäß [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 In drei Schritten loslegen

1. **Modell einrichten** (nur beim ersten Start): ein lokales LLM wählen (empfohlen — kostenlos und offline) und herunterladen, oder einen ChatGPT- / Gemini-API-Key eintragen
2. **Organisieren**: in der Seitenleiste „Dateien organisieren" wählen → Ordner auswählen → „Ordner analysieren" klicken → jede Kategorie bestätigen → ausführen
3. **Bereinigen**: in der Seitenleiste „Dateien bereinigen" wählen → Ordner auswählen → scannen → die vier Ergebnisgruppen durchsehen → per Klick zum Speicherort springen und selbst entscheiden

Die Oberflächensprache (15 Sprachen) lässt sich jederzeit unten rechts umschalten; die Sprache der Kategorien folgt automatisch der Oberflächensprache.

---

## 🎨 Änderungen dieser Ausgabe gegenüber dem Upstream

- Komplett neues Layout mit linker Seitenleiste (zwei Seiten: Organisieren / Bereinigen) + helles weißes Theme
- Neues App-Symbol
- Neue schreibgeschützte Funktion „Dateien bereinigen" (im Upstream nicht vorhanden)
- Oberflächensprache und Ausgabesprache der KI-Kategorien zu einer Einstellung zusammengeführt, Standard: vereinfachtes Chinesisch
- Optimierte Kategoriegranularität: standardmäßig einstufige Kategorieordner, Prompts erzwingen die Zusammenführung in gängige Oberkategorien
- Vollständige Lokalisierung in vereinfachtem Chinesisch

## 🔧 Aus dem Quellcode bauen

Die vollständigen Build-Schritte für macOS / Windows / Linux stehen in [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Schnellversion für macOS:

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

