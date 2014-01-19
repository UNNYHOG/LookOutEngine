//
//  LOLevelComplete.h
//  LookOut
//
//  Created by Pavel Ignatov on 01.10.13.
//
//

#ifndef __LookOut__LOLevelComplete__
#define __LookOut__LOLevelComplete__

#include <iostream>
#include "SunnyButton.h"

enum LOLCButtons
{
    LOLCB_Skip = 0,
    LOLCB_MainMenu,
    LOLCB_Store,
    LOLCB_Replay,
    LOLCB_Next,
    LOLCB_Share,
    LOLCB_Count,
};

class LOLevelComplete
{
    SunnyButton *buttons[LOLCB_Count];
    char *labelLevelComplete;
    char *labelResult[3];
    char *labelNextCoin;
    
    float levelTime;
    short levelTime2;
    short levelTime3;
    short levelCoins;
    
    char nextCoinTime[16];
    float nextCoinLength[2];
    float timeLength;
    
    float animationTime;
    
    SunnyVector3D coinRotationAngle;
    SunnyVector3D coinRotationSpeed;
    float nextCoinAnimationTime;
    short nextAnimatedCoin;
    
    void activateButtons();
    
    bool timerPlayed,levelLabelPlayed;
    SunnyMatrix playerPosition;
    
    SunnyVector4D lastWind;
    SunnyVector2D lastNewWind;
    short coinSoundPlayed;
public:
    bool isVisible;
    void prepareWindow(float time, short coins,short time2, short time3);
    void buttonPressed(SunnyButton *btn);
    void update();
    static void setTableWidth(float value);
    
    LOLevelComplete();
    ~LOLevelComplete();
    
    void setVisible(bool visible);
    void render();
    void renderOverlay();
    bool didHideEverything();
};

#endif /* defined(__LookOut__LOLevelComplete__) */
