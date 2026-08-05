---
layout: default
title: Architecture
---
# DCharVault Architecture Overview

## Introduction

DCharVault is an offline-first, highly secure diary and journaling application. It is built using a modern C++20 backend handling business logic and cryptography, and a Qt 6.8 QML frontend for a cross-platform user interface.

The application follows the **Model-View-ViewModel (MVVM)** architectural pattern, which cleanly separates the core security and data handling logic (Model) from the UI presentation (View), with the ViewModel acting as the intermediary.

<details>
<summary><strong>Fallback for GitHub Pages (optional, dependency-free)</strong></summary>

If your GitHub Pages theme/build does not render Mermaid, use this static fallback image:

![Architecture Diagram Fallback](assets/architecture-diagram-fallback.svg)

</details>

```mermaid
graph TD
    subgraph View Layer [QML UI]
        UI[Main.qml / Views]
    end
    subgraph ViewModel Layer [C++ QObject]
        LVM[LoginViewModel]
        DVM[DiaryViewModel]
        DLM[DiaryListModel]
        SVM[SessionViewModel]
    end
    subgraph Model Layer [C++ Core Backend]
        DM[DiaryManager]
        EM[EncryptionManager]
        DBM[DatabaseManager]
        SA[SecureAllocator]
        SM[SessionManager]
    end

    UI <-->|Property Bindings & Signals| LVM
    UI <-->|Property Bindings & Signals| DVM
    UI <-->|Property Bindings & Signals| DLM
    UI <-->|Property Bindings & Signals| SVM

    LVM -->|Auth Commands| DM
    DVM -->|CRUD Commands| DM
    DLM -->|Fetch Metadata| DM
    SVM -->|Lock Commands| DM
    SVM -->|State| SM

    DM <-->|Payload & Keys| EM
    DM <-->|Encrypted SQLite| DBM
    EM -.->|Memory Locks| SA
```

## Core Architecture (MVVM)

### 1. The Model Layer (`/model`)

The Model layer is responsible for the core business logic, cryptography, secure memory management, and data persistence. It operates independently of the UI framework.

**Key Components:**
* **`DiaryManager`**: The central orchestrator for the application's logic. It coordinates between the database, encryption system, and models. It handles operations like opening the diary, and creating, reading, updating, and deleting entries.
* **`EncryptionManager`**: Wraps Libsodium to provide cryptographic operations. It handles master key derivation (Argon2id), data encryption/decryption (XChaCha20-Poly1305), and salt generation.
* **`DatabaseManager`**: Manages the local SQLite database. It is responsible for storing and retrieving encrypted payloads, and handling database schemas and metadata.
* **`SecureAllocator`**: A custom C++ allocator wrapping Libsodium's memory management (`sodium_malloc` / `sodium_free`) to prevent sensitive data (like keys or plaintext passwords) from being swapped to disk or scraped from RAM.
* **`SessionManager`**: Tracks user activity and manages the application lock state with inactivity timeouts.
* **`DiaryEntry` & `EntryMetadata`**: Data structures representing the state and metadata of diary entries.

### 2. The ViewModel Layer (`/viewmodel`)

The ViewModel layer serves as the bridge between the C++ Model and the QML frontend. Classes in this layer inherit from `QObject` and expose properties, signals, and invocable methods to the UI.

**Key Components:**
* **`LoginViewModel`**: Handles the authentication flow, including vault creation and login. It securely passes passwords to the model and updates the UI state based on authentication success or failure.
* **`DiaryViewModel`**: Manages the state and operations for individual diary entries (reading, editing, deleting, saving).
* **`DiaryListModel`**: Inherits from `QAbstractListModel` to provide a model for QML list views (like a list of recent entries). It interacts with `DiaryManager` to load and display entry metadata.
*   **`SessionViewModel`**: Coordinates auto-locking and application session states, interfacing with `SessionManager` and `DiaryManager`.
*   **`SecurePasswordInput`**: A specialized component for handling password input securely within the UI before it reaches the backend.
*   **`ClipboardSanitizer`**: A utility to monitor the system clipboard and securely wipe it after a timeout to prevent sensitive data leakage.
*   **`RichTextController`**: A utility component that provides advanced rich text editing capabilities (bold, italic, underline, strikethrough, headers, lists, blockquotes, and highlighting) for the currently active QML TextArea/TextEdit.

### 3. The View Layer (`/ui` & `Main.qml`)

The View layer consists of QML files that define the user interface. They bind to the properties and methods exposed by the ViewModels. The UI does not directly interact with the Model layer; all interactions go through the ViewModels.

**Key UI Components & Features:**
*   **Modular Settings Overlay**: A tabbed configuration interface separating concerns like Appearance and Security.
*   **Custom Virtual Keyboard (Android)**: A securely integrated `virtualKeyboardPad` tied to `VaultPasswordField` and `SecurePasswordInput` hooks to prevent external keyboard keylogging.

## Data Flow Example: Creating a New Entry

1. **User Input:** The user types a title and content into the UI and clicks "Save".
2. **ViewModel Interaction:** The QML view calls `saveNewEntry()` on the `DiaryViewModel`.
3. **Model Execution:** The `DiaryViewModel` passes the plaintext data to `DiaryManager`.
4. **Processing & Encryption:** `DiaryManager` checks the input. If the title is empty, it securely strips HTML from the content and auto-generates a title. It then passes both the title and content to `EncryptionManager`, which encrypts them using XChaCha20-Poly1305 with the derived master key.
5. **Persistence:** `DiaryManager` sends the encrypted payload to `DatabaseManager`, which saves it to the SQLite database.
6. **UI Update:** The model notifies the `DiaryViewModel` of success, which emits a signal to update the UI.

## Security Guarantees

* **Zero-Knowledge:** The database only stores encrypted data and salts. The master password is never stored or logged.
* **Memory Protection:** Sensitive data (passwords, keys) is managed by `SecureAllocator` to prevent memory scraping.
* **Authenticated Encryption:** Data integrity is guaranteed via XChaCha20-Poly1305.
