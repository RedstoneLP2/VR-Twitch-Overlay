#include "overlaywidget.h"
#include "ui_overlaywidget.h"

OverlayWidget::OverlayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverlayWidget)
{
    ui->setupUi(this);
}

OverlayWidget::~OverlayWidget()
{
    delete ui;
}

void OverlayWidget::openDashboard(QString Username){
    ui->TwitchChat->setUrl(QUrl(QString::fromUtf8("https://dashboard.twitch.tv/popout/u/")+Username+QString::fromUtf8("/stream-manager/chat")));
    ui->TwitchFeed->setUrl(QUrl(QString::fromUtf8("https://dashboard.twitch.tv/popout/u/")+Username+QString::fromUtf8("/stream-manager/activity-feed")));
}