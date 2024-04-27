#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include <QDebug>
#include <QString>
#include <QRegularExpression>
int i=1;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager( this );



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //order colection'nın altında 1,2 gibi document Id ler var. i variable bunun documenları okumak için var


        //i string'e dönüştürülüyor, dynamicPart değişkenine atanıyor
        QString dynamicPart = QString::number(i);   //Document Id
        qDebug() << "Dynamic part as QString:" << dynamicPart;

        //Order'daki 1,2 gibi document id'nin url'i url değişkenine atanıyor
        QString url = "https://roboserve-fd298-default-rtdb.firebaseio.com/Orders/" +dynamicPart+ ".json";    //Url
        qDebug() << "Dynamic part as url:" << url;


        // Construct the QUrl using the dynamic URL
        QUrl dynamicUrl(url);

        //request yapılıp sonra connection yapılıyor
        m_networkReply = m_networkManager->get(QNetworkRequest(dynamicUrl));
        connect(m_networkReply,&QNetworkReply::readyRead,this,&MainWindow::networkReplyReadyRead);



        QString labelText = "Table Number: " + fromWhichTable + "\n"
                            + "Order Id: " + orderId + "\n"
                            + "Order List: " + orderList + "\n"
                            + "Order Status: " + orderStatus + "\n"
                            + "Price: " + price;


        ui->label->setText(labelText); // Set text of QLabel
        i++;


}

void MainWindow::networkReplyReadyRead()
{
    QByteArray temp;
    temp.append(m_networkReply->readAll());

    // Convert the QByteArray to a QString
    QString tempString = QString::fromUtf8(temp);

    // Define the characters to remove
    QString charactersToRemove = "{\"}[]/\\";
    for (const QChar &c : charactersToRemove) {
        tempString.remove(c);
    }

    // Convert the QString back to a QByteArray
    temp = tempString.toUtf8();

    //m_dataBuffer = temp;

    // Replace commas with semicolons before specific keys
    temp.replace(",OrderId:", ";OrderId:");
    temp.replace(",OrderList:", ";OrderList:");
    temp.replace(",OrderStatus:", ";OrderStatus:");
    temp.replace(",Price:", ";Price:");

    m_dataBuffer = temp;

    QList<QByteArray> parts = temp.split(';');



    // Loop through each part to extract key-value pairs
    for (const QByteArray& part : parts) {
        QString keyValue = QString::fromUtf8(part.trimmed());

        QString key = keyValue.section(':', 0, 0).trimmed();
        QString value = keyValue.section(':', 1).trimmed();

        // Assign values to variables based on keys
        if (key == "FromWhichTable")
            fromWhichTable = value;
        else if (key == "OrderId")
            orderId = value;
        else if (key == "OrderList")
            orderList = value;
        else if (key == "OrderStatus")
            orderStatus = value;
        else if (key == "Price")
            price = value;
    }


}
