#ifndef DIARYMANAGER_H
#define DIARYMANAGER_H

#include<vector>
#include<string>
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
    std::string_view id;
    std::string_view title;
    int64_t createdAt;
    int64_t updatedAt;
    bool bookmarked;
};

class DiaryManager{
    public:
        const std::vector<DiaryEntry>& readEntries() const;
        const DiaryEntry* readEntry(const std::string& id) const;
        std::string createEntry(const std::string& title, const std::string& content); // returns entry id 
        bool updateEntry(const std::string& id, const std::string& title, const std::string& content);
        bool deleteEntry(const std::string& id);
    
    private:
        std::vector<DiaryEntry> entries;
        std::string generateID();

        // validity checkers
        bool isValidId(const std::string& id) const noexcept;

        // helper setters
        [[nodiscard]] DiaryError setTitle(const std::string& id, const std::string& newTitle) const noexcept;
        [[nodiscard]] DiaryError setContent(const std::string& id, const std::string& newContent) const noexcept;
        
        // helper getters
        const std::string* getTitle(const std::string& id) const noexcept;
        const std::string* getContent(const std::string& id) const noexcept;
};

#endif
