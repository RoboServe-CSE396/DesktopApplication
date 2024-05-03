#include "getnextordertoprocess.h"
#include <QJsonArray>
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>



getnextordertoprocess::getnextordertoprocess(QObject *parent)
    : QObject{parent}
{
    m_networkManager= new QNetworkAccessManager(this);
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/Orders.json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&getnextordertoprocess::networkReplyReadyRead);
}
getnextordertoprocess::~getnextordertoprocess()
{
    m_networkManager->deleteLater();
}
void getnextordertoprocess::networkReplyReadyRead()
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



    int i=0;
    // Her bir tablo için işleme yap
    foreach (const QString &key, obj.keys()) {
        QJsonObject table = obj.value(key).toObject(); // Tablo nesnesini al


        //Masa bilgisi
        QString FromWhichTable = table.value("FromWhichTable").toString();
        double OrderId = table.value("OrderId").toDouble(); //Order id
        QJsonArray orderList = table.value("OrderList").toArray();  // Assume 'table' is a QJsonObject containing the data
        QString OrderStatus = table.value("OrderStatus").toString();
        double Price = table.value("Price").toDouble();

        handleInitialPosition(FromWhichTable,OrderId ,orderList ,OrderStatus,Price);

        //order id string olarak tutuluyor
        //QString orderIdString = QString::number(OrderId);


        // Burada bu değerleri kullanabilirsiniz
        //qDebug() <<FromWhichTable ;
        //qDebug()  << orderIdString;

        if(i==0)
        {
            break;
        }
        i++;

        }



        //QString priceString = "Price: " + QString::number(Price);
        //qDebug() <<" OrderStatus:" << OrderStatus;
        //qDebug() <<  priceString;
        //qDebug() <<"";




    }





void getnextordertoprocess::handleInitialPosition(QString FromWhichTable,double OrderId,QJsonArray orderList ,QString OrderStatus,double Price ){

    m_networkManager= new QNetworkAccessManager(this);
     QVariantMap newTable;
    newTable["FromWhichTable"] = FromWhichTable;
    newTable["OrderId"] = OrderId;
    newTable["OrderStatus"] = "Being processed";
    newTable["orderList"] = orderList;
    newTable["Price"] = Price;

    QJsonDocument jsonDoc=QJsonDocument::fromVariant(newTable);
    QNetworkRequest newPetRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/Orders.json"));
    newPetRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkManager->post(newPetRequest,jsonDoc.toJson());

    QString orderItems = "Order List: ";

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
    QString line =  "Table No: " + FromWhichTable + ", Order Id: " + QString::number(OrderId) + ", Order Items: " + orderItems + ", Status: " + OrderStatus + ", Price" + QString::number(Price);
    qDebug() <<line;



}

