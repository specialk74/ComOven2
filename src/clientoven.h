#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QByteArray>

#include "utils.h"

class ClientOven : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ClientOven(QObject *parent = 0);
    
signals:
    void toDeviceSignal (const QByteArray &);

public slots:
    void toClientSlot (const QByteArray &buffer);
    
protected slots:
    void fromClientsSlot();

protected:
    STATO_DECODER m_statoParser;
    QByteArray m_buffer;
};

#endif // CLIENT_H
