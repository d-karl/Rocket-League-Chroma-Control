#include "rllogparser.h"
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <string>

#include <clocale>
#include <locale>
#include <vector>

#define BUFSIZE 4096


using namespace std;


RLLogParser::RLLogParser(QObject *parent) : QObject(parent)
{

}

RLLogParser::~RLLogParser()
{
    emit finished();

}



void RLLogParser::parseLine(const std::string & line) {
    boolean someThingFound = false;

    // detect gameStart
   size_t searchResult = line.find("Signin: Profile OnLoaded LocalID=1", 0);
    if (searchResult != string::npos) {
        someThingFound = true;
        std::cout << "Found GameStart event" << std::endl;
        emit newState(RL_GameStart);
    }

    // detect MainMenu
    if (!someThingFound) {
       searchResult = line.find("DevOnline: Set rich presence to: Main Menu data: Menu", 0);
            if (searchResult != string::npos) {
                someThingFound = true;
                std::cout << "Found Main Menu event" << std::endl;
                emit newState(RL_MainMenu);
            }
    }

    // detect GoalScored
    if (!someThingFound) {
       searchResult = line.find("PostGoalScored", 0);
            if (searchResult != string::npos) {
                someThingFound = true;
                std::cout << "Found Goal scored event" << std::endl;
                emit newState(RL_GoalScored);
            }
    }

    //detect CountDown start
    if (!someThingFound) {
       searchResult = line.find("Countdown", 0);
            if (searchResult != string::npos) {
                someThingFound = true;
                std::cout << "Found CountDown event" << std::endl;
                emit newState(RL_Countdown);
            }
    }

    // detect Map loaded
    if (!someThingFound) {
       searchResult = line.find("Log: Bringing World joingametransition", 0);
            if (searchResult != string::npos) {
                someThingFound = true;
                std::cout << "Found MapLoaded event" << std::endl;
                emit newState(RL_MapLoaded);
            }
    }


    // detect joining of server?
    if (!someThingFound) {
       searchResult = line.find("Matchmaking: Received game server", 0);
            if (searchResult != string::npos) {
                someThingFound = true;
                std::cout << "Found FoundServer event" << std::endl;
                emit newState(RL_FoundServer);
            }
    }

    // detect joining of server?
    if (!someThingFound) {
       searchResult = line.find("Matchmaking: PlayerStartMatchmaking", 0);
            if (searchResult != string::npos) {
                someThingFound = true;
                std::cout << "StartMatchmaking event" << std::endl;
                emit newState(RL_FoundServer);
            }
    }


    if (!someThingFound) {
        searchResult = line.find("Active", 0);
             if (searchResult != string::npos) {
                 someThingFound = true;
                 std::cout << "Active event" << std::endl;
                 emit newState(RL_Go);
             }
     }

    // Exit: Exiting.
    if (!someThingFound) {
        // Preparing to exit.
        searchResult = line.find("Exit: Exiting", 0);
             if (searchResult != string::npos) {
                 someThingFound = true;
                 std::cout << "Exiting" << std::endl;
                 emit newState(RL_Exiting);
                 emit rocketLeagueExited();
             }
     }

    // player playing offline splitscreen
    if (!someThingFound) {
        searchResult = line.find("GameEvent: Init GameEvent_SoccarSplitscreen", 0);
             if (searchResult != string::npos) {
                 someThingFound = true;
                 std::cout << "Playing" << std::endl;
                 emit newState(RL_Playing);
             }
     }

    // player playing online
    if (!someThingFound) {
        searchResult = line.find("GameEvent: GotoGameState GameEvent_Soccar GameEvent_Soccar_", 0);
             if (searchResult != string::npos) {
                 someThingFound = true;
                 std::cout << "Playing" << std::endl;
                 emit newState(RL_Playing);
             }
     }


    // GameEvent: Init GameEvent_Tutorial
    if (!someThingFound) {
        searchResult = line.find("GameEvent: Init GameEvent_Tutorial", 0);
             if (searchResult != string::npos) {
                 someThingFound = true;
                 std::cout << "PlayingTut" << std::endl;
                 emit newState(RL_Tutorial);
             }
     }
}


