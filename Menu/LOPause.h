//
//  LOPause.h
//  LookOut
//
//  Created by Ignatov on 05.10.13.
//
//

#ifndef __LookOut__LOPause__
#define __LookOut__LOPause__

#include <iostream>
#include "SunnyButton.h"

enum LOPButtons
{
    LOPB_MainMenu = 0,
    LOPB_Store,
    LOPB_Replay,
    LOPB_Next,
    LOPB_Settings,
    LOPB_Count,
};

class LOPause
{
    SunnyButton *buttons[LOPB_Count];
    
    char *labelPause;
    char *labelEpisode;
    char *labelLevel;
    
    float levelLength;
    float episodeLength;
    float numberLength;
    float appearAlpha;
public:
    bool isVisible;
    LOPause();
    ~LOPause();
    void buttonPressed(SunnyButton *btn);
    void hideWindow();
    
    void setVisible(bool visible);
    void render();
    void update();
    
    SunnyButton * getStoreButton();
};

#endif /* defined(__LookOut__LOPause__) */
