#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

public slots:
    void networkReplyReadyRead();
signals:

private:
    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_networkReply;
    QByteArray m_dataBuffer; // Member variable to store data
};
#endif // MAINWINDOW_H
