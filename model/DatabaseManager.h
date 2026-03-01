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
    bool insertEntry(const QString &journal_name, const qint64 created_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content);
    bool deleteEntry(const qint64 id);
    bool updateEntry(const qint64 id, const QString &journal_name, const qint64 updated_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content);
private:

};

#endif // DATABASEMANAGER_H
