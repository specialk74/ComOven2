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

union lunghezza {
    quint32 u32;
    quint8 dato[4];
} ;

/*!
 * \brief AbstractDevice::toClients
 * \param buffer - Sono solo messaggi CAN
 */
void AbstractDevice::toClients_CAN (const QByteArray &buffer)
{
    QByteArray bufferToClients;
    bufferToClients.append((char) TIPO_CAN_MSG);
    lunghezza lng;
    lng.u32 = 17;
    bufferToClients.append(lng.dato[0]);
    bufferToClients.append(lng.dato[1]);
    bufferToClients.append(lng.dato[2]);
    bufferToClients.append(lng.dato[3]);
    bufferToClients.append(buffer);

    emit toClientsSignal(bufferToClients);
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

    // Funziona?
    lunghezza lng;
    lng.dato[0] = buffer[1];
    lng.dato[1] = buffer[2];
    lng.dato[2] = buffer[3];
    lng.dato[3] = buffer[4];

    if (lng.u32 != (quint32) buffer.length())
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
        getVersionFromDevice(msgToClients.versione_device_major, msgToClients.versione_device_minor);

        bufferToClients.fromRawData((const char *) &msgToClients, 11);
        emit toClientsSignal(bufferToClients);
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
