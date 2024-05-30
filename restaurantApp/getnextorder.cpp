#include "getnextorder.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

getnextorder::getnextorder(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{

}

void getnextorder::run()
{
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/orders.json")));
    connect(m_networkReply, &QNetworkReply::readyRead, this, &getnextorder::networkReplyReadyRead);
}

getnextorder::~getnextorder()
{
    m_networkManager->deleteLater();
    disconnect(m_networkReply, &QNetworkReply::readyRead, this, &getnextorder::networkReplyReadyRead);
}

void getnextorder::networkReplyReadyRead()
{
    qDebug() << "Get next order to process";

    QByteArray data = m_networkReply->readAll(); // read the data
    QJsonDocument doc = QJsonDocument::fromJson(data); // create JSON document

    if (!doc.isObject()) {
        //qDebug() << "Data is not an object!";
        return;
    }

    QJsonObject obj = doc.object(); // get the object

    // Create a QMap to store documents sorted by timestamp
    QMap<double, QString> sortedDocuments;

    // Iterate over the keys in the object . Waiting statusunde olan documentları sorted arraye timestampe göre assign ediyor
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("orderStatus").toString();
        if (OrderStatus == "Waiting") {
            // Extract timestamp from the document
            double timestamp = table.value("timestamp").toDouble();
            qDebug() << timestamp << "\n";
            // Insert the document ID and timestamp into the QMap
            sortedDocuments.insert(timestamp, key);
        }
    }

    // Sorted documenttaki ilk değer ilk siparişi içeriyor. Onun içib burda ilk document alınıp handleInitial Position fonksiyonuna gönderiliyor
    QString documentId = sortedDocuments.constBegin().value();
    QJsonObject table = obj.value(documentId).toObject();
    QString FromWhichTable = table.value("fromWhichTable").toString();
    int OrderId = table.value("OrderID").toInt();
    QJsonArray orderList = table.value("OrderList").toArray();
    QString OrderStatus = table.value("orderStatus").toString();
    double Price = table.value("Price").toDouble();
    // Extract the timestamp from the sorted QMap
    double timestamp = sortedDocuments.constBegin().key();

    handleInitialPosition(FromWhichTable, OrderId, orderList, OrderStatus, Price, documentId, timestamp);
}

