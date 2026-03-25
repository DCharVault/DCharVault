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

QByteArray EncryptionManager::generateRandomBytes(size_t length){
    QByteArray buffer;
    buffer.resize(length);
    randombytes_buf(buffer.data(),length);
    return buffer;
}

SecureVector EncryptionManager::deriveMasterKey(const SecureString &password, const QByteArray &salt)
{
    SecureVector key(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
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

QByteArray EncryptionManager::encryptString(const QString &inputString, const SecureVector &masterKey) {
    QByteArray utf8String = inputString.toUtf8();

    QByteArray nonce(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES, Qt::Uninitialized);
    randombytes_buf(nonce.data(), nonce.size());

    QByteArray cipherText(utf8String.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES, Qt::Uninitialized);
    unsigned long long cipherText_len;

    if(crypto_aead_xchacha20poly1305_ietf_encrypt(
            reinterpret_cast<unsigned char*>(cipherText.data()), &cipherText_len,
            reinterpret_cast<const unsigned char*>(utf8String.constData()), utf8String.size(),
            nullptr, 0, nullptr,
            reinterpret_cast<const unsigned char*>(nonce.constData()),
            masterKey.data()) < 0) {
        qCritical() << "Fatal: Failed to Encrypt String.\n";
        return QByteArray();
    }

    cipherText.resize(cipherText_len);
    qDebug() << "SUCCESS: String Encrypted!";
    return nonce + cipherText;
}

QString EncryptionManager::decryptString(const QByteArray &inputBytes, const SecureVector &masterKey){
    // It must contain at least a Nonce (24) and a MAC Tag (16).
    if (inputBytes.size() < crypto_aead_xchacha20poly1305_ietf_NPUBBYTES + crypto_aead_xchacha20poly1305_ietf_ABYTES) {
        qCritical() << "Fatal: Ciphertext is too short. Data is corrupted.";
        return QString();
    }

    QByteArray nonce = inputBytes.left(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
    QByteArray cipherText = inputBytes.mid(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
    QByteArray decryptedBytes(cipherText.size() - crypto_aead_xchacha20poly1305_ietf_ABYTES, Qt::Uninitialized);
    unsigned long long decryptedBytes_len;
    if(crypto_aead_xchacha20poly1305_ietf_decrypt(
            reinterpret_cast<unsigned char*>(decryptedBytes.data()), &decryptedBytes_len,
            nullptr,
            reinterpret_cast<const unsigned char*>(cipherText.constData()), cipherText.size(),
            nullptr,0,
            reinterpret_cast<const unsigned char*>(nonce.constData()),
            masterKey.data())!=0){
        qCritical() << "Fatal: Failed to Decrypt String. Authentication tag mismatched!\n";
        return QString();
    }
    decryptedBytes.resize(decryptedBytes_len);
    qDebug() << "SUCCESS: String Decrypted!";

    return QString::fromUtf8(decryptedBytes);
}
