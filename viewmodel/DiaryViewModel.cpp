#include"DiaryViewModel.h"

#include<QDebug>

DiaryViewModel::DiaryViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_diaryManager(manager){}

void DiaryViewModel::saveNewEntry(const QString &title, const QString &content){
    qDebug()<<"DiaryViewModel: Received request to save new entry";

    const std::string stdTitle = title.toStdString();
    const std::string stdContent = content.toStdString();

    int64_t newId = m_diaryManager.createEntry(stdTitle,stdContent);
    if(newId != -1) {
        qDebug() << "DiaryViewModel: Entry saved successfully with ID:" << newId;
        emit entrySavedSuccessfully();
    } else {
        qCritical() << "DiaryViewModel: Failed to save entry.";
        emit entrySaveFailed("Failed to encrypt or save the entry to the database.");
    }
}
