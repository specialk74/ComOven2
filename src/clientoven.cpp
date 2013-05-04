#include "clientoven.h"

ClientOven::ClientOven(QObject *parent) :
    QTcpSocket(parent)
{
    qDebug() << "CTor ClientOven";
    // Quando mi arrivano dei dati dal client
    m_statoParser = STATO_DLE_STX;
    connect (this, SIGNAL(readyRead()), this, SLOT(fromClientsSlot()));
}


/*!
 * \brief ClientOven::fromClients - Slot per gestire i dati che mi arrivano dal client
 */
void ClientOven::fromClientsSlot()
{
    qDebug("fromClientsSlot");
    QByteArray buffer = readAll();
    int start = 0;
    int end = buffer.length();
    // Fin tanto che non sono arrivato al fondo del buffer che il client mi ha spedisco, decodifico!
    while (start < end)
    {
        if (decode (buffer, m_buffer, start, m_statoParser))
        {
            // E' stato trovato un messaggio valido completo: mando un segnale per gestirlo
            emit toBusSignal(m_buffer);
            // Ripulisco il buffer perche' non serve piu'
            m_buffer.clear();
        }
    }
}

/*!
 * \brief ClientOven::toClient
 * \param buffer - buffer da spedire al Client: non devo fare nulla perche' e' gia' stato codificato da TcpGateway
 */
void ClientOven::toClient (const QByteArray &buffer)
{
    qDebug("toClient");

    write(buffer);
}
