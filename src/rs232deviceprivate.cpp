#include <QDebug>

#include "rs232deviceprivate.h"

static const char headDebug[] = "[Rs232DevicePrivate]";

/*!
 * \brief Rs232DevicePrivate::Rs232DevicePrivate - CTor
 * \param info
 * \param parent
 */
Rs232DevicePrivate::Rs232DevicePrivate(const QSerialPortInfo &info, QObject *parent) :
    QSerialPort(info, parent)
{
    m_debug = false;

    qDebug() << headDebug << "CTor" << info.portName();

    // Faccio partire un timer: se entro il suo timeout non ho trovato il converter mi autodistruggo
    connect (&m_timerAutodelete, SIGNAL(timeout()), this, SLOT(deleteLater()));
    m_timerAutodelete.start(2000);

    connect (&m_timerSendGetId, SIGNAL(timeout()), this, SLOT(sendMsgGetId()));

    // Sono riuscito a configurare la porta?
    if (configPort())
    {
        // Si, la porta e' fisica e non virtuale.
        m_statoParser = STATO_RS232_DLE_STX;
        m_checksum = 0;

        connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(errorSlot(QSerialPort::SerialPortError)));
        connect (this, SIGNAL(readyRead()), this, SLOT(fromDeviceSlot()));

        // Spedisco il messaggio per sapere se e' collegato un converter
        sendMsgGetId();
    }
}

Rs232DevicePrivate::~Rs232DevicePrivate()
{
    qDebug() << headDebug << "DTor" << portName();
}

/*!
 * \brief Rs232DevicePrivate::configPort - Parametri per configurare la porta seriale
 * \return true se riesce a configurare correttamente la porta seriale
 */
bool Rs232DevicePrivate::configPort ()
{
    if (!open(QIODevice::ReadWrite)) {
        QString testo = QString("Can't open %1, error code %2")
                    .arg(portName()).arg(error());
        debug(testo);
        return false;
    }

    if (!setBaudRate(QSerialPort::Baud115200)) {
        QString testo = QString("Can't set rate 115200 baud to port %1, error code %2")
                     .arg(portName()).arg(error());
        debug(testo);
        return false;
    }

    if (!setDataBits(QSerialPort::Data8)) {
        QString testo = QString("Can't set 8 data bits to port %1, error code %2")
                     .arg(portName()).arg(error());
        debug(testo);
        return false;
    }

    if (!setParity(QSerialPort::NoParity)) {
        QString testo = QString("Can't set no patity to port %1, error code %2")
                     .arg(portName()).arg(error());
        debug(testo);
        return false;
    }

    if (!setStopBits(QSerialPort::OneStop)) {
        QString testo = QString("Can't set 1 stop bit to port %1, error code %2")
                     .arg(portName()).arg(error());
        debug(testo);
        return false;
    }

    if (!setFlowControl(QSerialPort::NoFlowControl)) {
        QString testo = QString("Can't set no flow control to port %1, error code %2")
                     .arg(portName()).arg(error());
        debug(testo);
        return false;
    }

    return true;
}

/*!
 * \brief Rs232DevicePrivate::sendMsgGetId - Costruisco il messaggio GET_ID da spedire al converter
 */
void Rs232DevicePrivate::sendMsgGetId()
{
    QByteArray bufferIn;
    bufferIn.append((char) TIPO_TX_RS232_CAN_GET_ID);
    sendMsg(bufferIn);
}

/*!
 * \brief Rs232DevicePrivate::sendMsgCan - Costruisco il messaggio CAN_MSG da spedire al converter
 * \param msgCAN
 */
void Rs232DevicePrivate::sendMsgCan(const QByteArray &msgCAN)
{
    QByteArray bufferIn;
    bufferIn.append((char) TIPO_TX_RS232_CAN_MSG);
    bufferIn.append(msgCAN);
    sendMsg (bufferIn);
}

/*!
 * \brief Rs232DevicePrivate::sendMsg - Richiamata solo privatamente per spedire "fisicamente"
 *                                      i messaggi verso il device
 * \param bufferIn
 */
void Rs232DevicePrivate::sendMsg(const QByteArray &bufferIn)
{
    QByteArray bufferOut;
    quint8 checksum = cchecksum(bufferIn);
    encode(bufferIn, bufferOut);
    bufferOut.append(~checksum);

    write(bufferOut);

//    qDebug() << "write" << portName();
}

