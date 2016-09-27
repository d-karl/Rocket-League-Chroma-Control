#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rllogparser.h"
#include "attachtorllog.h"
#include <QtCore>
#include <iostream>
#include <string>
#include <QFileDialog>

using namespace std;

QSettings settings(QSettings::IniFormat, QSettings::UserScope, "RLCC", "RLCC");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainController = new MainController(this);

    mainController->startUpApp();

    connect(ui->LaunchButon, SIGNAL(clicked()), this, SLOT(launchButtonPressed()));
    connect(ui->DonationButton, SIGNAL(clicked()), this, SLOT(donationButtonPressed()));
    connect(mainController->attachWorker, SIGNAL(rocketLeagueRunning(bool)), this, SLOT(rocketLeagueStart(bool)));
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(helpButtonPressed()));

    QVariant pathVar = settings.value("RLPath");
    if (!pathVar.isNull()) {
        ui->LaunchButon->setEnabled(true);
    }
 //   QString string = "test";
 //   std::wstring testerino = string.toStdWString();

}

std::wstring donationURL = L"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=NTYNPG42JQJ3G";
std::wstring threadURL = L"https://insider.razerzone.com/index.php?threads/rocket-league-chroma-control.11637/";

void MainWindow::rocketLeagueStart(bool start) {
    if (start) {
        ui->LaunchButon->setEnabled(false);
    }
}

void MainWindow::launchButtonPressed() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    QSettings m("HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam\\",
                QSettings::NativeFormat);
    QString steamPath = QString(m.value("SteamExe").toString());

    QFileInfo yourFileInfo(steamPath);
    QString correctedCasePath = yourFileInfo.absoluteFilePath ();

    QStringList pathSplit = correctedCasePath.split("/");
    QStringListIterator folderIterator(pathSplit);
    QString drive = folderIterator.next();
    QDir current(drive);

    while (folderIterator.hasNext()) {
        QString toLookfor = folderIterator.next();
        QString correctCaseFolder;
        QString subfolder;
        QStringList subFolders = current.entryList();
        foreach(subfolder, subFolders) {
            if (QString::compare(subfolder, toLookfor, Qt::CaseInsensitive) == 0) {
                correctCaseFolder = QString(subfolder);
            }
        }
        QString currentPath(current.absolutePath());
        currentPath.append("/");
        current = QDir(currentPath + correctCaseFolder);


    }
    QString caseSensitiveSteamPath(current.absolutePath());
    qDebug() << "case sensitive: " << caseSensitiveSteamPath;

    //MessageBox(0, steamExe.toStdWString().c_str(), L"Cannot launch RL", MB_OK);

    if (!caseSensitiveSteamPath.isNull()) {
        caseSensitiveSteamPath.replace(QString("/"), QString("\\"));

        wstring wSteamPath = caseSensitiveSteamPath.toStdWString();
        if (!CreateProcess(wSteamPath.c_str(), L"Steam.exe -applaunch 252950 -log", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
           std::wstring error = L"Could not start Rocket League, looked for steam in ";
           error.append(wSteamPath);
           MessageBox(0, error.c_str(), L"Whoops...!", MB_OK);
        }
    } else {
         MessageBox(0, L"No path set! Please use the \"Set RL Path\" button before hitting launch.", L"Whoops...!", MB_OK);
    }



}

void MainWindow::setPathButtonPressed() {
    QString RLFileName = QFileDialog::getOpenFileName(this, tr("Locate RocketLeague.exe"),
                                                    "/home",
                                                    tr("RocketLeague.exe (RocketLeague.exe)"));
    if (RLFileName.isNull()) {
        MessageBox(0, L"RLCC cannot launch RL until you locate RocketLeague.exe\n\n"
                      "Normally, it is located inside your steam directory: \"steamapps\\common\\rocketleague\\Binaries\\Win32\"\n\n"
                      "If you cannot find it try Windows search", L"Cannot launch RL", MB_OK);
    } else {
        settings.setValue("RLPath", RLFileName);
        ui->LaunchButon->setEnabled(true);
    }

}

void MainWindow::helpButtonPressed() {
    ShellExecute(0, 0, threadURL.c_str(), 0, 0 , SW_SHOW );
}

void MainWindow::donationButtonPressed() {
    MessageBox(0, L"OMG! Wow! Thanks!\n(Seriously, glad you enjoy my program, thanks for considering)", L"Wow!", MB_OK);
    ShellExecute(0, 0, donationURL.c_str(), 0, 0 , SW_SHOW );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainController;
}
