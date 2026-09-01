#include"DatabaseManager.h"

#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
#include<QDebug>

DatabaseManager::DatabaseManager(){}

bool DatabaseManager::databaseInit(const QString& dbPath){
    // check if already a connection exists
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection)){
        {
            QSqlDatabase existingDb = QSqlDatabase::database(QSqlDatabase::defaultConnection);
            if(existingDb.databaseName() == dbPath && existingDb.isOpen()){
                return true;
            }
            existingDb.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    // driver
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // set file path
    db.setDatabaseName(dbPath);

    // check if openable
    if(!db.open()){
        qCritical()<<"Fatal Error: Failed to connect to Database"<<db.lastError().text();
        return false;
    }
    qDebug()<<"Success: SQLite database connected at "<<dbPath;
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

    const QString createDiaryMetadataTable = R"(
                                    CREATE TABLE IF NOT EXISTS journal_meta(
                                      id INTEGER PRIMARY KEY CHECK (id=1),
                                      journal_name TEXT NOT NULL,
                                      special_status TEXT NOT NULL DEFAULT 'none',
                                      shareable_status TEXT NOT NULL DEFAULT 'private'
                                    )
                                )";
    if(!query.exec(createDiaryMetadataTable)){
        qCritical()<<"Failed to create journal_meta: "<<query.lastError().text()<<"\n";
        return false;
    }

    const QString createDiaryTable = R"(
                                  CREATE TABLE IF NOT EXISTS journal(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    created_at INTEGER NOT NULL,
                                    updated_at INTEGER NOT NULL,
                                    bookmarked INTEGER DEFAULT 0 NOT NULL,
                                    encrypted_title BLOB NOT NULL,
                                    encrypted_content BLOB NOT NULL,
                                    completion_completed BLOB NOT NULL DEFAULT X'',
                                    completion_total     BLOB NOT NULL DEFAULT X''
                                  )
                                )";
    if(!query.exec(createDiaryTable)){
        qCritical()<<"Failed to journal entries table: "<<query.lastError().text()<<"\n";
        return false;
    }

    // migration query for old databases journals to support new format of completion_total and completed
    QSqlQuery migQuery;
    migQuery.exec("ALTER TABLE journal ADD COLUMN completion_completed BLOB NOT NULL DEFAULT X''");
    migQuery.exec("ALTER TABLE journal ADD COLUMN completion_total     BLOB NOT NULL DEFAULT X''");

    qDebug()<<"Success: Database tables verified!";
    return true;
}

void DatabaseManager::closeDatabase() {
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        {
            QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
            if (db.isOpen()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        qDebug() << "Success: Database connection closed securely.";
    } else {
        qDebug() << "Info: No database connection existed to close.";
    }
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

QString DatabaseManager::getJournalName() const {
    QSqlQuery query;
    query.prepare("SELECT journal_name FROM journal_meta WHERE id = 1");

    if(!query.exec()){
        qCritical() << "Failed to execute get journal name: " << query.lastError().text() << "\n";
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    qWarning() << "No journal name found in database.";
    return QString();
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

QByteArray DatabaseManager::getEntryTitle(int64_t id) const{
    QSqlQuery query;
    query.prepare("SELECT encrypted_title FROM journal WHERE id = :id");
    query.bindValue(":id",QVariant::fromValue(id));

    if(!query.exec()){
        qCritical() << "Failed to fetch content:" << query.lastError().text();
        return QByteArray();
    }

    if(query.next()){
        return query.value(0).toByteArray();
    }
    qWarning() << "No title found in database for ID:" << id;
    return QByteArray();
}

qint64 DatabaseManager::insertEntry(const qint64 created_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content, const QByteArray &
                                    encryptedCompletionCompleted, const QByteArray &encryptedCompletionTotal){
    QSqlQuery query;
    query.prepare("INSERT INTO journal(created_at,updated_at,encrypted_title,encrypted_content,completion_completed,completion_total) "
        "VALUES(:created_at,:updated_at,:encrypted_title,:encrypted_content,:completion_completed,:completion_total)");
    query.bindValue(":created_at",created_at);
    query.bindValue(":updated_at",created_at);
    query.bindValue(":encrypted_title",encrypted_title);
    query.bindValue(":encrypted_content",encrypted_content);
    query.bindValue(":completion_total",encryptedCompletionTotal);
    query.bindValue(":completion_completed",encryptedCompletionCompleted);
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

bool DatabaseManager::updateEntry(const qint64 id, const qint64 updated_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content, const
                                  QByteArray &encryptedCompletionCompleted, const QByteArray &encryptedCompletionTotal)
{
    QSqlQuery query;
    query.prepare("UPDATE journal SET "
                  "updated_at = :updated_at,"
                  "encrypted_title = :encrypted_title,"
                  "encrypted_content = :encrypted_content,"
                  "completion_total = :completion_total,"
                  "completion_completed = :completion_completed "
                  "WHERE id = :id"
                  );
    query.bindValue(":updated_at",updated_at);
    query.bindValue(":encrypted_title",encrypted_title);
    query.bindValue(":encrypted_content",encrypted_content);
    query.bindValue(":completion_total",encryptedCompletionTotal);
    query.bindValue(":completion_completed",encryptedCompletionCompleted);
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

    query.prepare("SELECT id, created_at, updated_at, bookmarked, encrypted_title, completion_completed, completion_total "
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
        meta.encryptedCompletionCompleted = query.value(5).toByteArray();
        meta.encryptedCompletionTotal = query.value(6).toByteArray();
        eMeta.push_back(meta);
    }

    qDebug() << "Success: Entry metadata fetched! Count:" << eMeta.size();
    return eMeta;
}

bool DatabaseManager::setJournalName(const QString &newJournal_name){
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO journal_meta(id,journal_name)
        VALUES (1,:journal_name)
        ON CONFLICT(id) DO UPDATE SET
            journal_name = excluded.journal_name
    )");
    query.bindValue(":journal_name",newJournal_name);
    if(!query.exec()){
        qCritical() << "Failed to set journal name:" << query.lastError().text();
        return false;
    }
    qCritical()<<"Database Statement: Journal Name has been set to: "<<newJournal_name;
    return true;
}
bool DatabaseManager::setShareableStatus(const bool isShareable){
    return true;
}
bool DatabaseManager::setSpecialStatus(const QString &status){
    return true;
}
