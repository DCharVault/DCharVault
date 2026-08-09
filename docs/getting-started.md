---
id: getting-started
title: Getting Started
sidebar_position: 2
---

# Getting Started

This guide covers how to build DCharVault from source on all supported platforms. The project uses CMake as its build system and vcpkg for dependency management on Windows.

## Prerequisites

You will need the following tools regardless of platform:

| Tool | Minimum Version | Notes |
|:---|:---|:---|
| **CMake** | 3.16+ | [cmake.org](https://cmake.org/download/) |
| **C++ Compiler** | C++20 support | GCC 10+, Clang 12+, or MSVC 2022+ |
| **Qt** | 6.8 | Components: `Quick`, `Sql`, `QuickControls2`, `QuickDialogs2` |
| **Libsodium** | Latest stable | [libsodium.org](https://libsodium.org) |
| **Git** | Any recent | For cloning |

---

## Cloning the Repository

```bash
git clone https://github.com/DCharVault/DCharVault.git
cd DCharVault
```

---

## 🐧 Linux (Ubuntu / Debian)

### 1. Install Dependencies

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config libsodium-dev
```

Install Qt 6.8 via the [Qt Maintenance Tool](https://www.qt.io/download-qt-installer) (recommended), or from `apt` if your distro ships a recent enough version:

```bash
# Ubuntu 24.04+ may have Qt 6.x in apt
sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev
```

### 2. Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 3. Run

```bash
./DCharVault
```

>TIP: RPATH / Bundling
>If you plan to distribute the binary, the CI pipeline uses `patchelf` to fix shared library RPATHs. Run the same steps as the [GitHub Actions Linux workflow](https://github.com/DCharVault/DCharVault/blob/main/.github/workflows/build.yml).


---

## 🪟 Windows (MSVC + vcpkg)

### 1. Install Libsodium via vcpkg

```cmd
vcpkg install libsodium:x64-windows
```

### 2. Set the Environment Variable

Set `SODIUM_ROOT_ENV` to your vcpkg installed directory, for example:

```cmd
set SODIUM_ROOT_ENV=C:\vcpkg\installed\x64-windows
```

Alternatively, add it as a permanent system environment variable in **System Properties → Advanced → Environment Variables**.

### 3. Build with CMake

```cmd
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

### 4. Package (Optional)

Use `windeployqt` to bundle the required Qt DLLs alongside the executable for distribution:

```cmd
windeployqt --release DCharVault.exe
```

---

## 🤖 Android (ARM64)

Building for Android requires additional toolchain components. Follow these steps in order.

### 1. Install Prerequisites

| Tool | Notes |
|:---|:---|
| **Android Studio** | Includes the SDK Manager |
| **Android NDK** | r25c or later (install via SDK Manager) |
| **Android SDK** | API level 26+ (Android 8+) |
| **Qt for Android** | Install the `android_arm64_v8a` target via the Qt Maintenance Tool |

### 2. Cross-Compile Libsodium for Android

Libsodium must be compiled for each Android ABI you intend to support. The official Libsodium repository includes a helper script:

```bash
# From the libsodium source directory
dist-build/android-arm64.sh
```

This produces a compiled library tree. Note the output path.

### 3. Set the Environment Variable

```bash
export SODIUM_ANDROID_ROOT_ENV=/path/to/compiled-libsodium-android-root
```

### 4. Configure and Build

Use the Qt Android CMake toolchain file:

```bash
mkdir build-android && cd build-android
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DQT_ANDROID_BUILD_ALL_ABIS=OFF \
  -DCMAKE_BUILD_TYPE=Release

cmake --build . --target apk
```

### 5. Signing the APK

For release builds, you need a keystore. The CI pipeline uses a secrets-backed keystore for signing:

```bash
# Sign with apksigner (from Android SDK build-tools)
apksigner sign \
  --ks my-release-key.jks \
  --ks-key-alias my-key-alias \
  --out DCharVault-release-signed.apk \
  DCharVault.apk
```

:::caution
The Android UI is currently in an **experimental phase**. Some UI elements may behave differently compared to desktop. The custom virtual keyboard is required on Android to prevent external keyboard keylogging.
:::

---

## CI/CD Pipeline

DCharVault features an automated cross-compilation pipeline using **GitHub Actions**. Each push to `main` triggers:

| Platform | Packaging | Notes |
|:---|:---|:---|
| 🐧 Linux | Manual RPATH patching with `patchelf` | Dependency isolation, produces a portable tarball |
| 🪟 Windows | `windeployqt` packaging | Vcpkg integration, produces a zip archive |
| 🤖 Android ARM64 | APK signing | Qt Android toolchain, automated Release keystore signing |

See the [`.github/workflows`](https://github.com/DCharVault/DCharVault/tree/main/.github/workflows) directory for the full pipeline definitions.
