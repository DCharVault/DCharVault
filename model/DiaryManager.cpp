#include "DiaryManager.h"
#include <algorithm>

// --- Helpers ---
DiaryEntry* DiaryManager::findEntryById(const int64_t id) {
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()) return nullptr;
    return &entries[it->second];
}

// --- Core Features ---
[[nodiscard]] DiaryError DiaryManager::openDiary(const std::string& path, const std::string& password) {
    // TODO: Init database, create tables, derive libsodium key, load entries from DB.
    return DiaryError::None;
}

[[nodiscard]] DiaryError DiaryManager::loadFromDisk() {
    // TODO: Read all rows from dbManager, decrypt them, put them in 'entries' vector.
    return DiaryError::None;
}

[[nodiscard]] int64_t DiaryManager::createEntry(const std::string& title, const std::string& content) {
    // TODO: Encrypt text, call dbManager.insertEntry(), get new ID, add to vector.
    return -1;
}

std::vector<DiaryEntrySummary> DiaryManager::readEntrySummaries() const {
    // (You can actually keep your old logic for this one, it was mostly fine)
    std::vector<DiaryEntrySummary> summaries;
    summaries.reserve(entries.size());
    for (const auto& entry : entries) {
        summaries.push_back({entry.id, entry.title, entry.createdAt, entry.updatedAt, entry.bookmarked});
    }
    return summaries;
}

const DiaryEntry* DiaryManager::readEntry(const int64_t id) const noexcept {
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()) return nullptr;
    return &entries[it->second];
}

[[nodiscard]] DiaryError DiaryManager::updateEntry(const int64_t id, const std::string& title, const std::string& content) {
    // TODO: Encrypt text, call dbManager.updateEntry(), update vector.
    return DiaryError::None;
}

[[nodiscard]] DiaryError DiaryManager::deleteEntry(const int64_t id) {
    // TODO: Call dbManager.deleteEntry(), securely wipe RAM, remove from vector.
    return DiaryError::None;
}
