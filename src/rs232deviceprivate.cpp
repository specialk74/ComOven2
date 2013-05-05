#include <QDebug>

#include "rs232deviceprivate.h"

Rs232DevicePrivate::Rs232DevicePrivate(const QSerialPortInfo &info, QObject *parent) :
    QSerialPort(info, parent)
{
    connect (&m_timer, SIGNAL(timeout()), this, SLOT(deleteLater()));
    m_timer.start(2000);

    if (configPort())
    {
        m_statoParser = STATO_DLE_STX;
        m_checksum = 0;

        connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(errorSlot(QSerialPort::SerialPortError)));
        connect (this, SIGNAL(readyRead()), this, SLOT(fromDeviceSlot()));

        sendRequest();
    }
}

Rs232DevicePrivate::~Rs232DevicePrivate()
{
    qDebug() << "DTor Rs232DevicePrivate for" << portName();
}

bool Rs232DevicePrivate::configPort ()
{
    if (!open(QIODevice::ReadWrite)) {
        qDebug() << QString("Can't open %1, error code %2")
                    .arg(portName()).arg(error());
        return false;
    }

    if (!setBaudRate(QSerialPort::Baud115200)) {
        qDebug() << QString("Can't set rate 115200 baud to port %1, error code %2")
                     .arg(portName()).arg(error());
        return false;
    }

    if (!setDataBits(QSerialPort::Data8)) {
        qDebug() << QString("Can't set 8 data bits to port %1, error code %2")
                     .arg(portName()).arg(error());
        return false;
    }

    if (!setParity(QSerialPort::NoParity)) {
        qDebug() << QString("Can't set no patity to port %1, error code %2")
                     .arg(portName()).arg(error());
        return false;
    }

    if (!setStopBits(QSerialPort::OneStop)) {
        qDebug() << QString("Can't set 1 stop bit to port %1, error code %2")
                     .arg(portName()).arg(error());
        return false;
    }

    if (!setFlowControl(QSerialPort::NoFlowControl)) {
        qDebug() << QString("Can't set no flow control to port %1, error code %2")
                     .arg(portName()).arg(error());
        return false;
    }

    return true;
}

void Rs232DevicePrivate::sendRequest()
{
    QByteArray bufferIn (13, 0);
    bufferIn[0] = TIPO_CAN_GET_ID;
    QByteArray bufferOut;

    quint8 checksum = cchecksum(bufferIn);
    encode(bufferIn, bufferOut);
    bufferOut.append(~checksum);

    write(bufferOut);
}

void Rs232DevicePrivate::errorSlot(QSerialPort::SerialPortError serialPortError)
{
    switch (serialPortError)
    {
    case QSerialPort::NoError:
        // Non faccio nulla
        break;

    case QSerialPort::ResourceError:
        qDebug() << "Converter scollegato?";
        deleteLater();
        break;

    default:
        qDebug() << "SerialPortError" << serialPortError;
        break;
    }
}

void Rs232DevicePrivate::fromDeviceSlot()
{
    QByteArray buffer = readAll();
    int start = 0;
    int end = buffer.length();
    // Fin tanto che non sono arrivato al fondo del buffer che il client mi ha spedisco, decodifico!
    while (start < end)
    {
        if (decode2 (buffer, m_buffer, start, m_statoParser, m_checksum))
        {
            handleMsg(m_buffer);
            // Ripulisco il buffer perche' non serve piu'
            m_buffer.clear();
        }
    }
}

quint32 counter = 0;

void Rs232DevicePrivate::handleMsg (const QByteArray & buffer)
{
    quint8 lunghezza = buffer.length();
    if (lunghezza < 1)
    {
        qDebug() << "Messaggio from Device corto";
        return;
    }

    switch (buffer[0])
    {
    case TIPO_CAN_ID:
        if (lunghezza < 7)
        {
            qDebug() << "Messaggio CAN_ID corto";
            return;
        }
        qDebug() << "Rx GET_ID" << counter++;
        m_timer.stop();
        m_statoInterno = buffer.at(1);
        m_versioneMajor = buffer.at(2);
        m_versioneMinor = buffer.at(3);
        m_comstat      = buffer.at(6);

        emit fondIt();

        QTimer::singleShot(100, this, SLOT(sendRequest()));
        break;
    }
}

void Rs232DevicePrivate::getVersion (quint8 & major, quint8 & minor)
{
    major = m_versioneMajor;
    minor = m_versioneMinor;
}

void Rs232DevicePrivate::getComStat (quint8 &comstat)
{
    comstat = m_comstat;
}

