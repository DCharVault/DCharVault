#ifndef DIARYVIEWMODEL_H
#define DIARYVIEWMODEL_H

#include<QObject>
#include<QString>
#include"model/DiaryManager.h"

class DiaryViewModel : public QObject{
    Q_OBJECT
public:
    explicit DiaryViewModel(DiaryManager& manager, QObject *parent=nullptr);
    Q_INVOKABLE void saveNewEntry(const qint64 id, const QString& title, const QString& content);
    Q_INVOKABLE QString loadEntryContent(const qint64 id);
    Q_INVOKABLE QString loadEntryTitle(const qint64 id);
    Q_INVOKABLE void deleteEntry(const qint64 id);
signals:
    void entrySavedSuccessfully(const qint64 savedId, const QString& finalizeTitle);
    void entrySaveFailed(const QString& errorMessage);
    void entryDeletedSuccessfully();
    void entryDeleteFailed();
private:
    DiaryManager& m_diaryManager;
};

#endif // DIARYVIEWMODEL_H
