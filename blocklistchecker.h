#ifndef BLOCKLISTCHECKER_H
#define BLOCKLISTCHECKER_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include <QTextCodec>
#include "ustawienia.h"
#include "log.h"
#include "settings.h"
#include "download.h"

namespace Ui {
class BlockListChecker;
}

class BlockListChecker : public QMainWindow
{
    Q_OBJECT

public:
    explicit BlockListChecker(QWidget *parent = 0);
    ~BlockListChecker();

private:
    Ui::BlockListChecker *ui;
    void createObjects();
    void createActions();
    void createTrayIcon();
    void createConnections();

    void readSettings();
    void writeSettings();

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QAction *ustawieniaAction;
    QAction *aboutAction;
    QAction *checkAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *aboutMenu;

    QTimer *timer;
    QThread* thread;
    Download* worker ;
    ustawienia *m_ustawienia;

    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void showSettings();
    void showAboutDialog();
    void zamknij();

    void pokazLog(QString msg);
    void slotTimerInterval(int interval);
    void changeTrayIcon(const Download::WYNIK &wynik);

};



#endif // BLOCKLISTCHECKER_H
