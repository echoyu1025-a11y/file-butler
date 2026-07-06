#!/bin/bash
set -e

# Resolve script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LLAMA_DIR="$SCRIPT_DIR/../include/external/llama.cpp"

if [ ! -d "$LLAMA_DIR" ]; then
    echo "Missing llama.cpp submodule. Please run:"
    echo "  git submodule update --init --recursive"
    exit 1
fi

PRECOMPILED_ROOT_DIR="$SCRIPT_DIR/../lib/precompiled"
HEADERS_DIR="$SCRIPT_DIR/../include/llama"

# Parse optional arguments (cuda=on/off, vulkan=on/off, blas=on/off/auto).
# Accept both bare key=value and GNU-style --key=value forms.
CUDASWITCH="OFF"
VULKANSWITCH="OFF"
BLASSWITCH="AUTO"
for arg in "$@"; do
    normalized_arg="${arg#--}"
    case "${normalized_arg,,}" in
        cuda=on) CUDASWITCH="ON" ;;
        cuda=off) CUDASWITCH="OFF" ;;
        vulkan=on) VULKANSWITCH="ON" ;;
        vulkan=off) VULKANSWITCH="OFF" ;;
        blas=on) BLASSWITCH="ON" ;;
        blas=off) BLASSWITCH="OFF" ;;
        blas=auto) BLASSWITCH="AUTO" ;;
    esac
done

if [[ "$CUDASWITCH" == "ON" && "$VULKANSWITCH" == "ON" ]]; then
    echo "Cannot enable both CUDA and Vulkan simultaneously. Choose one backend."
    exit 1
fi

echo "CUDA support: $CUDASWITCH"
echo "VULKAN support: $VULKANSWITCH"
echo "BLAS support: $BLASSWITCH (auto prefers OpenBLAS for CPU baseline)"

# Resolve OpenBLAS availability for both AUTO and explicit BLAS=ON requests.
openblas_available() {
    if command -v pkg-config >/dev/null 2>&1; then
        if pkg-config --exists openblas64 || pkg-config --exists openblas; then
            return 0
        fi
    fi

    local lib_candidate=""
    local header_candidate=""

    for candidate in \
        /usr/lib/libopenblas.so \
        /usr/lib64/libopenblas.so \
        /usr/lib64/openblas/libopenblas.so \
        /usr/lib/x86_64-linux-gnu/libopenblas.so \
        /usr/lib/x86_64-linux-gnu/openblas-pthread/libopenblas.so \
        /usr/lib/aarch64-linux-gnu/libopenblas.so \
        /usr/lib/aarch64-linux-gnu/openblas-pthread/libopenblas.so; do
        if [ -e "$candidate" ]; then
            lib_candidate="$candidate"
            break
        fi
    done

    for candidate in \
        /usr/include/cblas.h \
        /usr/include/openblas/cblas.h \
        /usr/include/openblas-pthread/cblas.h \
        /usr/include/x86_64-linux-gnu/cblas.h \
        /usr/include/x86_64-linux-gnu/openblas/cblas.h \
        /usr/include/x86_64-linux-gnu/openblas-pthread/cblas.h \
        /usr/include/aarch64-linux-gnu/cblas.h \
        /usr/include/aarch64-linux-gnu/openblas/cblas.h \
        /usr/include/aarch64-linux-gnu/openblas-pthread/cblas.h; do
        if [ -e "$candidate" ]; then
            header_candidate="$candidate"
            break
        fi
    done

    [[ -n "$lib_candidate" && -n "$header_candidate" ]]
}

resolve_blas_setting() {
    local requested="$1"
    if [[ "$requested" == "OFF" ]]; then
        echo "OFF"
        return 0
    fi
    if openblas_available; then
        echo "ON"
        return 0
    fi
    echo "OFF"
}

