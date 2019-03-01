#ifndef USTAWIENIA_H
#define USTAWIENIA_H

#include <QObject>
#include <QMutex>

class ustawienia : public QObject
{
    Q_OBJECT
public:
    static ustawienia* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();
            if (!m_Instance) {
                m_Instance = new ustawienia;
                m_Instance->readSettings();
            }
            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        m_Instance->writeSettings();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

    QString getIp() { return m_Ip; }
    void setIp(QString tmp) { m_Ip = tmp; }
    QString getDomena() { return m_Domena; }
    void setDomena(QString tmp) { m_Domena = tmp; }
    QString getLogFolder() { return m_LogFolder; }
    void setLogFolder(QString tmp) { m_LogFolder = tmp; }
    short unsigned int getCheckInterval() { return m_CheckInterval; }
    void setCheckInterval(short unsigned int tmp) { m_CheckInterval = tmp; }
    bool getAutostart() { return m_Autostart; }
    void setAutostart(bool tmp) { m_Autostart = tmp; }
    void writeSettings();

private:
    explicit ustawienia(QObject *parent = 0);

    ustawienia(const ustawienia &);
    ustawienia& operator=(const ustawienia &);
    static ustawienia* m_Instance;

    void readSettings();

    //------ustawienia------
    QString m_Ip;
    QString m_Domena;
    short unsigned int m_CheckInterval;
    bool m_Autostart;
    QString m_LogFolder;
};

#endif // USTAWIENIA_H
