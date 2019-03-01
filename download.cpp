#include "download.h"

Download::Download(QObject *parent) : QObject(parent)
{
    m_ustawienia = ustawienia::instance();
    m_wynik = NOT_CONNECT;
}

void Download::checkSite()
{
    m_site = spamhaus;
    Log::instance()->zapiszLog("<b>*** www.spamhaus.org ***</b>");
    url = "http://www.spamhaus.org/query/bl?ip=" + m_ustawienia->getIp();
    startRequestSpamhaus(url);
}


void Download::startRequestSpamhaus(QUrl _url)
{
    m_manager = new QNetworkAccessManager();
    m_strona = QString();

    QNetworkRequest req(_url);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.37 Safari/537.36");
    req.setRawHeader("Content-Type", "application/json; charset=utf-8");
    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
    req.setRawHeader("Accept-Language", "pl,en-US;q=0.8,en;q=0.6,de;q=0.4");
    req.setRawHeader("Connection", "keep-alive");

    m_reply = m_manager->get(req);
    connect(m_reply, SIGNAL(finished()),this, SLOT(slot_httpFinished()));
    connect(m_reply, SIGNAL(readyRead()),this, SLOT(slot_httpReadyRead()));
}

void Download::startRequestMxtoolbox(QUrl _url,QByteArray &jsonString)
{
    m_manager = new QNetworkAccessManager();
    m_strona = QString();

    QByteArray postDataSize = QByteArray::number(jsonString.size());

    QNetworkRequest req(_url);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.37 Safari/537.36");
    req.setRawHeader("Content-Type", "application/json; charset=utf-8");
    req.setRawHeader("Content-Length", postDataSize);
    req.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
    req.setRawHeader("Accept-Language", "pl,en-US;q=0.8,en;q=0.6,de;q=0.4");
    req.setRawHeader("Connection", "keep-alive");

    m_reply = m_manager->post(req, jsonString);
    connect(m_reply, SIGNAL(finished()),this, SLOT(slot_httpFinished()));
    connect(m_reply, SIGNAL(readyRead()),this, SLOT(slot_httpReadyRead()));
}


void Download::slot_httpReadyRead()
{
    QByteArray buf = m_reply->readAll();
    QString str = QString::fromUtf8(buf.data(),buf.count());
    m_strona.append(str);
}

void Download::slot_httpFinished()
{
    QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        url = newUrl;
        m_reply->deleteLater();
        if (m_site == spamhaus)
            startRequestSpamhaus(url);
        else
            startRequestMxtoolbox(url,jsonString);
        return;
    }
    m_reply->deleteLater();
    m_reply = 0;
    WYNIK wynik;

    if (m_site == spamhaus)
    {
        wynik = parsujSpamhaus();
        if (wynik == ERROR)
        {
            Log::instance()->zapiszLog("<font color=\"Red\">Test dla " + m_ustawienia->getDomena() + " zakończył się niepowidzeniem.</font>");
        }else if (wynik == OK){
            Log::instance()->zapiszLog("<font color=\"Green\">Test dla " + m_ustawienia->getDomena() + " zakończył się powidzeniem.</font>");
        }else
            Log::instance()->zapiszLog("<font color=\"Orange\">Test dla " + m_ustawienia->getDomena() + " zakończył się niepowidzeniem (brak połączenia z serwerem).</font>");
        m_wynik = wynik;
        m_site = mxtoolbox_domain;
        Log::instance()->zapiszLog("<b>*** mxtoolbox.com ***</b>");
        jsonString = "{\"inputText\":\"blacklist:" + m_ustawienia->getDomena().toLocal8Bit() + "\",\"resultIndex\":\"1\"}";
        url = QUrl("http://mxtoolbox.com/Public/Lookup.aspx/DoLookup2");
        startRequestMxtoolbox(url,jsonString);

    }else{
        m_strona = m_strona.replace("\\\\u003c", "<").replace("\\\\u003e", ">")
                        .replace("\\\\u0027", "\"").replace("\\\\u0026","&").replace("\\\\u0027","'")
                        .replace("\\\\n", "\n").replace("\\\\r","\r").replace("&nbsp;","");

        bool byl = false;
        QString newStr = QString();
        for (int i = 0; i<m_strona.count() - 1;i++)
        {
            if (m_strona.at(i) == '\\')
            {
                  byl = true;
                  continue;
            }
            if (byl)
            {
                if (m_strona.at(i) == '\"')
                {
                    newStr += "\"";
                }else{
                    newStr += "\\";
                }
                byl = false;
            }else{
                newStr += m_strona.at(i);
            }
        }
        m_strona =  newStr;
        if (m_site == mxtoolbox_domain) {
            wynik = parsujMxtoolbox();
            if (wynik == ERROR)
            {
                Log::instance()->zapiszLog("<font color=\"Red\">Test dla " + m_ustawienia->getDomena() + " zakończył się niepowidzeniem.</font>");
            }else if (wynik == OK){
                Log::instance()->zapiszLog("<font color=\"Green\">Test dla " + m_ustawienia->getDomena() + " zakończył się powidzeniem.</font>");
            }else
                Log::instance()->zapiszLog("<font color=\"Orange\">Test dla " + m_ustawienia->getDomena() + " zakończył się niepowidzeniem (brak połączenia z serwerem).</font>");
            if (m_wynik > wynik)
                m_wynik = wynik;
            m_site = mxtoolbox_ip;
            jsonString = "{\"inputText\":\"blacklist:" + m_ustawienia->getIp().toLocal8Bit() + "\",\"resultIndex\":\"1\"}";
            url = QUrl("http://mxtoolbox.com/Public/Lookup.aspx/DoLookup2");
            startRequestMxtoolbox(url,jsonString);
        }else if (m_site == mxtoolbox_ip) {
            wynik = parsujMxtoolbox();
            if (wynik == ERROR)
            {
                Log::instance()->zapiszLog("<font color=\"Red\">Test dla " + m_ustawienia->getIp() + " zakończył się niepowidzeniem.</font>");
            }else if (wynik == OK){
                Log::instance()->zapiszLog("<font color=\"Green\">Test dla " + m_ustawienia->getIp() + " zakończył się powidzeniem.</font>");
            }else
                Log::instance()->zapiszLog("<font color=\"Orange\">Test dla " + m_ustawienia->getIp() + " zakończył się niepowidzeniem (brak połączenia z serwerem).</font>");
            if (m_wynik > wynik)
                m_wynik = wynik;
            emit sig_wynik(m_wynik);
        }
    }
}

