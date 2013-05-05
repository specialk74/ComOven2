#include "rs232device.h"
#include "rs232deviceprivate.h"

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>


static const quint8 TipoDevice = 12;

Rs232Device * Rs232Device::m_Instance = NULL;

/*!
 * \brief Rs232Device::Instance
 * \param parent
 * \return Pointer all'unica istanza di Rs232Device
 */
Rs232Device * Rs232Device::Instance(QObject *parent)
{
    if (m_Instance == NULL)
        new Rs232Device(parent);

    return m_Instance;
}

Rs232Device::~Rs232Device ()
{
    m_Instance = NULL;
}

void Rs232Device::startSearch ()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            serial.close();
            Rs232DevicePrivate * devicePrivate = new Rs232DevicePrivate(info, this);
            connect (devicePrivate, SIGNAL(fondIt()), this, SLOT(foundItSlot()));
        }
    }

    m_timer.start(6000);
}

Rs232Device::Rs232Device(QObject *parent) : AbstractDevice (parent)
{
    m_devicePrivate = NULL;
    connect (&m_timer, SIGNAL(timeout()), this, SLOT(startSearch()));
    startSearch();
}

void Rs232Device::toDevice (const QByteArray &buffer)
{

}

quint8 Rs232Device::getTipoIdFromDevice()
{
    return TipoDevice;
}

quint8 Rs232Device::getComStatFromDevice()
{
    quint8 comstat = 0;
    if (m_devicePrivate)
        m_devicePrivate->getComStat(comstat);

    return comstat;
}

void Rs232Device::getVersionFromDevice (quint8 & major, quint8 & minor)
{
    major = 0;
    minor = 0;
    if (m_devicePrivate)
        m_devicePrivate->getVersion(major, minor);
}

void Rs232Device::foundItSlot()
{
    m_devicePrivate = (Rs232DevicePrivate *) sender();
    m_timer.stop();
    connect (m_devicePrivate, SIGNAL(destroyed()), this, SLOT(startSearch()));
}
