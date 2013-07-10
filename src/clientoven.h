#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QByteArray>

#include "utils.h"

class ClientOven : public QObject
{
    Q_OBJECT
public:
    explicit ClientOven(QObject *parent = 0);
    void setSocket (QTcpSocket *socket);
    void setDebug (const bool &val) { m_debug = val; }
    
signals:
    void toDeviceSignal (const QByteArray &);

public slots:
    void toClientSlot (const QByteArray &buffer);
    
protected slots:
    void fromClientsSlot();

protected:
    STATO_DECODER_TCPIP_MSG m_statoParser;
    QByteArray m_buffer;
    QTcpSocket *m_socket;
    bool m_debug;
};

#endif // CLIENT_H
