#ifndef REMOVEFROMPREPEAREDLIST_H
#define REMOVEFROMPREPEAREDLIST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>

class removefromprepearedlist : public QObject
{
    Q_OBJECT
public:
    void run(QString docName);
    explicit removefromprepearedlist(QObject *parent = nullptr);
    ~removefromprepearedlist();
signals:

public slots:
    void networkReplyReadyRead();
    void processDocument(const QString& documentId, QString FromWhichTable, double OrderId, QDateTime timestamp);


    void deleteRequestFinished();

private:
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_networkReply;
};

#endif // REMOVEFROMPREPEAREDLIST_H
