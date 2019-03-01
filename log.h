#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include "ustawienia.h"

class Log : public QObject
{
    Q_OBJECT
public:
    static Log* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();
            if (!m_Instance) {
                m_Instance = new Log;
                m_Instance->zapiszLog("[Log] Obiekt log utorzony.");
            }
            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        m_Instance->zapiszLog("[Log] Obiekt log usunięty.");
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }
    void zapiszLog(QString sKomunikat);

private:
    explicit Log(QObject *parent = 0);

    Log(const Log &);
    Log& operator=(const Log &);
    static Log* m_Instance;
    QString m_PlikLogow;
    ustawienia *m_ustawienia;

signals:
    void sendMsg(QString msg);
};

#endif // LOG_H
