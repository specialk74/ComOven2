#include <QDebug>
#include <QString>
#include "abstractdevice.h"
#include "utils.h"

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

void AbstractDevice::setVersioneSw (const quint8 &versioneMajor, const quint8 &versioneMinor)
{
    m_versioneMajor = versioneMajor;
    m_versioneMinor = versioneMinor;
}

#if 0
union lunghezza {
    quint32 u32;
    quint8 dato[4];
} ;
#endif

/*!
 * \brief AbstractDevice::toClients
 * \param buffer - Sono solo messaggi CAN
 */
void AbstractDevice::fromDeviceToClients (const QByteArray &msgCANfromDevice)
{
    QByteArray bufferToClients;
    bufferToClients.append((char) TIPO_TX_TCPIP_CAN_MSG);
#if 0
#if 0
    lunghezza lng;
    lng.u32 = 17;
    // Funziona? bge o lte?
    bufferToClients.append(lng.dato[0]);
    bufferToClients.append(lng.dato[1]);
    bufferToClients.append(lng.dato[2]);
    bufferToClients.append(lng.dato[3]);
#else
    bufferToClients.append(fromNumberToBuffer(_htonl((quint32) 17)));
#endif
#endif
    QByteArray temp;
    QDataStream ds(temp);
    ds << _htonl((quint32) 17);
    bufferToClients.append(temp);
    bufferToClients.append(msgCANfromDevice);

    emit toClientsSignal(bufferToClients);
}

/*!
 * \brief AbstractDevice::fromClientSlot
 * \param buffer - dati che mi arrivano dal Client
 */
const int lngHeadMsg = 5;
void AbstractDevice::fromClientSlot (const QByteArray &buffer)
{
    // Controllo che la lunghezza minima sia 5 (un byte per il tipo e 4 byte di lunghezza)
    if (buffer.length() < lngHeadMsg)
    {
        debug ("Lunghezza Messaggio corta");
        return;
    }

    // Recupero la lunghezza del messaggio dai dati
#if 0
#if 0
    // Funziona? bge o lte?
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
#else
    QByteArray temp = buffer.left(lngHeadMsg);
    temp.remove(0,1); // Tolgo il primo byte che e' il tipo
    if (_ntohl(fromBufferToNumber (temp)) != (quint32) buffer.length())
    {
        QString testo = QString ("Lunghezza Messaggio errata %1-%2").arg(_ntohl(fromBufferToNumber (temp))).arg(buffer.length());
        debug (testo);
        return;
    }
#endif
#endif
    QDataStream ds(buffer);
    quint8 comando;
    quint32 temp;
    ds >> comando;
    ds >> temp;
    quint32 lunghezza = _ntohl(temp);
    if (lunghezza != (quint32) buffer.length())
    {
        QString testo = QString ("Lunghezza Messaggio errata %1-%2").arg(lunghezza).arg(buffer.length());
        debug (testo);
        return;
    }

    switch (comando)
    {
    case TIPO_RX_TCPIP_CAN_MSG:
    {
        QByteArray bufferToDevice = buffer.right(buffer.length() - lngHeadMsg);
        toDevice (bufferToDevice);
    }
        break;

    case TIPO_RX_TCPIP_GET_ID:
    {
        QByteArray bufferToClients;
        struct IdStruct msgToClients;
        msgToClients.tipo = getTipoIdFromDevice(); // 12 (Device Rs232 Converter) o 13 (Device CAN FriendlyARM)
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
