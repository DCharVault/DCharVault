#ifndef DIARYMANAGER_H
#define DIARYMANAGER_H

#include"DiaryEntry.h"
#include"DatabaseManager.h"

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

    // Persistence
    SerializationFailed,
    DeserializationFailed,

    // Security
    EncryptionFailed,
    DecryptionFailed,
    IntegrityCheckFailed,
};

struct DiaryEntrySummary {
    int64_t id;
    std::string title; // CHANGED: string_view -> string
    int64_t createdAt;
    int64_t updatedAt;
    bool bookmarked;
};

class DiaryManager{
public:
    // app starts, but no diary is "open" yet
    DiaryManager() = default;
    
    // The worker function: Called later when the user interacts with the UI.
    [[nodiscard]] DiaryError openDiary(const std::string& path, const std::string& password);

    std::vector<DiaryEntrySummary> readEntrySummaries() const;
    // const std::vector<DiaryEntry>& readEntries() const noexcept;
    const DiaryEntry* readEntry(const int64_t id) const noexcept;
    // This is "Handle-based access" and is much safer for vectors.
    //[[nodiscard]] : if someone ignore return value error then warn them!!
    [[nodiscard]] int64_t createEntry(const std::string& title, const std::string& content);
    [[nodiscard]] DiaryError updateEntry(const int64_t id, const std::string& title, const std::string& content);
    [[nodiscard]] DiaryError deleteEntry(const int64_t id);

private:
    std::vector<uint8_t> masterKey;
    DatabaseManager dbManager;

    // constructors functions
    [[nodiscard]] DiaryError loadFromDisk();

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
