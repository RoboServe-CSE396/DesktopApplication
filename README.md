databasehandler.cpp dosyasında 
m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://desktopappdeneme-default-rtdb.firebaseio.com/Order.json")));
QUrl kısmı bizim projenin realtime database url ile değiştirilmeli.
