#include <QtTest/QtTest>
#include <QDebug>
#include <QSignalSpy>
#include "QtTestUtil/QtTestUtil.h"
#include <QTcpSocket>

#include "tcpgateway.h"
#include "clientoven.h"

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
        QVERIFY2(TcpGateway::Instance()->getMaxNumClients() == 16, "Numero Clients massimo diverso");
    }

    void testTcpPortValue ()
    {
        const int port = 5000;
        TcpGateway::Instance()->setPort (port);
        QVERIFY2(TcpGateway::Instance()->getPort() == port, "Non mantiene il numero della porta");
    }

    void testConnect()
    {
        const int port = 5000;

        TcpGateway::Instance()->setPort (port);
        TcpGateway::Instance()->startListen();
        QTcpSocket mysocket;
        mysocket.connectToHost("127.0.0.1", port);

        QVERIFY( mysocket.waitForConnected( 5000 ) );
        QVERIFY( mysocket.state() == QTcpSocket::ConnectedState );
        mysocket.close();
    }
};


QTTESTUTIL_REGISTER_TEST(TcpGatewayTest);

#include "tcpgatewaytest.moc"
