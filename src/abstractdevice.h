#ifndef ABSTRACTDEVICE_H
#define ABSTRACTDEVICE_H

#include <QObject>

struct IdStruct {
    quint8 tipo;
    quint32 lunghezza;
    quint8 stato_interno;
    quint8 versione_major;
    quint8 versione_minor;
    quint8 com_stat;
    quint8 versione_device_major;
    quint8 versione_device_minor;
};

class AbstractDevice : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDevice(QObject *parent = 0);

    void setDebug (const bool &val);
    inline bool getDebug () { return m_debug; }
    void setVersioneSwMajor (const quint8 &val);
    void setVersioneSwMinor (const quint8 &val);

signals:
    void toClientsSignal (const QByteArray &buffer);

public slots:
    void fromClientSlot (const QByteArray &buffer);

protected:
#define TIPO_CAN_MSG 0x00
#define TIPO_GET_ID  0x0B
    virtual void toDevice (const QByteArray &buffer) = 0;
    virtual quint8 getTipoIdFromDevice() = 0;
    virtual quint8 getComStatFromDevice() = 0;
    virtual quint8 getVersioneMajorFromDevice () = 0;
    virtual quint8 getVersioneMinorFromDevice () = 0;
    virtual void debug (const QString &testo);

    void toClients (const QByteArray &buffer);

    bool m_debug;

    quint8 m_versioneMajor;
    quint8 m_versioneMinor;
};

#endif // ABSTRACTDEVICE_H
