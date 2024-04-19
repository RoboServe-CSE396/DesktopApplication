#include "databasehandler.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDebug>

DatabaseHandler::DatabaseHandler(QObject *parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager( this );

    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://desktopappdeneme-default-rtdb.firebaseio.com/Order.json")));
    connect(m_networkReply,&QNetworkReply::readyRead,this,&DatabaseHandler::networkReplyReadyRead);
}

DatabaseHandler::~DatabaseHandler()
{
    m_networkManager->deleteLater();
}

void DatabaseHandler::networkReplyReadyRead()
{
    qDebug()<<m_networkReply->readAll();
}
