#ifndef DATABASEHANDLER1_H
#define DATABASEHANDLER1_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class databasehandler1 : public QObject
{
    Q_OBJECT
public:
    void handleInitialPosition();
    explicit databasehandler1(QObject *parent = nullptr);
    ~databasehandler1();
public slots:
    void networkReplyReadyRead();
signals:

private:

    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;

};

#endif // DATABASEHANDLER_H
