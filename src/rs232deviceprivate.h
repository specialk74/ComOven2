#ifndef RS232DEVICEPRIVATE_H
#define RS232DEVICEPRIVATE_H

#include "utils.h"

#include <QSerialPort>
#include <QTimer>

class Rs232DevicePrivate : public QSerialPort
{
    Q_OBJECT
public:
    explicit Rs232DevicePrivate(const QSerialPortInfo &info, QObject *parent = 0);
    ~Rs232DevicePrivate();

    void getVersion (quint8 & major, quint8 & minor);
    void getComStat (quint8 &comstat);

signals:
    void fondIt();

protected slots:
    void errorSlot(QSerialPort::SerialPortError serialPortError);
    void fromDeviceSlot();
    void sendRequest();

protected:
    void handleMsg (const QByteArray & buffer);
    bool configPort ();

private:
#define TIPO_CAN_MSG 0x00
#define TIPO_CAN_GET_ID 0x0B
#define TIPO_CAN_ID 0x0C

    quint8 m_checksum;
    QTimer m_timer;
    QByteArray m_buffer;
    STATO_DECODER m_statoParser;

    quint8 m_versioneMajor;
    quint8 m_versioneMinor;
    quint8 m_comstat;
    quint8 m_statoInterno;
};

#endif // RS232DEVICEPRIVATE_H
