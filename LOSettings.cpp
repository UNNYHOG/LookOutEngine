//
//  LOSettings.cpp
//  LookOut
//
//  Created by Ignatov on 11.11.13.
//
//

#include "LOSettings.h"
#include "iPhone_Strings.h"
#include "LOGlobalVar.h"

const int settingsVersion2 = 1;

struct SettingsData
{
    unsigned long tutorialInfo;
    float soundsVolume;
    float musicVolume;
    bool vibration;
    bool leftHand;
} settingsData;

bool LOSettings::addTutorialFlag(unsigned long flag)
{
    if (flag & settingsData.tutorialInfo) return false;
    
    settingsData.tutorialInfo |= flag;
    saveSettings();
    return true;
}

bool LOSettings::checkTurotialFlag(unsigned long flag)
{
    return (flag & settingsData.tutorialInfo);
}

void LOSettings::changeControlType()
{
    if (controlType == LOC_Joystic)
    {
        controlType = LOC_Accelerometer;
    } else
    {
        controlType = LOC_Joystic;
    }
}

void LOSettings::loadEmptyProfile()
{
    settingsData.tutorialInfo = 0;
    controlType = LOC_Joystic;
    settingsData.leftHand = false;
    settingsData.vibration = true;
    settingsData.soundsVolume = settingsData.musicVolume = 1;
}

bool LOSettings::canVibrate()
{
    return settingsData.vibration;
}

void LOSettings::changeVibrationSettings()
{
    settingsData.vibration = !settingsData.vibration;
}

bool LOSettings::isLeftHanded()
{
    return settingsData.leftHand;
}

void LOSettings::changeLeftHanded()
{
    settingsData.leftHand = !settingsData.leftHand;
    LOMenu::getSharedMenu()->leftHandedChanged();
}

float LOSettings::getSoundsVolume()
{
    return settingsData.soundsVolume;
}

float LOSettings::getMusicVolume()
{
    return settingsData.musicVolume;
}

void LOSettings::setSoundsVolume(float value)
{
    settingsData.soundsVolume = value;
}

void LOSettings::setMusicVolume(float value)
{
    settingsData.musicVolume = value;
}

void LOSettings::switchSoundsVolume()
{
    if (settingsData.soundsVolume>0.5)
        settingsData.soundsVolume = 0;
    else
        settingsData.soundsVolume = 1;
    losChangeSoundVolume(settingsData.soundsVolume);
}

void LOSettings::switchMusicVolume()
{
    if (settingsData.musicVolume>0.5)
        settingsData.musicVolume = 0;
    else
        settingsData.musicVolume = 1;
    losSetMusicVolume(settingsData.musicVolume);
}

void LOSettings::saveSettings()
{
    const char * fileName = getHomePath("settings.loset");
    
    FILE * stream = fopen(fileName,"wb");
    if (!stream)
    {
        printf("\n file not found for save : %s",fileName);
        return;
    }
    fwrite(&settingsVersion2, sizeof(settingsVersion2), 1, stream);
    fwrite(&settingsData, sizeof(settingsData), 1, stream);
    fwrite(&controlType, sizeof(controlType), 1, stream);
    fclose(stream);
}

bool LOSettings::loadSettings()
{
    const char * fileName = getHomePath("settings.loset");
    
    FILE * stream = fopen(fileName,"rb");
    if (!stream)
    {
        printf("\n file not found : %s",fileName);
        return false;
    }
    int version;
    fread(&version, sizeof(version), 1, stream);
    fread(&settingsData, sizeof(settingsData), 1, stream);
    fread(&controlType, sizeof(controlType), 1, stream);
    fclose(stream);
    
    return true;
}

void LOSettings::init()
{
    if (!loadSettings())
        loadEmptyProfile();
    
    losChangeSoundVolume(settingsData.soundsVolume);
    losSetMusicVolume(settingsData.musicVolume);
    LOMenu::getSharedMenu()->leftHandedChanged();
}

