#include "DiaryManager.h"
#include <algorithm>

#include<QDebug>

// --- Helpers ---
DiaryEntry* DiaryManager::findEntryById(const int64_t id) {
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()) return nullptr;
    return &entries[it->second];
}

// --- Core Features ---
[[nodiscard]] DiaryError DiaryManager::openDiary(const std::string& path, const std::string& password) {
    if(!dbManager.databaseInit(QString::fromStdString(path))){
        DiaryError::DatabaseOpenError;
    }
    if(!dbManager.createTable()){
        DiaryError::DatabaseError;
    }

    if (!encManager.initialize()) {
        return DiaryError::CryptoError;
    }

    const QString saltKey = "crypto_salt";
    QByteArray salt = dbManager.getConfigValue(saltKey);
    if(salt.isEmpty()){
        qDebug()<<"New vault Detected. generating new salt\n";
        salt = encManager.generateSalt();
        if(!dbManager.setConfigValue(saltKey,salt)){
            return DiaryError::DatabaseError;
        }
    }else{
        qDebug() << "Existing vault detected. Salt loaded.";
    }

    masterKey = encManager.deriveMasterKey(password,salt);
    if(masterKey.empty()){
        DiaryError::AuthenticationFailed;
    }
    qDebug() << "Success: Vault unlocked and Master Key securely loaded in memory.";
    // read bytes into encryption manager salt array from database manager call dbManager.getConfigValue()
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
