#include"DiaryViewModel.h"

#include<QDebug>

DiaryViewModel::DiaryViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_diaryManager(manager){}

void DiaryViewModel::saveNewEntry(const QString &title, const QString &content){
    qDebug()<<"DiaryViewModel: Received request to save new entry";

    int64_t newId = m_diaryManager.createEntry(title,content);
    if(newId != -1) {
        qDebug() << "DiaryViewModel: Entry saved successfully with ID:" << newId;
        emit entrySavedSuccessfully();
    } else {
        qCritical() << "DiaryViewModel: Failed to save entry.";
        emit entrySaveFailed("Failed to encrypt or save the entry to the database.");
    }
}

QString DiaryViewModel::loadEntryContent(qint64 id){
    qDebug() << "DiaryViewModel: Received request to load content for ID:" << id;
    return m_diaryManager.readEntryContent(id);
}
