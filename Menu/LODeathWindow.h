//
//  LODeathWindow.h
//  LookOut
//
//  Created by Ignatov on 10.10.13.
//
//

#ifndef __LookOut__LODeathWindow__
#define __LookOut__LODeathWindow__

#include <iostream>
#include "SunnyButton.h"
#include "LOScore.h"

enum LODWButtons
{
    LODWB_Ressurect = 0,
    LODWB_Store,
    LODWB_MainMenu,
    LODWB_Replay,
    LODWB_Spell_Magnet,
    LODWB_Spell_Time,
    LODWB_Spell_Doubler,
    LODWB_Lock,
    LODWB_Count,
};

class LODeathWindow
{
    char * labelGameOver;
    char * labelSaveMe;
    LOPriceType ressurectType;
    unsigned short ressurectionPrice;
    float animationTime;
    float rotationTime;
    bool crashed;
    SunnyMatrix snowballMatrix;
    float appearAlpha;
    bool hintShowedUp;
    
    void updateTasks(bool animated = false);
    void updateTasksAnimated();
    short lastPlaySound;
    bool ressurectionWasUsed;
    bool pausedForBuy;
    float ressurectionAlpha;
    float globalMoneyAlpha;
    
    void recalculateSurvivalPrices(SunnyButton * btn);
public:
    bool isVisible;
    LODeathWindow();
    ~LODeathWindow();
    
    SunnyButton * buttons[LODWB_Count];
    void checkForSpellsUpdates();
    void buttonPressed(SunnyButton *btn);
    void setVisible(bool visible);
    void render();
    void renderOverlay();
    void update();
    void renderCallBack(SunnyButton *btn);
};

#endif /* defined(__LookOut__LODeathWindow__) */
