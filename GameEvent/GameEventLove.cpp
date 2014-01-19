//
//  GameEventLove.cpp
//  dodge
//
//  Created by Pavel Ignatov on 05.01.14.
//  Copyright (c) 2014 UNNYHOG Entertainment. All rights reserved.
//

#include "GameEventLove.h"

bool GameEventLove::isComplete()
{
    return false;
}

void GameEventLove::activateQuest()
{
    switch (questNumber) {
        case 0:
            break;
        default:
            eventComplete = true;
            break;
    }
}

LOTask * GameEventLove::getActiveTask()
{
    if (gameQuest)
    {
        if (gameQuest->isAvailable())
            return gameQuest->task;
        else
            return NULL;
    }
    
    activateQuest();
    
    if (gameQuest)
    {
        if (gameQuest->isAvailable())
            return gameQuest->task;
        else
            return NULL;
    }
    
    return NULL;
}