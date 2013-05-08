#ifndef CANDEVICE_H
#define CANDEVICE_H

#include "abstractdevice.h"

#include <linux/can.h>
#include <linux/can/raw.h>

class CanDevice : public AbstractDevice
{
    Q_OBJECT
public:
    static CanDevice * Instance(QObject *parent = 0, const int &port = 0);
    ~CanDevice ();

protected:
#define TIPO_RX_TCPIP_ID  0x0D

    static CanDevice * m_Instance;
    explicit CanDevice(QObject *parent, const int &port);

    virtual void toDevice (const QByteArray &buffer);
    virtual quint8 getTipoIdFromDevice() { return TIPO_RX_TCPIP_ID; }
    virtual quint8 getComStatFromDevice();
    virtual void getVersionFromDevice (quint8 & versioneMajor, quint8 & versioneMinor);

    void setPort(const int &val);

protected slots:
    void fromDeviceSlot(int socket);

private:
    int m_socketCan;
    struct can_frame m_frame;

};

#endif // CANDEVICE_H
