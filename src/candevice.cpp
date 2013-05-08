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

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <QSocketNotifier>

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
    m_Instance = this;
    m_socketCan = 0;
    setPort(port);
}

/*!
 * \brief CanDevice::~CanDevice - DTor
 */
CanDevice::~CanDevice ()
{
    m_Instance = NULL;
    if (m_socketCan)
        close(m_socketCan);
}

// TODO
void CanDevice::toDevice (const QByteArray &)
{
    //m_frame.can_id =
    m_frame.can_id |=  CAN_EFF_FLAG;
    m_frame.can_dlc = 8;
    //memcpy (m_frame.data, cmessSend.get_dati(), 8);

    write (m_socketCan, &m_frame, sizeof(m_frame));
}

quint8 CanDevice::getComStatFromDevice()
{
    return 0;
}

void CanDevice::getVersionFromDevice (quint8 & versioneMajor, quint8 & versioneMinor)
{
    versioneMajor = 0;
    versioneMinor = 0;
}

static const char *sequenzaComandi[] =
{
    "ifconfig vcan%1 down",
    "ifconfig can%1 down",
    "ip link set can%1 type can bitrate 60606",
    "ifconfig can%1 up",
    "ifconfig vcan%1 up"
};

void CanDevice::setPort(const int &port)
{
    struct sockaddr_can addr;
    struct ifreq ifr;

    for (int idx = 0; idx < (sizeof(sequenzaComandi)/sizeof(sequenzaComandi[0])); idx++)
    {
        QString comando = QString (sequenzaComandi[idx]).arg(port);
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
        perror("SIOCGIFINDEX");
        exit(1);
    }

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

// TODO
void CanDevice::fromDeviceSlot(int socket)
{
    if (read (socket, &m_frame, sizeof(m_frame)) > 0)
    {
        //emit toClientsSignal(buffer);
    }
}
