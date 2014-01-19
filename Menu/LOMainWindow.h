//
//  LOMainWindow.h
//  LookOut
//
//  Created by Ignatov on 10.10.13.
//
//

#ifndef __LookOut__LOMainWindow__
#define __LookOut__LOMainWindow__

#include <iostream>
#include "SunnyButton.h"

enum LOMWButtons
{
    LOMWB_Play = 0,
    LOMWB_PlaySurvival,
    LOMWB_Store,
    LOMWB_Settings,
    LOMWB_Puzzle,
    LOMWB_Count,
};

class LOMainWindow
{
    SunnyButton *buttons[LOMWB_Count];
    SunnyVector2D overlayParameters[3];
    float mainWindowTime;
    
    SunnyMatrix snowballMatrix;
    float playScale;
    
public:
    bool isVisible;
    LOMainWindow();
    ~LOMainWindow();
    static LOMainWindow * getSharedWindow();
    
    void buttonPressed(SunnyButton *btn);
    
    void renderPlayButton();
    void update();
    void setVisible(bool visible);
    void render();
    void renderOverlay();
};

#endif /* defined(__LookOut__LOMainWindow__) */
