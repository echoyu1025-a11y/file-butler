#!/usr/bin/env bash

set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: verify_macos_release.sh --bundle <path> --dmg <path> [--app-display-name <name>]

Checks that the macOS bundle and the app staged inside the DMG both match the
version declared in app/include/app_version.hpp and use the canonical app names.
USAGE
}

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION_FILE="$ROOT_DIR/app/include/app_version.hpp"
BUNDLE_PATH=""
DMG_PATH=""
APP_DISPLAY_NAME="AI File Sorter"
MOUNT_POINT=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --bundle)
      BUNDLE_PATH="${2:-}"
      shift 2
      ;;
    --dmg)
      DMG_PATH="${2:-}"
      shift 2
      ;;
    --app-display-name)
      APP_DISPLAY_NAME="${2:-}"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ -z "$BUNDLE_PATH" || -z "$DMG_PATH" ]]; then
  usage
  exit 1
fi

if [[ ! -f "$VERSION_FILE" ]]; then
  echo "Version file not found at $VERSION_FILE" >&2
  exit 1
fi

EXPECTED_VERSION="$(
  grep -E 'APP_VERSION|Version\{' "$VERSION_FILE" | /usr/bin/head -n 1 \
    | sed -nE 's/.*\{[[:space:]]*([0-9]+)[[:space:]]*,[[:space:]]*([0-9]+)[[:space:]]*,[[:space:]]*([0-9]+)[[:space:]]*\}.*/\1.\2.\3/p'
)"

if [[ -z "$EXPECTED_VERSION" ]]; then
  echo "Unable to parse version from $VERSION_FILE" >&2
  exit 1
fi

plist_value() {
  local plist="$1"
  local key="$2"
  /usr/libexec/PlistBuddy -c "Print :$key" "$plist"
}

verify_bundle() {
  local bundle="$1"
  local location="$2"
  local expected_bundle_name="$3"
  local plist="$bundle/Contents/Info.plist"

  if [[ ! -d "$bundle" ]]; then
    echo "$location bundle not found at $bundle" >&2
    exit 1
  fi
  if [[ "$(basename "$bundle")" != "$expected_bundle_name" ]]; then
    echo "$location bundle name mismatch: expected $expected_bundle_name, found $(basename "$bundle")" >&2
    exit 1
  fi
  if [[ ! -f "$plist" ]]; then
    echo "$location Info.plist not found at $plist" >&2
    exit 1
  fi

  local bundle_name bundle_display_name bundle_version short_version
  bundle_name="$(plist_value "$plist" CFBundleName)"
  bundle_display_name="$(plist_value "$plist" CFBundleDisplayName)"
  bundle_version="$(plist_value "$plist" CFBundleVersion)"
  short_version="$(plist_value "$plist" CFBundleShortVersionString)"

  if [[ "$bundle_name" != "AIFileSorter" ]]; then
    echo "$location CFBundleName mismatch: expected AIFileSorter, found $bundle_name" >&2
    exit 1
  fi
  if [[ "$bundle_display_name" != "$APP_DISPLAY_NAME" ]]; then
    echo "$location CFBundleDisplayName mismatch: expected $APP_DISPLAY_NAME, found $bundle_display_name" >&2
    exit 1
  fi
  if [[ "$bundle_version" != "$EXPECTED_VERSION" ]]; then
    echo "$location CFBundleVersion mismatch: expected $EXPECTED_VERSION, found $bundle_version" >&2
    exit 1
  fi
  if [[ "$short_version" != "$EXPECTED_VERSION" ]]; then
    echo "$location CFBundleShortVersionString mismatch: expected $EXPECTED_VERSION, found $short_version" >&2
    exit 1
  fi
}

cleanup_mount() {
  if [[ -n "$MOUNT_POINT" && -d "$MOUNT_POINT" ]]; then
    hdiutil detach "$MOUNT_POINT" >/dev/null 2>&1 || true
    rm -rf "$MOUNT_POINT"
  fi
}

trap cleanup_mount EXIT

verify_bundle "$BUNDLE_PATH" "Bundle" "AIFileSorter.app"

if [[ ! -f "$DMG_PATH" ]]; then
  echo "DMG not found at $DMG_PATH" >&2
  exit 1
fi

MOUNT_POINT="$(mktemp -d)"
hdiutil attach -nobrowse -readonly -mountpoint "$MOUNT_POINT" "$DMG_PATH" >/dev/null
verify_bundle "$MOUNT_POINT/${APP_DISPLAY_NAME}.app" "DMG" "${APP_DISPLAY_NAME}.app"

echo "Verified macOS release bundle and DMG against version $EXPECTED_VERSION."
