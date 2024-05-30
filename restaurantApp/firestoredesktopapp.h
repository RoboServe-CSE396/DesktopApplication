#ifndef FIRESTOREDESKTOPAPP_H
#define FIRESTOREDESKTOPAPP_H

#include <QWidget>


#include <QPushButton>
#include <QVBoxLayout>
// Include necessary headers for Firebase Admin SDK in C++
// (Note: You'll need to find equivalent libraries for Firebase Admin SDK in C++)
// #include "firebase_admin.h"

class FirestoreDesktopApp : public QWidget {
    Q_OBJECT

public:
    FirestoreDesktopApp(QWidget *parent = nullptr) ;

    ~FirestoreDesktopApp() ;

private slots:
    void addEntry() ;

    void removeEntry() ;

    void handleOrder() ;

    void updateFlag() ;

    void printEntry() ;

    void printDeliveredEntry() ;

    void paymentTaken() ;

    void initialize() ;

private:
    QVBoxLayout *layout;
    QPushButton *add_button;
    QPushButton *remove_button;
    QPushButton *handle_button;
    QPushButton *update_button;
    QPushButton *print_button;
    QPushButton *printDelivered_button;
    QPushButton *payment_button;
    QPushButton *exit_button;
};

#endif // FIRESTOREDESKTOPAPP_H


