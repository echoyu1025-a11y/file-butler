[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | [Español](README.es.md) | **Italiano** | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Lascia che l'IA metta in ordine le tue cartelle disordinate e trovi i file che vale la pena ripulire. Supporta **macOS e Windows** e può funzionare completamente offline.


---

## ✨ Due funzioni principali

### 🗂️ Organizzazione dei file
Scegli una cartella: l'IA comprende ogni file (non solo il nome) e suggerisce automaticamente categorie e nomi di file più significativi:

- **Modelli IA locali** (Gemma 3 4B, ecc.): completamente offline, gratuiti, nessun file caricato — privacy garantita
- Sono supportate anche le API cloud come ChatGPT / Gemini (con la propria chiave)
- **Anteprima e conferma** prima dell'organizzazione, con **modalità prova** (vedi il risultato senza toccare i file) e **annulla**
- Per impostazione predefinita crea un solo livello di cartelle di categoria, evitando la frammentazione

### 🧹 Pulizia dei file
Analizza una cartella e riporta quattro categorie di file «forse non necessari»:

| Categoria | Descrizione |
|------|------|
| File spazzatura / cache | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db`, ecc. |
| File duplicati | File con contenuto identico (confronto hash), le copie superflue vengono contrassegnate |
| File di grandi dimensioni | File oltre i 100 MB, ordinati per dimensione |
| Cartelle vuote / file da zero byte | Directory vuote e file di dimensione 0 |

**🛡 Impegno di sicurezza: questo strumento conta e localizza soltanto — non elimina né sposta mai nulla.** Da ogni voce puoi saltare direttamente alla cartella che la contiene, per confermare ed eliminare manualmente.

---

## 📥 Download e installazione

### macOS (Apple Silicon)
1. Scarica il `.dmg` dalla pagina [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)
2. Aprilo e trascina l'applicazione nella cartella «Applicazioni»
3. Se al primo avvio viene bloccata: **clic destro sull'app → Apri** (l'app non è notarizzata da Apple)

### Windows
- Scarica l'artefatto `file-butler-windows` dall'ultima build riuscita nella pagina [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)
- Oppure compila dai sorgenti seguendo [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 Tre passi per iniziare

1. **Configura un modello** (solo al primo avvio): scegli un LLM locale (consigliato — gratuito e offline) e scaricalo, oppure inserisci una chiave API ChatGPT / Gemini
2. **Organizza**: scegli «Organizza file» nella barra laterale → seleziona una cartella → premi «Analizza cartella» → conferma ogni categoria → applica
3. **Pulisci**: scegli «Pulisci file» nella barra laterale → seleziona una cartella → esegui la scansione → esamina i quattro gruppi di risultati → fai clic per aprire la posizione di ogni file e gestiscilo tu stesso

Puoi cambiare la lingua dell'interfaccia (15 lingue) in qualsiasi momento in basso a destra; la lingua delle categorie segue automaticamente quella dell'interfaccia.

---

## 🎨 Modifiche di questa edizione rispetto al progetto originale

- Nuovo layout con barra laterale sinistra (due pagine: Organizzazione / Pulizia) + tema chiaro bianco
- Nuova icona dell'applicazione
- Nuova funzione di «Pulizia file» in sola lettura (assente nel progetto originale)
- Lingua dell'interfaccia e lingua di output delle categorie IA unificate in un'unica impostazione, predefinita cinese semplificato
- Granularità delle categorie ottimizzata: cartelle di categoria a un solo livello per impostazione predefinita, i prompt forzano il raggruppamento nelle grandi categorie comuni
- Localizzazione completa in cinese semplificato

## 🔧 Compilazione dai sorgenti

I passaggi completi di compilazione per macOS / Windows / Linux sono in [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Versione rapida per macOS:

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

