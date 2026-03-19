#include "DiaryManager.h"
#include <algorithm>

#include<QDebug>
#include<QDateTime>

DiaryEntry* DiaryManager::findEntryById(const int64_t id) {
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()) return nullptr;
    return &entries[it->second];
}

bool DiaryManager::isVaultOpened() const{
    return !masterKey.empty();
}

[[nodiscard]] DiaryError DiaryManager::openDiary(const QString& path, const SecureString& password) {
    if(!dbManager.databaseInit(path)){
        return DiaryError::DatabaseOpenError;
    }
    if(!dbManager.createTable()){
        return DiaryError::DatabaseError;
    }

    if (!encManager.initialize()) {
        return DiaryError::CryptoError;
    }

    const QString saltKey = "crypto_salt"; // todo: change to dynamic instead of hardcoded
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
        return DiaryError::AuthenticationFailed;
    }
    const QString verifyKey = "verification_block";
    QByteArray encryptedVerifyBlock = dbManager.getConfigValue(verifyKey);
    if(encryptedVerifyBlock.isEmpty()){
        // brand new vault: generate a new random value for verification
        qDebug() << "New vault. Generating randomized verification block...";
        QString randomText = encManager.generateRandomBytes(32).toHex(); // why convert to hex from qbytearray
        QByteArray newBlock = encManager.encryptString(randomText,masterKey);
        if(!dbManager.setConfigValue(verifyKey,newBlock)){
            masterKey.clear();
            return DiaryError::DatabaseError;
        }
    }else{
        // existing vault-> check MAC first
        qDebug() << "Testing Master Key against verification block...";
        QString decryptedText = encManager.decryptString(encryptedVerifyBlock,masterKey);
        if(decryptedText.isEmpty()){
            qCritical() << "Fatal: Incorrect Master Password! MAC Verification failed.";
            masterKey.clear();
            return DiaryError::AuthenticationFailed;
        }
        qDebug() << "Success: Password is mathematically correct. Vault Unlocked.";
    }

    qDebug() << "Success: Vault unlocked and Master Key securely loaded in memory.";
    // read bytes into encryption manager salt array from database manager call dbManager.getConfigValue()
    return DiaryError::None;
}

[[nodiscard]] DiaryError DiaryManager::loadFromDisk() {
    // TODO: Read all rows from dbManager, decrypt them, put them in 'entries' vector.
    return DiaryError::None;
}

std::vector<DiaryEntrySummary> DiaryManager::loadAllMetadata(){
    std::vector<DiaryEntrySummary> decryptedSummaries;
    if(masterKey.empty()){
        qCritical() << "Fatal: Master Key is empty. Cannot decrypt metadata.";
        return decryptedSummaries;
    }

    //fetch raw encrypted bytes from database
    std::vector<EntryMetadata> rawBytes = dbManager.getAllEntriesMetadata();

    for(const auto& meta : rawBytes){
        QString decryptedTitle = encManager.decryptString(meta.encryptedTitle,masterKey);
        if(decryptedTitle.isEmpty() && !meta.encryptedTitle.isEmpty()){
            qCritical() << "Warning: Failed to decrypt title for entry ID:" << meta.id;
            decryptedTitle = "[[ Decryption Failed - Corrupted ]]";
        }
        DiaryEntrySummary summary;
        summary.id = meta.id;
        summary.createdAt = meta.createdAt;
        summary.updatedAt = meta.updatedAt;
        summary.bookmarked = meta.bookmarked;
        summary.title = decryptedTitle;

        decryptedSummaries.push_back(summary);
    }
    qDebug() << "Success: Decrypted" << decryptedSummaries.size() << "titles for the sidebar.";
    return decryptedSummaries;
}

[[nodiscard]] int64_t DiaryManager::createEntry(const QString& title, const QString& content) {
    // TODO: Encrypt text, call dbManager.insertEntry(), get new ID, add to vector.
    if(masterKey.empty()){
        qCritical()<<"Master Key is Empty! can't create a new entry to this journal";
        return -1;
    }
    QByteArray titleEncrypted = encManager.encryptString(title,masterKey);
    QByteArray contentEncrypted = encManager.encryptString(content,masterKey);
    qint64 timeStamp = QDateTime::currentSecsSinceEpoch();
    // todo: link journal name here instead of 'Hardcoded journal'
    int64_t insertedId = dbManager.insertEntry("Hardcoded journal",timeStamp,titleEncrypted,contentEncrypted);
    return insertedId;
}

std::vector<DiaryEntrySummary> DiaryManager::readEntrySummaries() {
    return loadAllMetadata();
}

const DiaryEntry* DiaryManager::readEntry(const int64_t id) const noexcept {
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()) return nullptr;
    return &entries[it->second];
}

QString DiaryManager::readEntryContent(int64_t id){
    if (masterKey.empty()) {
        qCritical() << "Fatal: Master Key is empty. Cannot decrypt content.";
        return "";
    }

    QByteArray encryptedContent = dbManager.getEntryContent(id);
    if (encryptedContent.isEmpty()) {
        return ""; // Could be a genuinely blank note, or a DB error
    }

    QString decryptedContent = encManager.decryptString(encryptedContent,masterKey);
    if (decryptedContent.isEmpty() && !encryptedContent.isEmpty()) {
        qCritical() << "Warning: Failed to decrypt content for entry ID:" << id;
        return "[[ Decryption Failed - Data Corrupted ]]";
    }

    return decryptedContent;
}


[[nodiscard]] DiaryError DiaryManager::updateEntry(const int64_t id, const QString& title, const QString& content) {
    if(masterKey.empty()){
        qCritical()<<"Master Key is Empty! Cannot update entry";
        return DiaryError::MasterKeyNotFound;
    }
    QByteArray titleEncrypted = encManager.encryptString(title,masterKey);
    QByteArray contentEncrypted = encManager.encryptString(content,masterKey);
    qint64 updatedAt = QDateTime::currentSecsSinceEpoch();

    //todo: insert journal name here instead of hardcoded journal
    if(!dbManager.updateEntry(id,"Hardcoded journal",updatedAt,titleEncrypted,contentEncrypted)){
        return DiaryError::DatabaseError;
    }
    return DiaryError::None;
}

[[nodiscard]] DiaryError DiaryManager::deleteEntry(const int64_t id) {
    if(masterKey.empty()){
        qCritical()<<"Master Key is Empty! Cannot delete entry";
        return DiaryError::MasterKeyNotFound;
    }
    if(id<=0){
        return DiaryError::InvalidId;
    }
    if(!dbManager.deleteEntry(id)){
        return DiaryError::DatabaseError;
    }
    return DiaryError::None;
}
