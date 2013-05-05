#include <QtTest/QtTest>
#include <QDebug>
#include "QtTestUtil/QtTestUtil.h"

#include "utils.h"

// bool decode (const QByteArray &bufferIn, QByteArray &bufferOut, int & idx, STATO_DECODER & stato)
// void encode (const QByteArray &bufferIn, QByteArray &bufferOut);

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
        bufferIn.append(DLE);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == false);
        QVERIFY(stato == STATO_STX);
        QVERIFY(bufferOut.length() == 0);
        QVERIFY(start == 1);
    }

    void testDecode2 ()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        bufferIn.append(STX);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == false);
        QVERIFY(stato == STATO_DATO);
        QVERIFY(bufferOut.length() == 0);
        QVERIFY(start == 2);
    }

    void testDecode3 ()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        bufferIn.append(STX);
        bufferIn.append(DLE);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == false);
        QVERIFY(stato == STATO_ETX);
        QVERIFY(bufferOut.length() == 0);
        QVERIFY(start == 3);
    }

    void testDecode4 ()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        bufferIn.append(STX);
        bufferIn.append(DLE);
        bufferIn.append(ETX);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == true);
        QVERIFY(stato == STATO_DLE_STX);
        QVERIFY(bufferOut.length() == 0);
        QVERIFY(start == 4);
    }

    void testDecode5 ()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        bufferIn.append(STX);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(ETX);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == false);
        QVERIFY(stato == STATO_DATO);
        QVERIFY(bufferOut.length() == 2);
        QVERIFY(start == 5);
    }

    void testDecode6 ()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        bufferIn.append(STX);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(ETX);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == true);
        QVERIFY(stato == STATO_DLE_STX);
        QVERIFY(bufferOut.length() == 1);
        QVERIFY(start == 6);
    }

    void testDecode7 ()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        bufferIn.append(STX);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(ETX);
        bufferIn.append(DLE);
        bufferIn.append(STX);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(DLE);
        bufferIn.append(ETX);
        QByteArray bufferOut;
        STATO_DECODER stato = STATO_DLE_STX;
        int start = 0;

        QVERIFY(decode(bufferIn, bufferOut, start, stato) == true);
        QVERIFY(stato == STATO_DLE_STX);
        QVERIFY(bufferOut.length() == 1);
        QVERIFY(start == 6);

        bufferOut.clear();
        QVERIFY(decode(bufferIn, bufferOut, start, stato) == true);
        QVERIFY(stato == STATO_DLE_STX);
        QVERIFY(bufferOut.length() == 1);
        QVERIFY(start == 12);
    }

    void testEncode1()
    {
        QByteArray bufferIn;
        bufferIn.append(STX);
        QByteArray bufferOut;

        encode(bufferIn, bufferOut);
        QVERIFY(bufferOut.length() == 5);
        QVERIFY(bufferOut[0] == DLE);
        QVERIFY(bufferOut[1] == STX);
        QVERIFY(bufferOut[2] == STX);
        QVERIFY(bufferOut[3] == DLE);
        QVERIFY(bufferOut[4] == ETX);
    }

    void testEncode2()
    {
        QByteArray bufferIn;
        bufferIn.append(DLE);
        QByteArray bufferOut;

        encode(bufferIn, bufferOut);
        QVERIFY(bufferOut.length() == 6);
        QVERIFY(bufferOut[0] == DLE);
        QVERIFY(bufferOut[1] == STX);
        QVERIFY(bufferOut[2] == DLE);
        QVERIFY(bufferOut[3] == DLE);
        QVERIFY(bufferOut[4] == DLE);
        QVERIFY(bufferOut[5] == ETX);
    }
};

QTTESTUTIL_REGISTER_TEST(UtilsTest);

#include "utilstest.moc"
