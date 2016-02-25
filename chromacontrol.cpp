#include "chromacontrol.h"
#include <qglobal.h>
#include <windows.h>
#include "RzChromaSDKTypes.h"
#include "RzChromaSDKDefines.h"
#include "RzErrors.h"
#include <QtCore>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "chromaanimationutils.h"
#include "structcolorrgb.h"
#include <Qt>
#include "noiseutils.h"
#include <Gdiplus.h>

#include <math.h>

using namespace ChromaSDK;
using namespace ChromaSDK::Keyboard;
using namespace ChromaSDK::Keypad;
using namespace ChromaSDK::Mouse;
using namespace ChromaSDK::Mousepad;
using namespace ChromaSDK::Headset;
using namespace chromaAnimationUtils;
using namespace chromaAnimationUtils::easing;
using namespace noise;

#ifdef Q_OS_WIN64
#define CHROMASDKDLL "RzChromaSDK64.dll"
#else
#define CHROMASDKDLL "RzChromaSDK.dll"
#endif

const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

typedef RZRESULT (*INIT)(void);
typedef RZRESULT (*UNINIT)(void);
typedef RZRESULT (*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEHEADSETEFFECT)(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEMOUSEPADEFFECT)(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYPADEFFECT)(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*SETEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT (*DELETEEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT (*REGISTEREVENTNOTIFICATION)(HWND hWnd);
typedef RZRESULT (*UNREGISTEREVENTNOTIFICATION)(void);
typedef RZRESULT (*QUERYDEVICE)(RZDEVICEID DeviceId, ChromaSDK::DEVICE_INFO_TYPE &DeviceInfo);

INIT Init = NULL;
UNINIT Uninit = NULL;
CREATEEFFECT CreateEffect = NULL;
CREATEKEYBOARDEFFECT CreateKeyboardEffect = NULL;
CREATEMOUSEEFFECT CreateMouseEffect = NULL;
CREATEHEADSETEFFECT CreateHeadsetEffect = NULL;
CREATEMOUSEPADEFFECT CreateMousepadEffect = NULL;
CREATEKEYPADEFFECT CreateKeypadEffect = NULL;
SETEFFECT SetEffect = NULL;
DELETEEFFECT DeleteEffect = NULL;
QUERYDEVICE QueryDevice = NULL;

const COLORREF spotify_green = RGB(0, 255, 0);
const int animationMS = 33;

RZEFFECTID currentFrame = GUID_NULL;
RZEFFECTID newFrame = GUID_NULL;

ChromaControl::ChromaControl(QObject *parent) : QObject(parent)
{
    enabled = false;
    razerControlGained = false;
    m_ChromaSDKModule = NULL;
}

ChromaControl::~ChromaControl() {
    delete timer;
}

void ChromaControl::process() {

}

void ChromaControl::chromaControlToggle(bool enable) {
    if (enable == true) {
        // gain razer chroma control
        if(m_ChromaSDKModule == NULL)
        {
            std::string s(CHROMASDKDLL);
            std::wstring stemp = std::wstring(s.begin(), s.end());
            LPCWSTR sw = stemp.c_str();
            m_ChromaSDKModule = LoadLibrary(sw);
            if(m_ChromaSDKModule == NULL)
            {
                std::cout << "Did not manage to load Chroma SDK DLL" << std::endl;
                // ASSERT(GetLastError() == ERROR_SUCCESS);
                razerControlGained = false;
                returnValue = -1;
                exit(-1);
            } else {
                Init = (INIT)GetProcAddress(m_ChromaSDKModule, "Init");
                Uninit = (UNINIT)GetProcAddress(m_ChromaSDKModule, "UnInit");
                CreateEffect = (CREATEEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateEffect");
                CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateKeyboardEffect");
                CreateMouseEffect = (CREATEMOUSEEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateMouseEffect");
                CreateHeadsetEffect = (CREATEHEADSETEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateHeadsetEffect");
                CreateMousepadEffect = (CREATEMOUSEPADEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateMousepadEffect");
                CreateKeypadEffect = (CREATEKEYPADEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateKeypadEffect");
                SetEffect = (SETEFFECT)GetProcAddress(m_ChromaSDKModule, "SetEffect");
                DeleteEffect = (DELETEEFFECT)GetProcAddress(m_ChromaSDKModule, "DeleteEffect");
                QueryDevice = (QUERYDEVICE)GetProcAddress(m_ChromaSDKModule, "QueryDevice");
                Init();
                razerControlGained = true;
            }
        }
        pn_left.SetSeed(rand());
        pn_right.SetSeed(rand());

        // initialization done
        std::cout << "ChromaControl initialized" << std::endl;
        if (razerControlGained) {
            activeState = AS_Initial;
            Init();
            timer = new QTimer(this);
            timer->setTimerType(Qt::PreciseTimer);
            connect(timer, SIGNAL(timeout()), this, SLOT(updateChroma()));
            timer->start(animationMS);
            animationStep = 0;
            enabled = true;
        }
    } else {
        std::cout << "ChromaControl given up" << std::endl;
        if (razerControlGained) {
            Uninit();
            // unload dll to free up effects?
            WINBOOL unloaded = FreeLibrary(m_ChromaSDKModule);
            if (unloaded) {
                m_ChromaSDKModule = NULL;
                razerControlGained = false;
            }
        }
        // message timer to stop
        timer->stop();
        returnValue = 0;
    }
}

void ChromaControl::updateState(RLDetectedState state) {
    switch(state) {
    case RL_MainMenu:
    {
        if (activeState != AS_MainMenu) {
            playerPlayingTutorial = false;
            activeState = AS_MainMenu;
            int i;
            time_t t;

            time(&t);
            srand((unsigned int)t);   /* Zufallsgenerator initialisieren */
            pn_left.SetSeed(rand());
            pn_right.SetSeed(rand());

        }
         break;
    }
    case RL_GameStart:
    {
        activeState = AS_Initial;
        animationStep = 0;
        break;
        //    case RL_MapLoaded:
        //        activeState = AS_Playing;
        //        animationStep = 0;
                break;
    }
    case RL_Countdown:
    {
        if (!playerPlayingTutorial)
            activeState = AS_CountDown;
        else
            activeState = AS_CountDownTut;
        countDownStart = animationStep;
        break;
    }
    case RL_Go:
    {
        activeState = AS_Go;
        goStart = animationStep;

        break;
    }
    case RL_GoalScored:
    {
        activeState = AS_Goal;
        goalStart = animationStep;
        break;
    }

    case RL_Tutorial:
        playerPlayingTutorial = true;
        break;

    case RL_Playing:
        playerPlayingTutorial = false;
        break;

    case RL_Exiting:

        //TODO: figure out how to gracefully exit
        break;

    }
}

void ChromaControl::displayAndAdvanceFrame(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& Effect) {
    setRazerLogo(Effect, blue);
    DeleteEffect(newFrame);
    newFrame = GUID_NULL;
    CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &Effect, &newFrame);
    DeleteEffect(currentFrame);
    RZRESULT result = SetEffect(newFrame);
    currentFrame = newFrame;
    animationStep++;
}

void ChromaControl::drawCountDownProgress(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& Effect, int currentFrameInPart, int part) {
    double colProgress = easeOutExpo(currentFrameInPart, 0.0, 0.33, 30);
    colProgress = (colProgress + (part *0.33)) * ChromaSDK::Keyboard::MAX_COLUMN;
    ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Mask = {};
    maskUntilColumnInterPolated(Mask, colProgress);
    maskEffect1(Effect, Mask);
    ChromaControl::displayAndAdvanceFrame(Effect);
}

void ChromaControl::updateChroma() {
    if (enabled) {
        switch(activeState) {
        case AS_Initial:
        {
            pn_left.SetFrequency(1);
            pn_right.SetFrequency(1);
            // display white pulsing animation
            float animationTimeMS = 4000;
            double sineStep = 2 * PI * ((float) animationMS / animationTimeMS);
            double factor = sin(- PI / 2.0 + (animationStep * sineStep)) /2.0 + 0.5;
            // factor should now be between 0 and 1
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Effect = {};
            DeleteEffect(newFrame);
            newFrame = GUID_NULL;
            float grayScale = 255.0 * factor;
            COLORREF grayColor = RGB(grayScale, grayScale, grayScale);
            for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
            {
                for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
                {
                    Effect.Color[row][col] = grayColor;
                }
            }
            CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &Effect, &newFrame);
            displayAndAdvanceFrame(Effect);
            break;
        }
        case AS_MainMenu:
        {
            pn_left.SetFrequency(1);
            pn_right.SetFrequency(1);
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Effect = {};
            generateBlueOrangeFlag(Effect);
            CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &Effect, &newFrame);
            displayAndAdvanceFrame(Effect);
            break;
        }
        case AS_CountDown:
        {
            pn_left.SetFrequency(1);
            pn_right.SetFrequency(1);
            int framesSinceCDStart = animationStep - countDownStart;
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Effect = {};
            generateBlueOrangeFlag(Effect);
            // let flag drawing do its thing
            if (framesSinceCDStart < 30) {
                // first fade out existing visualization

                    // display white pulsing animation
                    double factor = 1 - easeOutExpo(framesSinceCDStart, 0.0, 1.0, 30);
                    // factor should now be between 1 and 0
                    ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Mask = {};
                    COLORREF mask = RGB(255 * factor, 255 * factor, 255 * factor);
                    for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
                    {
                        for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
                        {
                            Mask.Color[row][col] = mask;
                        }
                    }
                    chromaAnimationUtils::maskEffect1(Effect, Mask);
                    displayAndAdvanceFrame(Effect);
            } else
            if (framesSinceCDStart < 60) {
                drawCountDownProgress(Effect, framesSinceCDStart - 30, 0);
            } else

            if (framesSinceCDStart < 90) {
                drawCountDownProgress(Effect, framesSinceCDStart - 60, 1);
            } else

            if (framesSinceCDStart < 120) {
                drawCountDownProgress(Effect, framesSinceCDStart - 90, 2);
            }
            break;
        }
        case AS_CountDownTut:
        {
            pn_left.SetFrequency(1);
            pn_right.SetFrequency(1);
            int framesSinceCDStart = animationStep - countDownStart;
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Effect = {};
            generateBlueOrangeFlag(Effect);
            // let flag drawing do its thing
            if (framesSinceCDStart < 30) {
                drawCountDownProgress(Effect, framesSinceCDStart, 0);
            } else

            if (framesSinceCDStart < 60) {
                drawCountDownProgress(Effect, framesSinceCDStart - 30, 1);
            } else

            if (framesSinceCDStart < 90) {
                drawCountDownProgress(Effect, framesSinceCDStart - 60, 2);
            }
            break;
        }
        case AS_Go :
        {
            int frameInGo = animationStep - goStart;
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Effect = {};
            generateBlueOrangeFlag(Effect);
            double angle = -0.8;
            double strokeWidth = 4;
            double progress = easeInOutQuad(frameInGo, 0.0, 22.0 + strokeWidth, 20);
            generateBlueOrangeFlag(Effect);

            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Mask = {};
            double midPointVirtual = ChromaSDK::Keyboard::MAX_ROW/2 + 0.5;

            for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
            {
                double outerPoint = fabs(row-midPointVirtual) * angle - angle/2 + progress;
                double factor = outerPoint;
                int lowerbound = floor(factor);
                if (lowerbound >= -2.0) {
                    int upperbound = std::min(22, lowerbound + (int)strokeWidth);
                    for(UINT col=std::max(0,lowerbound); col<upperbound; col++)
                    {
                        double rowFactor;
                        if (col == lowerbound || col == 0) {
                            rowFactor = 1- (factor - lowerbound);
                        } else if (col == upperbound - 1) {
                            rowFactor = (factor - lowerbound);
                        } else {
                            rowFactor = 1.0;
                        }
                        if (col == 7);

                        Mask.Color[row][col] = RGB(goldIsh.r * rowFactor, goldIsh.g * rowFactor, goldIsh.b * rowFactor);
                    }

                }

            }

            overlayEffect1With2(Effect, Mask);
            displayAndAdvanceFrame(Effect);
            if (frameInGo > 20) {
                activeState = AS_MainMenu;
            }
            break;
        }
        case AS_Goal:
        {
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Effect ={};
            for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
            {
                for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
                {
                    Effect.Color[row][col] = RGB(goldIsh.r, goldIsh.g, goldIsh.b);
                }
            }
            // effect now full gold-ish
            int framesInGoal = animationStep - goalStart;
            // right side
            ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Mask ={};
            if (framesInGoal < 10) {
                double animProgress = easeOutExpo(framesInGoal, 0.0, 1.0, 10);
                // initialize  mask to 0 (full mask), then from middle create mask,  with linear values between 255 (outside) to 0(inside)
                for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
                {
                    for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
                    {
                        Mask.Color[row][col] = RGB(red.r * animProgress, 0, 0);
                    }
                }
                chromaAnimationUtils::maskEffect1(Effect, Mask);
            } else if (framesInGoal < 60) {
                for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
                {
                    int colDistanceToCenter = abs(col- ChromaSDK::Keyboard::MAX_COLUMN/2);
                    int frameOffsetForCol = 10 + (50 / ChromaSDK::Keyboard::MAX_COLUMN/2 * colDistanceToCenter);
                    double colProgress = easeOutExpo(framesInGoal - frameOffsetForCol, 0.0, 1.0, 60);
                    colProgress = 1 - colProgress;

                        for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
                        {
                            if (colProgress <= 1.0) {
                                Mask.Color[row][col] = RGB(red.r * colProgress, 0, 0);
                            } else {
                                Mask.Color[row][col] = RGB(red.r, 0, 0);
                        }
                    }
                }
                maskEffect1(Effect, Mask);
            } else if (framesInGoal <= 120) {
                if (framesInGoal == 60) {
                }
                double freqFactor = easeInOutSin(framesInGoal -75, 1.0, 5.0, 22.5);
                if (freqFactor >= 1) {
                    pn_left.SetFrequency(freqFactor);
                    pn_right.SetFrequency(freqFactor);
                }
                generateBlueOrangeFlag(Effect);
                // let flag drawing do its thing
                // first fade out existing visualization

                // display white pulsing animation
                double factor = easeInOutQuad(framesInGoal - 60, 0.0, 1.0, 30);
                // factor should now be between 0 and 1

                ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Mask = {};
                COLORREF mask = RGB(255 * factor, 255 * factor, 255 * factor);
                if (framesInGoal >= 90) {
                    mask = RGB(255, 255, 255);
                }
                for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
                {
                    for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
                    {
                        Mask.Color[row][col] = mask;
                    }
                }
                chromaAnimationUtils::maskEffect1(Effect, Mask);
                }

                if (framesInGoal == 120) {
                    activeState = AS_MainMenu;
                    pn_left.SetFrequency(1);
                    pn_right.SetFrequency(1);
                }
            displayAndAdvanceFrame(Effect);

            break;

        }

        default:
            break;
    }

    }
}

void ChromaControl::generateBlueOrangeFlag(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE &Effect) {
    // orange vs blue flags waving
    double zTimeStep = 0.025;
    // setup blue vs orange base color effect
    DeleteEffect(newFrame);
    newFrame = GUID_NULL;
    double thisAnimStep = -1.0 + animationStep * zTimeStep;

    for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
    {
        for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
        {
            int assignBlueTo;
            switch(row) {
            case 0:
                assignBlueTo = LOBYTE(RZKEY_F8);
                break;
            case 1:
                assignBlueTo = LOBYTE(RZKEY_9);
                break;
            case 2:
                assignBlueTo = LOBYTE(RZKEY_I);
                break;
            case 3:
                assignBlueTo = LOBYTE(RZKEY_K);
                break;
            case 4:
                assignBlueTo = LOBYTE(RZKEY_M);
                break;
            case 5:
                assignBlueTo = LOBYTE(RZKEY_SPACE);
                break;
            }

            double x = (double)col/((double)ChromaSDK::Keyboard::MAX_COLUMN);
            double y = (double)row/((double)ChromaSDK::Keyboard::MAX_ROW);


            module::Perlin* choose;
            colorRGB color;
            double n;

            if (col <= assignBlueTo) {
                choose = &pn_left;
                color = blue;
                n = choose->GetValue(x , y , thisAnimStep);
            } else {
                choose = &pn_right;
                color=orange;
                n = choose->GetValue(x , y, thisAnimStep);
            }

            n = std::max(-1.0, n);
            n = std::min(1.0, n);
            n = (n / 2.0) + 0.5;

            Effect.Color[row][col] = RGB(color.r * n, color.g * n, color.b *n);

        }
    }
    // set razer logo straight blue
    setRazerLogo(Effect, blue);
    // set effect etc
}
