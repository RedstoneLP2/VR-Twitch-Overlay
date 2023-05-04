#ifndef TWITCHDASHBOARD_H
#define TWITCHDASHBOARD_H

#include <QtWidgets/QWidget>

namespace Ui {
class TwitchDashboard;
}

class TwitchDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit TwitchDashboard(QWidget *parent = 0);
    ~TwitchDashboard();
    void openDashboard(QString);

private:
    Ui::TwitchDashboard *ui;
};

#endif // TWITCHDASHBOARD_H
