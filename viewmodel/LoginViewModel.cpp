#include"LoginViewModel.h"
#include<QUrl>

#include<QDebug>

#include "SecurePasswordInput.h"

LoginViewModel::LoginViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_diaryManager(manager){}

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

    bool openState = (m_diaryManager.openDiary(localFilePath, passwordField->getSecureBuffer()) == DiaryError::None);
    passwordField->clearPassword();
    if(openState){
        qDebug() << "ViewModel: Login successful. Firing success signal to QML.";
        emit loginSuccess();
    }else{
        qDebug() << "ViewModel: Login failed. Firing failure signal to QML.";
        emit loginFailed();
    }
}
