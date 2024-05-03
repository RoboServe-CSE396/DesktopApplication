#ifndef PRINTORDERS_H
#define PRINTORDERS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class printorders : public QObject
{
    Q_OBJECT
public:
    void handleInitialPosition();
    void run();
    explicit printorders(QObject *parent = nullptr);
    ~printorders();
public slots:
    void networkReplyReadyRead();
signals:

private:

    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;

};

#endif // PRINTORDERS_H
