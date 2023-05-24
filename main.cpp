#include "overlaywidget.h"
#include "mainsettings.h"
#include "openvroverlaycontroller.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OverlayWidget *pOverlayWidget = new OverlayWidget;
        
    COpenVROverlayController::SharedInstance()->Init();

    COpenVROverlayController::SharedInstance()->SetWidget( pOverlayWidget );

    MainSettings *pMainSettings = new MainSettings;
    
    pMainSettings->show();

    // don't show widgets that you're going display in an overlay
    //w.show();

    return a.exec();
}

