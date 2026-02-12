#ifndef DIARYMANAGER_H
#define DIARYMANAGER_H

#include<vector>
#include<string>
#include <unordered_map>
#include"DiaryEntry.h"

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
    std::string id;    // CHANGED: string_view -> string (Safety over micro-optimization)
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
    const DiaryEntry* readEntry(const std::string& id) const noexcept;
    // CHANGED: Return std::string (ID) instead of pointer.
    // This is "Handle-based access" and is much safer for vectors.
    //[[nodiscard]] : if someone ignore return value error then warn them!!
    [[nodiscard]] std::string createEntry(const std::string& title, const std::string& content);
    [[nodiscard]] DiaryError updateEntry(const std::string& id, const std::string& title, const std::string& content);
    [[nodiscard]] DiaryError deleteEntry(const std::string& id);

private:
    // constructors functions
    [[nodiscard]] DiaryError loadFromDisk();

    // std::vector<DiaryEntrySummary> summaryCache;  still in disccuesion can sigthly increase Ram cosuption while minimizing cpu overhead
    std::vector<DiaryEntry> entries;
    std::unordered_map<std::string, size_t> idToIndex;

    // validity checkers
    bool isLoadedFromDisk() const noexcept;
    bool isValidId(const std::string& id) const noexcept;

    // helper setters
    [[nodiscard]] DiaryError setTitle(const std::string& id, const std::string& newTitle) noexcept;
    [[nodiscard]] DiaryError setContent(const std::string& id, const std::string& newContent) noexcept;

    // helper getters
    const std::string* getTitle(const std::string& id) const noexcept;
    const std::string* getContent(const std::string& id) const noexcept;

    // Helper to find non-const entry internally
    DiaryEntry* findEntryById(const std::string& id);
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
