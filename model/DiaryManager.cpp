#include"DiaryManager.h"

const DiaryEntry* DiaryManager::readEntry(std::string id){

}

std::string DiaryManager::createEntry(const std::string &title, const std::string &content){
     // PERFORMANCE: 'emplace_back' constructs the object directly inside the vector.
    // We pass the constructor arguments (title, content) directly.
    DiaryEntry& entry = entries.emplace_back(title, content);

    entry.id = generateID();
    entry.createdAt = getCurrentTime(); // hypothetical function
    entry.updatedAt = getLastSaveTime(); // another hypothetical function

    entries.push_back(entry);

    return entry.id;
}

bool DiaryManager::updateEntry(const std::string& id, const std::string& title, const std::string& content){
    if(id.empty()) return false;
    for(auto& entry : entries){
        if(entry.id == id)
        {
            entry.title = title;
            entry.content = content;
            entry.updatedAt = getLastSaveTime(); 
            return true;
        }
    }
    return false;
}

bool DiaryManager::deleteEntry(const std::string &id){
    if(id.empty()) return false;
    for(auto it=entries.begin(); it!=entries.end(); ++it){
        if(it->id == id){
             // SECURITY NOTE: std::string destructor does not wipe memory (data remains in RAM).
            // To be truly secure, you would need to overwrite it.title and it.content here.
            entries.erase(it);
            return true;
        }
    }
    return false;
}
