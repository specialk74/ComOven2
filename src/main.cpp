#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QCoreApplication>

const char projectName[] = "ComOven2";
const int portServer = 6800;
const char version[] = "Versione: 1.4";

void usage (void)
{
    qDebug() << "Usage: " << projectName << "[OPTION...]";
    qDebug() << "  " << version;
    qDebug() << "      -d, --debug         emette messaggi di debug";
    qDebug() << "      -p, --port=PORT     porta in ascolto del server (default" << portServer << ")";
    qDebug() << "      -V, --version       visualizza la versione";
    qDebug() << "      -h, --help          visualizza questo help";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int port = portServer;
    bool debug = false;
    bool printUsage = false;

    QStringList args = app.arguments();
    QRegExp rxArgDebug("-d");
    QRegExp rxArgDebugLong("--debug");
    QRegExp rxArgHelp("-h");
    QRegExp rxArgHelpLong("--help");
    QRegExp rxArgVersion("-V");
    QRegExp rxArgVersionLong("--version");
    QRegExp rxArgPort("-p([0-9]{1,})");
    QRegExp rxArgPortLong("--port=([0-9]{1,})");

    for (int i = 1; i < args.size(); ++i) {
        if ((rxArgPort.indexIn(args.at(i)) != -1 )) {
            port =  rxArgPort.cap(1).toInt();
            qDebug() << i << ":" << args.at(i) << rxArgPort.cap(1) << port;
        }
        else if ((rxArgPortLong.indexIn(args.at(i)) != -1 )) {
            port =  rxArgPortLong.cap(1).toInt();
            qDebug() << i << ":" << args.at(i) << rxArgPortLong.cap(1) << port;
        }
        else if ((rxArgDebug.indexIn(args.at(i)) != -1) || (rxArgDebugLong.indexIn(args.at(i)) != -1)) {
            qDebug() << i << ":" << args.at(i) << "Enable Debug";
            debug = true;
        }
        else if ((rxArgVersion.indexIn(args.at(i)) != -1) || (rxArgVersionLong.indexIn(args.at(i)) != -1)) {
            qDebug() << version;
            return 0;
        }
        else if ((rxArgHelp.indexIn(args.at(i)) != -1) || (rxArgHelpLong.indexIn(args.at(i)) != -1)) {
            usage();
            return 0;
        }
        else {
            qDebug() << "Uknown arg:" << args.at(i);
            printUsage = true;
        }
    }

    if (printUsage)
        usage();

    return app.exec();
}
