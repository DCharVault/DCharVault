#ifndef DIARYMANAGER_H
#define DIARYMANAGER_H

#include<vector>
#include<string>
#include"DiaryEntry.h"

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

};

#endif
