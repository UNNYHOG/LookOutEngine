//
//  LOButton.h
//  LookOut
//
//  Created by Pavel Ignatov on 12.09.12.
//
//

#ifdef WIN32
#define SKIP_PHYSICS
#endif

#ifndef __LookOut__LOGameButton__
#define __LookOut__LOGameButton__

#include "LOObject.h"
#include "SunnyVector2D.h"
#include "SunnyCollisionDetector.h"
#include <iostream>

enum LOButtonState
{
    LOBS_Down = 0,
    LOBS_Up,
    LOBS_WaitingDown,
    LOBS_WaitingUp,
};

class LOGameButton
{
    SunnyVector2D position;
public:
    SC_Circle *body;
    LOButtonState buttonState;
    short repeat;
    
    LOGameButton();
    ~LOGameButton();
    
    void initWithParams(SunnyVector2D pos,short repetable);
    void render();
    void reset();
};

#endif /* defined(__LookOut__LOButton__) */
