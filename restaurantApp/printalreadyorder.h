#ifndef PRINTALREADYORDER_H
#define PRINTALREADYORDER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class printalreadyorder : public QObject
{
    Q_OBJECT
public:
    void run();
    void handleInitialPosition();
    explicit printalreadyorder(QObject *parent = nullptr);
    ~printalreadyorder();
public slots:
    void networkReplyReadyRead();
signals:

private:

    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;

};

#endif // PRINTALREADYORDER_H
