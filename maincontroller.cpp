#include "maincontroller.h"
#include "rllogparser.h"
#include "attachtorllog.h"
#include <iostream>
#include <QCoreApplication>
#include <QtCore>

MainController::MainController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<RLDetectedState>();
    qRegisterMetaType<std::string>();
}

MainController::~MainController() {

}

void MainController::startUpApp() {
    chromaThread = new QThread();
    chromaWorker = new ChromaControl();
    chromaWorker->moveToThread(chromaThread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(chromaThread, SIGNAL(started()), chromaWorker, SLOT(process()));
    connect(chromaWorker, SIGNAL(finished()), chromaWorker, SLOT(deleteLater()));
    connect(chromaThread, SIGNAL(finished()), chromaThread, SLOT(deleteLater()));
    connect(chromaThread, SIGNAL(finished()), this, SLOT(chromaThreadDone()));
    chromaThread->start();


    logParseThread = new QThread();
    parserWorker = new RLLogParser();
    parserWorker->moveToThread(logParseThread);
    connect(parserWorker, SIGNAL(finished()), parserWorker, SLOT(deleteLater()));
    connect(logParseThread, SIGNAL(finished()), logParseThread, SLOT(deleteLater()));
    logParseThread->start();

    attachToConsoleThread = new QThread();
    attachWorker = new AttachToRLLog();
    attachWorker->moveToThread(attachToConsoleThread);
    connect(attachToConsoleThread, SIGNAL(started()), attachWorker, SLOT(startUp()));
    connect(attachWorker, SIGNAL(finished()), attachWorker, SLOT(deleteLater()));
    connect(attachToConsoleThread, SIGNAL(finished()), attachToConsoleThread, SLOT(deleteLater()));
    attachToConsoleThread->start();

    // lines from console to parser
    connect(attachWorker, SIGNAL(newLineFound(std::string)), parserWorker, SLOT(parseLine(std::string)),
                  Qt::QueuedConnection);
    // states from rllogparser to chromacontrol
    connect(parserWorker, SIGNAL(newState(RLDetectedState)), chromaWorker, SLOT(updateState(RLDetectedState)),
            Qt::QueuedConnection);

    // rlRunning to Chromacontrol
    connect (attachWorker, SIGNAL(rocketLeagueRunning(bool)), chromaWorker, SLOT(chromaControlToggle(bool)));

    connect (this, SIGNAL(signalChromaControlRunning(bool)), chromaWorker, SLOT(chromaControlToggle(bool)));

    connect(parserWorker, SIGNAL(rocketLeagueExited()), this, SLOT(rocketLeagueExited()));

    connect (this, SIGNAL(signalDetachConsole()), attachWorker, SLOT(detachFromConsoleAndReset()));


}

void MainController::chromaThreadDone()
{
    int retVal = chromaWorker->returnValue;
    switch (retVal) {
    case -1:
        // could not gain razer control for some reason
        break;
    case 0:
        // thread ended according to plan
        break;
    }
}

void MainController::rocketLeagueExited() {
    emit signalDetachConsole();
    emit signalChromaControlRunning(false);

    std::cout << "MainController::rocketLeagueExited, calling for restart" << std::endl;
    QCoreApplication::exit(0);
}
