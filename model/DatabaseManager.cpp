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
    QString createDiaryTable = R"(
                                  CREATE TABLE IF NOT EXISTS journal(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    journal_name TEXT NOT NULL,
                                    created_at INTEGER NOT NULL,
                                    updated_at INTEGER NOT NULL,
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

bool DatabaseManager::insertEntry(const QString &journal_name, const int64_t created_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content){
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
        return false;
    }

    qDebug()<<"Success: Entry inserted!";
    return true;
}

bool DatabaseManager::deleteEntry(const int64_t id){
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

bool DatabaseManager::updateEntry(const int64_t id, const QString &journal_name, const int64_t updated_at, const QByteArray &encrypted_title, const QByteArray &encrypted_content)
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
