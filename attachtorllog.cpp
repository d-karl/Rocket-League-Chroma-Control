#include "attachtorllog.h"
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <strsafe.h>
#include <stdio.h>
#include <string>
#include <QtCore>


AttachToRLLog::AttachToRLLog(QObject* parent) : QObject(parent)
{
    threadFound = false;
}

void AttachToRLLog::startUp() {
    tryConsoleConnection();
}

AttachToRLLog::~AttachToRLLog()
{
    delete scanLogTimer;
    emit finished();
}

void AttachToRLLog::tryConsoleConnection()
{
    if (!consoleConnected) {
        std::cout << "AttachToRLLog::tryConsoleConnection" << std::endl;
        consoleConnected = false;
        RLProcID = searchForProcess(RL);
        linesRead = 0;
        if (threadFound) {
            std::cout << "AttachToRLLog::tryConsoleConnection - attaching to console" << std::endl;
            if (AttachConsole(RLProcID))
            {
                    hConsole = CreateFile(L"CONOUT$",
                        GENERIC_READ | GENERIC_WRITE,
                        0, 0, OPEN_EXISTING, 0, 0);
                    consoleConnected = true;
                    emit rocketLeagueRunning(true);
                    scanLogTimer = new QTimer(this);
                    connect(scanLogTimer, SIGNAL(timeout()), this, SLOT(readLatest()));
                    scanLogTimer->start(100);
            }
            else
            {
                std::cout << "AttachToRLLOG: connection to console failed" << std::endl;
                QTimer::singleShot(1000, Qt::CoarseTimer, this, SLOT(tryConsoleConnection()));
            }
        } else {
            QTimer::singleShot(1000, Qt::CoarseTimer, this, SLOT(tryConsoleConnection()));
        }
    }

}

void AttachToRLLog::detachFromConsoleAndReset() {
    std::cout << "AttachToRLLog::detachFromConsoleAndReset" << std::endl;
    HWND console = GetConsoleWindow();
    bool success = FreeConsole();
    std::cout << "Freeconsole success: " << success << std::endl;
    PostMessage(console, WM_CLOSE, 0, 0);
    consoleConnected = false;
    linesRead = 0;
    QTimer::singleShot(2000, Qt::CoarseTimer, this, SLOT(tryConsoleConnection()));
}

void AttachToRLLog::readLatest()
{
    if (consoleConnected) {
        CONSOLE_SCREEN_BUFFER_INFO console_buffer_info = {0};
        if (GetConsoleScreenBufferInfo(hConsole, &console_buffer_info))
        {
            COORD screenBufferSize = console_buffer_info.dwSize;
            COORD curserLoc = console_buffer_info.dwCursorPosition;

            // special case to clear console once curser reaches max screen buffer size
            if (curserLoc.Y == screenBufferSize.Y - 1) {
                COORD topLeft  = { 0, 10 };
                DWORD written;
                FillConsoleOutputCharacterA(
                        hConsole, ' ', screenBufferSize.X * screenBufferSize.Y, topLeft, &written
                    );
                    FillConsoleOutputAttribute(
                        hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
                        screenBufferSize.X * screenBufferSize.Y, topLeft, &written
                    );
                    SetConsoleCursorPosition(hConsole, topLeft);
                    curserLoc =  topLeft;
                    linesRead = 9;
            }

            // begin actually reading lines
            int readToLine = curserLoc.Y;
            int readFromLine = linesRead;
            int linesToReadNow = readToLine - readFromLine;
            if (linesToReadNow > 0) {
                CHAR_INFO buffer[screenBufferSize.X * linesToReadNow];
                COORD buffer_size  = { screenBufferSize.X, linesToReadNow};
                COORD buffer_index = { 0, 0 };
                SMALL_RECT read_region = { 0, readFromLine, screenBufferSize.X - 1, readToLine};

                if (!ReadConsoleOutputA(hConsole, buffer, buffer_size, buffer_index, &read_region)) {
                    std::cout << "Error reading console output " << GetLastError() << std::endl;
                }
                // iterate lines
                for (int i = 0; i < linesToReadNow; i++) {
                    std::string line = "";
                    // iterate characters in line
                    for (int j = 0; j < screenBufferSize.X - 1; j++) {
                        line+= buffer[i * screenBufferSize.X + j].Char.AsciiChar;
                    }
                    //std::cout << line << std::endl;
                    emit newLineFound(line);
                }

            linesRead = readToLine;
            }


        } else {
            std::wcout << L"Could not get ScreenBufferInfo, last error: " << GetLastError() << std::endl;
        }
    }
}

DWORD AttachToRLLog::searchForProcess(SearchProc searchFor) {
    HANDLE hProcessSnap;
     HANDLE hProcess;
     PROCESSENTRY32 pe32;
     DWORD dwPriorityClass;

     // Take a snapshot of all processes in the system.
     hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
     if( hProcessSnap == INVALID_HANDLE_VALUE )
     {
       std::cout << "CreateToolhelp32Snapshot (of processes)" << std::endl;
     }

     // Set the size of the structure before using it.
     pe32.dwSize = sizeof( PROCESSENTRY32 );


     // find rocketLeague process

     // Retrieve information about the first process,
       // and exit if unsuccessful
       if( !Process32First( hProcessSnap, &pe32 ) )
       {
         std::cout << "Process32First" << std::endl;
         CloseHandle( hProcessSnap );          // clean the snapshot object
       }

       // Now walk the snapshot of processes, and
       // display information about each process in turn

    //   std::wstring

       do{
            std::wstring found;
            found = std::wstring(pe32.szExeFile);

           switch (searchFor) {
           case RL:
               if (found.compare(L"RocketLeague.exe") == 0) {
                   std::wcout << "Acccepted " << found << std::endl;
                   threadFound = true;
                    return pe32.th32ProcessID;
               }
               break;
           case LogConsole:
               if (found.compare(L"conhost.exe") == 0 && pe32.th32ParentProcessID == RLProcID) {
                    std::wcout << "Acccepted " << found  << " with process id: "<< pe32.th32ProcessID << std::endl;
                    threadFound = true;
                    return pe32.th32ProcessID;
               }
           }
       } while( Process32Next( hProcessSnap, &pe32 ) );
       CloseHandle( hProcessSnap );

}

