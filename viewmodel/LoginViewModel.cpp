#include"LoginViewModel.h"
#include<QDebug>


LoginViewModel::LoginViewModel(DiaryManager &manager, QObject *parent)
    : QObject(parent), m_diaryManager(manager){}


void LoginViewModel::attemptLogin(const QString &password){
    qDebug() << "ViewModel received login attempt from QML UI.";

    //hard coded assumption: database exists here
    QString dbPath = "secure_vault.db";

    if(m_diaryManager.openDiary(dbPath,password.toStdString())==DiaryError::None){
        qDebug() << "ViewModel: Login successful. Firing success signal to QML.";
        emit loginSuccess();
    }else{
        qDebug() << "ViewModel: Login failed. Firing failure signal to QML.";
        emit loginFailed();
    }
}
