#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "attachtorllog.h"
#include "chromacontrol.h"
#include "maincontroller.h"
#include <string>

using namespace  std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MainController* mainController;
    wstring ReadRegValue(HKEY root, wstring key, wstring name);


public slots:
    void launchButtonPressed();
    void donationButtonPressed();
    void rocketLeagueStart(bool);
    void helpButtonPressed();
    void setPathButtonPressed();
};

#endif // MAINWINDOW_H
