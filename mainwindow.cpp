#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rllogparser.h"
#include "attachtorllog.h"
#include <QtCore>
#include <iostream>
#include <string>

using namespace std;

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

    wstring RLFolder = ReadRegValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 252950", L"InstallLocation");
    RLFolder.append(L"\\Binaries\\Win32\\RocketLeague.exe");
    std::wcout << "RL exe: " << RLFolder << std::endl;
    if (!CreateProcess(RLFolder.c_str(), L"RocketLeague.exe -log", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
       std::wstring error = L"Could not start Rocket League, tried path from your registry: ";
       error.append(RLFolder);
       MessageBox(0, error.c_str(), L"Whoops...!", MB_OK);
    }
}

void MainWindow::helpButtonPressed() {
    ShellExecute(0, 0, threadURL.c_str(), 0, 0 , SW_SHOW );
}

void MainWindow::donationButtonPressed() {
    MessageBox(0, L"OMG! Wow! Thanks!\n(Seriously, glad you enjoy my little program, thanks for considering)", L"Wow!", MB_OK);
    ShellExecute(0, 0, donationURL.c_str(), 0, 0 , SW_SHOW );
}

wstring MainWindow::ReadRegValue(HKEY root, wstring key, wstring name)
{
    HKEY hKey;
    if (RegOpenKeyEx(root, key.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        throw "Could not open registry key";

    DWORD type;
    DWORD cbData;
    if (RegQueryValueEx(hKey, name.c_str(), NULL, &type, NULL, &cbData) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        throw "Could not read registry value";
    }

    if (type != REG_SZ)
    {
        RegCloseKey(hKey);
        throw "Incorrect registry value type";
    }

    wstring value(cbData/sizeof(wchar_t), L'\0');
    if (RegQueryValueEx(hKey, name.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &cbData) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        throw "Could not read registry value";
    }

    RegCloseKey(hKey);

    size_t firstNull = value.find_first_of(L'\0');
    if (firstNull != string::npos)
        value.resize(firstNull);

    return value;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainController;
}
