#include"DatabaseManager.h"

#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
#include<QDebug>

DatabaseManager::DatabaseManager(){}

bool DatabaseManager::databaseInit(const QString& dbPath){
    // check if already a connection exists
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection)){
        return true;
    }

    // driver
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // set file path
    db.setDatabaseName(dbPath);

    // check if openable
    if(!db.open()){
        qCritical()<<"Fatal Error: Failed to connect to Database"<<db.lastError().text()<<'\n';
        return false;
    }
    qDebug()<<"Success: SQLite database connected at "<<dbPath<<'\n';
    return true;
}

bool DatabaseManager::createTable(){
    QSqlQuery query;

    const QString createConfigTable = R"(
                                   CREATE TABLE IF NOT EXISTS vault_config(
                                     key TEXT PRIMARY KEY,
                                     value BLOB NOT NULL
                                   )
                                )";
    if(!query.exec(createConfigTable)){
        qCritical()<<"Failed to create config_vault: "<<query.lastError().text()<<"\n";
        return false;
    }

    const QString createDiaryTable = R"(
                                  CREATE TABLE IF NOT EXISTS journal(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    journal_name TEXT NOT NULL,
                                    created_at INTEGER NOT NULL,
                                    updated_at INTEGER NOT NULL,
                                    bookmarked INTEGER DEFAULT 0 NOT NULL,
                                    encrypted_title BLOB NOT NULL,
                                    encrypted_content BLOB NOT NULL
                                  )
                                )";
    if(!query.exec(createDiaryTable)){
        qCritical()<<"Failed to journal entries table: "<<query.lastError().text()<<"\n";
        return false;
    }

    qDebug()<<"Success: Database tables verified!";
    return true;
}

bool DatabaseManager::setConfigValue(const QString &key, const QByteArray &value){
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO vault_config (key, value) VALUES (:key, :value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    if(!query.exec()){
        qCritical() << "Failed to insert or update config table: " << query.lastError().text() << "\n";
        return false;
    }

    qDebug() << "Success: Config key '" << key << "' saved!";
    return true;
}

QByteArray DatabaseManager::getConfigValue(const QString &key) const {
    QSqlQuery query;
    query.prepare("SELECT value FROM vault_config WHERE key = :key");
    query.bindValue(":key", key);

    if(!query.exec()){
        qCritical() << "Failed to execute get config value: " << query.lastError().text() << "\n";
        return QByteArray();
    }

    if (query.next()) {
        return query.value("value").toByteArray();
    }
    return QByteArray();
}

QByteArray DatabaseManager::getEntryContent(int64_t id) const{
    QSqlQuery query;
    query.prepare("SELECT encrypted_content FROM journal WHERE id = :id");
    query.bindValue(":id",QVariant::fromValue(id));

    if (!query.exec()) {
        qCritical() << "Failed to fetch content:" << query.lastError().text();
        return QByteArray();
    }

    if(query.next()){
        return query.value(0).toByteArray();
    }
    qWarning() << "No content found in database for ID:" << id;
    return QByteArray();
}

qint64 DatabaseManager::insertEntry(const QString &journal_name, const qint64 created_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content){
    QSqlQuery query;
    query.prepare("INSERT INTO journal(journal_name,created_at,updated_at,encrypted_title,encrypted_content) "
                  "VALUES(:journal_name,:created_at,:updated_at,:encrypted_title,:encrypted_content)");
    query.bindValue(":journal_name",journal_name);
    query.bindValue(":created_at",created_at);
    query.bindValue(":updated_at",created_at);
    query.bindValue(":encrypted_title",encrypted_title);
    query.bindValue(":encrypted_content",encrypted_content);
    if(!query.exec()){
        qCritical()<<"Failed to insert journal entries : "<<query.lastError().text()<<"\n";
        return -1;
    }

    qDebug()<<"Success: Entry inserted!";

    bool ok=false;
    qint64 insertedId = query.lastInsertId().toLongLong(&ok);
    if (!ok) {
        qCritical() << "Fatal: Failed to convert last insert ID to qint64. Data state unknown.\n";
        return -1;
    }
    return insertedId;
}

bool DatabaseManager::deleteEntry(const qint64 id){
    QSqlQuery query;
    query.prepare("DELETE FROM journal WHERE id = :id");
    query.bindValue(":id",id);
    if(!query.exec()){
        qCritical()<<"Failed to delete journal entries : "<<query.lastError().text()<<"\n";
        return false;
    }
    qDebug()<<"Success: Entry deleted!";
    return true;
}

bool DatabaseManager::updateEntry(const qint64 id, const QString &journal_name, const qint64 updated_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content)
{
    QSqlQuery query;
    query.prepare("UPDATE journal SET "
                  "journal_name = :journal_name,"
                  "updated_at = :updated_at,"
                  "encrypted_title = :encrypted_title,"
                  "encrypted_content = :encrypted_content "
                  "WHERE id = :id"
                  );
    query.bindValue(":journal_name",journal_name);
    query.bindValue(":updated_at",updated_at);
    query.bindValue(":encrypted_title",encrypted_title);
    query.bindValue(":encrypted_content",encrypted_content);
    query.bindValue(":id",id);
    if(!query.exec()){
        qCritical()<<"Failed to update journal entries : "<<query.lastError().text()<<"\n";
        return false;
    }
    qDebug()<<"Success: Entry updated!";
    return true;
}

std::vector<EntryMetadata> DatabaseManager::getAllEntriesMetadata(){
    std::vector<EntryMetadata> eMeta;
    QSqlQuery query;

    query.prepare("SELECT id, created_at, updated_at, bookmarked, encrypted_title "
                  "FROM journal "
                  "ORDER BY created_at DESC");

    if(!query.exec()){
        qCritical() << "Failed to fetch journal entries metadata:" << query.lastError().text();
        return eMeta;
    }

    while(query.next()){
        EntryMetadata meta;
        meta.id = query.value(0).toLongLong();
        meta.createdAt = query.value(1).toLongLong();
        meta.updatedAt = query.value(2).toLongLong();
        meta.bookmarked = query.value(3).toLongLong();
        meta.encryptedTitle = query.value(4).toByteArray();
        eMeta.push_back(meta);
    }

    qDebug() << "Success: Entry metadata fetched! Count:" << eMeta.size();
    return eMeta;
}

bool DatabaseManager::updateJournalName(const QString& newJournal_name){
    // Todo: sql query to update journal name
    QSqlQuery query;
    return true;
}
