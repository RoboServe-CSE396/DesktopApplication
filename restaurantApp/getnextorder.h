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
    void handleInitialPosition(QString FromWhichTable,int OrderId,QJsonArray orderList ,QString OrderStatus,double Price,QString firstDoc ,double timestamp);
    // Function to update order status in currentlyProcessedOrder collection
    void updateOrderStatus(const QString &orderId);

    ~getnextorder();
public slots:
    void networkReplyReadyRead();
signals:

private:
private:
    //QJsonObject obj;

    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;
signals:
};

#endif // GETNEXTORDER_H
