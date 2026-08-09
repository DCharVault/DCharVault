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

Persists whether the vault database file should be treated as shareable (e.g., accessible via Android `content://` URI sharing). When `true`, `lockVault` will sync the modified database back to the user-selected external location. Returns `false` if the config write fails.

### `setSpecialStatus`

```cpp
bool setSpecialStatus(const QString& status);
```

Sets an application-defined status string in the `config` table under the `special_status` key. This is a general-purpose persistence mechanism used to store flags such as "first_launch" or "pending_migration". Returns `false` if the write fails.

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

A custom C++ STL-compatible memory allocator that wraps Libsodium's guarded memory management. It is the foundation of the `SecureString` and `SecureVector` types used throughout the codebase for sensitive data.

### Security Properties

| Property | Mechanism |
|:---|:---|
| **Guard pages** | OS-level guard pages are placed before and after each allocation to detect out-of-bounds access |
| **Memory pinning** | Memory is locked in RAM via `mlock()` / `VirtualLock()`, preventing it from being swapped to the pagefile or disk |
| **Secure zeroing** | Before deallocation, memory is overwritten with zeros using `sodium_memzero()`, which is not optimised away by the compiler |
| **Canary values** | Libsodium places canary values adjacent to each allocation to detect overflow at free-time |

### Key Methods (STL Allocator Interface)

#### `allocate`

```cpp
T* allocate(std::size_t n);
```

Allocates `n` objects of type `T` using `sodium_malloc`. Throws `std::bad_alloc` if the allocation fails (e.g., system memory lock limit exceeded). This replaces the standard `::operator new`.

#### `deallocate`

```cpp
void deallocate(T* p, std::size_t n) noexcept;
```

Securely zeroes and frees the allocation using `sodium_free`. The zero-wipe is unconditional and compiler-resistant — it always runs even in optimised builds.

### Usage

`SecureAllocator` is not typically used directly. Instead, use the type aliases it powers:

```cpp
// A std::basic_string using SecureAllocator
using SecureString = std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;

// A std::vector using SecureAllocator
using SecureVector = std::vector<uint8_t, SecureAllocator<uint8_t>>;
```

:::warning
Do **not** mix `SecureAllocator`-backed types with standard allocator types without explicitly zeroing the data first. Passing a `SecureString` to a function that copies it into a `std::string` will silently place the sensitive data in an unprotected heap allocation.
:::

---

## Data Structures & Types

### `DiaryError` Enum

The primary error type returned by all fallible Model operations. See the [Error Handling guide](./error-handling) for the full reference and recovery strategies.

```cpp
enum class DiaryError {
    Success,
    VaultAlreadyOpen, VaultNotOpen,
    AuthenticationFailed, InvalidMAC,
    DatabaseNotFound, DatabaseCorrupt,
    EntryNotFound,
    EncryptionFailed, DecryptionFailed,
    DatabaseWriteFailed, DatabaseReadFailed,
    ConfigWriteFailed, ConfigReadFailed,
    SecureMemoryError, UnknownError,
};
```

---

### `SessionState` Enum

Represents the current lifecycle state of the user session. Returned by `SessionManager::state()`.

```cpp
enum class SessionState {
    Active,     // Vault is unlocked and the user is interacting with the app
    Locked,     // Vault is locked due to inactivity or an explicit lockNow() call
    LoggedOut,  // No vault is loaded; the app is at the login screen
};
```

| State | Description |
|:---|:---|
| `Active` | The master key is in memory. Entries can be read and written. |
| `Locked` | The session timed out or the user locked manually. The UI shows the re-authentication screen. |
| `LoggedOut` | No vault is open. `isVaultOpened()` returns `false`. |

---

### `SecureString`

```cpp
using SecureString = std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;
```

A `std::string`-compatible type backed by `SecureAllocator`. Used exclusively for storing master passwords in memory before they are passed to the `EncryptionManager`. Its contents are automatically zeroed on destruction.

---

### `SecureVector`

```cpp
using SecureVector = std::vector<uint8_t, SecureAllocator<uint8_t>>;
```

A `std::vector<uint8_t>`-compatible type backed by `SecureAllocator`. Used for derived master keys and intermediate cryptographic byte buffers. Its contents are automatically zeroed on destruction.

---

### `DiaryEntrySummary`

Used for lightweight UI rendering in lists. Only decrypted metadata — never the full entry content.

```cpp
struct DiaryEntrySummary {
    int64_t id;          // Unique database row ID
    QString title;       // Decrypted entry title
    int64_t createdAt;   // Unix timestamp (ms)
    int64_t updatedAt;   // Unix timestamp (ms)
    int64_t bookmarked;  // 0 = not bookmarked, 1 = bookmarked
};
```

---

### `EntryMetadata`

Raw database retrieval structure returned before full decryption. The title is still encrypted at this stage.

```cpp
struct EntryMetadata {
    int64_t id;
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
    QByteArray encryptedTitle;  // XChaCha20-Poly1305 ciphertext
};
```

---

### `DiaryEntry`

Represents a fully decrypted entry loaded into `SecureAllocator`-backed memory. This struct should only exist in memory for as long as it is actively needed.

```cpp
struct DiaryEntry {
    int64_t id;
    std::string title;    // Plaintext — consider using SecureString for long-lived instances
    std::string content;  // Plaintext — same caution applies
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
};
```
