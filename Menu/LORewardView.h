//
//  LORewardView.h
//  LookOut
//
//  Created by Pavel Ignatov on 27.10.13.
//
//

#ifndef __LookOut__LORewardView__
#define __LookOut__LORewardView__

#include <iostream>
#include "LOMagicBox.h"
#include "LOTutorial.h"
#include "SunnyButton.h"

struct LOCrashBox
{
    SunnyVector3D position;
    SunnyVector3D velocity;
    SunnyVector3D rotation;
    SunnyVector3D rotationVelocity;
};

class LORewardView : public LOTutorial
{
    LOMagicBoxType magicBoxType;
    char * labelTap;
    SunnyButton * tapButton;
    
    SunnyMatrix * crashModels;
    LOCrashBox * modelsVelocity;
    
    LOIceState iceState;
    void prepareToCrash();
    float timer;
    float overlayAlpha;
    
    LODrop * drop;
    char * labelYouFound;
    char * labelTake;
    SunnyButton * takeButton;
    
    void drawDrop(SunnyMatrix * m,LODropInfo *info, float alpha);
    void drawDropOverlay(SunnyMatrix * m,LODropInfo *info, float alpha);
    bool soundsPlayed[maxDropCount];
    bool magicSoundPlayed;
    bool playTapSound;
    bool playDestroySound;
    bool needToDropPuzzle;
public:
    LORewardView();
    ~LORewardView();
    
    void render();
    bool renderOverlay();
    void update(float deltaTime);
    void initWithMagicBox(LOMagicBoxType type);
    void buttonPressed(SunnyButton * btn);
    void closeWindow();
};

#endif /* defined(__LookOut__LORewardView__) */