resolve_cuda_host_compiler() {
    local candidate=""
    local version=""
    local major=""

    for candidate in /usr/bin/g++-15 /usr/bin/g++-14 /usr/bin/g++-13 /usr/bin/g++-12 /usr/bin/g++-11 /usr/bin/g++-10 /usr/bin/g++; do
        [ -x "$candidate" ] || continue
        version="$("$candidate" -dumpfullversion -dumpversion 2>/dev/null || true)"
        major="${version%%.*}"
        if [[ "$major" =~ ^[0-9]+$ ]] && (( major >= 6 && major <= 15 )); then
            echo "$candidate"
            return 0
        fi
    done

    echo ""
}

resolve_cuda_compiler() {
    local candidate=""

    if [[ -n "${CUDACXX:-}" && -x "${CUDACXX}" ]]; then
        echo "${CUDACXX}"
        return 0
    fi

    if candidate="$(command -v nvcc 2>/dev/null)" && [[ -n "$candidate" && -x "$candidate" ]]; then
        echo "$candidate"
        return 0
    fi

    for candidate in /usr/local/cuda/bin/nvcc /usr/local/cuda-*/bin/nvcc; do
        if [[ -x "$candidate" ]]; then
            echo "$candidate"
            return 0
        fi
    done

    echo ""
}

normalize_shared_library_rpaths() {
    if ! command -v patchelf >/dev/null 2>&1; then
        echo "Warning: patchelf not found; skipping RUNPATH fix for llama libraries."
        return 0
    fi

    local target_dir=""
    local lib=""
    for target_dir in "$@"; do
        [[ -d "$target_dir" ]] || continue
        while IFS= read -r -d '' lib; do
            patchelf --set-rpath '$ORIGIN' "$lib" || true
        done < <(find "$target_dir" -maxdepth 1 -type f -name '*.so*' -print0)
    done
}

