[简体中文](../../README.md) | [English](README.en.md) | [Français](README.fr.md) | [Deutsch](README.de.md) | **Español** | [Italiano](README.it.md) | [Nederlands](README.nl.md) | [한국어](README.ko.md) | [हिन्दी](README.hi.md) | [Türkçe](README.tr.md) | [Svenska](README.sv.md) | [Norsk](README.no.md) | [Dansk](README.da.md) | [Suomi](README.fi.md) | [Íslenska](README.is.md)

# File Butler (文件管家)

Deja que la IA organice tus carpetas desordenadas y encuentre los archivos que vale la pena limpiar. Compatible con **macOS y Windows**, puede funcionar completamente sin conexión.


---

## 📸 Screenshots

| Organize | Clean up |
| --- | --- |
| ![Organize](../screenshots/organize.png) | ![Clean up](../screenshots/cleanup.png) |

## ✨ Dos funciones principales

### 🗂️ Organización de archivos
Elige una carpeta: la IA comprende cada archivo (no solo su nombre) y sugiere automáticamente categorías y nombres de archivo más significativos:

- **Modelos de IA locales** (Gemma 3 4B, etc.): totalmente sin conexión, gratuitos, sin subir archivos — privacidad garantizada
- También se pueden usar API en la nube como ChatGPT / Gemini (con tu propia clave)
- **Vista previa y confirmación** antes de organizar, con **modo de simulación** (ver el resultado sin tocar los archivos) y **deshacer**
- Por defecto solo crea un nivel de carpetas de categorías, evitando la fragmentación

### 🧹 Limpieza de archivos
Escanea una carpeta y clasifica cuatro tipos de archivos «posiblemente innecesarios»:

| Categoría | Descripción |
|------|------|
| Archivos basura / caché | `.tmp` `.log` `.cache` `.DS_Store` `Thumbs.db`, etc. |
| Archivos duplicados | Archivos con contenido idéntico (comparación de hash), se marcan las copias sobrantes |
| Archivos grandes | Archivos de más de 100 MB, ordenados por tamaño |
| Carpetas vacías / archivos de cero bytes | Directorios vacíos y archivos de tamaño 0 |

**🛡 Compromiso de seguridad: esta herramienta solo cuenta y localiza — nunca elimina ni mueve nada.** Cada elemento permite saltar directamente a su carpeta, para que tú lo confirmes y lo elimines manualmente.

---

## 📥 Descarga e instalación

### macOS (Apple Silicon)
1. Descarga el `.dmg` desde la página de [Releases](https://github.com/echoyu1025-a11y/file-butler/releases)
2. Ábrelo y arrastra la aplicación a la carpeta «Aplicaciones»
3. Si se bloquea al abrirla por primera vez: **clic derecho en la aplicación → Abrir** (la aplicación no está notarizada por Apple)

### Windows
- Descarga el artefacto `file-butler-windows` de la última compilación exitosa en la página de [Actions](https://github.com/echoyu1025-a11y/file-butler/actions)
- O compílalo desde el código fuente siguiendo [docs/UPSTREAM_README.md](../UPSTREAM_README.md)

---

## 🚀 Tres pasos para usarlo

1. **Configura un modelo** (solo la primera vez): elige un LLM local (recomendado — gratuito y sin conexión) y descárgalo, o introduce una clave API de ChatGPT / Gemini
2. **Organiza**: elige «Organizar archivos» en la barra lateral → selecciona una carpeta → pulsa «Analizar carpeta» → confirma cada categoría → aplica
3. **Limpia**: elige «Limpiar archivos» en la barra lateral → selecciona una carpeta → escanea → revisa los cuatro grupos de resultados → haz clic para ir a la ubicación de cada archivo y decide tú mismo

Puedes cambiar el idioma de la interfaz (15 idiomas) en cualquier momento desde la esquina inferior derecha; el idioma de las categorías sigue automáticamente al de la interfaz.

---

## 🎨 Cambios de esta edición respecto al proyecto original

- Nuevo diseño con barra lateral izquierda (dos páginas: Organización / Limpieza) + tema claro blanco
- Nuevo icono de la aplicación
- Nueva función de «Limpieza de archivos» de solo lectura (no existe en el proyecto original)
- Idioma de la interfaz e idioma de salida de las categorías de IA unificados en un solo ajuste, por defecto chino simplificado
- Granularidad de categorías optimizada: carpetas de categorías de un solo nivel por defecto, los prompts fuerzan la agrupación en grandes categorías comunes
- Localización completa al chino simplificado

## 🔧 Compilar desde el código fuente

Los pasos completos de compilación para macOS / Windows / Linux están en [docs/UPSTREAM_README.md](../UPSTREAM_README.md). Versión rápida para macOS:

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

