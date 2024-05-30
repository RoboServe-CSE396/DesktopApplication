#include "removefromprepearedlist.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>


QString globaldocName1;

removefromprepearedlist::removefromprepearedlist(QObject *parent)
    : QObject{parent}
{}

void removefromprepearedlist::run(QString docName)
{
    globaldocName1 = docName;
    m_networkManager= new QNetworkAccessManager(this);
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/" + docName +".json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&removefromprepearedlist::networkReplyReadyRead);
}

removefromprepearedlist::~removefromprepearedlist()
{
    m_networkManager->deleteLater();
    disconnect (m_networkReply,&QNetworkReply::readyRead,this,&removefromprepearedlist::networkReplyReadyRead);
}

void removefromprepearedlist::networkReplyReadyRead()
{
    QByteArray data = m_networkReply->readAll(); // Read the data
    QJsonDocument doc = QJsonDocument::fromJson(data); // Create a JSON document

    if (!doc.isObject()) {
        //qDebug() << "Data is not an object!";
        return;
    }

    QJsonObject obj = doc.object(); // Get the JSON object

    // Create a QMap to store documents sorted by timestamp
    QMap<double, QString> sortedDocuments;

    // Iterate over the keys in the object
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("orderStatus").toString();

        // Check if the OrderStatus is "Ready"
        if (OrderStatus == "Ready") {
            double timestamp = table.value("timestamp").toDouble();
            sortedDocuments.insert(timestamp, key);
        }
    }

    // Check if there are any documents
    if (sortedDocuments.isEmpty()) {
        qDebug() << "No documents found!";
        return;
    }

    // Remove the first document (the one with the earliest timestamp)
    double timestampToRemove = sortedDocuments.constBegin().key();
    QString documentIdToRemove = sortedDocuments.constBegin().value();
    sortedDocuments.remove(timestampToRemove);

    // Process the removed document
    QString FromWhichTable = obj.value(documentIdToRemove).toObject().value("fromWhichTable").toString();
    int OrderId = obj.value(documentIdToRemove).toObject().value("OrderID").toInt();
    processDocument(documentIdToRemove, FromWhichTable, OrderId, timestampToRemove);

    // Optionally, update the collection to reflect the removal of the processed document
    // You can send a request to delete the document from the collection
}

//Sipariş teslim edildikten sonra silinmesini sağlayan fonksiyon

void removefromprepearedlist::processDocument(const QString& documentId,QString FromWhichTable,int OrderId,double timestamp)
{


    //Bu blokta new table oluşturuluyor
    m_networkManager= new QNetworkAccessManager(this);
    QVariantMap newTable;
    newTable["fromWhichTable"] = FromWhichTable;
    newTable["OrderID"] = OrderId;
    newTable["timestamp"] = timestamp;
    newTable["orderStatus"] = "Ready";
    qDebug()<<"Order is like that " <<OrderId;
    QVariantMap anotherCollectionData = newTable;

    // Implement your logic to process the document with the given ID
    qDebug() << "Processing document:" << documentId;

    // Send a DELETE request to remove the document from the collection.
    //İlgili url'deki dosya için QNetworkRequest objesi oluşturuluyor
    QNetworkAccessManager* deleteManager = new QNetworkAccessManager(this);
        QNetworkRequest deleteRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/" + globaldocName1 + "/" + documentId + ".json"));



    if(globaldocName1 == "Orders"){
        // yukardaki table currentlyProcessedOrder collection'a ekleniyor
        QJsonDocument anotherCollectionJsonDoc = QJsonDocument::fromVariant(anotherCollectionData);
        QNetworkRequest anotherCollectionRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/goals.json"));
        anotherCollectionRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
        m_networkManager->post(anotherCollectionRequest, anotherCollectionJsonDoc.toJson());
    }



    updateCurrentlyProcessedOrder(OrderId);


    deleteManager->deleteResource(deleteRequest);
    connect(deleteManager, &QNetworkAccessManager::finished, this, &removefromprepearedlist::deleteRequestFinished);
}


void removefromprepearedlist::updateCurrentlyProcessedOrder(int OrderId)
{
    // Send a GET request to retrieve all documents from currentlyProcessedOrder collection
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/currentlyProcessedOrder.json"));
    QNetworkReply* reply = manager->get(request);

    // Connect to handle the reply
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll(); // Read the data from the reply
        QJsonDocument doc = QJsonDocument::fromJson(data); // Create a JSON document

        if (!doc.isObject()) {
            //qDebug() << "Data is not an object!";
            reply->deleteLater();
            manager->deleteLater();
            return;
        }

        QJsonObject obj = doc.object(); // Get the JSON object

        QString documentIdToUpdate;
        // Iterate over the keys in the object
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            QString key = it.key();
            QJsonObject document = it.value().toObject();
            int orderIdInDocument = document.value("OrderID").toInt();

            // Check if the OrderId in the document matches the specified OrderId
            if (orderIdInDocument == OrderId) {
                documentIdToUpdate = key;
                break; // Stop searching after finding the matching document
            }
        }

        if (documentIdToUpdate.isEmpty()) {
            //qDebug() << "OrderId not found in currentlyProcessedOrder collection!";
            reply->deleteLater();
            manager->deleteLater();
            return;
        }

        // Update the OrderStatus to "OnItsWay"
        QJsonObject updateObj;
        updateObj["orderStatus"] = "OnItsWay";
        QJsonDocument updateDoc(updateObj);

        QNetworkRequest updateRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/currentlyProcessedOrder/" + documentIdToUpdate + ".json"));
        updateRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

        // Use sendCustomRequest to perform a PATCH request
        QNetworkReply* updateReply = manager->sendCustomRequest(updateRequest, "PATCH", updateDoc.toJson());

        connect(updateReply, &QNetworkReply::finished, [=]() {
            if (updateReply->error() == QNetworkReply::NoError) {
                qDebug() << "OrderStatus updated to OnItsWay!";
            } else {
                qDebug() << "Failed to update OrderStatus: " << updateReply->errorString();
            }
            updateReply->deleteLater();
            manager->deleteLater();
        });

        reply->deleteLater();
    });
}


void removefromprepearedlist::deleteRequestFinished()
{

}
