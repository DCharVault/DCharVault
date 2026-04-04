# DCharVault 

A cross-platform, end-to-end encrypted local journaling application focusing on secure memory management and native performance. Built with C++20, Qt6, and Libsodium.

## 🛡️ Security Architecture
DCharVault is designed to ensure zero plaintext leakage to disk or swap memory.
* **Key Derivation:** Argon2id (`crypto_pwhash`) with interactive limits.
* **Data Encryption:** XChaCha20-Poly1305 (AEAD) for authenticated encryption of all entries and metadata.
* **Memory Safety:** Implementation of a custom C++ `SecureAllocator` wrapping `sodium_malloc` and `sodium_free`. This ensures sensitive data (keys, passwords, raw text) is protected by memory guard pages and securely zeroed out before deallocation, preventing RAM scraping attacks.
* **Verification:** Master password verification via encrypted MAC blocks rather than storing key hashes.

## 🛠️ Tech Stack
* **Core:** C++20
* **GUI & Cross-Platform Framework:** Qt 6.8 (C++ Backend / QML Frontend)
* **Cryptography:** Libsodium
* **Database:** SQLite (Encrypted payloads only)

## CI/CD & Build Pipeline
Automated cross-compilation pipeline utilizing GitHub Actions:
* **Linux (Ubuntu):** Manual bundling, RPATH patching (`patchelf`), and dependency isolation.
* **Windows (MSVC):** Vcpkg integration and `windeployqt` packaging.
* **Android (ARM64):** Cross-compiled via Qt Android toolchain with automated Release APK keystore signing.

## ⚠️ Status
Currently in active development. Core cryptography and Qt/QML integration are functional.
