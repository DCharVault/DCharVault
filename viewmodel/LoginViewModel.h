#ifndef LOGINVIEWMODEL_H
#define LOGINVIEWMODEL_H

#include"model/DiaryManager.h"
#include"ClipboardSanitizer.h"
#include<QObject>
#include<QString>

class SecurePasswordInput;

class LoginViewModel : public QObject{
    Q_OBJECT
    Q_PROPERTY(ClipboardSanitizer* sanitizer READ sanitizer CONSTANT)

public:
    explicit LoginViewModel(DiaryManager& manager,QObject *parent=nullptr);

<<<<<<< HEAD
    ClipboardSanitizer* sanitizer() {return &m_sanitizer;}

    // Q_INVOKABLE makes this function callable directly from QML button onClicked
=======
>>>>>>> 855b2e4 (UI Logic For Journal Creation)
    Q_INVOKABLE void authenticate(SecurePasswordInput* passwordField, const QString& dbUrl);
    Q_INVOKABLE void updateTitleBar(bool isDark);

    Q_INVOKABLE void createVault(SecurePasswordInput* passwordField, const QString& dbUrl);

    Q_INVOKABLE void createVaultAndroid(const QString& journalName,SecurePasswordInput *passwordField);
signals:
    // Emitted to tell QML to transition to the Home Screen
    void loginSuccess();
    // Emitted to tell QML to show a red error text box
    void loginFailed();

    void dbNotFound();

private:
    DiaryManager& m_diaryManager;
    ClipboardSanitizer m_sanitizer;
};

#endif // LOGINVIEWMODEL_H
