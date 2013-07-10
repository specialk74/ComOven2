/*! \mainpage My Personal Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

#include "candevice.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <QSocketNotifier>
#include <QDebug>

// Purtroppo nel compilatore del FriendlyARM non c'e' questa define
#ifndef PF_CAN
#define PF_CAN 29
#define AF_CAN PF_CAN
#endif

static const char headDebug[] = "[CanDevice]";

CanDevice * CanDevice::m_Instance = NULL;

/*!
 * \brief CanDevice::Instance
 * \param parent
 * \return Pointer all'unica istanza di CanDevice
 */
CanDevice * CanDevice::Instance(QObject *parent, const int &port)
{
    if (m_Instance == NULL)
        new CanDevice(parent, port);

    return m_Instance;
}

/*!
 * \brief CanDevice::CanDevice - CTor
 * \param parent
 */
CanDevice::CanDevice(QObject *parent, const int &port) : AbstractDevice (parent)
{
    qDebug() << headDebug << "CTor";
    m_Instance = this;
    m_socketCan = 0;
    m_exist = false;
    setPort(port);
}

/*!
 * \brief CanDevice::~CanDevice - DTor
 */
CanDevice::~CanDevice ()
{
    qDebug() << headDebug << "DTor";

    m_Instance = NULL;
    if (m_socketCan)
        close(m_socketCan);
}

/*!
 * \brief CanDevice::toDevice
 * \param buffer
 */
void CanDevice::toDevice (const QByteArray &buffer)
{
    m_frame.can_id = _ntohl(fromBufferToNumber(buffer));
    m_frame.can_id |=  CAN_EFF_FLAG;
    m_frame.can_dlc = 8;
    memcpy (m_frame.data, buffer.data() + 4, buffer.length() - 4);

    write (m_socketCan, &m_frame, sizeof(m_frame));
}

/*!
 * \brief CanDevice::getComStatFromDevice
 * \return
 */
quint8 CanDevice::getComStatFromDevice()
{
    return 0;
}

/*!
  Sequenza di comandi da mandare al sistema per configurare il device CAN
  */
static const char *sequenzaComandi[] =
{
    "ifconfig vcan%1 down",
    "ifconfig can%1 down",
    "ip link set can%1 type can bitrate 60606",
    "ifconfig can%1 up",
    "ifconfig vcan%1 up"
};

/*!
 * \brief CanDevice::setPort
 * \param port
 */
void CanDevice::setPort(const int &port)
{
    struct sockaddr_can addr;
    struct ifreq ifr;

    qDebug()<< "CanDevice::setPort";

    for (unsigned int idx = 0; idx < (sizeof(sequenzaComandi)/sizeof(sequenzaComandi[0])); idx++)
    {
        QString comando = QString (sequenzaComandi[idx]).arg(port);
        qDebug() << comando;
        system(comando.toLocal8Bit());
    }

    // Codice recuperato da candump.c in utils
    m_socketCan = socket (PF_CAN, SOCK_RAW, CAN_RAW);

    addr.can_family = AF_CAN;
    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    //strcpy(ifr.ifr_name, "can0");
    sprintf (ifr.ifr_name, "can%d", port);
    if (ioctl(m_socketCan, SIOCGIFINDEX, &ifr) < 0)
    {
        // Il device non esiste
        return;
    }

    // Il device esiste
    m_exist = true;

    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_socketCan, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return;
    }

    int loopback = 0; /* 0 = disabled, 1 = enabled (default) */
    setsockopt(m_socketCan, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));

    int recv_own_msgs = 0; /* 0 = disabled (default), 1 = enabled */
    setsockopt(m_socketCan, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));

    QSocketNotifier *notifier = new QSocketNotifier (m_socketCan, QSocketNotifier::Read, this);
    if (notifier)
        connect (notifier, SIGNAL(activated(int)), this, SLOT(fromDeviceSlot(int)));
}

/*!
 * \brief CanDevice::fromDeviceSlot
 * \param socket
 */
void CanDevice::fromDeviceSlot(int socket)
{
    if (read (socket, &m_frame, sizeof(m_frame)) > 0)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        quint32 id = _htonl(m_frame.can_id);
        stream << (quint32) id;
        stream << (quint8) m_frame.data[0];
        stream << (quint8) m_frame.data[1];
        stream << (quint8) m_frame.data[2];
        stream << (quint8) m_frame.data[3];
        stream << (quint8) m_frame.data[4];
        stream << (quint8) m_frame.data[5];
        stream << (quint8) m_frame.data[6];
        stream << (quint8) m_frame.data[7];

        fromDeviceToClients(buffer);
    }
}

void CanDevice::buildGetId(QByteArray & bufferForDevice)
{
    qDebug() << headDebug << "buildGetId";
    QDataStream stream(&bufferForDevice, QIODevice::WriteOnly);
    stream << getComStatFromDevice();
}
