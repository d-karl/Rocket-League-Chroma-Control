#ifndef ATTACHTORLLOG_H
#define ATTACHTORLLOG_H

#include <windows.h>
#include <QObject>
#include <string>
#include <QTimer>

enum SearchProc {
    RL,
    LogConsole
};

class AttachToRLLog : public QObject
{
    Q_OBJECT

public:
    explicit AttachToRLLog(QObject *parent = 0);
    ~AttachToRLLog();

private:
    DWORD RLProcID;
    DWORD searchForProcess(SearchProc searchFor);
    int linesRead;
    HANDLE hConsole;
    QTimer* scanLogTimer;
    bool consoleConnected;
    bool threadFound;


signals:
    void rocketLeagueRunning(bool running);
    void newLineFound(const std::string & line);
    void finished();
    void error(QString err);

public slots:
    void startUp();
    void readLatest();
    void tryConsoleConnection();
    void detachFromConsoleAndReset();

};

#endif // ATTACHTORLLOG_H
