#ifndef LOGINVIEWMODEL_H
#define LOGINVIEWMODEL_H

#include"model/DiaryManager.h"
#include<QObject>
#include<QString>

class LoginViewModel : public QObject{
    Q_OBJECT

public:
    explicit LoginViewModel(DiaryManager& manager,QObject *parent=nullptr);

    // Q_INVOKABLE makes this function callable directly from QML button onClicked
    Q_INVOKABLE void attemptLogin(const QString& password);

signals:
    // Emitted to tell QML to transition to the Home Screen
    void loginSuccess();
    // Emitted to tell QML to show a red error text box
    void loginFailed();


private:
    DiaryManager& m_diaryManager;

};

#endif // LOGINVIEWMODEL_H
