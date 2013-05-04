#include <QtTest/QtTest>
#include <QDebug>
#include <QSignalSpy>
#include "QtTestUtil/QtTestUtil.h"
#include <QTcpSocket>

#include "tcpgateway.h"
#include "clientoven.h"

// Lo inserisco qua in modo che se viene variato dal programma originale sono costretto
// a rifarlo anche qua
const int MaxConnessioni = 16;

class TcpGatewayTest : public QObject
{
    Q_OBJECT
    
public:
    TcpGatewayTest()
    {

    }
    
private Q_SLOTS:
    void testNumeroClients()
    {
        QVERIFY2(TcpGateway::Instance()->getMaxNumClients() == MaxConnessioni, "Numero Clients massimo diverso");
    }

    void testTcpPortValue ()
    {
        const int port = 6800;
        TcpGateway::Instance()->setPort (port);
        QVERIFY2(TcpGateway::Instance()->getPort() == port, "Non mantiene il numero della porta");
    }

    void testConnect()
    {
        const int port = 6800;

        qDebug() << "Provo una sola connessione";
        TcpGateway::Instance()->setPort (port);
        TcpGateway::Instance()->startListen();
        QTcpSocket mysocket;
        mysocket.connectToHost("127.0.0.1", port);

        QVERIFY( mysocket.waitForConnected( 5000 ) );
        QVERIFY( mysocket.state() == QTcpSocket::ConnectedState );
        mysocket.close();
        delete TcpGateway::Instance();
    }

    void testConnections ()
    {
        const int port = 6800;

        TcpGateway::Instance()->setDebug(true);
        TcpGateway::Instance()->setPort (port);
        TcpGateway::Instance()->startListen();
        QList<QTcpSocket*> listSocket;

        qDebug() << "Provo il massimo delle connessioni";
        for (int i = 0; i < MaxConnessioni; i++)
        {
            QTcpSocket *mysocket = new QTcpSocket;
            mysocket->connectToHost("127.0.0.1", port);

            QVERIFY( mysocket->waitForConnected( 5000 ) );
            QVERIFY( mysocket->state() == QTcpSocket::ConnectedState );
            listSocket.append(mysocket);
            qDebug() << "Connessione #" << i+1;
        }
        qDebug() << "Forzo il numero delle connessioni";
        {
            QTcpSocket mysocket;
            mysocket.connectToHost("127.0.0.1", port);

            QVERIFY( mysocket.waitForDisconnected( 5000 ) );
            qDebug() << mysocket.state();
            QVERIFY( mysocket.state() == QTcpSocket::UnconnectedState);

            mysocket.close();
        }

        qDebug() << "Libero la RAM";
        {
            while (listSocket.count())
                delete listSocket.first();
        }
        delete TcpGateway::Instance();
    }
};


QTTESTUTIL_REGISTER_TEST(TcpGatewayTest);

#include "tcpgatewaytest.moc"
