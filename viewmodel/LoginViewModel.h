#ifndef LOGINVIEWMODEL_H
#define LOGINVIEWMODEL_H

#include"model/DiaryManager.h"
#include<QObject>
#include<QString>

class SecurePasswordInput;

class LoginViewModel : public QObject{
    Q_OBJECT

public:
    explicit LoginViewModel(DiaryManager& manager,QObject *parent=nullptr);

    // Q_INVOKABLE makes this function callable directly from QML button onClicked
    Q_INVOKABLE void authenticate(SecurePasswordInput* passwordField, const QString& dbUrl);
    Q_INVOKABLE void updateTitleBar(bool isDark);

signals:
    // Emitted to tell QML to transition to the Home Screen
    void loginSuccess();
    // Emitted to tell QML to show a red error text box
    void loginFailed();

    void dbNotFound();

private:
    DiaryManager& m_diaryManager;

};

#endif // LOGINVIEWMODEL_H
