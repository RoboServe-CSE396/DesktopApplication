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
        qDebug() << "Data is not an object!";
        return;
    }

    QJsonObject obj = doc.object(); // Get the JSON object

    // Create a QMap to store documents sorted by timestamp
    QMap<QDateTime, QString> sortedDocuments;

    // Iterate over the keys in the object
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("OrderStatus").toString();

        // Check if the OrderStatus is "Ready"
        if (OrderStatus == "Ready") {
            QDateTime timestamp = QDateTime::fromString(table.value("timestamp").toString(), Qt::ISODate);
            sortedDocuments.insert(timestamp, key);
        }
    }

    // Check if there are any documents
    if (sortedDocuments.isEmpty()) {
        qDebug() << "No documents found!";
        return;
    }

    // Remove the first document (the one with the earliest timestamp)
    QDateTime timestampToRemove = sortedDocuments.constBegin().key();
    QString documentIdToRemove = sortedDocuments.constBegin().value();
    sortedDocuments.remove(timestampToRemove);

    // Process the removed document
    QString FromWhichTable = obj.value(documentIdToRemove).toObject().value("FromWhichTable").toString();
    double OrderId = obj.value(documentIdToRemove).toObject().value("OrderId").toDouble();
    processDocument(documentIdToRemove, FromWhichTable, OrderId, timestampToRemove);

    // Optionally, update the collection to reflect the removal of the processed document
    // You can send a request to delete the document from the collection
}

void removefromprepearedlist::processDocument(const QString& documentId,QString FromWhichTable,double OrderId,QDateTime timestamp)
{
    //Bu blokta new table oluşturuluyor
    m_networkManager= new QNetworkAccessManager(this);
    QVariantMap newTable;
    newTable["FromWhichTable"] = FromWhichTable;
    newTable["OrderId"] = OrderId;
    newTable["timestamp"] = timestamp;

    QVariantMap anotherCollectionData = newTable;

    // Implement your logic to process the document with the given ID
    qDebug() << "Processing document:" << documentId;

    // Send a DELETE request to remove the document from the collection
    QNetworkAccessManager* deleteManager = new QNetworkAccessManager(this);
    QNetworkRequest deleteRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/" + globaldocName1 + "/" + documentId + ".json"));

    if(globaldocName1 == "Orders"){
        // yukardaki table currentlyProcessedOrder collection'a ekleniyor
        QJsonDocument anotherCollectionJsonDoc = QJsonDocument::fromVariant(anotherCollectionData);
        QNetworkRequest anotherCollectionRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/goals.json"));
        anotherCollectionRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
        m_networkManager->post(anotherCollectionRequest, anotherCollectionJsonDoc.toJson());
    }


    deleteManager->deleteResource(deleteRequest);
    connect(deleteManager, &QNetworkAccessManager::finished, this, &removefromprepearedlist::deleteRequestFinished);
}

void removefromprepearedlist::deleteRequestFinished()
{

}
