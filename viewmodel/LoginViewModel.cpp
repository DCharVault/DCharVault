#include"LoginViewModel.h"
#include<QUrl>
#include<QDir>
#include<QStandardPaths>
#include<QDebug>

#include "SecurePasswordInput.h"


#include <QWindow>
#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif


LoginViewModel::LoginViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_diaryManager(manager),m_sanitizer(30000,this){}

void LoginViewModel::authenticate(SecurePasswordInput *passwordField, const QString& dbUrl){
    if (!passwordField) {
        qCritical() << "Error: Password field is null.";
        return;
    }

    // convert qml file: //url to a native OS file path
    const QString localFilePath = QUrl(dbUrl).isLocalFile() ? QUrl(dbUrl).toLocalFile() : dbUrl;
    if(localFilePath.isEmpty()){
        qCritical()<<"Error: No database file selected";
        emit dbNotFound();
        return;
    }

    bool openState = (m_diaryManager.openDiary("\0",localFilePath, passwordField->getSecureBuffer()) == DiaryError::None); // for auth we do not need filename
    passwordField->clearPassword();
    if(openState){
        qDebug() << "ViewModel: Login successful. Firing success signal to QML.";
        emit loginSuccess();
    }else{
        qDebug() << "ViewModel: Login failed. Firing failure signal to QML.";
        emit loginFailed();
    }
}

void LoginViewModel::updateTitleBar(bool isDark) {
#ifdef Q_OS_WIN
    // We get the active window from the application
    for (QWindow *window : QGuiApplication::allWindows()) {
        if (window->winId()) {
            HWND hwnd = (HWND)window->winId();
            BOOL dark = isDark ? TRUE : FALSE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        }
    }
#endif
}

void LoginViewModel::createVault(const QString& newJournalName,SecurePasswordInput *passwordField, const QString &dbUrl){
    if(!passwordField){
        qCritical() << "Error: Password field is null.";
        return;
    }
    // convert qml file: //url to a native OS file path
    const QString localFilePath = QUrl(dbUrl).isLocalFile() ? QUrl(dbUrl).toLocalFile() : dbUrl;
    if(localFilePath.isEmpty()){
        qCritical()<<"Error: No database file selected";
        emit dbNotFound();
        return;
    }
    bool openState = (m_diaryManager.openDiary(newJournalName,localFilePath, passwordField->getSecureBuffer()) == DiaryError::None);
    passwordField->clearPassword();
    if(openState){
        qDebug() << "ViewModel: Login successful. Firing success signal to QML.";
        emit loginSuccess();
    }else{
        qDebug() << "ViewModel: Login failed. Firing failure signal to QML.";
        emit loginFailed();
    }
}

void LoginViewModel::createVaultAndroid(const QString &journalName, SecurePasswordInput *passwordField){
    if(!passwordField){
        qCritical() << "Error: Password field is null.";
        return;
    }
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir dir(docsPath);
    dir.mkdir("DCharVault");

    QString safeName = journalName.trimmed();

    if(!safeName.endsWith(".db")){
        safeName+=".db";
    }

    QString fullPath = docsPath + "/DCharVault/" + safeName;

    bool createState = (m_diaryManager.openDiary(journalName, fullPath, passwordField->getSecureBuffer()) == DiaryError::None);
    passwordField->clearPassword();

    if(createState){
        qDebug() << "ViewModel: Login successful. Firing success signal to QML.";
        emit loginSuccess();
    }else{
        qDebug() << "ViewModel: Login failed. Firing failure signal to QML.";
        emit loginFailed();
    }
}
