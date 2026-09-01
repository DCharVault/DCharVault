#include "DiaryManager.h"
#include <algorithm>

#include<QDebug>
#include<QFile>
#include<QTextDocument>
#include<QDateTime>
#include<string>
#include<QString>
#include"TitleGenerators.h"
#include "CompletionCalculator.h"

namespace DefaultsDM_Values{
    constexpr uint32_t DEFAULT_SESSION_TIMEOUT_SECONDS = 420; // 7mins default timeout session period
    constexpr uint32_t DEFAULT_CLIPBOARD_TIMEOUT_SECONDS = 30;  // 30s default clipboard timeout
}

DiaryEntry* DiaryManager::findEntryById(const int64_t id) {
    auto it = idToIndex.find(id);
    if(it == idToIndex.end()) return nullptr;
    return &entries[it->second];
}

void DiaryManager::setContentUri(const QString& contentUri, const QString& localCachePath) {
    m_contentUri = contentUri;
    m_localCachePath = localCachePath;
}

[[nodiscard]] DiaryError DiaryManager::lockVault(){
    if(!isVaultOpened()){ return DiaryError::None; }

    // Android: sync encrypted .db back to original content:// URI before wiping
    if (!m_contentUri.isEmpty() && !m_localCachePath.isEmpty()) {
        QFile src(m_localCachePath);
        QFile dst(m_contentUri);
        if (src.open(QIODevice::ReadOnly) && dst.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            dst.write(src.readAll());
            qDebug() << "DiaryManager: Synced encrypted vault back to external storage.";
        } else {
            qWarning() << "DiaryManager: Failed to sync vault back to" << m_contentUri;
        }
        m_contentUri.clear();
        m_localCachePath.clear();
    }

    sodium_memzero(masterKey.data(), masterKey.capacity());
    masterKey.clear();
    masterKey.shrink_to_fit();
    SecureVector().swap(masterKey);

    for(auto& entry : entries){
        // Expand string size to its full capacity so data() covers all allocated memory safely
        entry.title.resize(entry.title.capacity(), '\0');
        entry.content.resize(entry.content.capacity(), '\0');

        sodium_memzero(entry.title.data(), entry.title.capacity());
        sodium_memzero(entry.content.data(), entry.content.capacity());

        entry.title.clear();
        entry.content.clear();
    }

    std::vector<DiaryEntry>().swap(entries);
    std::unordered_map<int64_t, size_t>().swap(idToIndex);

    dbManager.closeDatabase();

    return DiaryError::None;
}

bool DiaryManager::isVaultOpened() const{
    return !masterKey.empty();
}

[[nodiscard]] DiaryError DiaryManager::saveSessionTimeout(uint32_t seconds)
{
    if (!isVaultOpened()) {
        return DiaryError::MasterKeyNotFound;
    }
    const QString sessionTimeout = "session_timeout";
    const QString sessionSecs = QString::number(seconds);
    const QByteArray valueBytes = encManager.encryptString(sessionSecs,masterKey);
    if (valueBytes.isEmpty()) {
        return DiaryError::EncryptionFailed;
    }

    if(!dbManager.setConfigValue(sessionTimeout, valueBytes)){
        return DiaryError::DatabaseError;
    }
    return DiaryError::None;
}
uint32_t DiaryManager::loadSessionTimeout() const
{
    if (!isVaultOpened()) {
        return DefaultsDM_Values::DEFAULT_SESSION_TIMEOUT_SECONDS;
    }
    const QString sessionTimeout = "session_timeout";
    const QByteArray encSessionBytes = dbManager.getConfigValue(sessionTimeout);
    const QString valueBytes = encManager.decryptString(encSessionBytes,masterKey);
    if (valueBytes.isEmpty()) {
        return DefaultsDM_Values::DEFAULT_SESSION_TIMEOUT_SECONDS;
    }
    bool isValidSeconds;
    uint32_t seconds = valueBytes.toUInt(&isValidSeconds);
    if (!isValidSeconds) {
        qWarning() << "Invalid session timeout in database. Using default.";
        return DefaultsDM_Values::DEFAULT_SESSION_TIMEOUT_SECONDS;
    }
    return seconds;
}

