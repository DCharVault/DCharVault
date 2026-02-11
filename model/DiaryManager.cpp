#include"DiaryManager.h"

//setters
[[nodiscard]] DiaryError DiaryManager::setTitle(const std::string& id, const std::string& newTitle) noexcept{
    if(id.empty()) return DiaryError::EmptyId;
    // if (!isValidId(id)) return nullptr; -- to be implemented later
    for(const auto& entry : entries){
        if(entry.id == id){
            entry.title = newTitle;
            // update time here too
            return DiaryError::None;
        }
    }
    return DiaryError::EntryNotFound;
}
[[nodiscard]] DiaryError DiaryManager::setContent(const std::string& id, const std::string& newContent) noexcept{
    if(id.empty()) return DiaryError::EmptyId;
    for(const auto& entry : entries){
        if(entry.id == id){
            entry.content = newContent;
            // update time here too
            return DiaryError::None;
        }
    }
    return DiaryError::EntryNotFound;
}

//getters
const std::string* DiaryManager::getTitle(const std::string& id) const noexcept{
    if(id.empty()) return nullptr;
    // if (!isValidId(id)) return nullptr; -- to be implemented later
    for(const auto& entry : entries){
        if(entry.id==id){
            return &entry.title;
        }
    }
    return nullptr;
}
const std::string* DiaryManager::getContent(const std::string& id) const noexcept{
    if(id.empty()) return nullptr;
    // if (!isValidId(id)) return nullptr; -- to be implemented later
    for(const auto &entry : entries){
        if(entry.id == id){
            return &entry.content;
        }
    }
    return nullptr;
}


const DiaryEntry* DiaryManager::readEntry(std::string& id) const noexcept{ // not & since reference returns cannot be null if id page do not found
    if(id.empty()) return nullptr;
    // if (!isValidId(id)) return nullptr; -- to be implemented later
    for(const auto& entry : entries){
        if(entry.id==id){
            return &entry;
        }
    }
    return nullptr;
}

const DiaryEntry* DiaryManager::createEntry(const std::string &title, const std::string &content){
    if (title.empty() || content.empty())
        return nullptr; // creation failed (fast-path semantics)

    // PERFORMANCE: 'emplace_back' constructs the object directly inside the vector.
    // pass to the constructor arguments (title, content) directly.
    DiaryEntry& entry = entries.emplace_back(title, content);

    entry.id = generateID();
    // entry.createdAt = entry.getCurrentTime(); // hypothetical function  
    // entry.updatedAt = entry.getCurrentTime();
    return &entries.back();
}

[[nodiscard]] DiaryError DiaryManager::updateEntry(const std::string& id, const std::string& title, const std::string& content){
    if(id.empty()) return DiaryError::EmptyId;
    if(title.empty()) return DiaryError::EmptyTitle;
    if(content.empty()) return DiaryError::EmptyContent;

    for(auto& entry : entries){
        if(entry.id == id)
        {
            entry.title = title;
            entry.content = content;
            entry.updatedAt = getLastSaveTime(); 
            return DiaryError::None;
        }
    }
    return DiaryError::EntryNotFound;
}

[[nodiscard]] DiaryError DiaryManager::deleteEntry(const std::string &id){
    if(id.empty()) return DiaryError::EmptyId;

    for(auto it=entries.begin(); it!=entries.end(); ++it){
        if(it->id == id){
             // SECURITY NOTE: std::string destructor does not wipe memory (data remains in RAM).
            // To be truly secure, you would need to overwrite it.title and it.content here.
            entries.erase(it);
            return DiaryError::None;
        }
    }
    return DiaryError::EntryNotFound;
}