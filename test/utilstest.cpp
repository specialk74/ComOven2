#include <QtTest/QtTest>
#include <QDebug>
#include "QtTestUtil/QtTestUtil.h"

#include "utils.h"

// bool decode (const QByteArray &bufferIn, QByteArray &bufferOut, int & idx, STATO_DECODER & stato)

class UtilsTest : public QObject
{
    Q_OBJECT

public:
    UtilsTest()
    {

    }

private Q_SLOTS:
    void testDecode1 ()
    {
        QByteArray bufferIn;
        bufferIn.append(0x010);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == false);
    }
};

QTTESTUTIL_REGISTER_TEST(UtilsTest);

#include "utilstest.moc"