[[nodiscard]] DiaryError DiaryManager::saveClipboardTimeout(uint32_t seconds)
{
    if (!isVaultOpened()) {
        return DiaryError::MasterKeyNotFound;
    }
    const QString clipboardTimeout = "clipboard_timeout";
    const QString clipboardSecs = QString::number(seconds);
    const QByteArray valueBytes = encManager.encryptString(clipboardSecs,masterKey);
    if (valueBytes.isEmpty()) {
        return DiaryError::EncryptionFailed;
    }

    if(!dbManager.setConfigValue(clipboardTimeout, valueBytes)){
        return DiaryError::DatabaseError;
    }
    return DiaryError::None;
}
uint32_t DiaryManager::loadClipboardTimeout() const
{
    if (!isVaultOpened()) {
        return DefaultsDM_Values::DEFAULT_CLIPBOARD_TIMEOUT_SECONDS;
    }
    const QString clipboardTimeout = "clipboard_timeout";
    const QByteArray encClipboardBytes = dbManager.getConfigValue(clipboardTimeout);
    const QString valueBytes = encManager.decryptString(encClipboardBytes,masterKey);
    if (valueBytes.isEmpty()) {
        return DefaultsDM_Values::DEFAULT_CLIPBOARD_TIMEOUT_SECONDS;
    }
    bool isValidSeconds;
    uint32_t seconds = valueBytes.toUInt(&isValidSeconds);
    if (!isValidSeconds) {
        qWarning() << "Invalid clipboard timeout in database. Using default.";
        return DefaultsDM_Values::DEFAULT_CLIPBOARD_TIMEOUT_SECONDS;
    }
    return seconds;
}

DiaryError DiaryManager::saveConfig(const QString &key, const QString &value) {
    if (!isVaultOpened()) return DiaryError::MasterKeyNotFound;

    const QByteArray encrypted = encManager.encryptString(value,masterKey);
    if (encrypted.isEmpty()) return DiaryError::EncryptionFailed;
    if (!dbManager.setConfigValue(key,encrypted)) return DiaryError::DatabaseError;

    return DiaryError::None;
}

QString DiaryManager::loadConfig(const QString &key, const QString &defaultValue) const {
    if (!isVaultOpened()) return defaultValue;

    const QByteArray encrypted = dbManager.getConfigValue(key);
    if (encrypted.isEmpty()) return defaultValue;

    const QString value = encManager.decryptString(encrypted,masterKey);

    return value.isEmpty() ? defaultValue : value;
}

