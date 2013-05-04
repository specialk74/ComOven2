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
    void toBusSignal (const QByteArray &);

public slots:
    void toClient (const QByteArray &buffer);
    
protected slots:
    void fromClientsSlot();

protected:
    STATO_DECODER m_statoParser;
    QByteArray m_buffer;
};

#endif // CLIENT_H