/*!
 * \brief Rs232DevicePrivate::handleMsgRxFromDevice - Decodifica il tipo di messaggio
 *                                                  - e lo gestisce
 * \param buffer
 */
void Rs232DevicePrivate::handleMsgRxFromDevice (const QByteArray & buffer)
{
//    qDebug() << "handleMsgRxFromDevice" << portName();
    quint8 lunghezza = buffer.length();
    if (lunghezza < 1)
    {
        debug("Messaggio from Device corto");
        return;
    }

    switch (buffer[0])
    {
    case TIPO_RX_RS232_CAN_ID:
        if (lunghezza < 7)
        {
            debug("Messaggio CAN_ID corto");
            return;
        }

        if (m_timerAutodelete.isActive())
        {
            // La prima volta in assoluto scrivo un messaggio di debug per sapere a quale seriale
            // il converter e' stato trovato
            QString testo = QString ("Converter found on %1").arg(portName());
            debug(testo);
        }

        // Interrompo il timer per l'auto-delete
        m_timerAutodelete.stop();

        // Mi tengo da parte i valori se mai un Client dovesse chiedermeli
        m_statoInterno = buffer.at(1);
        m_versioneMajor = buffer.at(2);
        m_versioneMinor = buffer.at(3);
        m_comstat      = buffer.at(6);

        // Faccio sapere a Rs232Device che ho trovato un converter e che quindi puo' interrompere
        // la ricerca
        emit fondItSignal();

        // Ripeto il GET_ID ogni 6 sec: ma serve? Si, perche'cosi' rinfresco m_comstat e m_statoInterno
        m_timerSendGetId.start(6000);
        break;

    case TIPO_RX_RS232_CAN_MSG:
    {
        if (lunghezza != 13)
        {
            debug("Lunghezza messaggio CAN_MSG non std");
            return;
        }

        // Tolgo solo il primo carattere
        QByteArray bufferOut = buffer.right(buffer.length() - 1);
        // Messaggio da spedire verso i Clients
        emit toClientsSignal(bufferOut);
    }
        break;
    }
}

void Rs232DevicePrivate::debug (const QString &testo)
{
    if (m_debug)
        qDebug() << headDebug << qPrintable(testo);
}

/*************************************************************
 *
 *                        SLOTS
 *
 *
 *************************************************************/
/*!
 * \brief Rs232DevicePrivate::errorSlot - Gestisce se viene scollegato il converter
 * \param serialPortError
 */
void Rs232DevicePrivate::errorSlot(QSerialPort::SerialPortError serialPortError)
{
    if (m_debug)
        qDebug() << "Error" << serialPortError;

    switch (serialPortError)
    {
    case QSerialPort::NoError:
        // Non faccio nulla
        break;

    case QSerialPort::ResourceError:
    case QSerialPort::WriteError:
    case QSerialPort::ReadError:
        debug("Converter scollegato?");
        deleteLater();
        break;

    default:
    {
        QString testo = QString ("SerialPortError %1").arg(serialPortError);
        debug(testo);
    }
        break;
    }
}

/*!
 * \brief Rs232DevicePrivate::fromDeviceSlot - Legge i byte dalla porta seriale, li decodifica
 *                                             e li passa a "handleMsgRxFromDevice" per gestirli
 */
void Rs232DevicePrivate::fromDeviceSlot()
{
    QByteArray buffer = readAll();
    int start = 0;

    // Fin tanto che non sono arrivato al fondo del buffer che il client mi ha spedisco, decodifico!
    while (start < buffer.length())
    {
        if (decodeRs232Msg (buffer, m_buffer, start, m_statoParser, m_checksum))
        {
            handleMsgRxFromDevice(m_buffer);
            // Ripulisco il buffer perche' gia' gestito
            m_buffer.clear();
        }
    }
}

/*************************************************************
 *
 *                        GET/SET
 *
 *
 *************************************************************/
/*!
 * \brief Rs232DevicePrivate::getVersion
 * \param major
 * \param minor
 */
void Rs232DevicePrivate::getVersion (quint8 & versioneMajor, quint8 & versioneMinor)
{
    versioneMajor = m_versioneMajor;
    versioneMinor = m_versioneMinor;
}

/*!
 * \brief Rs232DevicePrivate::getComStat
 * \param comstat
 */
void Rs232DevicePrivate::getComStat (quint8 &comstat)
{
    comstat = m_comstat;
}


