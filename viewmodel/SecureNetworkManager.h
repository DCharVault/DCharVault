#ifndef SECURENETWORKMANAGER_H
#define SECURENETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQmlNetworkAccessManagerFactory>

class BlockedNetworkReply : public QNetworkReply{
    Q_OBJECT
public:
    BlockedNetworkReply(const QNetworkRequest &req,QNetworkAccessManager::Operation op, QObject *parent = nullptr);

    void abort() override;
protected:
    qint64 readData(char *data, qint64 maxlen) override;


};


class SecureNetworkManager : public QNetworkAccessManager{
    Q_OBJECT
public:
    SecureNetworkManager(QObject *parent = nullptr);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData=nullptr) override;
};


class SecureNetworkManagerFactory : public QQmlNetworkAccessManagerFactory{
public:
    QNetworkAccessManager *create(QObject *parent) override;
};

#endif // SECURENETWORKMANAGER_H