void getnextorder::handleInitialPosition(QString FromWhichTable, int OrderId, QJsonArray orderList, QString OrderStatus, double Price, QString firstDoc, double timestamp)
{
    qDebug() << "Timestamp: " << timestamp;
    // Create new table data
    QVariantMap newTable;
    newTable["fromWhichTable"] = FromWhichTable;
    newTable["OrderID"] = OrderId;
    newTable["orderStatus"] = "Being processed";
    newTable["OrderList"] = orderList;
    newTable["Price"] = Price;
    newTable["timestamp"] = timestamp;

    // Prepare data for currentlyProcessedOrder collection
    QVariantMap anotherCollectionData = newTable;

    // Update the order status and write the table to the relevant URL
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(newTable);
    QNetworkRequest newPetRequest(QString("https://roboserve-fd298-default-rtdb.firebaseio.com/orders/%1.json").arg(firstDoc));
    newPetRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkManager->put(newPetRequest, jsonDoc.toJson());






     QVariantMap newTable1;
     QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/currentlyProcessedOrder.json")));
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
                     updatedData["orderStatus"] = "Being processed";
                     updatedData["OrderList"] = orderList;
                     updatedData["Price"] = price;
                     updatedData["timestamp"] = timestamp;

                     QJsonDocument updatedDoc(updatedData);
                     QByteArray jsonData = updatedDoc.toJson();

                     QNetworkRequest request(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/currentlyProcessedOrder/" + documentId + ".json"));
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



    // Prepare order items for printing
    QString orderItems;
    for (int i = 0; i < orderList.size(); ++i) {
        // Retrieve the element at index 'i' and convert it to a QJsonValue
        QJsonValue value = orderList.at(i);

        // Convert the QJsonValue to a QString and append it to orderItems
        orderItems.append(value.toString());

        // Append comma and space only if it's not the last element
        if (i < orderList.size() - 1) {
            orderItems.append(", ");
        }
    }

    // Print order details
    QString line = "Table No: " + FromWhichTable + ", Order Id: " + QString::number(OrderId) + ", Order Items: " + orderItems + ", Status: " + OrderStatus + ", Price: " + QString::number(Price);
    qDebug() << line;
}

/*#include "getnextorder.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

getnextorder::getnextorder(QObject *parent)
    : QObject{parent}, m_networkManager(new QNetworkAccessManager(this))
{

}

void getnextorder::run()
{
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/orders.json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&getnextorder::networkReplyReadyRead);
}

getnextorder::~getnextorder()
{
    m_networkManager->deleteLater();
    disconnect (m_networkReply,&QNetworkReply::readyRead,this,&getnextorder::networkReplyReadyRead);
}

void getnextorder::networkReplyReadyRead()
{
    qDebug()<< "Get next order to process";

    QByteArray data = m_networkReply->readAll(); // read the data
    QJsonDocument doc = QJsonDocument::fromJson(data); // create JSON document

    if (!doc.isObject()) {
        //qDebug() << "Data is not an object!";
        return;
    }

    QJsonObject obj = doc.object(); // get the object

    // Create a QMap to store documents sorted by timestamp
    QMap<int, QString> sortedDocuments;


    // Iterate over the keys in the object . Waiting statusunde olan documentları sorted arraye timestampe göre assign ediyor
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("orderStatus").toString();
        if (OrderStatus == "Waiting") {
            // Extract timestamp from the document
            int timestamp = table.value("timestamp").toInt();
            qDebug()<<timestamp+"\n";
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
    int timestamp = sortedDocuments.constBegin().key();

    handleInitialPosition(FromWhichTable, OrderId ,orderList ,OrderStatus, Price, documentId, timestamp);
}

void getnextorder::handleInitialPosition(QString FromWhichTable, int OrderId, QJsonArray orderList, QString OrderStatus, double Price, QString firstDoc, int timestamp)
{
    qDebug() << "Timestamp: " << timestamp;
    // Create new table data
    QVariantMap newTable;
    newTable["fromWhichTable"] = FromWhichTable;
    newTable["OrderID"] = OrderId;
    newTable["orderStatus"] = "Being processed";
    newTable["OrderList"] = orderList;
    newTable["Price"] = Price;
    newTable["timestamp"] = timestamp;

    // Prepare data for currentlyProcessedOrder collection
    QVariantMap anotherCollectionData = newTable;

    // Update the order status and write the table to the relevant URL
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(newTable);
    QNetworkRequest newPetRequest(QString("https://roboserve-fd298-default-rtdb.firebaseio.com/orders/%1.json").arg(firstDoc));
    newPetRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkManager->put(newPetRequest, jsonDoc.toJson());

    // Write the table to currentlyProcessedOrder collection
    QJsonDocument anotherCollectionJsonDoc = QJsonDocument::fromVariant(anotherCollectionData);
    QNetworkRequest anotherCollectionRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/currentlyProcessedOrder.json"));
    anotherCollectionRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkManager->post(anotherCollectionRequest, anotherCollectionJsonDoc.toJson());

    // Prepare order items for printing
    QString orderItems;
    for (int i = 0; i < orderList.size(); ++i) {
        // Retrieve the element at index 'i' and convert it to a QJsonValue
        QJsonValue value = orderList.at(i);

        // Convert the QJsonValue to a QString and append it to orderItems
        orderItems.append(value.toString());

        // Append comma and space only if it's not the last element
        if (i < orderList.size() - 1) {
            orderItems.append(", ");
        }
    }

    // Print order details
    QString line = "Table No: " + FromWhichTable + ", Order Id: " + QString::number(OrderId) + ", Order Items: " + orderItems + ", Status: " + OrderStatus + ", Price: " + QString::number(Price);
    qDebug() << line;
}
*/
/*#include "getnextorder.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

getnextorder::getnextorder(QObject *parent)
    : QObject{parent}
{

}

void getnextorder::run()
{
    m_networkManager= new QNetworkAccessManager(this);
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/orders.json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&getnextorder::networkReplyReadyRead);
}

getnextorder::~getnextorder()
{
    m_networkManager->deleteLater();
    disconnect (m_networkReply,&QNetworkReply::readyRead,this,&getnextorder::networkReplyReadyRead);
}

void getnextorder::networkReplyReadyRead()
{
    qDebug()<< "Get next order to process";

    QByteArray data = m_networkReply->readAll(); // ilk veriyi oku
    QJsonDocument doc = QJsonDocument::fromJson(data); // JSON belgesi oluştur

    if (!doc.isObject()) {
        //qDebug() << "Veri bir nesne değil!";
        return;
    }

    QJsonObject obj = doc.object(); // Nesneyi al
    //QString firstDoc = obj.constBegin().key();
    //qDebug()<<obj.constBegin().key();   //İlk document id'yi bastırıyor


    // Create a QMap to store documents sorted by timestamp
    QMap<QDateTime, QString> sortedDocuments;


    // Iterate over the keys in the object . Waiting statusunde olan documentları sorted arraye timestampe göre assign ediyor
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("orderStatus").toString();
        if (OrderStatus == "Waiting") {
            // Extract timestamp from the document
            QDateTime timestamp = QDateTime::fromString(table.value("timestamp").toString(), Qt::ISODate);
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
        QDateTime timestamp = sortedDocuments.constBegin().key();

        handleInitialPosition(FromWhichTable,OrderId ,orderList ,OrderStatus,Price,documentId,timestamp);


}





void getnextorder::handleInitialPosition(QString FromWhichTable,int OrderId,QJsonArray orderList ,QString OrderStatus,double Price ,QString (firstDoc),QDateTime timestamp)
{
    qDebug()<<timestamp;
    //Bu blokta new table oluşturuluyor
    m_networkManager= new QNetworkAccessManager(this);
    QVariantMap newTable;
    newTable["fromWhichTable"] = FromWhichTable;
    newTable["OrderID"] = OrderId;
    newTable["orderStatus"] = "Being processed";
    newTable["OrderList"] = orderList;
    newTable["Price"] = Price;
    newTable["timestamp"] = timestamp;

     QVariantMap anotherCollectionData = newTable;

    //oluşturulan table ilgili url'ye yazdırılıyor, order status being proccesed olarak güncelleniyor
    QJsonDocument jsonDoc=QJsonDocument::fromVariant(newTable);

     qDebug()<<"firstdocs "<<firstDoc;
    QNetworkRequest newPetRequest(QString("https://roboserve-fd298-default-rtdb.firebaseio.com/orders/%1.json").arg(firstDoc));
    newPetRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkManager->put(newPetRequest,jsonDoc.toJson());


    // yukardaki table currentlyProcessedOrder collection'a ekleniyor
    QJsonDocument anotherCollectionJsonDoc = QJsonDocument::fromVariant(anotherCollectionData);
    QNetworkRequest anotherCollectionRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/currentlyProcessedOrder.json"));
    anotherCollectionRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkManager->post(anotherCollectionRequest, anotherCollectionJsonDoc.toJson());


    QString orderItems ;

    for (int i = 0; i < orderList.size(); ++i) {
        // Retrieve the element at index 'i' and convert it to a QJsonValue
        QJsonValue value = orderList.at(i);

        // Convert the QJsonValue to a QString and print it
        //qDebug() << "Order List Item[" << i << "]: " << value.toString();
        orderItems.append(value.toString());

        // Append comma and space only if it's not the last element
        if (i < orderList.size() - 1) {
            orderItems.append(", ");
        }
    }
    QString line =  "Table No: " + FromWhichTable + ", Order Id: " + QString::number(OrderId)+ ", Order Items: " + orderItems + ", Status: " + OrderStatus + ", Price: " + QString::number(Price);
    qDebug() <<line;



}
*/
