#ifndef DIARYVIEWMODEL_H
#define DIARYVIEWMODEL_H

#include<QObject>
#include<QString>
#include"model/DiaryManager.h"

class DiaryViewModel : public QObject{
    Q_OBJECT
public:
    explicit DiaryViewModel(DiaryManager& manager, QObject *parent=nullptr);
    Q_INVOKABLE void saveNewEntry(const QString& title, const QString& content);
signals:
    void entrySavedSuccessfully();
    void entrySaveFailed(const QString& errorMessage);
private:
    DiaryManager& m_diaryManager;
};

#endif // DIARYVIEWMODEL_H
