//
//  GameQuest.cpp
//  dodge
//
//  Created by Pavel Ignatov on 04.01.14.
//  Copyright (c) 2014 UNNYHOG Entertainment. All rights reserved.
//

#include "GameQuest.h"
#include "LOGlobalVar.h"

GameQuest::GameQuest(LOTaskType type, int info, bool isSurv, int map, int progress)
{
    task = new LOTask;
    task->taskType = type;
    task->taskInfo = info;
    task->taskProgress = progress;
    
    survival = isSurv;
    mapNumber = map;
}

bool GameQuest::isAvailable()
{
    if (survival != isSurvival) return false;
    
    if (survival)
        return (selectedStepNum + 1 == mapNumber);
    else
        return (selectedStepNum*loLevelsPerStep + selectedLevelNum + 1 == mapNumber);
}

GameQuest::~GameQuest()
{
    delete task;
}