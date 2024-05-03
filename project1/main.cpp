#include "mainwindow.h"
#include "printorders.h"
#include "printalreadyorder.h"
#include <QApplication>
#include "getnextorder.h"
#include <QTimer>
#include "orderstatusready.h"
#include "removefromprepearedlist.h"
#include "paymentwithid.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //printorders dbhandler;
    //dbhandler.run();

    //printalreadyorder dbhandler1;
    //dbhandler1.run();

    //getnextorder gn;
    //gn.run();

    //orderstatusready osr;
    //osr.run("Orders");
    //osr.run("currentlyProcessedOrder");

    //removefromprepearedlist rfpl;
    //rfpl.run("Orders");


    //paymentwithid pwi;
    //pwi.removeDocumentWithOrderId(22);

    MainWindow w;
    w.show();
    return a.exec();
}
