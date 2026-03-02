#include"EncryptionManager.h"
#include <sodium.h>
#include<QDebug>

EncryptionManager::EncryptionManager(){}

bool EncryptionManager::initialize(){
    if(sodium_init()<0){
        qCritical()<<"Fatal: Library couldn't be initialized.\n";
        return false;
    }
    return true;
}

QByteArray EncryptionManager::generateSalt(){
    return QByteArray();
}
