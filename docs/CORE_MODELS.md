---
layout: default
title: Core Models APIs
---

# Core Models API Documentation

This document defines the core backend engine of DCharVault. These classes, located in the `/model` directory, are strictly responsible for data orchestration, cryptographic security, and SQLite persistence. They contain no UI logic.

---

## `DiaryManager`

The central orchestrator. It manages the lifecycle of the vault and coordinates data flow between the UI (ViewModels), the database, and the encryption pipeline.

### Vault Lifecycle

#### `openDiary`

```cpp
[[nodiscard]] DiaryError openDiary(
    const QString& journalName,
    const QString& path,
    const SecureString& password
);
```

Initializes the database and cryptography modules, derives the master key, and verifies the vault MAC. Returns a specific `DiaryError` enum on failure.

#### `isVaultOpened`

```cpp
bool isVaultOpened() const;
```

Returns `true` if a vault is currently unlocked and the master key is active in memory.

#### `setContentUri`

```cpp
void setContentUri(const QString& contentUri, const QString& localCachePath);
```

Tracks the original Android `content://` URI and the local sandbox cache path. Used to sync the modified database back to external storage during `lockVault`.

---

### Entry Operations (CRUD)

#### `createEntry`

```cpp
[[nodiscard]] int64_t createEntry(
    const QString& title,
    const QString& content
);
```

Encrypts and saves a new entry to the database. Returns the generated database ID, or `-1` on failure.

#### `updateEntry`

```cpp
[[nodiscard]] DiaryError updateEntry(
    int64_t id,
    const QString& title,
    const QString& content
);
```

Re-encrypts and updates an existing entry’s title and content.

#### `deleteEntry`

```cpp
[[nodiscard]] DiaryError deleteEntry(int64_t id);
```

Permanently deletes an entry from the database.

#### `lockVault`

```cpp
[[nodiscard]] DiaryError lockVault();
```

Securely wipes all memory containing sensitive vault data (such as the master key and decrypted entries) and effectively locks the active session.

### `saveSessionTimeout`

```cpp
[[nodiscard]] DiaryError saveSessionTimeout(uint32_t seconds);
```

Saves the specified session timeout value to the database.

### `loadSessionTimeout`

```cpp
uint32_t loadSessionTimeout() const;
```

Loads the currently configured session timeout value from the database.

### `saveClipboardTimeout`

```cpp
[[nodiscard]] DiaryError saveClipboardTimeout(uint32_t seconds);
```

Saves the specified clipboard sanitization timeout value to the database.

### `loadClipboardTimeout`

```cpp
uint32_t loadClipboardTimeout() const;
```

Loads the currently configured clipboard sanitization timeout value from the database.

---

### Data Retrieval

#### `readEntrySummaries`

```cpp
std::vector<DiaryEntrySummary> readEntrySummaries();
```

Decrypts and retrieves metadata (IDs, titles, timestamps) for all entries to populate the UI list.

#### `readEntryTitle`

```cpp
QString readEntryTitle(int64_t id);
```

Decrypts and retrieves the title of a specific entry.

#### `readEntryContent`

```cpp
QString readEntryContent(int64_t id);
```

Decrypts and retrieves the full body content of a specific entry.

#### `readEntry`

```cpp
const DiaryEntry* readEntry(int64_t id) const noexcept;
```

Returns a pointer to a cached or loaded `DiaryEntry` object.

---

## `EncryptionManager`

A stateless cryptographic wrapper over the `libsodium` library. It ensures all cryptographic operations are executed safely.

### `initialize`

```cpp
bool initialize();
```

Bootstraps the `libsodium` library. Must be called before any cryptographic operations.

### `deriveMasterKey`

```cpp
[[nodiscard]] SecureVector deriveMasterKey(
    const SecureString& password,
    const QByteArray& salt
);
```

Derives the master encryption key using the Argon2id algorithm. Returns a securely allocated vector.

### `encryptString`

```cpp
[[nodiscard]] QByteArray encryptString(
    const QString& inputString,
    const SecureVector& masterKey
);
```

Encrypts a plaintext string using XChaCha20-Poly1305 (AEAD).

### `decryptString`

```cpp
[[nodiscard]] QString decryptString(
    const QByteArray& inputBytes,
    const SecureVector& masterKey
);
```

Decrypts and authenticates a payload using XChaCha20-Poly1305.

### `generateSalt`

```cpp
[[nodiscard]] QByteArray generateSalt();
```

Generates a secure random salt for password hashing.

### `generateRandomBytes`

