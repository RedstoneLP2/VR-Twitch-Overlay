#include "mainsettings.h"
#include "ui_mainsettings.h"
#include "TwitchDashboard.h"
#include "openvroverlaycontroller.h"

MainSettings::MainSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSettings)
{
    ui->setupUi(this);
}

MainSettings::~MainSettings()
{
    delete ui;
}

void MainSettings::on_StartBtn_clicked(){
    COpenVROverlayController::SharedInstance()->OpenTwitchDashboard(ui->UsernameEdit->text());
}

void MainSettings::on_TwitchDashboardBtn_clicked()
{
    TwitchDashboard *pTwitchDashboard = new TwitchDashboard;
    pTwitchDashboard->show();
    pTwitchDashboard->openDashboard(ui->UsernameEdit->text());
}

void MainSettings::on_PositionSliderX_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayPosX((float) value);
    ui->PositionSpinBoxX->setValue(value/100.0f);
}
void MainSettings::on_PositionSliderY_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayPosY((float) value);
    ui->PositionSpinBoxY->setValue(value/100.0f);
}
void MainSettings::on_PositionSliderZ_valueChanged(int value){
    COpenVROverlayController::SharedInstance()->SetOverlayPosZ((float) value);
    ui->PositionSpinBoxZ->setValue(value/100.0f);
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

void MainSettings::on_PositionSpinBoxX_valueChanged(double value){
    ui->PositionSliderX->setValue((int)(value*100));
}
void MainSettings::on_PositionSpinBoxY_valueChanged(double value){
    ui->PositionSliderY->setValue((int)(value*100));
}
void MainSettings::on_PositionSpinBoxZ_valueChanged(double value){
    ui->PositionSliderZ->setValue((int)(value*100));
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
