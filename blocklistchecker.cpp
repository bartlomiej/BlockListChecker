#include "blocklistchecker.h"
#include "ui_blocklistchecker.h"

BlockListChecker::BlockListChecker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BlockListChecker)
{
    ui->setupUi(this);
    connect(Log::instance(),SIGNAL(sendMsg(QString)),this,SLOT(pokazLog(QString)));
    Log::instance()->zapiszLog("*** Uruchomienie programu " + QCoreApplication::applicationName() + " v " + QCoreApplication::applicationVersion() + " ***");
    setWindowTitle(QApplication::applicationName() + " v " + QApplication::applicationVersion());
    setWindowIcon(QIcon(":/ico/images/working.png"));

    createObjects();
    createActions();
    createTrayIcon();
    createConnections();

    trayIcon->setIcon(QIcon(":/ico/images/working-white.png"));
    trayIcon->show();
    thread->start();
    timer->setInterval(m_ustawienia->getCheckInterval() * 1000);
    worker->checkSite();
    readSettings();
}

BlockListChecker::~BlockListChecker()
{
    delete ui;
}

void BlockListChecker::createObjects()
{
    m_ustawienia = ustawienia::instance();
    thread = new QThread(this);
    worker = new Download();
    timer = new QTimer(0);

    timer->moveToThread(thread);
    worker->moveToThread(thread);
}

void BlockListChecker::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(checkAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    fileMenu = new QMenu(tr("Plik"));
    fileMenu->addAction(quitAction);
    editMenu = new QMenu(tr("Edycja"));
    editMenu->addAction(ustawieniaAction);
    editMenu->addAction(checkAction);
    aboutMenu = new QMenu(tr("Pomoc"));
    aboutMenu->addAction(aboutAction);
    ui->menuBar->addMenu(fileMenu);
    ui->menuBar->addMenu(editMenu);
    ui->menuBar->addMenu(aboutMenu);
}

void BlockListChecker::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimalizuj"), this);
    maximizeAction = new QAction(tr("Ma&ksymalizuj"), this);
    restoreAction = new QAction(tr("&Przywróć"), this);
    quitAction = new QAction(QIcon(":/ico/images/close.png"),tr("&Koniec"), this);
    ustawieniaAction = new QAction(QIcon(":/ico/images/settings.png"),tr("&Ustawienia"), this);
    aboutAction = new QAction(QIcon(":/ico/images/information.png"),tr("&Informacje o aplikacji"), this);
    checkAction = new QAction(tr("&Sprawdź"), this);
}

void BlockListChecker::createConnections()
{
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(zamknij()));
    connect(ustawieniaAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(checkAction, SIGNAL(triggered()), worker, SLOT(checkSite()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    qRegisterMetaType<Download::WYNIK>("Download::WYNIK");
    connect(worker, SIGNAL(sig_wynik(Download::WYNIK)), this, SLOT(changeTrayIcon(Download::WYNIK)));
    connect(thread, SIGNAL(started()), timer, SLOT(start()));
    connect(timer, SIGNAL(timeout()), worker, SLOT(checkSite()));
}

void BlockListChecker::showAboutDialog()
{
    QMessageBox::information(this, "O programie", QString("Cieślak Bartłomiej\n\nKontakt: cieslak.bartlomiej@gmail.com\nWebsite: http://cieslak.info.pl\n\n" + QApplication::applicationName() + " v " + QApplication::applicationVersion()));
}

void BlockListChecker::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setIniCodec("UTF-8");
    settings.beginGroup("BlockListChecker");
    restoreGeometry(settings.value("geometry",saveGeometry()).toByteArray());
    restoreState(settings.value("savestate",saveState() ).toByteArray());
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size",size() ).toSize());
    if (settings.value("maximized",isMaximized()).toBool())
        showMaximized();
    settings.endGroup();
}

void BlockListChecker::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setIniCodec("UTF-8");
    settings.beginGroup("BlockListChecker");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("savestate",saveState() );
    settings.setValue("maximized",isMaximized());
    if (!isMaximized()) {
        settings.setValue("pos",pos());
        settings.setValue("size",size());
    }
    settings.endGroup();
}

void BlockListChecker::setVisible(bool visible)
 {
     minimizeAction->setEnabled(visible);
     maximizeAction->setEnabled(!isMaximized());
     restoreAction->setEnabled(isMaximized() || !visible);
     QMainWindow::setVisible(visible);
 }

void BlockListChecker::iconActivated(QSystemTrayIcon::ActivationReason reason)
 {
     switch (reason) {
     case QSystemTrayIcon::Trigger:
     case QSystemTrayIcon::DoubleClick:
        if (isHidden())
            show();
        else
            hide();
         break;
     case QSystemTrayIcon::MiddleClick:
         showMessage();
         break;
     default:
         ;
     }
 }

void BlockListChecker::showMessage()
 {
     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
     trayIcon->showMessage("titleEdit->text()", "bodyEdit->toPlainText()", icon,
                           1000 * 1000);
 }

void BlockListChecker::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }else{
        writeSettings();
        ustawienia::instance()->drop();
        thread->quit();
        thread->wait();
        event->accept();
    }
}

void BlockListChecker::zamknij()
{
    writeSettings();
    ustawienia::instance()->drop();
    QApplication::quit();
}

void BlockListChecker::changeTrayIcon(const Download::WYNIK &wynik)
{
    if (wynik == Download::OK)
    {
        Log::instance()->zapiszLog(tr("<b><font color=\"Green\">Test zakończył się sukcesem.</font></b>"));
        setWindowIcon(QIcon(":/ico/images/up-green.png"));
        trayIcon->setIcon(QIcon(":/ico/images/up-green.png"));
    }else if (wynik == Download::NOT_CONNECT) {
        Log::instance()->zapiszLog(tr("<b><font color=\"Orange\">Dane niekompletne (brak połączenia serwerem).</font></b>"));
        setWindowIcon(QIcon(":/ico/images/warning-orange.png"));
        trayIcon->setIcon(QIcon(":/ico/images/warning-orange.png"));
    }else{
        Log::instance()->zapiszLog(tr("<b><font color=\"Red\">Test zakończył się niepowidzeniem.</font></b>"));
        setWindowIcon(QIcon(":/ico/images/down-red.png"));
        trayIcon->setIcon(QIcon(":/ico/images/down-red.png"));
    }
}

void BlockListChecker::showSettings()
{
    Settings *set = new Settings(this);
    connect(set, SIGNAL(zmianaTimeout(int)),this, SLOT(slotTimerInterval(int)));
    set->exec();
}

void BlockListChecker::slotTimerInterval(int interval)
{
    qDebug() << "Interwal: " << interval;
    timer->setInterval(interval * 1000);
}

void BlockListChecker::pokazLog(QString msg)
{
    ui->pteLog->moveCursor(QTextCursor::End);
    ui->pteLog->textCursor().insertHtml(msg + "<br />");
    ui->pteLog->moveCursor(QTextCursor::End);
}
