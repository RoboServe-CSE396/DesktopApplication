#include "printalreadyorder.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

printalreadyorder::printalreadyorder(QObject *parent)
    : QObject{parent}
{}

void printalreadyorder::run()
{
    m_networkManager= new QNetworkAccessManager(this);
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/AlreadyDeliveredOrder.json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&printalreadyorder::networkReplyReadyRead);
}

printalreadyorder::~printalreadyorder()
{
    m_networkManager->deleteLater();
    // Assuming m_networkReply is your QNetworkReply object and networkReplyReadyRead is the slot in printalreadyorder

    disconnect(m_networkReply, &QNetworkReply::readyRead, this, &printalreadyorder::networkReplyReadyRead);

}
void printalreadyorder::networkReplyReadyRead()
{
    qDebug()<< "Print Already Order";

    QByteArray data = m_networkReply->readAll(); // Tüm veriyi oku
    QJsonDocument doc = QJsonDocument::fromJson(data); // JSON belgesi oluştur

    if (!doc.isObject()) {
        qDebug() << "Veri bir nesne değil!";
        return;
    }

    QJsonObject obj = doc.object(); // Nesneyi al


    // Her bir tablo için işleme yap
    foreach (const QString &key, obj.keys()) {
        QJsonObject table = obj.value(key).toObject(); // Tablo nesnesini al


        //Masa bilgisi
        QString FromWhichTable = "Table No: " + table.value("FromWhichTable").toString();

        //Order id
        QString orderIdString = table.value("OrderId").toString();


        QJsonArray orderList = table.value("OrderList").toArray();


        QString OrderStatus = table.value("OrderStatus").toString();
        double Price = table.value("Price").toDouble();

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


        QString priceString = "Price: " + QString::number(Price);

        QString line =  FromWhichTable + ", " + orderIdString + ", " + orderItems + ", Status: " + OrderStatus + ", " + priceString;
        qDebug() <<line;

    }
}
void printalreadyorder::handleInitialPosition(){

}
