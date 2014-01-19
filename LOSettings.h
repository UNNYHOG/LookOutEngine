//
//  LOSettings.h
//  LookOut
//
//  Created by Ignatov on 11.11.13.
//
//

#ifndef __LookOut__LOSettings__
#define __LookOut__LOSettings__

#include <iostream>
#include "LOGlobalVar.h"

#define TUTORIAL_SNOWFLAKES (1)
#define TUTORIAL_FIREBALL (1<<1)
#define TUTORIAL_GROUNDBALL (1<<2)
#define TUTORIAL_WATERBALL (1<<3)
#define TUTORIAL_TELEPORTATION (1<<4)
#define TUTORIAL_SURVIVAL (1<<5)

class LOSettings
{
public:
    static bool addTutorialFlag(unsigned long flag);
    static bool checkTurotialFlag(unsigned long flag);
    
    static void init();
    static void loadEmptyProfile();
    static void saveSettings();
    static bool loadSettings();
    static bool canVibrate();
    static void changeVibrationSettings();
    static bool isLeftHanded();
    static void changeLeftHanded();
    static float getSoundsVolume();
    static float getMusicVolume();
    static void setSoundsVolume(float value);
    static void setMusicVolume(float value);
    static void switchSoundsVolume();
    static void switchMusicVolume();
    
    static void changeControlType();
};

#endif /* defined(__LookOut__LOSettings__) */
