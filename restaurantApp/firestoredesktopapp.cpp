#include "firestoredesktopapp.h"
#include "printorders.h"
#include "orderstatusready.h"
#include "removefromprepearedlist.h"
#include "paymentwithid.h"
#include "firestoredesktopapp.h"
#include "getnextorder.h"
#include "printorders.h"
#include "printalreadyorder.h"


FirestoreDesktopApp::FirestoreDesktopApp(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle("Firestore Desktop App");
    layout = new QVBoxLayout(this);


    remove_button = new QPushButton("Remove the order from being prepared orders list", this);
    connect(remove_button, &QPushButton::clicked, this, &FirestoreDesktopApp::removeEntry);
    layout->addWidget(remove_button);

    handle_button = new QPushButton("Get Next Order To Process", this);
    connect(handle_button, &QPushButton::clicked, this, &FirestoreDesktopApp::handleOrder);
    layout->addWidget(handle_button);

    update_button = new QPushButton("Order is put on the robot! Set order status as ready!", this);
    connect(update_button, &QPushButton::clicked, this, &FirestoreDesktopApp::updateFlag);
    layout->addWidget(update_button);

    print_button = new QPushButton("Print All Current Orders", this);
    connect(print_button, &QPushButton::clicked, this, &FirestoreDesktopApp::printEntry);
    layout->addWidget(print_button);

    //printDelivered_button = new QPushButton("Print All Already Delivered Orders", this);
    //connect(printDelivered_button, &QPushButton::clicked, this, &FirestoreDesktopApp::printDeliveredEntry);
    //layout->addWidget(printDelivered_button);

    //payment_button = new QPushButton("Payment is taken for the order with id2", this);
    //connect(payment_button, &QPushButton::clicked, this, &FirestoreDesktopApp::paymentTaken);
    //layout->addWidget(payment_button);

    exit_button = new QPushButton("Exit", this);
    connect(exit_button, &QPushButton::clicked, this, &QWidget::close);
    layout->addWidget(exit_button);

    setLayout(layout);

    initialize();
}

FirestoreDesktopApp::~FirestoreDesktopApp() {
    // Clean up dynamically allocated memory
    delete layout;
    delete add_button;
    delete remove_button;
    delete handle_button;
    delete update_button;
    delete print_button;
    delete printDelivered_button;
    delete payment_button;
    delete exit_button;
}
void FirestoreDesktopApp::addEntry() {
    // Function to add a new entry to Firestore
}

void FirestoreDesktopApp::removeEntry() {
    // Function to remove an entry from Firestore
    removefromprepearedlist *rfpl = new removefromprepearedlist(this);
    rfpl->run("orders");

}

void FirestoreDesktopApp::handleOrder() {
    // Function to handle order

    getnextorder *gn  = new getnextorder(this);
    gn->run();
}

void FirestoreDesktopApp::updateFlag() {
    // Function to update flag

    orderstatusready *osr = new orderstatusready(this);
    osr->run("orders");
    osr->run("currentlyProcessedOrder");
}

void FirestoreDesktopApp::printEntry() {
    // Function to print all current orders

    // Instantiate the printorders object
    printorders *printer = new printorders(this);
    // Call the run function to print all orders
    printer->run();
}

void FirestoreDesktopApp::printDeliveredEntry() {
    // Function to print all already delivered orders
}

void FirestoreDesktopApp::paymentTaken() {
    // Function to handle payment for orders
}

void FirestoreDesktopApp::initialize() {
    // Initialize any required variables or collections from Firestore
}
