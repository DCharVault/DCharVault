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

private:
};

#endif // ENCRYPTIONMANAGER_H