```cpp
[[nodiscard]] QByteArray generateRandomBytes(size_t length = 32);
```

Generates a buffer of cryptographically secure pseudo-random bytes.

---

## `DatabaseManager`

Handles all direct SQLite interactions. It only ever stores or retrieves encrypted byte arrays; it never handles plaintext data.

### `databaseInit`

```cpp
bool databaseInit(const QString& dbPath);
```

Initializes the SQLite database connection.

### `createTable`

```cpp
bool createTable();
```

Creates the necessary schemas (`entries`, `config`) if they do not already exist.

### `closeDatabase`

```cpp
void closeDatabase();
```

Securely closes the active database connection and removes the default connection from `QSqlDatabase`.

### `insertEntry`

```cpp
qint64 insertEntry(
    qint64 created_at,
    const QByteArray& encrypted_title,
    const QByteArray& encrypted_content
);
```

Inserts a new encrypted entry and returns its row ID.

### `updateEntry`

```cpp
bool updateEntry(
    qint64 id,
    qint64 updated_at,
    const QByteArray& encrypted_title,
    const QByteArray& encrypted_content
);
```

Updates the encrypted payload of an existing entry.

### `deleteEntry`

```cpp
bool deleteEntry(qint64 id);
```

Deletes a specific entry by its ID.

### `setConfigValue`

```cpp
bool setConfigValue(
    const QString& key,
    const QByteArray& value
);
```

Stores a configuration key-value pair (e.g., the vault salt or verification block).

### `setJournalName`

```cpp
bool setJournalName(const QString& newJournal_name);
```

Updates the name of the journal in the database.

### `setShareableStatus`

```cpp
bool setShareableStatus(const bool isShareable);
```

Sets the shareable status of the database.

### `setSpecialStatus`

```cpp
bool setSpecialStatus(const QString& status);
```

Sets a special status flag in the database.

### `getConfigValue`

```cpp
QByteArray getConfigValue(const QString& key) const;
```

Retrieves a configuration value by its key.

### `getJournalName`

```cpp
QString getJournalName() const;
```

Retrieves the name of the journal from the database.

### `getAllEntriesMetadata`

```cpp
std::vector<EntryMetadata> getAllEntriesMetadata();
```

Retrieves metadata (including encrypted titles) for all stored entries.

### `getEntryTitle`

```cpp
QByteArray getEntryTitle(int64_t id) const;
```

Retrieves the raw encrypted title payload for a specific entry.

### `getEntryContent`

```cpp
QByteArray getEntryContent(int64_t id) const;
```

Retrieves the raw encrypted content payload for a specific entry.

---

## `SessionManager`

Manages session states and inactivity timeouts. It tracks user activity and provides information to trigger lock events.

### `recordActivity`

```cpp
void recordActivity();
```

Updates the last activity timestamp to prevent the session from expiring.

### `lock` / `unlock`

```cpp
void lock();
void unlock();
```

Transitions the internal session state between `Locked` and `Active`.

### `setTimeoutSeconds`

```cpp
void setTimeoutSeconds(uint32_t seconds);
```

Updates the session inactivity timeout.

### `timeoutSeconds`

```cpp
uint32_t timeoutSeconds() const;
```

Retrieves the current inactivity timeout duration in seconds.

### `isSessionExpired`

```cpp
[[nodiscard]] bool isSessionExpired() const;
```

Checks if the time since the last activity exceeds the configured timeout.

### `state`

```cpp
SessionState state() const;
```

Returns the current state of the session (`Active`, `Locked`, or `LoggedOut`).

---

## `SecureAllocator`

A custom C++ STL-compatible memory allocator designed to handle sensitive data safely.

- **Mechanism:** Wraps `sodium_malloc` and `sodium_free`.
- **Security:** Uses OS guard pages to detect out-of-bounds access, pins memory in RAM to prevent disk swapping, and securely zeroes memory before deallocation.
- **Usage:** Powers the `SecureString` and `SecureVector` types used for passwords and cryptographic keys.

---

## Data Structures

### `DiaryEntrySummary`

Used for lightweight UI rendering in lists.

```cpp
struct DiaryEntrySummary {
    int64_t id;
    QString title;
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
};
```

---

### `EntryMetadata`

Raw database retrieval structure before full decryption.

```cpp
struct EntryMetadata {
    int64_t id;
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
    QByteArray encryptedTitle;
};
```

---

### `DiaryEntry`

Represents a fully decrypted, loaded entry in memory.

```cpp
struct DiaryEntry {
    int64_t id;
    std::string title;
    std::string content;
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
};
```
