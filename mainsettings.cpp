#include "mainsettings.h"
#include "ui_mainsettings.h"
#include <QSettings>
#include <QCloseEvent>
#include <QtWidgets/QApplication>
#include "openvroverlaycontroller.h"


MainSettings::MainSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSettings)
{
    ui->setupUi(this);
    readSettings();
    on_StartBtn_clicked();
}

MainSettings::~MainSettings()
{
    writeSettings();
    delete ui;
}

void MainSettings::on_StartBtn_clicked(){
    COpenVROverlayController::SharedInstance()->OpenTwitchDashboard(ui->UsernameEdit->text());
    writeSettings();
}

void MainSettings::on_TwitchDashboardBtn_clicked()
{
    if(!pTwitchDashboard){
    pTwitchDashboard = new TwitchDashboard;
    }
    pTwitchDashboard->show();
    pTwitchDashboard->openDashboard(ui->UsernameEdit->text());
}

void MainSettings::on_PositionSliderX_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayPosX((float) value/100.0f);
    ui->PositionSpinBoxX->setValue(value/100.0f);
}
void MainSettings::on_PositionSliderY_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayPosY((float) value/100.0f);
    ui->PositionSpinBoxY->setValue(value/100.0f);
}
void MainSettings::on_PositionSliderZ_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayPosZ((float) value/100.0f);
    ui->PositionSpinBoxZ->setValue(value/100.0f);
}

void MainSettings::on_PositionSpinBoxX_valueChanged(double value){
    ui->PositionSliderX->setValue((int)(value*100));
}
void MainSettings::on_PositionSpinBoxY_valueChanged(double value){
    ui->PositionSliderY->setValue((int)(value*100));
}
void MainSettings::on_PositionSpinBoxZ_valueChanged(double value){
    ui->PositionSliderZ->setValue((int)(value*100));
}

void MainSettings::on_angleSliderX_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayRotX((float) value);
    ui->angleSpinBoxX->setValue((double)value);
}
void MainSettings::on_angleSliderY_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayRotY((float) value);
    ui->angleSpinBoxY->setValue((double)value);
}
void MainSettings::on_angleSliderZ_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayRotZ((float) value);
    ui->angleSpinBoxZ->setValue((double)value);
}

void MainSettings::on_angleSpinBoxX_valueChanged(double value){
    ui->angleSliderX->setValue((int)value);
}
void MainSettings::on_angleSpinBoxY_valueChanged(double value){
    ui->angleSliderY->setValue((int)value);
}
void MainSettings::on_angleSpinBoxZ_valueChanged(double value){
    ui->angleSliderZ->setValue((int)value);
}


void MainSettings::writeSettings()
{
    QSettings settings("RedstoneLP2", "VR-StreamOverlay");


    settings.beginGroup("OverlaySettings");
    settings.setValue("twitchUsername", ui->UsernameEdit->text());
    settings.setValue("XRotation", ui->angleSpinBoxX->value());
    settings.setValue("YRotation", ui->angleSpinBoxY->value());
    settings.setValue("ZRotation", ui->angleSpinBoxZ->value());
    settings.setValue("XPosition", ui->PositionSpinBoxX->value());
    settings.setValue("YPosition", ui->PositionSpinBoxY->value());
    settings.setValue("ZPosition", ui->PositionSpinBoxZ->value());
    settings.endGroup();
}

void MainSettings::readSettings()
{
    QSettings settings("RedstoneLP2", "VR-StreamOverlay");


    settings.beginGroup("OverlaySettings");
    ui->UsernameEdit->setText(settings.value("twitchUsername").toString());
    on_angleSliderX_valueChanged(settings.value("XRotation").toInt());
    on_angleSliderY_valueChanged(settings.value("YRotation").toInt());
    on_angleSliderZ_valueChanged(settings.value("ZRotation").toInt());
    on_PositionSliderX_valueChanged(settings.value("XPosition").toInt());
    on_PositionSliderY_valueChanged(settings.value("YPosition").toInt());
    on_PositionSliderZ_valueChanged(settings.value("ZPosition").toInt());
    settings.endGroup();
}

void MainSettings::closeEvent(QCloseEvent *event) {
    
    pTwitchDashboard->~TwitchDashboard();
    
    COpenVROverlayController::SharedInstance()->Shutdown();
    
    QApplication::quit();
    
    event->accept();
}
