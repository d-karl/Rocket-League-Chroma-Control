#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include "chromacontrol.h"
#include <QThread>
#include "rllogparser.h"
#include "attachtorllog.h"

Q_DECLARE_METATYPE (std::string)
Q_DECLARE_METATYPE (RLDetectedState)

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    ~MainController();
    void startUpApp();
    ChromaControl* chromaWorker;
    RLLogParser* parserWorker;
    AttachToRLLog* attachWorker;

signals:
    void signalDetachConsole();
    void signalChromaControlRunning(bool);

public slots:
    void chromaThreadDone();
    void rocketLeagueExited();

private:
    QThread* chromaThread;
    QThread* logParseThread;
    QThread* attachToConsoleThread;
    void createChromaThread();
    void createlogParserThread();
    void createConsoleAttachThread();

};

#endif // MAINCONTROLLER_H
