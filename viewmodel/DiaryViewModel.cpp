#include"DiaryViewModel.h"

#include<QDebug>

DiaryViewModel::DiaryViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_diaryManager(manager){}

void DiaryViewModel::saveNewEntry(const qint64 id, const QString &title, const QString &content){
    if(id==-1){
        // new note
        int64_t newId = m_diaryManager.createEntry(title,content);
        if(newId != -1) {
            qDebug() << "DiaryViewModel: Entry saved successfully with ID:" << newId;
            emit entrySavedSuccessfully(newId,loadEntryTitle(newId));
        } else {
            qCritical() << "DiaryViewModel: Failed to save entry.";
            emit entrySaveFailed("Failed to encrypt or save the entry to the database.");
        }
    }else{
        if(m_diaryManager.updateEntry(id,title,content)==DiaryError::None){
          emit entrySavedSuccessfully(id,loadEntryTitle(id));
        }else{
            emit entrySaveFailed("Failed to update Entry");
        }
    }
}

QString DiaryViewModel::loadEntryTitle(const qint64 id){
    qDebug()<<"DiaryViewModel: Received request to load title for ID: <<id;";
    return m_diaryManager.readEntryTitle(id);
}

QString DiaryViewModel::loadEntryContent(const qint64 id){
    qDebug() << "DiaryViewModel: Received request to load content for ID: " << id;
    return m_diaryManager.readEntryContent(id);
}

void DiaryViewModel::deleteEntry(const qint64 id)
{
    qDebug() << "DiaryViewModel: Received request to delete content for ID: "<<id;
    if(m_diaryManager.deleteEntry(id)==DiaryError::None){
        emit entryDeletedSuccessfully();
    }else{
        emit entryDeleteFailed();
    }
}
