#ifndef PAYMENTWITHID_H
#define PAYMENTWITHID_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "removefromprepearedlist.h"

class paymentwithid : public QObject
{
    Q_OBJECT
public:
    explicit paymentwithid(QObject *parent = nullptr);
    void removeDocumentWithOrderId(double OrderId);
    ~paymentwithid();

signals:
private:
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_networkReply;
     removefromprepearedlist* removefromprepearedlistInstance;
};

#endif // PAYMENTWITHID_H
