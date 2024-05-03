#ifndef ORDERSTATUSREADY_H
#define ORDERSTATUSREADY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class orderstatusready : public QObject
{
    Q_OBJECT
public:
    void run(QString docName);

    void handleInitialPosition(QString FromWhichTable,double OrderId,QJsonArray orderList ,QString OrderStatus,double Price,QString firstDoc ,QDateTime timestamp);

    explicit orderstatusready(QObject *parent = nullptr);
     ~orderstatusready();
signals:

private:
    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;

public slots:
    void networkReplyReadyRead();

};

#endif // ORDERSTATUSREADY_H
