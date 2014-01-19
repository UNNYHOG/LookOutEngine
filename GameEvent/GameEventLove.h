//
//  GameEventLove.h
//  dodge
//
//  Created by Pavel Ignatov on 05.01.14.
//  Copyright (c) 2014 UNNYHOG Entertainment. All rights reserved.
//

#ifndef __dodge__GameEventLove__
#define __dodge__GameEventLove__

#include <iostream>
#include "GameEvent.h"

class GameEventLove: public GameEvent
{
    void activateQuest();
public :
    
    bool isComplete();
    LOTask * getActiveTask();
};

#endif /* defined(__dodge__GameEventLove__) */
