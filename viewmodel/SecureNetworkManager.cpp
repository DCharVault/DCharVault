#include "SecureNetworkManager.h"

#include<QMetaObject>
#include<QString>

// BlockedNetworkReply
BlockedNetworkReply::BlockedNetworkReply(const QNetworkRequest &req, QNetworkAccessManager::Operation op, QObject *parent)
    : QNetworkReply(parent)
{
    setRequest(req);
    setOperation(op);

    setError(QNetworkReply::ContentAccessDenied,"Blocked for Privacy");

    setOpenMode(QIODevice::ReadOnly);

    QMetaObject::invokeMethod(this, [this]() {
        emit errorOccurred(QNetworkReply::ContentAccessDenied);
        emit finished();
    }, Qt::QueuedConnection);
}

void BlockedNetworkReply::abort(){}

qint64 BlockedNetworkReply::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return -1;
}





// SecureNetworkManager
SecureNetworkManager::SecureNetworkManager(bool blockRemote, QObject *parent)
    :QNetworkAccessManager(parent), m_blockRemote(blockRemote)
{}

void SecureNetworkManager::setBlockRemote(bool block)
{
    m_blockRemote = block;
}

QNetworkReply *SecureNetworkManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    if (!m_blockRemote) {
        return QNetworkAccessManager::createRequest(op, request, outgoingData);
    }

    QString scheme = request.url().scheme().toLower();
    if (scheme == QLatin1String("qrc") || scheme == QLatin1String("data") || scheme.isEmpty()){
        return QNetworkAccessManager::createRequest(op, request, outgoingData);
    }else{
        qWarning() << "SECURITY: Blocked unauthorized network request to:" << request.url().toString();
        return new BlockedNetworkReply(request, op, this);
    }
}





// SecureNetworkManagerFactory
SecureNetworkManagerFactory::SecureNetworkManagerFactory(bool blockRemote) : m_blockRemote(blockRemote)
{}

void SecureNetworkManagerFactory::setBlockRemote(bool block)
{
    m_blockRemote = block;
}
bool SecureNetworkManagerFactory::blockRemote() const
{
    return m_blockRemote;
}

QNetworkAccessManager *SecureNetworkManagerFactory::create(QObject *parent)
{
    return new SecureNetworkManager(m_blockRemote,parent);
}