Download::WYNIK Download::parsujSpamhaus()
{
    //Usunięcie komentarzy
    qDebug() << m_strona.length();
    QRegExp kom("<!--(.*)-->");
    kom.setMinimal(true);
    kom.setCaseSensitivity(Qt::CaseSensitive);
    m_strona.remove(kom);
    qDebug() << m_strona.length();
    //------
    QString Stan = QString();
    QString Blacklist = "www.spamhaus.org";
    WYNIK ret = NOT_CONNECT;
    QRegExp rx("<B><FONT color.*>(.*)</FONT></B>");
    rx.setMinimal(true);
    rx.setCaseSensitivity(Qt::CaseSensitive);
    int pos=0;
    while((pos = rx.indexIn(m_strona, pos)) != -1)
    {
        if (rx.cap(1).indexOf("is not listed") > 0)
        {
            Stan = "<font color=\"Green\">" + rx.cap(1) + "</font>";
            Log::instance()->zapiszLog(Blacklist + " - " + Stan);
            ret = OK;
        }else if (rx.cap(1).indexOf("is listed") > 0){
            Stan = "<font color=\"Red\">" + rx.cap(1) + "</font>";
            Log::instance()->zapiszLog(Blacklist + " - " + Stan);
            ret = ERROR;
        }
        pos += rx.matchedLength();
    }
    return ret;
}

Download::WYNIK Download::parsujMxtoolbox()
{
    QString podstrona = QString();
    QString Stan = QString();
    QString Blacklist = QString();
    WYNIK ret = NOT_CONNECT;
    QRegExp rx("<td><img(.*)</tr>");
    rx.setMinimal(true);
    int pos=0;
    while((pos = rx.indexIn(m_strona, pos)) != -1)
    {
        podstrona = rx.cap(0);
        QRegExp bx("<img.*>(.*)</td>");
        bx.setMinimal(true);
        int pos1 = 0;
        bx.indexIn(podstrona, pos1);
        if (bx.cap(1).trimmed() == "OK")
        {
            Stan = "<font color=\"Green\">" + bx.cap(1).trimmed() + "</font>";
            ret = OK;
        }else if (bx.cap(1).trimmed() == "TIMEOUT")
        {
            Stan = "<font color=\"Orange\">" + bx.cap(1).trimmed() + "</font>";
            ret = OK;
        }else{
            Stan = "<font color=\"Red\">" + bx.cap(1).trimmed() + "</font>";
            ret = ERROR;
        }
        bx.setPattern("<span.class=\"bld_name\">(.*)</span>");
        bx.indexIn(podstrona, pos1);
        Blacklist = bx.cap(1).trimmed();
        Log::instance()->zapiszLog(Blacklist + " - " + Stan);
        pos += rx.matchedLength();
    }
    return ret;
}
