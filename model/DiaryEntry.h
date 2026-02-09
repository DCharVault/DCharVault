#ifndef DIARYENTRY_H
#define DIARYENTRY_H
#include<string>

struct DiaryEntry{
    bool entryIsBookmarked = false;
    std::string entryId;
    std::string entryCreatedAt;
    std::string entryUpdatedAt;
    std::string entryTitle;
    std::string entryContent;
};


#endif