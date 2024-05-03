// paymentwithid.cpp

#include "paymentwithid.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

paymentwithid::paymentwithid(QObject *parent)
    : QObject{parent}, removefromprepearedlistInstance(new removefromprepearedlist(this))
{}

paymentwithid::~paymentwithid()
{
    delete removefromprepearedlistInstance;
}

void paymentwithid::removeDocumentWithOrderId(double OrderId)
{
    // Send a GET request to retrieve all documents from alreadyDeliveredOrders collection
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/AlreadyDeliveredOrder.json"));
    QNetworkReply* reply = manager->get(request);

    // Connect to handle the reply
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll(); // Read the data from the reply
        QJsonDocument doc = QJsonDocument::fromJson(data); // Create a JSON document

        if (!doc.isObject()) {
            qDebug() << "Data is not an object!";
            return;
        }

        QJsonObject obj = doc.object(); // Get the JSON object

        // Iterate over the keys in the object
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            QString key = it.key();
            QJsonObject document = it.value().toObject();
            double orderIdInDocument = document.value("OrderId").toDouble();

            // Check if the OrderId in the document matches the specified OrderId
            if (orderIdInDocument == OrderId) {
                // Send a DELETE request to remove the document
                QNetworkAccessManager* deleteManager = new QNetworkAccessManager(this);
                QNetworkRequest deleteRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/AlreadyDeliveredOrder/" + key + ".json"));
                QNetworkReply* deleteReply = deleteManager->deleteResource(deleteRequest);
                connect(deleteReply, &QNetworkReply::finished, removefromprepearedlistInstance, &removefromprepearedlist::deleteRequestFinished);

                return; // Stop searching after finding the matching document
            }
        }

        qDebug() << "Document with OrderId" << OrderId << "not found in alreadyDeliveredOrders collection!";
        reply->deleteLater(); // Clean up
    });
}
