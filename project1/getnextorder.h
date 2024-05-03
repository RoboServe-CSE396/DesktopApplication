#ifndef GETNEXTORDER_H
#define GETNEXTORDER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class getnextorder : public QObject
{
    Q_OBJECT
public:
    void run();
    explicit getnextorder(QObject *parent = nullptr);
    void handleInitialPosition(QString FromWhichTable,double OrderId,QJsonArray orderList ,QString OrderStatus,double Price,QString firstDoc ,QDateTime timestamp);

    ~getnextorder();
public slots:
    void networkReplyReadyRead();
signals:

private:

    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;
signals:
};

#endif // GETNEXTORDER_H
