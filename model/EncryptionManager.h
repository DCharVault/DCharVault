#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <sodium.h>

#include<QByteArray>
#include<string>
#include<vector>
#include<cstdint>

class EncryptionManager{
public:
    EncryptionManager();

    bool initialize();

    QByteArray generateSalt();

    std::vector<uint8_t> deriveMasterKey(const std::string& password, const QByteArray& salt);

    QByteArray encryptString(const QString& inputString, const std::vector<uint8_t>& masterKey);
    QString decryptString(const QByteArray& inputBytes, const std::vector<uint8_t>& masterkey);

private:
};

#endif // ENCRYPTIONMANAGER_H
