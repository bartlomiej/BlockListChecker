#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QEventLoop>
#include "ustawienia.h"
#include "log.h"

class Download : public QObject
{
    Q_OBJECT

public:
    explicit Download(QObject *parent = 0);

    enum WYNIK
    {
        ERROR = 0,
        NOT_CONNECT,
        OK
    };

private:
    enum SITE
    {
        spamhaus = 0,
        mxtoolbox_domain,
        mxtoolbox_ip
    };

    SITE m_site;
    WYNIK m_wynik;

    QString m_strona;
    QUrl url;
    QByteArray jsonString;
    ustawienia *m_ustawienia;

    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;

    void startRequestMxtoolbox(QUrl _url, QByteArray &jsonString);
    void startRequestSpamhaus(QUrl _url);
    WYNIK parsujMxtoolbox();
    WYNIK parsujSpamhaus();

signals:
    void sig_wynik(const Download::WYNIK &wynik);

public slots:
    void checkSite();

private slots:
    void slot_httpReadyRead();
    void slot_httpFinished();
};

Q_DECLARE_METATYPE(Download::WYNIK)

#endif // DOWNLOAD_H
