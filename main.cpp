#include "blocklistchecker.h"
#include <QApplication>
#include <QTextCodec>
#include "singleapplication.h"

int main(int argc, char *argv[])
{
#ifndef Q_OS_UNIX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1250"));
#else
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

    SingleApplication a(argc, argv);
    a.setApplicationName("BlockListChecker");
    a.setApplicationVersion("1.0.5");
    a.setOrganizationDomain("cieslak.info.pl");
    a.setOrganizationName("CIESLAK");

    BlockListChecker w;
    w.show();

    return a.exec();
}
