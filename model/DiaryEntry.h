#ifndef DIARYENTRY_H
#define DIARYENTRY_H
#include<string>
#include<cstdint> 


struct DiaryEntry{
    bool bookmarked = false;
    int64_t createdAt=0;
    int64_t updatedAt=0;
    std::string id;
    std::string title;
    std::string content;
    DiaryEntry(const std::string& title, const std::string& content) : title{title}, content{content}{};
};


#endif
