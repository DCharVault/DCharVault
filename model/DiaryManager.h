#ifndef DIARYMANAGER_H
#define DIARYMANAGER_H

#include"SecureAllocator.h"
#include"DiaryEntry.h"
#include"DatabaseManager.h"
#include"EncryptionManager.h"

#include<vector>
#include<string>
#include <unordered_map>


enum class DiaryError{
    None,

    // Validation
    EmptyId,
    EmptyTitle,
    EmptyContent,
    InvalidId,
    InvalidDateTime,

    // Generation
    IdGenerationFailed,
    DateTimeGenerationFailed,

    // State
    EntryNotFound,
    EntryAlreadyExists,
    MasterKeyNotFound,

    // Persistence
    SerializationFailed,
    DeserializationFailed,

    // Security
    AuthenticationFailed,
    CryptoError,
    EncryptionFailed,
    DecryptionFailed,
    IntegrityCheckFailed,

    // Database
    DatabaseOpenError,
    DatabaseError,
};

struct DiaryEntrySummary {
    int64_t id;
    QString title;
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
};

class DiaryManager{
public:
    // app starts, but no diary is "open" yet
    DiaryManager() = default;
    
    // The worker function: Called later when the user interacts with the UI.
    [[nodiscard]] DiaryError openDiary(const QString& path, const SecureString& password);

    std::vector<DiaryEntrySummary> readEntrySummaries();
    QString readEntryContent(int64_t id);
    QString readEntryTitle(int64_t id);
    // const std::vector<DiaryEntry>& readEntries() const noexcept;
    const DiaryEntry* readEntry(const int64_t id) const noexcept;
    // This is "Handle-based access" and is much safer for vectors.
    //[[nodiscard]] : if someone ignore return value error then warn them!!
    [[nodiscard]] int64_t createEntry(const QString& title, const QString& content);
    [[nodiscard]] DiaryError updateEntry(const int64_t id, const QString& title, const QString& content);
    [[nodiscard]] DiaryError deleteEntry(const int64_t id);

    bool isVaultOpened() const;

private:
    QString journal_name; 
    SecureVector masterKey;
    DatabaseManager dbManager;
    EncryptionManager encManager;

    // constructor functions
    [[nodiscard]] DiaryError loadFromDisk();
    std::vector<DiaryEntrySummary> loadAllMetadata();

    // std::vector<DiaryEntrySummary> summaryCache;  still in disccuesion can sigthly increase Ram cosuption while minimizing cpu overhead
    std::vector<DiaryEntry> entries;
    std::unordered_map<int64_t, size_t> idToIndex;

    // validity checkers
    bool isLoadedFromDisk() const noexcept;
    bool isValidId(const int64_t id) const noexcept;

    // Helper to find non-const entry internally
    DiaryEntry* findEntryById(const int64_t id);
};

#endif


/*
┌───────────────┐
│ DiaryManager  │   owns entries
│               │
│  vector<DiaryEntry>
│               │
│  ├── readEntry(id)        -> full entry
│  ├── create/update/delete
│  └── readEntrySummaries() -> UI list
└───────────────┘

*/
