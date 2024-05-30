#ifndef GETNEXTORDERTOPROCESS_H
#define GETNEXTORDERTOPROCESS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class getnextordertoprocess : public QObject
{
    Q_OBJECT
public:
    void deleteDocument(const QString &documentId);
    void deleteRequestFinished(QNetworkReply *reply);
    void handleInitialPosition(QString FromWhichTable,double OrderId,QJsonArray orderList ,QString OrderStatus,double Price );
    explicit getnextordertoprocess(QObject *parent = nullptr);
    ~getnextordertoprocess();
public slots:
    void networkReplyReadyRead();
signals:

private:

    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;
};

#endif // GETNEXTORDERTOPROCESS_H
