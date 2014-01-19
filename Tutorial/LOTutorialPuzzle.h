//
//  LOTutorialPuzzle.h
//  LookOut
//
//  Created by Pavel Ignatov on 17.11.13.
//
//

#ifndef __LookOut__LOTutorialPuzzle__
#define __LookOut__LOTutorialPuzzle__

#include <iostream>
#include "LOTutorial.h"
#include "LOPuzzleObject.h"

class LOTutorialPuzzle : public LOTutorial
{
    char * puzzleText;
    SunnyButton * puzzleButton;
    SunnyButton * laterButton;
    char * laterText;
    char * puzzleButtonText;
    float puzzleDelayTime;
    
    LOPuzzleObject* puzzleObject;
    bool justClose;
public:
    LOTutorialPuzzle(SunnyVector2D pos, unsigned short number, bool _justClose);
    ~LOTutorialPuzzle();
    
    bool blockRenderOverlays();
    void render();
    void update(float deltaTime);
    void buttonPressed(SunnyButton *btn);
};


#endif /* defined(__LookOut__LOTutorialPuzzle__) */
