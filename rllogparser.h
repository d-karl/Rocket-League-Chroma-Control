#ifndef RLLOGPARSER_H
#define RLLOGPARSER_H

#include <QObject>
#include <chromacontrol.h>
#include <QTimer>
#include <fstream>
#include <string>

class RLLogParser : public QObject
{
    Q_OBJECT
public:

    explicit RLLogParser(QObject *parent = 0);
    ~RLLogParser();

public slots:
    void parseLine(const std::string &line);

signals:
    void finished();
    void error(QString err);
    void newState(RLDetectedState state);
    void rocketLeagueExited();

private:
  std::wstring rlLogFilePath;
  const std::string* convertPath;
};

#endif // RLLOGPARSER_H
