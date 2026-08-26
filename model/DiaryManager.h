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
    int completionTotal;
    int completionCompleted;
};

class DiaryManager{
public:
    // app starts, but no diary is "open" yet
    DiaryManager() = default;
    
    // The worker function: Called later when the user interacts with the UI.
    [[nodiscard]] DiaryError openDiary(const QString& journalName, const QString& path, const SecureString& password);

    std::vector<DiaryEntrySummary> readEntrySummaries();
    QString readEntryContent(int64_t id);
    QString readEntryTitle(int64_t id);
    // const std::vector<DiaryEntry>& readEntries() const noexcept;
    const DiaryEntry* readEntry(const int64_t id) const noexcept;


    [[nodiscard]] int64_t createEntry(const QString& title, const QString& content);
    [[nodiscard]] DiaryError updateEntry(const int64_t id, const QString& title, const QString& content);
    [[nodiscard]] DiaryError deleteEntry(const int64_t id);

    [[nodiscard]] DiaryError lockVault();

    bool isVaultOpened() const;

    // Android: track original content:// URI for sync-back on lock
    void setContentUri(const QString& contentUri, const QString& localCachePath);

    [[nodiscard]] DiaryError saveSessionTimeout(uint32_t seconds);
    uint32_t loadSessionTimeout() const;
    [[nodiscard]] DiaryError saveClipboardTimeout(uint32_t seconds);
    uint32_t loadClipboardTimeout() const;


private:
    QString journal_name;
    QString m_contentUri;     // Android: original content:// URI for sync-back
    QString m_localCachePath; // Android: sandbox path we actually opened
    SecureVector masterKey;
    DatabaseManager dbManager;
    EncryptionManager encManager;

    // constructor functions
    [[nodiscard]] DiaryError loadFromDisk(const QString& path);
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

