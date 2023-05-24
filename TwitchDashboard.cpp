#include "TwitchDashboard.h"
#include "ui_TwitchDashboard.h"
#include "iostream"

TwitchDashboard::TwitchDashboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TwitchDashboard)
{
    //QWidget::setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
}

TwitchDashboard::~TwitchDashboard()
{
    delete ui;
}

void TwitchDashboard::openDashboard(QString Username){
    ui->webEngineView->setUrl(QUrl(QString::fromUtf8("https://dashboard.twitch.tv/u/")+Username));
}