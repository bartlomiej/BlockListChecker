#include "settings.h"
#include "ui_settings.h"
#include <QDebug>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    ui->leIp->setInputMask("000.000.000.000;_");

    ui->buttonBox->addButton(tr("Zapisz"),QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("Anuluj"),QDialogButtonBox::RejectRole);
    //ui->buttonBox->addButton(tr("Zamknij"),QDialogButtonBox::DestructiveRole);

    m_ustawienia = ustawienia::instance();
    ui->leIp->setText(m_ustawienia->getIp());
    ui->leDomena->setText(m_ustawienia->getDomena());
    ui->leFolderLogow->setText(m_ustawienia->getLogFolder());
    ui->ckbAutostart->setChecked(m_ustawienia->getAutostart());
    ui->sbInterwal->setValue(m_ustawienia->getCheckInterval());

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(zapiszUstawienia()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

Settings::~Settings()
{
    delete ui;
}

void Settings::zapiszUstawienia()
{
    m_ustawienia->setIp(ui->leIp->text());
    m_ustawienia->setDomena(ui->leDomena->text());
    m_ustawienia->setLogFolder(ui->leFolderLogow->text());
    m_ustawienia->setAutostart(ui->ckbAutostart->isChecked());
    m_ustawienia->setCheckInterval(ui->sbInterwal->value());
    qDebug() << "zapiszUstawienia";
    emit zmianaTimeout(ui->sbInterwal->value());
    m_ustawienia->writeSettings();
}
