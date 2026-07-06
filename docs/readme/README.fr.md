[简体中文](../../README.md) | [English](README.en.md) | **Français** | [Deutsch](README.de.md) | [Español](README.es.md) | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Laissez l'IA organiser vos dossiers en désordre et repérer les fichiers qui méritent d'être nettoyés. Compatible **macOS et Windows**, fonctionne entièrement hors ligne.


---

## ✨ Deux fonctions principales

### 🗂️ Organisation de fichiers
Choisissez un dossier : l'IA comprend chaque fichier (pas seulement son nom) et propose automatiquement des catégories et des noms de fichiers plus parlants :

- **Modèles d'IA locaux** (Gemma 3 4B, etc.) : entièrement hors ligne, gratuits, aucun fichier envoyé — confidentialité garantie
- Les API cloud comme ChatGPT / Gemini sont aussi prises en charge (avec votre propre clé)
- **Aperçu et confirmation** avant l'organisation, avec un **mode simulation** (voir le résultat sans toucher aux fichiers) et une fonction **annuler**
- Par défaut, un seul niveau de dossiers de catégories est créé, pour éviter la fragmentation

### 🧹 Nettoyage de fichiers
Analyse un dossier et recense quatre catégories de fichiers « peut-être inutiles » :

| Catégorie | Description |
|------|------|
| Fichiers indésirables / cache | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db`, etc. |
| Fichiers en double | Fichiers au contenu strictement identique (comparaison de hachage), copies superflues signalées |
| Fichiers volumineux | Fichiers de plus de 100 Mo, triés par taille |
| Dossiers vides / fichiers de zéro octet | Répertoires vides et fichiers de taille nulle |

**🛡 Engagement de sécurité : cet outil ne fait que compter et localiser — il ne supprime ni ne déplace jamais rien.** Chaque élément permet d'ouvrir directement son dossier, pour que vous confirmiez et supprimiez vous-même.

---

## 📥 Téléchargement et installation

### macOS (Apple Silicon)
1. Téléchargez le `.dmg` depuis la page [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)
2. Ouvrez-le et glissez l'application dans le dossier « Applications »
3. Si le premier lancement est bloqué : **clic droit sur l'application → Ouvrir** (l'application n'est pas notariée par Apple)

### Windows
- Téléchargez l'artefact `file-butler-windows` depuis la dernière compilation réussie sur la page [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)
- Ou compilez depuis les sources en suivant [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 Trois étapes pour commencer

1. **Configurer un modèle** (au premier lancement uniquement) : choisissez un LLM local (recommandé — gratuit et hors ligne) et téléchargez-le, ou saisissez une clé API ChatGPT / Gemini
2. **Organiser** : choisissez « Organisation de fichiers » dans la barre latérale → sélectionnez un dossier → cliquez sur « Analyser le dossier » → confirmez chaque catégorie → appliquez
3. **Nettoyer** : choisissez « Nettoyage de fichiers » dans la barre latérale → sélectionnez un dossier → lancez l'analyse → parcourez les quatre groupes de résultats → cliquez pour ouvrir l'emplacement de chaque fichier et traitez-le vous-même

Vous pouvez changer la langue de l'interface (15 langues) à tout moment en bas à droite ; la langue des catégories suit automatiquement celle de l'interface.

---

## 🎨 Modifications de cette édition par rapport au projet d'origine

- Nouvelle disposition avec barre latérale gauche (deux pages : Organisation / Nettoyage) + thème clair blanc
- Nouvelle icône d'application
- Nouvelle fonction de « Nettoyage de fichiers » en lecture seule (absente du projet d'origine)
- Langue de l'interface et langue des catégories générées par l'IA fusionnées en un seul réglage, par défaut le chinois simplifié
- Granularité des catégories optimisée : dossiers de catégories à un seul niveau par défaut, les prompts forcent le regroupement dans de grandes catégories courantes
- Localisation complète en chinois simplifié

## 🔧 Compilation depuis les sources

Les étapes complètes de compilation pour macOS / Windows / Linux se trouvent dans [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Version rapide pour macOS :

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

