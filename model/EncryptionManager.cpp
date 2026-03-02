#include"EncryptionManager.h"
#include <sodium.h>
#include<QDebug>

EncryptionManager::EncryptionManager(){}

bool EncryptionManager::initialize(){
    if(sodium.init()<0){
        qCritical()<<"Fatal: Library couldn't be initialized.\n";
        return false;
    }
    return true;
}

QByteArray EncryptionManager::generateSalt(){

}
