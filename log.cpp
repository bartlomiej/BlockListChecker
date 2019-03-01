#include "log.h"

Log::Log(QObject *parent) :
    QObject(parent)
{
    m_ustawienia = ustawienia::instance();
}

Log* Log::m_Instance = 0;

void Log::zapiszLog(QString sKomunikat)
{
    static QMutex mutex;
    mutex.lock();
    QString fileLink = QDir::currentPath() + QDir::separator() + m_ustawienia->getLogFolder() + QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".log";
    QFile file(fileLink);
    if( !file.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text) )
    {
        mutex.unlock();
        return;
    }
    QTextStream ts( &file );
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " - " << sKomunikat << "\n";
    file.close();
    emit sendMsg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - " + sKomunikat);
    mutex.unlock();
}
