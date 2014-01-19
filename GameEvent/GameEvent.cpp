//
//  GameEvent.cpp
//  dodge
//
//  Created by Pavel Ignatov on 04.01.14.
//  Copyright (c) 2014 UNNYHOG Entertainment. All rights reserved.
//

#include "GameEvent.h"
#include "GameEventLove.h"
#include "LOGlobalVar.h"

GameEvent * gameEvents[LOGE_Count];

void GameEvent::loadAllEvents()
{
    gameEvents[LOGE_Love] = new GameEventLove;
}

void GameEvent::setQuestNumber(int number)
{
    questNumber = number;
}

void GameEvent::setParameters(char * _parameters, unsigned long _beginTime, unsigned long _endTime)
{
    parameters = new char[strlen(_parameters)+1];
    strcpy(parameters, _parameters);
    
    beginTime = _beginTime;
    endTime = _endTime;
}

bool GameEvent::isActive()
{
    unsigned long time = getCurrentServerTime();
    return (time >= beginTime && time <= endTime);
}

GameEvent::GameEvent()
{
    parameters = NULL;
    questNumber = 0;
    beginTime = endTime = 0;
    gameQuest = 0;
    eventComplete = false;
}

GameEvent::~GameEvent()
{
    if (parameters)
        delete []parameters;
    if (gameQuest)
        delete gameQuest;
}

bool GameEvent::isComplete()
{
    return false;
}

void GameEvent::completeTask(LOTask * task)
{
    for (short i = 0;i<LOGE_Count;i++)
    {
        if (task == gameEvents[i]->gameQuest->task)
            gameEvents[i]->startNextQuest();
    }
}

void GameEvent::startNextQuest()
{
    questNumber++;
    if (gameQuest)
        delete gameQuest;
    activateQuest();
}

void GameEvent::checkTaskComplition(LOTask * task)
{
    if (task->taskProgress>=task->taskInfo)
        completeTask(task);
}

LOTask * GameEvent::getActiveTask()
{
    return NULL;
}

void GameEvent::activateQuest()
{
    
}

void GameEvent::getEventProgress(short eventNum, short &quest, int &progress)
{
    if (eventNum < LOGE_Count)
    {
        quest = gameEvents[eventNum]->questNumber;
        if (gameEvents[eventNum]->gameQuest)
            progress = gameEvents[eventNum]->gameQuest->task->taskProgress;
    }
}

void GameEvent::setEventProgress(short eventNum, short quest, int progress)
{
    if (eventNum < LOGE_Count)
    {
        gameEvents[eventNum]->questNumber = quest;
        gameEvents[eventNum]->activateQuest();
        if (gameEvents[eventNum]->gameQuest)
            gameEvents[eventNum]->gameQuest->task->taskProgress = progress;
    }
}

GameQuest * GameEvent::getAvailableQuest()
{
    for (short i = 0;i<LOGE_Count;i++)
    {
        if (gameEvents[i]->isActive() && !gameEvents[i]->isComplete())
            return gameEvents[i]->gameQuest;
    }
    return NULL;
}

LOTask * GameEvent::getAvailableTask()
{
    for (short i = 0;i<LOGE_Count;i++)
    {
        if (gameEvents[i]->isActive() && !gameEvents[i]->isComplete())
            return gameEvents[i]->getActiveTask();
    }
    return NULL;
}
