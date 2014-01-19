//
//  LOTasksView.h
//  LookOut
//
//  Created by Ignatov on 21.10.13.
//
//

#ifndef __LookOut__LOTasksView__
#define __LookOut__LOTasksView__

#include <iostream>

const short activeTasksCount = 3;

#include "LOScore.h"
#include "SunnyButton.h"

const short taskViewStarsCount = 5;
const float taskViewStarAppearTime = 0.5;
const float taskTextDissapearAnimationTime = 0.5;
const float taskTextAppearAnimationTime = 0.5;
const float taskTextBorderWaitTime = 3;
const float taskTextBorderDissapearTime = 1;

class LOStarAnimation
{
public:
    bool isAnimated;
    float animationTime;
    float scale;
    bool update();
    void animate();
    void stopAnimation();
};

class LOTaskTextAnimation
{
public:
    LOTaskTextAnimation();
    bool isTextDissapearing();
    bool isAnimated;
    float animationTime;
    float progress;
    float borderAlpha;
    char *oldText;
    bool update();
    void animate(char * text);
    void stopAnimation();
};

class LOMagicBoxAppear
{
public:
    bool isAnimated;
    float animationTime;
    void prepareToAnimation();
    void update(float deltaTime);
    float scale;
    float overlayAlpha;
    short soundTag;
};

class LOTasksView
{
    char ** tasksNames;
    LOTask **activeTasks;
    bool *newTask;
    char percentage[10];
    
    char ** tasksNamesBuf;
    LOTask ** activeTasksBuf;
    char percentageBuf[10];
    short bufTasksLoaded;
    
    float appearAlpha;
    char * labelComplete;
    float labelCompleteSize;
    
    LOStarAnimation starsAnimation[taskViewStarsCount];
    LOTaskTextAnimation taskTextAnimation[activeTasksCount];

#ifdef DEVELOPER_MODE
    SunnyButton * testButtons[activeTasksCount];
#endif
    LOMagicBoxAppear animateBox;
    SunnyButton * lockButton;
    bool playStarSound;
public:
    bool isVisible;
    static LOTasksView *getSharedView();
    
    LOTasksView();
    ~LOTasksView();
    void updateInformation();
    void loadBufTasksIfNecessary(short firstP, short lastP);
    void setVisible(bool visible);
    void render(short firstP, short lastP, float firstPOffset);
    void renderOverlay();
    
    void animateTaskComplete(short num,char * oldText);
    
    void updateTasksProgress(bool animated = false);
    void testCompleteTask(short num);
    bool isLocked();
};

#endif /* defined(__LookOut__LOTasksView__) */