[[nodiscard]] DiaryError DiaryManager::openDiary(const QString& journalName, const QString& path, const SecureString& password) {
    if(!dbManager.databaseInit(path)){
        return DiaryError::DatabaseOpenError;
    }
    if(!dbManager.createTable()){
        return DiaryError::DatabaseError;
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

        return DiaryError::AuthenticationFailed;
    }
    const QString verifyKey = "verification_block";
    QByteArray encryptedVerifyBlock = dbManager.getConfigValue(verifyKey);
    if(encryptedVerifyBlock.isEmpty()){
        // brand new vault: generate a new random value for verification
        //todo: this is where need that journal name
        if(!journalName.isEmpty() && dbManager.setJournalName(journalName)){
            qCritical()<<"Journal Name has been set to: "<<journalName;
        }else{
            QString newJournalName = QString::fromStdString(TitleGenerator::generatorJournalTitle());
            if(dbManager.setJournalName(newJournalName))
                qCritical()<<"Journal Name has been generated to: "<<newJournalName;
            else
                qCritical()<<"Journal Name title generator has been failed!";
        }
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

[[nodiscard]] DiaryError DiaryManager::loadFromDisk(const QString& path) {
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

        int completionCompleted = 0;
        int completionTotal = 0;

        if (!meta.encryptedCompletionCompleted.isEmpty()) {
            QString decVal = encManager.decryptString(meta.encryptedCompletionCompleted, masterKey);
            bool ok = false;
            int val = decVal.toInt(&ok);
            if (ok) completionCompleted = val;
        }
        if (!meta.encryptedCompletionTotal.isEmpty()) {
            QString decVal = encManager.decryptString(meta.encryptedCompletionTotal, masterKey);
            bool ok = false;
            int val = decVal.toInt(&ok);
            if (ok) completionTotal = val;
        }

        DiaryEntrySummary summary;
        summary.id = meta.id;
        summary.createdAt = meta.createdAt;
        summary.updatedAt = meta.updatedAt;
        summary.bookmarked = meta.bookmarked;
        summary.title = decryptedTitle;
        summary.completionCompleted = completionCompleted;
        summary.completionTotal     = completionTotal;

        decryptedSummaries.push_back(summary);
    }
    qDebug() << "Success: Decrypted" << decryptedSummaries.size() << "titles for the sidebar.";
    return decryptedSummaries;
}

[[nodiscard]] int64_t DiaryManager::createEntry(const QString& title, const QString& content) {
    if(masterKey.empty()){
        qCritical()<<"Master Key is Empty! can't create a new entry to this journal";
        return -1;
    }
    if(title.isEmpty() && content.isEmpty()){
        qCritical()<<"Cannot Have Anything to Save in Entry";
        return -1;
    }

    QByteArray titleEncrypted;
    if(title.isEmpty()){
        QString htmlChunk = content.left(1024);
        QTextDocument textDoc;
        textDoc.setHtml(htmlChunk);
        QString textClean = textDoc.toPlainText().simplified();
        const std::string contentStr = textClean.toStdString();
        size_t len = std::min(contentStr.size(),(size_t)16);
        const std::string contentHead = contentStr.substr(0,len);
        QString newTitle = QString::fromStdString(TitleGenerator::generatorEntryTitle(contentHead));
        titleEncrypted = encManager.encryptString(newTitle,masterKey);
    }
    else{
        titleEncrypted = encManager.encryptString(title,masterKey);
    }
    QByteArray contentEncrypted = encManager.encryptString(content,masterKey);
    qint64 timeStamp = QDateTime::currentSecsSinceEpoch();

    const CompletionResult completion = CompletionCalculator::calculate(content);
    const QByteArray encCompCompleted = encManager.encryptString(QString::number(completion.completed), masterKey);
    const QByteArray encCompTotal = encManager.encryptString(QString::number(completion.total), masterKey);

    int64_t insertedId = dbManager.insertEntry(timeStamp,titleEncrypted,contentEncrypted, encCompCompleted, encCompTotal);
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

QString DiaryManager::readEntryTitle(int64_t id){
    if(masterKey.empty()){
        qCritical() << "Fatal: Master Key is empty. Cannot decrypt content.";
        return "";
    }
    QByteArray encryptedTitle = dbManager.getEntryTitle(id);
    if(encryptedTitle.isEmpty()){
        return "";
    }
    QString decryptedTitle = encManager.decryptString(encryptedTitle,masterKey);
    if(decryptedTitle.isEmpty() && !encryptedTitle.isEmpty()){
        qCritical() << "Warning: Failed to decrypt title for entry ID:" << id;
        return "[[ Decryption Failed - Data Corrupted ]]";
    }
    return decryptedTitle;
}

QString DiaryManager::readEntryContent(int64_t id){
    if (masterKey.empty()) {
        qCritical() << "Fatal: Master Key is empty. Cannot decrypt content.";
        return "";
    }

    QByteArray encryptedContent = dbManager.getEntryContent(id);
    if (encryptedContent.isEmpty()) {
        return ""; // could be a genuinely blank note, or a DB error
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

    if(title.isEmpty() && content.isEmpty()){
        qCritical()<<"Cannot Have Anything to Save in Entry";
        return DiaryError::None;
    }

    QByteArray titleEncrypted;
    if(title.isEmpty()){
        QString htmlChunk = content.left(1024);
        QTextDocument textDoc;
        textDoc.setHtml(htmlChunk);
        QString textClean = textDoc.toPlainText().simplified();
        const std::string contentStr = textClean.toStdString();
        size_t len = std::min(contentStr.size(),(size_t)16);
        const std::string contentHead = contentStr.substr(0,len);
        QString newTitle = QString::fromStdString(TitleGenerator::generatorEntryTitle(contentHead));
        titleEncrypted = encManager.encryptString(newTitle,masterKey);
    }
    else{
        titleEncrypted = encManager.encryptString(title,masterKey);
    }

    QByteArray contentEncrypted = encManager.encryptString(content,masterKey);
    qint64 updatedAt = QDateTime::currentSecsSinceEpoch();

    const CompletionResult completion = CompletionCalculator::calculate(content);

    const QByteArray encCompCompleted = encManager.encryptString(QString::number(completion.completed), masterKey);
    const QByteArray encCompTotal     = encManager.encryptString(QString::number(completion.total), masterKey);

    //todo: insert journal name here instead of hardcoded journal
    if(!dbManager.updateEntry(id,updatedAt,titleEncrypted,contentEncrypted,encCompCompleted,encCompTotal)){
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
