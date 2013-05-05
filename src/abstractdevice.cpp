#include <QDebug>
#include <QString>
#include "abstractdevice.h"

static const char headDebug[] = "[Device]";

AbstractDevice::AbstractDevice(QObject *parent) :
    QObject(parent)
{
    m_debug = false;
    m_versioneMajor = m_versioneMinor = 0;
}

void AbstractDevice::setDebug(const bool &val)
{
    m_debug = val;
}

void AbstractDevice::debug (const QString &testo)
{
    if (m_debug)
        qDebug() << headDebug << qPrintable(testo);
}

void AbstractDevice::setVersioneSwMajor (const quint8 &val)
{
    m_versioneMajor = val;
}

void AbstractDevice::setVersioneSwMinor (const quint8 &val)
{
    m_versioneMinor = val;
}

void AbstractDevice::toClients (const QByteArray &buffer)
{
    emit toClientsSignal(buffer);
}

/*!
 * \brief AbstractDevice::fromClientSlot
 * \param buffer - dati che mi arrivano dal Client
 */
void AbstractDevice::fromClientSlot (const QByteArray &buffer)
{
    // Controllo che la lunghezza minima sia 5 (un byte per il tipo e 4 byte di lunghezza)
    if (buffer.length() < 5)
    {
        debug ("Lunghezza Messaggio corta");
        return;
    }

    // Recupero la lunghezza del messaggio dai dati
    union {
            quint32 u32;
            quint8 dato[4];
        } lunghezza;


    lunghezza.dato[0] = buffer[1];
    lunghezza.dato[1] = buffer[2];
    lunghezza.dato[2] = buffer[3];
    lunghezza.dato[3] = buffer[4];

    if (lunghezza.u32 != (quint32) buffer.length())
    {
        debug ("Lunghezza Messaggio errata");
        return;
    }

    switch (buffer[0])
    {
    case TIPO_CAN_MSG:
    {
        QByteArray bufferToDevice = buffer.right(5);
        toDevice (bufferToDevice);
    }
        break;

    case TIPO_GET_ID:
    {
        QByteArray bufferToClients;
        struct IdStruct msgToClients;
        msgToClients.tipo = getTipoIdFromDevice();
        msgToClients.lunghezza = 9;
        msgToClients.stato_interno = 0;
        msgToClients.versione_major = m_versioneMajor;
        msgToClients.versione_minor = m_versioneMinor;
        msgToClients.com_stat = getComStatFromDevice();
        msgToClients.versione_device_major = getVersioneMajorFromDevice();
        msgToClients.versione_device_minor = getVersioneMinorFromDevice();

        bufferToClients.fromRawData((const char *) &msgToClients, 11);
        toClients(bufferToClients);
    }
        break;

    default:
    {
        QString testo = QString ("Messaggio sconosiuto: %1").arg(buffer[0]);
        debug (testo);
    }
        break;
    }
}
