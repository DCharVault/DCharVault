#include "SecureNetworkManager.h"

#include<QTimer>
#include<QString>

// BlockedNetworkReply
BlockedNetworkReply::BlockedNetworkReply(const QNetworkRequest &req, QNetworkAccessManager::Operation op, QObject *parent)
    : QNetworkReply(parent)
{
    setRequest(req);
    setOperation(op);

    setError(QNetworkReply::ContentAccessDenied,"Blocked for Privacy");

    setOpenMode(QIODevice::ReadOnly);
    QTimer::singleShot(0,this,[this](){
       emit errorOccurred(QNetworkReply::ContentAccessDenied);
       emit finished();
    });
}

void BlockedNetworkReply::abort(){}

qint64 BlockedNetworkReply::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return -1;
}





// SecureNetworkManager
SecureNetworkManager::SecureNetworkManager(QObject *parent)
    :QNetworkAccessManager(parent)
{}

QNetworkReply *SecureNetworkManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QString scheme = request.url().scheme().toLower();
    if (scheme == "qrc" || scheme == "file" || scheme == "data"){
        return QNetworkAccessManager::createRequest(op, request, outgoingData);
    }else{
        qWarning() << "SECURITY: Blocked unauthorized network request to:" << request.url().toString();
        return new BlockedNetworkReply(request, op, this);
    }
}





// SecureNetworkManagerFactory
QNetworkAccessManager *SecureNetworkManagerFactory::create(QObject *parent)
{
    return new SecureNetworkManager(parent);
}
