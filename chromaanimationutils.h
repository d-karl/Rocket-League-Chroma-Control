#ifndef CHROMAANIMATIONUTILS_H
#define CHROMAANIMATIONUTILS_H

#include <windows.h>
#include "RzChromaSDKTypes.h"
#include "structcolorrgb.h"
#include <math.h>

using namespace ChromaSDK;
using namespace ChromaSDK::Keyboard;
using namespace ChromaSDK::Keypad;
using namespace ChromaSDK::Mouse;
using namespace ChromaSDK::Mousepad;
using namespace ChromaSDK::Headset;

#define PI 3.14159265

namespace chromaAnimationUtils{

    void maskEffect1(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect1, ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& mask);
    void overlayEffect1With2(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect1, ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect2);
    void setRazerLogo(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect, colorRGB color);
    void linearInterpolateIntoColor1(colorRGB& color1, colorRGB color2, double interPolationFactor);
    void maskUntilColumnInterPolated(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect1, double columnProgress);


    namespace easing {
        double easeInOutSin(double currentFrame, double startValue, double endVal, double durationFrames);
        double easeInOutQuad(double currentFrame, double startValue, double endVal, double durationFrames);
        double easeOutExpo(double currentFrame, double startValue, double endVal, double durationFrames);
    }
}

#endif // CHROMAANIMATIONUTILS_H
