#ifndef TCPGATEWAY_H
#define TCPGATEWAY_H

#include <QMap>
#include <QTcpServer>

class ClientOven;
class QTcpSocket;

class TcpGateway : public QTcpServer
{
    Q_OBJECT
public:
    static TcpGateway * Instance(QObject *parent = 0);
    ~TcpGateway();
    int getMaxNumClients ();

    void setDebug (const bool &val);

    void setPort (const quint16 &);
    quint16 getPort(void);

    bool startListen (void);

public slots:
    void fromBusSlot (const QByteArray &);

signals:
    void toBusSignal (const QByteArray &);
    void toClient (const QByteArray &);

protected slots:
    void newConnectionSlot();
    void disconnectedSlot();

protected:
    explicit TcpGateway(QObject *parent);
    static TcpGateway * m_Instance;
    //void incomingConnection(int handle);

    void debug (const QString &testo);

    quint16 m_port;
    bool m_debug;

    QMap<QTcpSocket*,ClientOven*> m_clients;
};

#endif // TCPGATEWAY_H
