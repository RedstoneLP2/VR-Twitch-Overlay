#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QtWidgets/QWidget>

namespace Ui {
class OverlayWidget;
}

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget *parent = 0);
    ~OverlayWidget();
    void openDashboard(QString, QString YoutubeLink = QString(""));

private slots:


private:
    Ui::OverlayWidget *ui;
};

#endif // OVERLAYWIDGET_H
