#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
     ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager( this );

    //order colection'nın altında 1,2 gibi document Id ler var. i variable bunun documenları okumak için var
    int i = 1;

    //i string'e dönüştürülüyor, dynamicPart değişkenine atanıyor
    QString dynamicPart = QString::number(i);   //Document Id
    qDebug() << "Dynamic part as QString:" << dynamicPart;

    //Order'daki 1,2 gibi document id'nin url'i url değişkenine atanıyor
    QString url = "https://desktopappdeneme-default-rtdb.firebaseio.com/Order/" +dynamicPart+ ".json";    //Url
    qDebug() << "Dynamic part as url:" << url;


    // Construct the QUrl using the dynamic URL
    QUrl dynamicUrl(url);

    //request yapılıp sonra connection yapılıyor
    m_networkReply = m_networkManager->get(QNetworkRequest(dynamicUrl));
    connect(m_networkReply,&QNetworkReply::readyRead,this,&MainWindow::networkReplyReadyRead);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{


    qDebug() << "Data in m_dataBuffer:" << m_dataBuffer;

    ui->label->setText(m_dataBuffer); // Set text of QLabel

}

void MainWindow::networkReplyReadyRead()
{


    m_dataBuffer.append(m_networkReply->readAll());
}
