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
        qDebug() << "Veri bir nesne değil!";
        return;
    }

    QJsonObject obj = doc.object(); // Nesneyi al

    // Create a QMap to store documents sorted by timestamp
    QMap<QDateTime, QString> sortedDocuments;

    // Iterate over the keys in the object . Waiting statusunde olan documentları sorted arraye timestampe göre assign ediyor
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        QString key = it.key();
        QJsonObject table = it.value().toObject();
        QString OrderStatus = table.value("OrderStatus").toString();
        if (OrderStatus == "Being processed") {
            // Extract timestamp from the document
            QDateTime timestamp = QDateTime::fromString(table.value("timestamp").toString(), Qt::ISODate);
            // Insert the document ID and timestamp into the QMap
            sortedDocuments.insert(timestamp, key);
        }
    }
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


}
void orderstatusready::handleInitialPosition(QString FromWhichTable,double OrderId,QJsonArray orderList ,QString OrderStatus,double Price ,QString (firstDoc),QDateTime timestamp)
{
    // Create a new QNetworkAccessManager for handling network requests
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    //Bu blokta new table oluşturuluyor
    m_networkManager= new QNetworkAccessManager(this);
    QVariantMap newTable;
    newTable["FromWhichTable"] = FromWhichTable;
    newTable["OrderId"] = OrderId;
    newTable["OrderStatus"] = "Ready";
    newTable["OrderList"] = orderList;
    newTable["Price"] = Price;
    newTable["timestamp"] = timestamp;

     QVariantMap anotherCollectionData = newTable;

    //oluşturulan table ilgili url'ye yazdırılıyor, order status being proccesed olarak güncelleniyor
    QJsonDocument jsonDoc=QJsonDocument::fromVariant(newTable);
    QNetworkRequest newPetRequest(QString("https://roboserve-fd298-default-rtdb.firebaseio.com/"+ globaldocName +"/%1.json").arg(firstDoc));
    newPetRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkManager->put(newPetRequest,jsonDoc.toJson());





}

