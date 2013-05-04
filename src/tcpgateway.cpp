#include "tcpgateway.h"
#include "clientoven.h"

#include <QTcpSocket>

#define MAX_CLIENTS 16

const char headDebug[] = "[TcpGateway]";

TcpGateway * TcpGateway::m_Instance = NULL;

/*!
 * \brief TcpGateway::Instance
 * \param parent
 * \return Pointer all'unica istanza di TcpGateway
 */
TcpGateway * TcpGateway::Instance(QObject *parent)
{
    if (m_Instance == NULL)
        new TcpGateway(parent);

    return m_Instance;
}

TcpGateway::TcpGateway(QObject *parent) :
    QTcpServer(parent)
{
    m_Instance = this;
    setMaxPendingConnections (MAX_CLIENTS);

    m_port = 0;
    m_debug = false;

    // Ogni nuova connessione da parte dei clients, viene gestita dallo slot "newConnectionSlot"
    connect (this, SIGNAL(newConnection()), this, SLOT(newConnectionSlot()));
}

/*!
 * \brief TcpGateway::startListen - Inizia l'ascolto da parte del server sulla porta settata
 * \return true - se il bind ha avuto successo
 *         false - se il bind non ha avuto successo oppure la porta settata e' zero
 */
bool TcpGateway::startListen(void)
{
    if (getPort() == 0)
    {
        debug("Error: port not set");
        return false;
    }

    QString testo = QString ("Server ComOven2 listen on port %1").arg(m_port);
    debug(testo);

    return listen(QHostAddress::Any, m_port);
}


void TcpGateway::debug (const QString &testo)
{
    if (m_debug)
        qDebug() << headDebug << qPrintable(testo);
}

/*************************************************************
 *
 *                        GET/SET
 *
 *
 *************************************************************/

/*!
 * \brief TcpGateway::getMaxNumClients
 * \return Ritorna il numero massimo di connessioni consentite
 */
int TcpGateway::getMaxNumClients ()
{
    return MAX_CLIENTS;
}

/*!
 * \brief TcpGateway::setPort
 * \param val - valore della porta di ascolto del server
 */
void TcpGateway::setPort(const quint16 &val)
{
    m_port = val;
}

/*!
 * \brief TcpGateway::getPort
 * \return Il valore della porta di ascolto del server
 */
quint16 TcpGateway::getPort(void)
{
    return m_port;
}

void TcpGateway::setDebug (const bool &val)
{
    m_debug = val;
}

/*************************************************************
 *
 *                        SLOTS
 *
 *
 *************************************************************/

/*!
 * \brief TcpGateway::newConnectionSlot
 *  Tutte le nuovi connessioni da parte dei clients vengono gestite qua.
 *  Vengono inserite dentro la lista m_clients in modo che quando dovro' mandare un messaggio
 *  avro' un riferimento.
 */
void TcpGateway::newConnectionSlot(void)
{
    debug("New Connection");
    while (hasPendingConnections())
    {
        QTcpSocket *socket = nextPendingConnection();
        if (socket)
        {
            ClientOven *client = new ClientOven(this);
            client->setSocketDescriptor(socket->socketDescriptor());
            m_clients.insert(socket, client);
            // Quando il client si disconnette
            connect (socket, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
            // Quando devo spedire un messaggio al Client
            connect (this, SIGNAL(toClient(QByteArray)), client, SLOT(toClient(QByteArray)));
            // Quando devo spedire i dati al Bus
            connect (client, SIGNAL(toBusSignal(QByteArray)), this, SIGNAL(toBusSignal(QByteArray)));
        }
    }
}

/*!
 * \brief TcpGateway::fromBus - Slot per gestire i dati che mi arrivano dal Bus
 */
void TcpGateway::fromBusSlot(const QByteArray &bufferBus)
{
    QByteArray buffer;
    // Lo trasformo in modo che i Client possano leggerlo corretamente
    encode (bufferBus, buffer);
    // In questo modo lo spedisco a tutti i Clients collegati
    emit toClient(buffer);
}

/*!
 * \brief TcpGateway::disconnectedSlot - Slot per eliminare il riferimento al client che era connesso
 */
void TcpGateway::disconnectedSlot (void)
{
    QTcpSocket *socket = (QTcpSocket *) sender();
    ClientOven *client = m_clients[socket];
    m_clients.remove(socket);
    delete client;
    delete socket;
}
