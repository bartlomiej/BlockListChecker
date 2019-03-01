#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "ustawienia.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private:
    Ui::Settings *ui;

    ustawienia *m_ustawienia;

private slots:
    void zapiszUstawienia();

signals:
    void zmianaTimeout(int interval);
};

#endif // SETTINGS_H
