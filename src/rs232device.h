#ifndef RS232DEVICE_H
#define RS232DEVICE_H

#include <abstractdevice.h>
#include <QTimer>
class Rs232DevicePrivate;

class Rs232Device : public AbstractDevice
{
    Q_OBJECT

public:
    static Rs232Device * Instance(QObject *parent = 0);
    ~Rs232Device ();

protected:
    static Rs232Device * m_Instance;
    explicit Rs232Device(QObject *parent);

    virtual void toDevice (const QByteArray &buffer);
    virtual quint8 getTipoIdFromDevice();
    virtual quint8 getComStatFromDevice();
    virtual void getVersionFromDevice (quint8 & major, quint8 & minor);

protected slots:
    void startSearch ();
    void foundItSlot();

private:
    Rs232DevicePrivate * m_devicePrivate;
    QTimer m_timer;
};

#endif // RS232DEVICE_H
