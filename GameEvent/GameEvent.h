//
//  GameEvent.h
//  dodge
//
//  Created by Pavel Ignatov on 04.01.14.
//  Copyright (c) 2014 UNNYHOG Entertainment. All rights reserved.=
//

#ifndef __dodge__GameEvent__
#define __dodge__GameEvent__

#include <iostream>
#include "LOScore.h"
#include "GameQuest.h"

enum LOGameEvents
{
    LOGE_Love = 0,
    LOGE_Count,
};

class GameEvent
{
protected:
    char * parameters;
    unsigned long beginTime, endTime;
    int questNumber;
    GameQuest * gameQuest;
    bool eventComplete;
    void startNextQuest();
    virtual void activateQuest();
    static void completeTask(LOTask * task);
public:
    GameEvent();
    virtual ~GameEvent();

    void setQuestNumber(int number);
    void setParameters(char * _parameters, unsigned long _beginTime, unsigned long _endTime);
    bool isActive();
    virtual bool isComplete();
    virtual LOTask * getActiveTask();
    static GameQuest * getAvailableQuest();
    static LOTask * getAvailableTask();
    static void checkTaskComplition(LOTask * task);
    static void setEventProgress(short eventNum, short quest, int progress);
    static void getEventProgress(short eventNum, short &quest, int &progress);
    
    static void loadAllEvents();
};

#endif /* defined(__dodge__GameEvent__) */
