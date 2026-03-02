#include"EncryptionManager.h"

#include<QDebug>

EncryptionManager::EncryptionManager(){}

bool EncryptionManager::initialize(){
    if(sodium_init()<0){
        qCritical()<<"Fatal: Library Sodium.h couldn't be initialized.\n";
        return false;
    }
    qDebug()<<"SUCCESS: Libsodium initialized.\n";
    return true;
}

QByteArray EncryptionManager::generateSalt(){
    QByteArray salt(crypto_pwhash_SALTBYTES,Qt::Uninitialized);
    randombytes_buf(salt.data(),salt.size());
    return salt;
}

std::vector<uint8_t> EncryptionManager::deriveMasterKey(const std::string &password, const QByteArray &salt)
{
    std::vector<uint8_t> key(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
    if(crypto_pwhash(key.data(),key.size(),
                      password.c_str(),password.length(),
                      reinterpret_cast<const unsigned char*>(salt.constData()),
                      crypto_pwhash_OPSLIMIT_INTERACTIVE,
                      crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_ARGON2ID13)<0){
        qCritical()<<"Out of Memory! Hashing Failed\n";
        return {};
    }
    qDebug()<<"SUCCESS: Derived Master Key.\n";
    return key;
}
