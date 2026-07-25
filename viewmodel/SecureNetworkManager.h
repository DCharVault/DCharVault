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
    SecureNetworkManager(bool blockRemote = true, QObject *parent = nullptr);
    void setBlockRemote(bool block);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData=nullptr) override;

private:
    bool m_blockRemote;

};


class SecureNetworkManagerFactory : public QQmlNetworkAccessManagerFactory{
public:
    SecureNetworkManagerFactory(bool blockRemote=true);

    void setBlockRemote(bool block);
    bool blockRemote() const;

    QNetworkAccessManager *create(QObject *parent) override;

private:
    bool m_blockRemote;
};

#endif // SECURENETWORKMANAGER_H

























