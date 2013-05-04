#include <QtTest/QtTest>
#include <QDebug>
#include <QSignalSpy>
#include "QtTestUtil/QtTestUtil.h"
#include <QTcpSocket>
#include <QThread>

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

        QVERIFY( mysocket.waitForConnected( 1000 ) );
        QVERIFY( mysocket.state() == QTcpSocket::ConnectedState );
        delete TcpGateway::Instance();
    }

    void testConnections ()
    {
        const int port = 6800;

        TcpGateway::Instance()->setPort (port);
        TcpGateway::Instance()->startListen();

        QList<QTcpSocket*> m_list;
        qDebug() << "Provo il massimo delle connessioni";
        QTcpSocket *mysocket;
        for (int i = 0; i < MaxConnessioni; i++)
        {
            mysocket = new QTcpSocket (this);
            m_list.append(mysocket);
            mysocket->connectToHost("127.0.0.1", port);
            qApp->processEvents();

            QVERIFY( mysocket->waitForConnected( 100 ) );
            QVERIFY( mysocket->state() == QTcpSocket::ConnectedState );
        }
        {
            mysocket = new QTcpSocket (this);
            m_list.append(mysocket);
            mysocket->connectToHost("127.0.0.1", port);
            QVERIFY( mysocket->waitForConnected( 1000 ) );
            // Devo inserire piu' processEvents altrimenti non aggiorna la situazione del socket
            qApp->processEvents();
            qApp->processEvents();
            qApp->processEvents();
            qApp->processEvents();
            QVERIFY( mysocket->state() == QTcpSocket::UnconnectedState );
        }

        while (m_list.count()) {
            delete m_list.first();
            m_list.removeFirst();
        }
    }
};


QTTESTUTIL_REGISTER_TEST(TcpGatewayTest);

#include "tcpgatewaytest.moc"
