#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QMainWindow>
#include "chromacontrol.h"
#include "maincontroller.h"

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = 0);
    ~DebugWindow();

private:
    Ui::DebugWindow *ui;
    MainController* mainController;
    void emitStartChroma();

signals:
    void newState(RLDetectedState state);
    void signalStartChroma(bool start);

public slots:
    void buttonStateInitialPressed();
    void buttonStateMainMenuPressed();
    void buttonStateMapLoadedPressed();
    void buttonStateCountDownPressed();
    void buttonStateGoPressed();
    void buttonStateGoalPressed();
};

#endif // DEBUGWINDOW_H
