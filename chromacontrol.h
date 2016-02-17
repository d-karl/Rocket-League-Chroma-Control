#ifndef CHROMACONTROL_H
#define CHROMACONTROL_H

#pragma once

#include <QObject>
#include <windows.h>
#include <QTimer>
#include "RzChromaSDKTypes.h"
#include <libnoise/noise.h>
#include "noiseutils.h"

enum RLDetectedState {
    RL_GameStart,
    RL_MainMenu,
    RL_GoalScored,
    RL_Countdown,
    RL_Go,
    RL_MapLoaded,
    RL_FoundServer,
    RL_Exiting,
    RL_Tutorial,
    RL_Playing
};

enum ActiveState {
    AS_Initial,
    AS_MainMenu,
    AS_FoundMatch,
    AS_Playing,
    AS_PlayingTut,
    AS_Goal,
    AS_CountDown,
    AS_CountDownTut,
    AS_Go
};

class ChromaControl : public QObject
{
    Q_OBJECT
public:
    explicit ChromaControl(QObject *parent = 0);
    ~ChromaControl();
    int returnValue;

public slots:
    void process();
    void chromaControlToggle(bool enable);
    void updateChroma();
    void updateState(RLDetectedState state);

signals:
    void finished();
    void error(QString err);

private:
    void generateBlueOrangeFlag(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE &Effect);
    void drawCountDownRegOrangeGreen();
    void displayAndAdvanceFrame(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE &Effect);
    void drawCountDownProgress(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& Effect, int currentFrameInPart, int part);

    bool enabled;
    bool razerControlGained;
    HMODULE m_ChromaSDKModule;
    QTimer *timer;
    ActiveState activeState;
    int animationStep;
    noise::module::Perlin pn_left;
    noise::module::Perlin pn_right;
    int countDownStart;
    int goStart;
    int goalStart;
    bool playerPlayingTutorial;
};

#endif // CHROMACONTROL_H
