#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include<QString>
#include<QByteArray>
#include<string>

class DatabaseManager{
public:
    DatabaseManager();
    bool databaseInit(const QString& dbPath);
    bool createTable();
    bool insertEntry(const QString &journal_name, const int64_t created_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content);
    bool deleteEntry(const int64_t id);
    bool updateEntry(const int64_t id, const QString &journal_name, const int64_t updated_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content);
private:

};

#endif // DATABASEMANAGER_H
