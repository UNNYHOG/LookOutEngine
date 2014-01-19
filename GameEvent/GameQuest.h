//
//  GameQuest.h
//  dodge
//
//  Created by Pavel Ignatov on 04.01.14.
//  Copyright (c) 2014 UNNYHOG Entertainment. All rights reserved.
//

#ifndef __dodge__GameQuest__
#define __dodge__GameQuest__

#include <iostream>
#include "LOScore.h"

class GameQuest
{
    bool survival;
    int mapNumber;
public :
    LOTask * task;
    
    GameQuest(LOTaskType type, int info, bool isSurv, int map, int progress);
    ~GameQuest();
    
    bool isAvailable();
};

#endif /* defined(__dodge__GameQuest__) */
