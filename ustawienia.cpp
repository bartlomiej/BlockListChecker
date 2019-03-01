#include "ustawienia.h"
#include <QSettings>
#include <QApplication>
#include <QDir>

ustawienia::ustawienia(QObject *parent) :
    QObject(parent)
{
}

ustawienia* ustawienia::m_Instance = 0;

void ustawienia::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setIniCodec("UTF-8");
    settings.beginGroup("Ustawienia");
    m_Domena = settings.value("Domena","cieslak.info.pl").toString();
    m_Ip = settings.value("IP","192.168.000.100").toString();
    m_LogFolder = settings.value("LogFolder","Logi").toString();
    if (m_LogFolder.right(1) != QDir::separator()) m_LogFolder += QDir::separator();
    QDir d(QDir::currentPath() + QDir::separator() + m_LogFolder);
    if (!d.exists())
        d.mkdir(QDir::currentPath() + QDir::separator() + m_LogFolder);
    m_CheckInterval = settings.value("CheckInterval","3600").toInt();
    m_Autostart = settings.value("Autostart","0").toBool();
    QSettings settings1("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    if (m_Autostart) {
        settings1.setValue(QCoreApplication::applicationName(), QCoreApplication::applicationFilePath().replace('/','\\'));
    }else{
        settings1.remove(QCoreApplication::applicationName());
    }
    settings.endGroup();
}

void ustawienia::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setIniCodec("UTF-8");
    settings.beginGroup("Ustawienia");
    settings.setValue("Domena",m_Domena);
    settings.setValue("IP",m_Ip);
    settings.setValue("CheckInterval",m_CheckInterval);
    settings.setValue("LogFolder",m_LogFolder);
    settings.setValue("Autostart",m_Autostart);
    QSettings settings1("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    if (m_Autostart) {
        settings1.setValue(QCoreApplication::applicationName(), QCoreApplication::applicationFilePath().replace('/','\\'));
    }else{
        settings1.remove(QCoreApplication::applicationName());
    }
    settings.endGroup();
}
