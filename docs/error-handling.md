---
id: error-handling
title: Error Handling
sidebar_position: 6
---

# Error Handling

DCharVault uses a typed `DiaryError` enum to communicate failure conditions from the Model layer up through the ViewModel and into the UI. This guide documents every error code and the recommended recovery strategy for each.

---

## The `DiaryError` Enum

`DiaryError` is returned by any Model-layer operation that can fail in a meaningful, recoverable way. It is defined in the core model headers and is the primary mechanism for communicating failures without throwing exceptions.

```cpp
enum class DiaryError {
    // Success
    Success,

    // Vault / Authentication Errors
    VaultAlreadyOpen,
    VaultNotOpen,
    AuthenticationFailed,
    InvalidMAC,
    DatabaseNotFound,
    DatabaseCorrupt,

    // Entry Operation Errors
    EntryNotFound,
    EncryptionFailed,
    DecryptionFailed,
    DatabaseWriteFailed,
    DatabaseReadFailed,

    // Session / Configuration Errors
    ConfigWriteFailed,
    ConfigReadFailed,

    // Memory / System Errors
    SecureMemoryError,
    UnknownError,
};
```

---

## Error Code Reference

### Vault & Authentication Errors

| Code | Meaning | Recommended Recovery |
|:---|:---|:---|
| `Success` | Operation completed without error | Continue normally |
| `VaultAlreadyOpen` | `openDiary` was called when a vault was already active | Call `lockVault()` first, then retry |
| `VaultNotOpen` | An operation was attempted on a closed/locked vault | Navigate user to the login screen |
| `AuthenticationFailed` | The master password did not produce a valid key | Prompt user to retry; do **not** log the attempt in plaintext |
| `InvalidMAC` | The verification MAC block failed authentication — database may be tampered with | Warn the user of potential data integrity issues; do not decrypt further |
| `DatabaseNotFound` | The specified `.db` file path does not exist | Emit `dbNotFound` signal; prompt user to select a valid vault file |
| `DatabaseCorrupt` | The SQLite database file is structurally invalid | Show a non-recoverable error; advise the user to restore from backup |

### Entry Operation Errors

| Code | Meaning | Recommended Recovery |
|:---|:---|:---|
| `EntryNotFound` | No entry exists for the given `id` | Refresh the entry list via `DiaryListModel::loadEntries()` |
| `EncryptionFailed` | Libsodium encryption operation returned an error | Retry once; if it persists, it may indicate a `SecureMemory` allocation failure |
| `DecryptionFailed` | Decryption or authentication of an entry failed | The entry may be corrupted; surface the error to the user without exposing partial plaintext |
| `DatabaseWriteFailed` | An `INSERT` or `UPDATE` SQL operation failed | Check available disk space; retry the operation |
| `DatabaseReadFailed` | A `SELECT` SQL operation returned an unexpected result | Refresh the entry list; if persistent, may indicate corruption |

### Session & Configuration Errors

| Code | Meaning | Recommended Recovery |
|:---|:---|:---|
| `ConfigWriteFailed` | Saving a configuration value (e.g., timeout) to the database failed | Show a non-blocking warning to the user; setting will revert to default on next launch |
| `ConfigReadFailed` | Reading a configuration value failed | Fall back to the default value silently |

### System Errors

| Code | Meaning | Recommended Recovery |
|:---|:---|:---|
| `SecureMemoryError` | `sodium_malloc` failed to allocate a guarded memory region | This is a critical system-level failure; lock the vault immediately and surface an error |
| `UnknownError` | An unclassified error occurred | Log the context and surface a generic error to the user |

---

## Handling `DiaryError` in ViewModels

ViewModels receive `DiaryError` values from the Model layer and translate them into Qt signals that the QML UI can react to. Here is the standard pattern:

```cpp
// In LoginViewModel::authenticate(...)
DiaryError result = m_diaryManager->openDiary(name, path, password);

switch (result) {
    case DiaryError::Success:
        emit loginSuccess();
        break;
    case DiaryError::AuthenticationFailed:
    case DiaryError::InvalidMAC:
        emit loginFailed();
        break;
    case DiaryError::DatabaseNotFound:
        emit dbNotFound();
        break;
    default:
        emit loginFailed(); // Treat unknown vault errors as auth failures
        break;
}
```

```cpp
// In DiaryViewModel::saveNewEntry(...)
DiaryError result = m_diaryManager->updateEntry(id, title, content);

if (result != DiaryError::Success) {
    emit entrySaveFailed(diaryErrorToString(result));
} else {
    emit entrySavedSuccessfully(id, title);
}
```

---

## Security Considerations

- **Never log plaintext passwords or decrypted content** alongside error messages, even in debug builds.
- **`InvalidMAC` is a security-critical error.** It may indicate tampering. Do not partially decrypt or expose any data when this error is returned.
- **`DecryptionFailed` entries should be surfaced to the user** rather than silently skipped — silent failures can mask data corruption.
- **`SecureMemoryError` is non-recoverable.** The vault must be locked immediately to prevent any sensitive data from remaining in unprotected heap memory.