build_variant() {
    local variant="$1"
    local cuda_flag="$2"
    local vulkan_flag="$3"
    local blas_flag="$4"
    local runtime_subdir="$5"
    local cuda_host_compiler="$6"
    local cuda_compiler="$7"

    local build_dir="$LLAMA_DIR/build-$variant"
    rm -rf "$build_dir"
    mkdir -p "$build_dir"

    echo "Building variant '$variant' (CUDA=$cuda_flag, VULKAN=$vulkan_flag, BLAS=$blas_flag)..."

    cd "$LLAMA_DIR"

    local cmake_args=(
        -DGGML_CUDA="$cuda_flag"
        -DGGML_VULKAN="$vulkan_flag"
        -DGGML_OPENCL=OFF
        -DGGML_BLAS="$blas_flag"
        -DBUILD_SHARED_LIBS=ON
        -DGGML_NATIVE=OFF
        -DCMAKE_C_FLAGS="-mavx2 -mfma"
        -DCMAKE_CXX_FLAGS="-mavx2 -mfma"
        -S .
        -B "$build_dir"
    )

    if [[ "$blas_flag" == "ON" ]]; then
        cmake_args+=( -DGGML_BLAS_VENDOR=OpenBLAS )
    fi
    if [[ "$cuda_flag" == "ON" ]]; then
        cmake_args+=(
            -DCMAKE_CUDA_COMPILER="$cuda_compiler"
            -DCMAKE_CUDA_HOST_COMPILER="$cuda_host_compiler"
        )
    fi

    cmake "${cmake_args[@]}"
    cmake --build "$build_dir" --config Release -- -j"$(nproc)"

    local variant_root="$PRECOMPILED_ROOT_DIR/$variant"
    local variant_bin="$variant_root/bin"
    local variant_lib="$variant_root/lib"
    local ggml_runtime_root="$SCRIPT_DIR/../lib/ggml"
    local runtime_dir="$ggml_runtime_root/$runtime_subdir"

    rm -rf "$variant_bin" "$variant_lib" "$runtime_dir"
    mkdir -p "$variant_bin" "$variant_lib" "$runtime_dir"

    shopt -s nullglob
    for so in "$build_dir"/bin/*.so "$build_dir"/bin/*.so.*; do
        cp -P "$so" "$variant_bin/"
        cp -P "$so" "$runtime_dir/"
    done
    for lib in "$build_dir"/lib/*.a; do
        cp "$lib" "$variant_lib/"
    done
    for so in "$build_dir"/lib/*.so "$build_dir"/lib/*.so.*; do
        cp -P "$so" "$variant_lib/"
        cp -P "$so" "$runtime_dir/"
    done
    shopt -u nullglob

    normalize_shared_library_rpaths "$variant_bin" "$variant_lib" "$runtime_dir"

    cd "$SCRIPT_DIR"
}

# Determine BLAS setting (AUTO falls back to OFF if OpenBLAS is missing)
RESOLVED_BLAS="$(resolve_blas_setting "$BLASSWITCH")"
if [[ "$BLASSWITCH" == "ON" && "$RESOLVED_BLAS" == "OFF" ]]; then
    echo "Requested BLAS=ON but OpenBLAS development files were not found." >&2
    echo "Install the OpenBLAS development package (for example: Fedora/RHEL 'openblas-devel', Debian/Ubuntu 'libopenblas-dev'), or rerun with blas=off." >&2
    exit 1
fi
if [[ "$RESOLVED_BLAS" == "OFF" && "$BLASSWITCH" == "AUTO" ]]; then
    echo "OpenBLAS development files not detected; building CPU baseline without BLAS. Install the OpenBLAS development package and rerun for BLAS acceleration."
fi

# Always build a CPU baseline (OpenBLAS when available)
CUDA_HOST_COMPILER=""
CUDA_COMPILER=""
if [[ "$CUDASWITCH" == "ON" ]]; then
    CUDA_COMPILER="$(resolve_cuda_compiler)"
    if [[ -z "$CUDA_COMPILER" ]]; then
        echo "CUDA requested but no nvcc compiler was found." >&2
        echo "Ensure the full CUDA Toolkit is installed and either set CUDACXX=/full/path/to/nvcc or add nvcc to PATH." >&2
        exit 1
    fi
    CUDA_HOST_COMPILER="$(resolve_cuda_host_compiler)"
    if [[ -z "$CUDA_HOST_COMPILER" ]]; then
        echo "CUDA requested but no supported g++ host compiler was found." >&2
        echo "Install a CUDA-supported g++ version (for example g++-15, g++-14, or g++-13 depending on your CUDA release) and retry." >&2
        exit 1
    fi
    echo "CUDA compiler: $CUDA_COMPILER"
    echo "CUDA host compiler: $CUDA_HOST_COMPILER"
fi

build_variant "cpu" "OFF" "OFF" "$RESOLVED_BLAS" "wocuda" "$CUDA_HOST_COMPILER" "$CUDA_COMPILER"

# Build requested accelerator variant if applicable
REQUESTED_VARIANT="cpu"
REQUESTED_RUNTIME="wocuda"
if [[ "$CUDASWITCH" == "ON" ]]; then
    REQUESTED_VARIANT="cuda"
    REQUESTED_RUNTIME="wcuda"
elif [[ "$VULKANSWITCH" == "ON" ]]; then
    REQUESTED_VARIANT="vulkan"
    REQUESTED_RUNTIME="wvulkan"
fi

if [[ "$REQUESTED_VARIANT" != "cpu" ]]; then
    build_variant "$REQUESTED_VARIANT" "$CUDASWITCH" "$VULKANSWITCH" "$RESOLVED_BLAS" "$REQUESTED_RUNTIME" "$CUDA_HOST_COMPILER" "$CUDA_COMPILER"
fi

# Copy headers once (from the source tree)
rm -rf "$HEADERS_DIR" && mkdir -p "$HEADERS_DIR"
cp "$LLAMA_DIR/include/llama.h" "$HEADERS_DIR"
cp "$LLAMA_DIR"/ggml/src/*.h "$HEADERS_DIR"
cp "$LLAMA_DIR"/ggml/include/*.h "$HEADERS_DIR"

# Clean up build directories
rm -rf "$LLAMA_DIR"/build-*
