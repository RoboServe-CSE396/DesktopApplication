#include "getnextorder.h"
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
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/Orders.json")));
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
        qDebug() << "Veri bir nesne değil!";
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
        QString OrderStatus = table.value("OrderStatus").toString();
        if (OrderStatus == "Waiting") {
            // Extract timestamp from the document
            QDateTime timestamp = QDateTime::fromString(table.value("timestamp").toString(), Qt::ISODate);
            // Insert the document ID and timestamp into the QMap
            sortedDocuments.insert(timestamp, key);
        }
    }


    /* //burda timestampe göre doğru mu sıralanmış diye print etmiştim
    // Process the documents in the order of their timestamps
    for (auto it = sortedDocuments.constBegin(); it != sortedDocuments.constEnd(); ++it) {
        QString documentId = it.value();
        QJsonObject table = obj.value(documentId).toObject();
        QString FromWhichTable = table.value("FromWhichTable").toString();
        double OrderId = table.value("OrderId").toDouble();
        QJsonArray orderList = table.value("OrderList").toArray();
        QString OrderStatus = table.value("OrderStatus").toString();
        double Price = table.value("Price").toDouble();

        // Extract the timestamp from the sorted QMap
        QDateTime timestamp = it.key();

        // Print the timestamp
        qDebug() << "Timestamp:" << timestamp.toString(Qt::ISODate); // Print timestamp in ISO 8601 format

        // Print the details of the document
        qDebug() << "Document ID:" << documentId;
        qDebug() << "From Which Table:" << FromWhichTable;
        qDebug() << "Order ID:" << OrderId;
        qDebug() << "Order List:" << orderList;
        qDebug() << "Order Status:" << OrderStatus;
        qDebug() << "Price:" << Price;
    }*/



        //Sorted documenttaki ilk değer ilk siparişi içeriyor. Onun içib burda ilk document alınıp handleInitial Position fonksiyonuna gönderiliyor
        QString documentId = sortedDocuments.constBegin().value();
        QJsonObject table = obj.value(documentId).toObject();
        QString FromWhichTable = table.value("FromWhichTable").toString();
        double OrderId = table.value("OrderId").toDouble();
        QJsonArray orderList = table.value("OrderList").toArray();
        QString OrderStatus = table.value("OrderStatus").toString();
        double Price = table.value("Price").toDouble();
        // Extract the timestamp from the sorted QMap
        QDateTime timestamp = sortedDocuments.constBegin().key();

        handleInitialPosition(FromWhichTable,OrderId ,orderList ,OrderStatus,Price,documentId,timestamp);

/*
    int i=0;
    // Her bir tablo için işleme yap
    foreach (const QString &key, obj.keys()) {
        QString documentId = key;
        qDebug()<<documentId;

        QJsonObject table = obj.value(key).toObject(); // Tablo nesnesini al


        //Masa bilgisi
        QString FromWhichTable = table.value("FromWhichTable").toString();
        double OrderId = table.value("OrderId").toDouble(); //Order id
        QJsonArray orderList = table.value("OrderList").toArray();  // Assume 'table' is a QJsonObject containing the data
        QString OrderStatus = table.value("OrderStatus").toString();
        double Price = table.value("Price").toDouble();

        if(OrderStatus == "Waiting")
        {

            i=-1;
             handleInitialPosition(FromWhichTable,OrderId ,orderList ,OrderStatus,Price,documentId);
            break;

        }


    }*/

}





void getnextorder::handleInitialPosition(QString FromWhichTable,double OrderId,QJsonArray orderList ,QString OrderStatus,double Price ,QString (firstDoc),QDateTime timestamp)
{

    //Bu blokta new table oluşturuluyor
    m_networkManager= new QNetworkAccessManager(this);
    QVariantMap newTable;
    newTable["FromWhichTable"] = FromWhichTable;
    newTable["OrderId"] = OrderId;
    newTable["OrderStatus"] = "Being processed";
    newTable["OrderList"] = orderList;
    newTable["Price"] = Price;
    newTable["timestamp"] = timestamp;

     QVariantMap anotherCollectionData = newTable;

    //oluşturulan table ilgili url'ye yazdırılıyor, order status being proccesed olarak güncelleniyor
    QJsonDocument jsonDoc=QJsonDocument::fromVariant(newTable);
    QNetworkRequest newPetRequest(QString("https://roboserve-fd298-default-rtdb.firebaseio.com/Orders/%1.json").arg(firstDoc));
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
    QString line =  "Table No: " + FromWhichTable + ", Order Id: " + QString::number(OrderId) + ", Order Items: " + orderItems + ", Status: " + OrderStatus + ", Price: " + QString::number(Price);
    qDebug() <<line;



}
