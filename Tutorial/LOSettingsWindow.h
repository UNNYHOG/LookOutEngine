//
//  LOSettingsWindow.h
//  LookOut
//
//  Created by Pavel Ignatov on 18.11.13.
//
//

#ifndef __LookOut__LOSettingsWindow__
#define __LookOut__LOSettingsWindow__

#include <iostream>
#include "LOTutorial.h"

enum SettingsButtons
{
    SB_Music,
    SB_Sounds,
    SB_Vibration,
    SB_GameCenter,
#ifndef __ANDROID__
    SB_Leaderboard,
#endif
    SB_Control,
    SB_LeftHand,
    SB_Count,
};

class LOSettingsWindow : public LOTutorial
{
    SunnyButton * buttons[SB_Count];
public:
    LOSettingsWindow();
    ~LOSettingsWindow();
    
    void render();
    void buttonPressed(SunnyButton *btn);
};

#endif /* defined(__LookOut__LOSettingsWindow__) */
