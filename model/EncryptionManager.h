#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <sodium.h>
#include"SecureAllocator.h"
#include<QByteArray>
#include<string>
#include<vector>
#include<cstdint>

class EncryptionManager{
public:
    EncryptionManager();

    bool initialize();

    QByteArray generateSalt();

    SecureVector deriveMasterKey(const SecureString& password, const QByteArray& salt);

    QByteArray encryptString(const QString& inputString, const SecureVector& masterKey);
    QString decryptString(const QByteArray& inputBytes, const SecureVector& masterkey);

    // generates secure randombytes
    QByteArray generateRandomBytes(size_t length = 32);

private:
};

#endif // ENCRYPTIONMANAGER_H
