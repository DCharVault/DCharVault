---
id: intro
title: Introduction
sidebar_position: 1
---

# DCharVault

**DCharVault** is an offline-first, highly secure diary and journaling application built on C++20 and Qt 6.8. It is designed so that your private entries never leave your device unencrypted — and are never accessible in plain text, even on disk.

## What Is DCharVault?

DCharVault was built as a personal project to explore the complexities of integrating low-level cryptographic memory management with a high-level cross-platform GUI framework.

At its core, it combines:

- **XChaCha20-Poly1305 (AEAD)** authenticated encryption for all diary data.
- **Argon2id** key derivation to securely transform a master password into an encryption key.
- A **custom `SecureAllocator`** that prevents sensitive data from being swapped to disk or scraped from RAM.
- A **zero-knowledge SQLite database** — only encrypted payloads are ever persisted.

## Target Platforms

| Platform | Status |
|:---|:---|
| 🖥️ Linux (Ubuntu/Debian) | ✅ Supported |
| 🪟 Windows | ✅ Supported |
| 🤖 Android (ARM64) | ✅ Supported (experimental UI) |

## Documentation Overview

| Section | Description |
|:---|:---|
| [Getting Started](./getting-started) | How to build DCharVault from source on all supported platforms |
| [Architecture](./ARCHITECTURE) | High-level MVVM design, data flow diagrams, and security guarantees |
| [Core Models API](./CORE_MODELS) | C++ backend — `DiaryManager`, `EncryptionManager`, `DatabaseManager`, and data types |
| [ViewModel API](./VIEW_MODELS) | QML-facing bridge layer — properties, signals, and invokable methods |
| [Error Handling](./error-handling) | The `DiaryError` enum, error codes, and recovery strategies |

## ⚠️ Disclaimer

DCharVault is an **architectural experiment and personal project**. It has not undergone a formal third-party security audit. It does not protect against kernel-level keyloggers or a fully compromised OS environment. Use it accordingly.
