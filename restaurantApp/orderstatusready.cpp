#include "orderstatusready.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

QString globaldocName;

orderstatusready::orderstatusready(QObject *parent)
    : QObject{parent}
{

}

void orderstatusready::run(QString docName)
{
    globaldocName = docName;
    m_networkManager= new QNetworkAccessManager(this);
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/" + docName + ".json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&orderstatusready::networkReplyReadyRead);



}


orderstatusready::~orderstatusready()
{
    m_networkManager->deleteLater();

    disconnect (m_networkReply,&QNetworkReply::readyRead,this,&orderstatusready::networkReplyReadyRead);

}

void orderstatusready::networkReplyReadyRead()
{
    qDebug()<< "Order status ready";
    QByteArray data = m_networkReply->readAll(); // ilk veriyi oku
    QJsonDocument doc = QJsonDocument::fromJson(data); // JSON belgesi oluştur

    if (!doc.isObject()) {
        //qDebug() << "Veri bir nesne değil!";
        return;
    }

    QJsonObject obj = doc.object(); // Nesneyi al

    // Create a QMap to store documents sorted by timestamp
    QMap<double, QString> sortedDocuments;

    // Iterate over the keys in the object . Waiting statusunde olan documentları sorted arraye timestampe göre assign ediyor
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("orderStatus").toString();
        if (OrderStatus == "Being processed") {

            // Extract timestamp from the document
            double timestamp = table.value("timestamp").toDouble();
            // Insert the document ID and timestamp into the QMap
            sortedDocuments.insert(timestamp, key);
        }
    }
    //Sorted documenttaki ilk değer ilk siparişi içeriyor. Onun içib burda ilk document alınıp handleInitial Position fonksiyonuna gönderiliyor
    QString documentId = sortedDocuments.constBegin().value();
    QJsonObject table = obj.value(documentId).toObject();
    QString FromWhichTable = table.value("fromWhichTable").toString();
    int OrderId = table.value("OrderID").toInt();
    QJsonArray orderList = table.value("OrderList").toArray();
    QString OrderStatus = table.value("orderStatus").toString();
    double Price = table.value("Price").toDouble();
    // Extract the timestamp from the sorted QMap
    double timestamp = sortedDocuments.constBegin().key();

    handleInitialPosition(FromWhichTable,OrderId ,orderList ,OrderStatus,Price,documentId,timestamp);


}
void orderstatusready::handleInitialPosition(QString FromWhichTable,int OrderId,QJsonArray orderList ,QString OrderStatus,double Price ,QString (firstDoc),double timestamp)
{
    // Create a new QNetworkAccessManager for handling network requests
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    //Bu blokta new table oluşturuluyor
    m_networkManager= new QNetworkAccessManager(this);
    QVariantMap newTable;
    newTable["fromWhichTable"] = FromWhichTable;
    newTable["OrderID"] = OrderId;
    newTable["orderStatus"] = "Ready";
    newTable["OrderList"] = orderList;
    newTable["Price"] = Price;
    newTable["timestamp"] = timestamp;

     QVariantMap anotherCollectionData = newTable;

    //oluşturulan table ilgili url'ye yazdırılıyor, order status Ready olarak güncelleniyor
    QJsonDocument jsonDoc=QJsonDocument::fromVariant(newTable);
    QNetworkRequest newPetRequest(QString("https://roboserve-fd298-default-rtdb.firebaseio.com/"+ globaldocName +"/%1.json").arg(firstDoc));
    newPetRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkManager->put(newPetRequest,jsonDoc.toJson());




    QVariantMap newTable1;
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/orders.json")));
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);

            if (!doc.isObject()) {
                qDebug() << "Data is not an object!";
                reply->deleteLater();
                return;
            }

            QJsonObject obj = doc.object();
            QString documentId;
            QString orderStatus;
            int orderId;
            double price;
            QString fromWhichTable;
            QJsonArray orderList;
            double timestamp;

            // Iterate over the keys in the object to find the matching OrderId
            for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
                QString key = it.key();
                QJsonObject table = it.value().toObject();
                int currentOrderId = table.value("OrderID").toInt();
                if (currentOrderId == OrderId) {
                    documentId = key;
                    orderStatus = table.value("orderStatus").toString();
                    orderId = currentOrderId;
                    price = table.value("Price").toDouble();
                    fromWhichTable = table.value("fromWhichTable").toString();
                    orderList = table.value("OrderList").toArray();
                    timestamp = table.value("timestamp").toDouble();

                    // Update the document
                    QJsonObject updatedData;
                    updatedData["fromWhichTable"] = fromWhichTable;
                    updatedData["OrderID"] = orderId;
                    updatedData["orderStatus"] = "Ready";
                    updatedData["OrderList"] = orderList;
                    updatedData["Price"] = price;
                    updatedData["timestamp"] = timestamp;

                    QJsonDocument updatedDoc(updatedData);
                    QByteArray jsonData = updatedDoc.toJson();

                    QNetworkRequest request(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/orders/" + documentId + ".json"));
                    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                    QNetworkReply *updateReply = m_networkManager->put(request, jsonData);
                    connect(updateReply, &QNetworkReply::finished, [=]() {
                        updateReply->deleteLater();
                    });

                    break;
                }
            }

            if (!documentId.isEmpty()) {
                qDebug() << "Document ID for OrderId" << OrderId << "is" << documentId;
                // Now you have updated the document with the new data
            } else {
                qDebug() << "Document ID for OrderId" << OrderId << "not found in currentlyProcessedOrder";
            }
        } else {
            qDebug() << "Error occurred:" << reply->errorString();
        }

        reply->deleteLater();
    });



}

