#include "DiaryManager.h"
#include <algorithm> // Required for std::fill (used in secure deletion)

// ==================================================================================
// HELPER METHODS
// ==================================================================================

// Internal Helper: FindEntryById
// WHY: We need to find entries in both read-only (const) and modifiable contexts.
// avoiding code duplication reduces bugs.
// PERFORMANCE: This is a linear search O(N). For < 1000 entries, this is faster
// than a HashMap due to CPU cache locality (vectors are contiguous in RAM).
DiaryEntry* DiaryManager::findEntryById(const std::string& id) {
    // FIX: Renamed 'entry' to 'it' so it matches 'it->second' below
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()){
        return nullptr;
    }
    // directly return the pointer from the vector using the stored index
    // it->second is the index. entries[index] is the object. & takes the address.
    return &entries[it->second];
    /*
    If you call readEntry("C99"):
    Map Lookup: The map says "C99 is at index 2". (it->second is 2).
    Vector Access: We look at entries[2].
    Return Pointer: We return the address (&) of that entry so the user can read it.
    */
}

// FIX: Removed DiaryManager::DiaryManager() because it is marked '= default' in the header.
// Redefining it here causes a compile error.


// ==================================================================================
// CORE FEATURES
// ==================================================================================


[[nodiscard]] DiaryError DiaryManager::openDiary(const std::string& path, const std::string& password){

    // 1. If a diary is already open, clear it securely
    if (!entries.empty()) {
        entries.clear();
        entries.shrink_to_fit(); // Release RAM back to Android/Windows
        idToIndex.clear();
    }

    // perform actual disk(database SQLite) I/O (pseudo code)
    // if (!diary.exists()) return DiaryError::DeserializationFailed;


    // 2. Database logic
    // We attempt to open the SQLite file at 'path' using 'password'
    // if (db.open(path, password) != SUCCESS) return DiaryError::DecryptionFailed;

    // 3. Fill the vector from DB
    // ...

    return DiaryManager::loadFromDisk();
}


[[nodiscard]] DiaryError DiaryManager::loadFromDisk(){
    // PRE-RESERVE memory (Optimization!)
    // This prevents the map from "re-shuffling" while you fill it.
    idToIndex.reserve(entries.size());

    for(size_t i = 0; i<entries.size(); ++i){
        idToIndex[entries[i].id] = i;
    }

    return DiaryError::None;
}



// 1. Create Entry
// GOAL: Add a new entry to the RAM database safely and efficiently.
std::string DiaryManager::createEntry(const std::string &title, const std::string &content) {
    // VALIDATION: Fail fast if data is invalid.
    if (title.empty() || content.empty()) return "";

    // PERFORMANCE: emplace_back vs push_back
    // 'push_back' creates a temporary object, copies it into the vector, then destroys the temp.
    // 'emplace_back' builds the object DIRECTLY inside the vector's memory.
    // This saves 1 construction + 1 destruction cycle.
    DiaryEntry& entry = entries.emplace_back(title, content);

    // LOGIC: Set Metadata
    // In a real app, generateUUID() would create a unique string like "550e8400-e29b..."
    entry.id = "UUID_PLACEHOLDER";
    entry.createdAt = 1000;        // Placeholder timestamp
    entry.updatedAt = 1000;


    // making sure that idToIndex can update itself to newely created entry
    idToIndex[entry.id] = entries.size()-1;

    // SAFETY CRITICAL: Return the ID, not the Pointer!
    // If we returned '&entry', and the vector later resized (grew), that pointer
    // would point to deleted memory (Dangling Pointer).
    // The ID (std::string) is a safe "Handle" to find the data later.
    return entry.id;
}

// 2. Read Summaries
// GOAL: Provide a lightweight list for the UI (ListView).
// The UI doesn't need the full content (which could be huge), just the title/date.
std::vector<DiaryEntrySummary> DiaryManager::readEntrySummaries() const{
    std::vector<DiaryEntrySummary> summaries;

    // PERFORMANCE: Reserve Memory
    // We know exactly how many items we need. 'reserve' allocates all memory at once.
    // Without this, the vector might resize multiple times as we add items, which is slow.
    summaries.reserve(entries.size());

    for (const auto& entry : entries) {
        // SAFETY: We copy data into a new 'Summary' struct.
        // We use std::string (Deep Copy) instead of string_view (Pointer).
        // Why? If the main 'entries' vector is modified while the UI is reading this summary,
        // a string_view would point to garbage. std::string ensures the UI has its own safe copy.
        summaries.push_back({entry.id, entry.title, entry.createdAt, entry.updatedAt, entry.bookmarked});
    }
    return summaries;
}

// 3. Read Single Entry
// GOAL: Get the full data for the Editor.
const DiaryEntry* DiaryManager::readEntry(const std::string& id) const noexcept {

    auto it = idToIndex.find(id);
    // FIX: Compare to idToIndex.end(), NOT 'entires.end()' (which was a typo and wrong type)
    if(it == idToIndex.end()) return nullptr; 
    return &entries[it->second];
}

// 4. Update Entry
// GOAL: Modify an existing entry.
DiaryError DiaryManager::updateEntry(const std::string& id, const std::string& title, const std::string& content) {
    if (id.empty()) return DiaryError::EmptyId;
    if (title.empty()) return DiaryError::EmptyTitle;

    // LOCATE: Find the object in RAM.
    DiaryEntry* entry = findEntryById(id);

    if (entry) {
        // UPDATE: Modify the fields.
        entry->title = title;
        entry->content = content;
        entry->updatedAt = 2000; // Mock timestamp update
        return DiaryError::None;
    }

    return DiaryError::EntryNotFound;
}

// 5. Delete Entry
// GOAL: Remove an entry and ensuring the data is truly gone (Sanitization).
DiaryError DiaryManager::deleteEntry(const std::string &id) {
    if (id.empty()) return DiaryError::EmptyId;
    auto it = idToIndex.find(id);
    if (it == idToIndex.end()) return DiaryError::EntryNotFound;

    size_t indexToDelete = it->second;
    size_t lastIndex = entries.size() - 1;

    volatile char* p = (volatile char*)entries[indexToDelete].content.data();
    std::fill(p, p + entries[indexToDelete].content.size(), 0);

    // 1. Swap with the last element (unless it IS the last element)
    if (indexToDelete != lastIndex) {
        // Move the last element into the gap
        entries[indexToDelete] = std::move(entries.back());
        
        // IMPORTANT: Update the Map for the element that just moved!
        idToIndex[entries[indexToDelete].id] = indexToDelete;
    }


    // 2. Remove the now-duplicate last element
    entries.pop_back();
    
    // 3. Remove the deleted ID from the map
    idToIndex.erase(id);

    return DiaryError::None;
}
