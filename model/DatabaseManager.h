#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include<QString>
#include<QByteArray>
#include<string>

struct EntryMetadata{
    int64_t id;
    int64_t createdAt;
    int64_t updatedAt;
    int64_t bookmarked;
    QByteArray encryptedTitle;
};

class DatabaseManager{
public:
    DatabaseManager();
    bool databaseInit(const QString& dbPath);
    bool createTable();

    bool setConfigValue(const QString& key, const QByteArray& value);


    qint64 insertEntry(const QString& journal_name, const qint64 created_at, const QByteArray& encrypted_title, const QByteArray& encrypted_content);
    bool deleteEntry(const qint64 id);
    bool updateEntry(const qint64 id, const QString& journal_name, const qint64 updated_at, const QByteArray& encrypted_title, const QByteArray& encrypted_content);

    QByteArray getConfigValue(const QString& key) const;
    std::vector<EntryMetadata> getAllEntriesMetadata();
    QByteArray getEntryContent(int64_t id) const;
private:
};

#endif // DATABASEMANAGER_H
