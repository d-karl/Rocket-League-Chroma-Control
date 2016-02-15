#include "debugwindow.h"
#include "ui_debugwindow.h"
#include <iostream>


DebugWindow::DebugWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);

    mainController = new MainController(this);

    mainController->startUpApp();

   connect (ui->button_state_initial, SIGNAL(clicked()), this, SLOT(buttonStateInitialPressed()));
   connect (ui->button_state_mainMenu, SIGNAL(clicked()), this, SLOT(buttonStateMainMenuPressed()));
   connect (ui->button_state_mapLoaded, SIGNAL(clicked()), this, SLOT(buttonStateMapLoadedPressed()));
   connect (ui->button_state_countdown, SIGNAL(clicked()), this, SLOT(buttonStateCountDownPressed()));
   connect (ui->button_state_go, SIGNAL(clicked()), this, SLOT(buttonStateGoPressed()));
   connect (ui->button_state_goalscored, SIGNAL(clicked()), this, SLOT(buttonStateGoalPressed()));
   connect (this, SIGNAL(newState(RLDetectedState)), mainController->chromaWorker, SLOT(updateState(RLDetectedState)));
   connect (this, SIGNAL(signalStartChroma(bool)), mainController->chromaWorker, SLOT(chromaControlToggle(bool)));
   QTimer::singleShot(500, this, &emitStartChroma);
}

void DebugWindow::emitStartChroma() {
    emit signalStartChroma(true);
}

DebugWindow::~DebugWindow()
{
    delete ui;
    delete mainController;
}


void DebugWindow::buttonStateInitialPressed()
{
    emit(newState(RL_GameStart));
}

void DebugWindow::buttonStateMainMenuPressed()
{
     emit(newState(RL_MainMenu));
}

void DebugWindow::buttonStateMapLoadedPressed()
{
     emit(newState(RL_Exiting));
}

void DebugWindow::buttonStateCountDownPressed()
{
     emit(newState(RL_Countdown));
}

void DebugWindow::buttonStateGoPressed() {
     emit(newState(RL_Go));
}

void DebugWindow::buttonStateGoalPressed() {
     emit(newState(RL_GoalScored));
}
