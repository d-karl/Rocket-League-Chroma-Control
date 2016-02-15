#include "chromaanimationutils.h"
#include <algorithm>
#include <iostream>

double chromaAnimationUtils::easing::easeInOutQuad(double currentFrame, double startValue, double endVal, double durationFrames) {
    currentFrame /= durationFrames/2;
    if (currentFrame < 1) return endVal/2*currentFrame*currentFrame + startValue;
    currentFrame--;
    return -endVal/2 * (currentFrame*(currentFrame-2) - 1) + startValue;
}

double chromaAnimationUtils::easing::easeInOutSin(double currentFrame, double startValue, double endVal, double durationFrames) {
    return -endVal/2 * (cos(PI*currentFrame/durationFrames) - 1) + startValue;
}

double chromaAnimationUtils::easing::easeOutExpo(double currentFrame, double startValue, double endVal, double durationFrames) {
    return endVal * (-pow( 2, -10 * (currentFrame/durationFrames)) + 1 ) + startValue;
}

void chromaAnimationUtils::setRazerLogo(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect, colorRGB color) {
    effect.Color[HIBYTE(RZKEY_RZRLOGO)][LOBYTE(RZKEY_RZRLOGO)] = RGB(color.r, color.g, color.b);
}

void chromaAnimationUtils::linearInterpolateIntoColor1(colorRGB& color1, colorRGB color2, double interPolationFactor) {
     color1.r = color1.r + ( (color2.r - color1.r) * interPolationFactor);
     color1.g = color1.g + ( (color2.g - color1.g) * interPolationFactor);
     color1.b = color1.b + ( (color2.b - color1.b) * interPolationFactor);
}


void chromaAnimationUtils::overlayEffect1With2(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect1, ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect2) {
    colorRGB color1;
    colorRGB color2;
    for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
    {
        for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
        {
            color2.r = (int) GetRValue(effect2.Color[row][col]);
            color2.g = (int) GetGValue(effect2.Color[row][col]);
            color2.b = (int) GetBValue(effect2.Color[row][col]);
            if (color2.r != 0 || color2.g != 0 || color2.b != 0) {
                effect1.Color[row][col] = RGB(color2.r, color2.g, color2.b);
            }
        }
    }
}

// mix color from effect 1 with mask. masks should only have colors between 255 (no mask) to 0 (max mask)
void chromaAnimationUtils::maskEffect1(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect1, ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& mask) {
    colorRGB color1;
    colorRGB maskColor;
    for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
    {
        for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
        {
            color1.r = (int) GetRValue(effect1.Color[row][col]);
            color1.g = (int) GetGValue(effect1.Color[row][col]);
            color1.b = (int) GetBValue(effect1.Color[row][col]);
            maskColor.r = (int) GetRValue(mask.Color[row][col]);
            double maskFactor = (double) maskColor.r / 255.0;
            effect1.Color[row][col] = RGB(color1.r * maskFactor, color1.g * maskFactor, color1.b * maskFactor);
        }
    }
}

void chromaAnimationUtils::maskUntilColumnInterPolated(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE& effect1, double columnProgress) {
    int lastFullCol = (int) columnProgress;
    float maskFactor = columnProgress - lastFullCol;
    for(UINT col=0; col<ChromaSDK::Keyboard::MAX_COLUMN; col++)
    {
        for(UINT row=0; row<ChromaSDK::Keyboard::MAX_ROW; row++)
        {
            if (lastFullCol == col) {
                effect1.Color[row][col] = 255 * maskFactor;
            } else if (col < lastFullCol) {
                effect1.Color[row][col] = 255;
            }
            // other case is current column before lastFullCol -> let colum stay unmasked
        }
    }

}
