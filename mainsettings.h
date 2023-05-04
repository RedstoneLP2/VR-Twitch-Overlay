#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QtWidgets/QWidget>
#include "overlaywidget.h"

namespace Ui {
class MainSettings;
}

class MainSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MainSettings(QWidget *parent = 0);
    ~MainSettings();

private slots:
    void on_TwitchDashboardBtn_clicked();
    void on_StartBtn_clicked();
    void on_PositionSliderX_valueChanged(int);
    void on_PositionSliderY_valueChanged(int);
    void on_PositionSliderZ_valueChanged(int);
    void on_angleSliderX_valueChanged(int);
    void on_angleSliderY_valueChanged(int);
    void on_angleSliderZ_valueChanged(int);

    void on_PositionSpinBoxX_valueChanged(double);
    void on_PositionSpinBoxY_valueChanged(double);
    void on_PositionSpinBoxZ_valueChanged(double);
    void on_angleSpinBoxX_valueChanged(double);
    void on_angleSpinBoxY_valueChanged(double);
    void on_angleSpinBoxZ_valueChanged(double);



private:
    Ui::MainSettings *ui;
    OverlayWidget *pOverlay;
};

#endif // MAINSETTINGS_H
