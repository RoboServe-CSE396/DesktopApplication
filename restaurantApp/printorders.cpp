#include "printorders.h"
#include <string>
#include <QJsonDocument>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QMainWindow> // Include QMainWindow header

printorders::printorders(QObject *parent)
    : QObject{parent}
{

}
void printorders::run()
{
    m_networkManager= new QNetworkAccessManager(this);
    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://roboserve-fd298-default-rtdb.firebaseio.com/orders.json")));
    connect (m_networkReply,&QNetworkReply::readyRead,this,&printorders::networkReplyReadyRead);
}
printorders::~printorders()
{
    m_networkManager->deleteLater();
    disconnect (m_networkReply,&QNetworkReply::readyRead,this,&printorders::networkReplyReadyRead);
}

void printorders::networkReplyReadyRead()
{
    QByteArray data = m_networkReply->readAll(); // Read all data
    QJsonDocument doc = QJsonDocument::fromJson(data); // Create JSON document

    if (!doc.isObject()) {
        qDebug() << "Data is not an object!";
        return;
    }

    QString ordersInfo; // String to store orders info

    QJsonObject obj = doc.object(); // Get the object

    // Create a QTableWidget
    QTableWidget *tableWidget = new QTableWidget();
    tableWidget->setColumnCount(5); // Set the number of columns

    // Set headers for each column
    tableWidget->setHorizontalHeaderLabels({"Table", "Order ID", "Items", "Status", "Price"});

    // Set text color for each header item
    tableWidget->horizontalHeaderItem(0)->setForeground(Qt::blue); // Table
    tableWidget->horizontalHeaderItem(1)->setForeground(Qt::blue); // Order ID
    tableWidget->horizontalHeaderItem(2)->setForeground(Qt::blue); // Items
    tableWidget->horizontalHeaderItem(3)->setForeground(Qt::blue); // Status
    tableWidget->horizontalHeaderItem(4)->setForeground(Qt::blue); // Price

    // Align header text to the left
    for (int i = 0; i < tableWidget->columnCount(); ++i) {
        tableWidget->horizontalHeaderItem(i)->setTextAlignment(Qt::AlignLeft);
    }


    // Set stylesheet to add padding between columns and rows
    tableWidget->setStyleSheet("QTableView { padding: 10px; }"); // Adjust the padding value as needed

    // Iterate through orders and populate the table
    int row = 0;
    foreach (const QString &key, obj.keys()) {
        QJsonObject table = obj.value(key).toObject(); // Get table object

        // Process table data
        QString fromWhichTable = table.value("fromWhichTable").toString();
        int OrderIDString = table.value("OrderID").toInt();
        QJsonArray orderList = table.value("OrderList").toArray();
        QString orderStatus = table.value("orderStatus").toString();
        double price = table.value("Price").toDouble();
        QString orderItems;


        for (int i = 0; i < orderList.size(); ++i) {
            QJsonValue value = orderList.at(i);
            orderItems.append(value.toString());

            if (i < orderList.size() - 1) {
                orderItems.append(", ");
            }
        }

        // Populate table
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(fromWhichTable));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(OrderIDString)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(orderItems));
        tableWidget->setItem(row, 3, new QTableWidgetItem(orderStatus));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(price)));
          //qDebug() << "OrderId:" << OrderIDString;
        ++row;
    }

    // Set the table to be read-only
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->resize(1050,200);



    // Show the table
    tableWidget->resizeColumnsToContents();
    tableWidget->resizeRowsToContents();
    tableWidget->show();

    // Set each column to a width of 200 pixels
    for (int i = 0; i < tableWidget->columnCount(); ++i) {
        tableWidget->setColumnWidth(i, 200);
    }



}
void printorders::handleInitialPosition(){

}
