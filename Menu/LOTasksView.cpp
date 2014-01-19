//
//  LOTasksView.cpp
//  LookOut
//
//  Created by Ignatov on 21.10.13.
//
//

#include "LOTasksView.h"
#include "LOGlobalVar.h"
#include "LOFont.h"
#include "iPhone_Strings.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

const SunnyVector4D taskCompleteColor = SunnyVector4D(255/255.,210/255.,0,1);
const SunnyVector4D taskNewColor = SunnyVector4D(126./255.,240./255.,170./255,1);

LOTasksView *sharedTasksView = 0;

const SunnyVector2D starsPositionsForTasks[taskViewStarsCount] = {SunnyVector2D(166,286),SunnyVector2D(284,286),SunnyVector2D(404,284),SunnyVector2D(524,286),SunnyVector2D(642,286)};
const SunnyVector2D shelfsPositionsForTasks[] = {SunnyVector2D(480,442),SunnyVector2D(482,608),SunnyVector2D(486,774)};
const SunnyVector2D shelfsSizesForTasks[] = {SunnyVector2D(828,128),SunnyVector2D(832,140),SunnyVector2D(824,150)};

void LOStarAnimation::animate()
{
    isAnimated = true;
    scale = 0;
    animationTime = 0;
}

void LOStarAnimation::stopAnimation()
{
    scale = 1;
    isAnimated = false;
}

bool LOStarAnimation::update()
{
    if (isAnimated)
    {
        animationTime+=deltaTime;
        float k = 0.7;
        float s = 1.3;
        if (animationTime<taskViewStarAppearTime*k)
            scale = s * (animationTime/(taskViewStarAppearTime*k));
        else
            if (animationTime<taskViewStarAppearTime)
            {
                scale = s - (s-1)*(animationTime-taskViewStarAppearTime*k)/(taskViewStarAppearTime*(1-k));
            } else
            {
                stopAnimation();
            }
    }
    return isAnimated;
}

void LOTaskTextAnimation::stopAnimation()
{
    isAnimated = false;
    progress = 1;
    borderAlpha = 0;
    if (oldText)
        delete []oldText;
    oldText = 0;
}

LOTaskTextAnimation::LOTaskTextAnimation()
{
    oldText = 0;
}

bool LOTaskTextAnimation::isTextDissapearing()
{
    return animationTime<taskTextDissapearAnimationTime;
}

bool LOTaskTextAnimation::update()
{
    if (isAnimated)
    {
        animationTime += deltaTime;
        if (isTextDissapearing())
        {
            progress = (1-animationTime/taskTextDissapearAnimationTime);
        } else
        {
            float time = animationTime - taskTextDissapearAnimationTime;
            if (time < taskTextAppearAnimationTime)
            {
                progress = time/taskTextAppearAnimationTime;
            } else
            {
                progress = 1;
                time -= taskTextAppearAnimationTime + taskTextBorderWaitTime;
                if (time>0)
                {
                    borderAlpha = 1.0 - time/taskTextBorderDissapearTime;
                    if (borderAlpha<=0)
                        stopAnimation();
                }
            }
            return false;
        }
        return true;
    }
    return false;
}

void LOTaskTextAnimation::animate(char * text)
{
    animationTime = 0;
    if (oldText)
        delete []oldText;
    oldText = text;
    isAnimated = true;
    progress = 1;
    borderAlpha = 1;
}

void LOTasksView::animateTaskComplete(short num,char * oldText)
{
    int count = LOScore::getStarsCount();
    
    if (count<taskViewStarsCount)
        starsAnimation[count].animate();
    
    taskTextAnimation[num].animate(oldText);
    lockButton->hidden = false;
    
    playStarSound = true;
}

void testButtonPressed(SunnyButton * btn)
{
    sharedTasksView->testCompleteTask(btn->tag);
}

void taskViewLockButtonPressed(SunnyButton *btn)
{
    
}

LOTasksView::LOTasksView()
{
    for (short i = 0;i<taskViewStarsCount;i++)
        starsAnimation[i].stopAnimation();
    
    for (short i = 0;i<activeTasksCount;i++)
        taskTextAnimation[i].stopAnimation();
    
    sharedTasksView = this;
    isVisible = false;
    activeTasks = 0;
    tasksNames = new char*[activeTasksCount];
    tasksNamesBuf = new char*[activeTasksCount];
    for (short i = 0;i<activeTasksCount;i++)
    {
        tasksNames[i] = 0;
        tasksNamesBuf[i] = 0;
    }
    
    newTask = new bool[activeTasksCount];
    
    labelComplete = getLocalizedLabel("Completed");
    labelCompleteSize = LOFont::getTextSize(labelComplete);
    
    if (isVAOSupported)
    {
        glGenVertexArrays(1, &solidVAO);
        glBindVertexArray(solidVAO);
    }
    glGenBuffers(1, &solidVBO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);
    float solid[] = {0,0.5, 0,-0.5, 1,0.5, 1, -0.5};
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4, solid, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
#ifdef DEVELOPER_MODE
    for (short i = 0;i<activeTasksCount;i++)
    {
        testButtons[i] = sunnyAddButton(SunnyVector2D(shelfsPositionsForTasks[i].x*mapSize.x/1920,(1280-shelfsPositionsForTasks[i].y)/1280*mapSize.y));
        sunnyButtonSetCallbackFunc(testButtons[i], testButtonPressed);
        testButtons[i]->tag = i;
        testButtons[i]->hidden = true;
    }
#endif
    
    lockButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(100, 100));
    lockButton->hidden = true;
    sunnyButtonSetCallbackFunc(lockButton, taskViewLockButtonPressed);
    
    activeTasksBuf = new LOTask*[activeTasksCount];
    for (short i = 0;i<activeTasksCount;i++) activeTasksBuf[i] = NULL;
    bufTasksLoaded = 0;
    
    playStarSound = false;
}

LOTasksView::~LOTasksView()
{
    delete [] activeTasksBuf;
    for (short i = 0;i<activeTasksCount;i++)
    {
        delete [] tasksNames[i];
        delete [] tasksNamesBuf[i];
    }
    delete []tasksNames;
    delete []tasksNamesBuf;
    delete []newTask;
}

LOTasksView *LOTasksView::getSharedView()
{
    return sharedTasksView;
}

void LOTasksView::setVisible(bool visible)
{
    if (isVisible == visible) return;
    
#ifdef DEVELOPER_MODE
    for (short i = 0;i<activeTasksCount;i++)
        testButtons[i]->hidden = !visible;
#endif
    
    for (short i = 0;i<taskViewStarsCount;i++)
        starsAnimation[i].stopAnimation();
    for (short i = 0;i<activeTasksCount;i++)
        taskTextAnimation[i].stopAnimation();
    animateBox.isAnimated = false;
    
    isVisible = visible;
    
    updateInformation();
    
    for (short i = 0;i<activeTasksCount;i++)
        newTask[i] = false;
    
    appearAlpha = 1;
}

bool LOTasksView::isLocked()
{
    return !lockButton->hidden;
}

void LOTasksView::testCompleteTask(short num)
{
    if (activeTasks && activeTasks[num])
    {
        activeTasks[num]->taskProgress = activeTasks[num]->taskInfo;
        activeTasks[num]->complete = true;
    }
}

void LOTasksView::updateTasksProgress(bool animated)
{
    bool anyChanges = false;
    for (short i = 0;i<activeTasksCount;i++)
        if (activeTasks[i])
        {
            if (activeTasks[i]->complete)
            {
                activeTasks[i]->active = false;
                animateTaskComplete(i, getLocalizedTaskName(activeTasks[i]));
                activeTasks[i] = LOScore::activateRandomTask(selectedStepNum,i);
                LOScore::addStarForTask();
                newTask[i] = true;
                
                if (tasksNames[i])
                    delete [] tasksNames[i];
                tasksNames[i] = 0;
                if (activeTasks[i])
                {
                    tasksNames[i] = getLocalizedTaskName(activeTasks[i]);
                }
                anyChanges = true;
            }
        }
    
    if (anyChanges)
        LOScore::saveScores();
}

void LOTasksView::loadBufTasksIfNecessary(short firstP, short lastP)
{
    if (firstP==lastP)
    {
        bufTasksLoaded = 0;
        return;
    }
    
    short num = firstP;
    if (selectedStepNum == firstP) num = lastP;
    if (num>=loStepsCount || num<0) return;
    
    if (bufTasksLoaded == num-selectedStepNum) return;
    
    LOScore::loadActiveTasks(num, activeTasksBuf);
    
    bufTasksLoaded = num-selectedStepNum;
    
    for (short i = 0;i<activeTasksCount;i++)
    {
        if (tasksNamesBuf[i])
            delete [] tasksNamesBuf[i];
        tasksNamesBuf[i] = 0;
        if (activeTasksBuf[i])
            tasksNamesBuf[i] = getLocalizedTaskName(activeTasksBuf[i]);
    }
    
    short p = LOScore::getCompletedTasksCount(num)*100/LOScore::getTasksCount(num);
    short len = 0;
    if (p>=100)
    {
        percentageBuf[len++] = '0' + p/100;
        p = p%100;
    }
    if (p>=10)
    {
        percentageBuf[len++] = '0' + p/10;
        p = p%10;
    }
    
    percentageBuf[len++] = '0' + p;
    percentageBuf[len++] = '%';
    percentageBuf[len] = '\0';
}

void LOTasksView::updateInformation()
{
    for (short i = 0;i<activeTasksCount;i++)
        activeTasksBuf[i] = 0;
    bufTasksLoaded = 0;
    
    if (selectedStepNum>=loStepsCount) return;
    activeTasks = activeMap->loadActiveTasks();
    for (short i = 0;i<activeTasksCount;i++)
    {
        if (tasksNames[i])
            delete [] tasksNames[i];
        tasksNames[i] = 0;
        if (activeTasks[i])
        {
            tasksNames[i] = getLocalizedTaskName(activeTasks[i]);
        }
    }
    
    short p = LOScore::getCompletedTasksCount(selectedStepNum)*100/LOScore::getTasksCount(selectedStepNum);
    short p1 = p;
    short len = 0;
    if (p1>=100)
    {
        percentage[len++] = '0' + p/100;
        p = p%100;
    }
    if (p1>=10)
    {
        percentage[len++] = '0' + p/10;
        p = p%10;
    }
    
    percentage[len++] = '0' + p;
    percentage[len++] = '%';
    percentage[len] = '\0';
}

void LOMagicBoxAppear::prepareToAnimation()
{
    isAnimated = true;
    animationTime = 0;
    overlayAlpha = 0;
    scale = 1;
    soundTag = 0;
}

void LOMagicBoxAppear::update(float deltaTime)
{
    if (isAnimated)
    {
        animationTime += deltaTime;
        
        const float s = 1.3;
        const float blinkPerioud = 0.6;
        const float dimTime = blinkPerioud;
        float t = animationTime;
        if (t<blinkPerioud)
        {
            if (soundTag==0)
            {
                soundTag++;
                losPlayStarSound(true);
            }
            scale = 1.0 + (s-1)*sin(M_PI*t/blinkPerioud);
            overlayAlpha = (scale-1.0)/(s-1.0);
        } else
        {
            t-=blinkPerioud;
            if (t<blinkPerioud)
            {
                if (soundTag==1)
                {
                    soundTag++;
                    losPlayStarSound(true);
                }
                scale = 1.0 + (s-1)*sin(M_PI*t/blinkPerioud);
                overlayAlpha = (scale-1.0)/(s-1.0);
            } else
            {
                t-=blinkPerioud;
                if (t<=dimTime)
                {
                    scale = 1.0 - t/dimTime;
                    overlayAlpha = 0;
                } else
                {
                    isAnimated = false;
                    if (LOScore::getMagicBoxCount(LOMB_Small))
                    {
                        LOMenu::getSharedMenu()->showMagicBox(LOMB_Small);
                    }
                }
            }
        }
    }
}

void LOTasksView::render(short firstP, short lastP, float firstPOffset)
{
    if (!isVisible) return;
    if (playStarSound)
    {
        playStarSound = false;
        losPlayStarSound();
    }

    
    loadBufTasksIfNecessary(firstP, lastP);
    
    //Update Stars
    bool isStarsAnimated = false;
    for (short i = 0;i<taskViewStarsCount;i++)
        if (starsAnimation[i].update())
        {
            isStarsAnimated = true;
            if (i==taskViewStarsCount-1)
            {
                if (!animateBox.isAnimated)
                    animateBox.prepareToAnimation();
            }
        }
    if (!isStarsAnimated)
    {
        for (short i = 0;i<activeTasksCount;i++)
            if(taskTextAnimation[i].update())
                isStarsAnimated = true;
    }
    if (!isStarsAnimated && animateBox.isAnimated)
    {
        animateBox.update(deltaTime);
    } else
    {
        if (!isStarsAnimated)
            lockButton->hidden = true;
    }
    //Update Stars...
    
    const float mapKoef = mapSize.x/1920;
    const float farDist = -20;
    SunnyMatrix m;
    
    //if (activeTasks)
    for (short p = firstP;p<=lastP;p++)
    {
        bool stageLocked = !LOScore::isStageEnabled(p);
        float stageLockAlpha = 1;
        if (stageLocked) stageLockAlpha = 0.8;
        
        float offset = -firstPOffset + (mapZoomBounds.y-mapZoomBounds.x)*(p-firstP);
        sunnyUseShader(globalShaders[LOS_SolidObject]);
        if (isVAOSupported)
            glBindVertexArray(solidVAO);
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, solidVBO);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        SunnyVector4D color = SunnyVector4D(110,221,249,255*appearAlpha*stageLockAlpha)/255;
        SunnyVector4D colorBack = SunnyVector4D(159,189,208,255*appearAlpha*stageLockAlpha)/255;
        
        glUniform4fv(uniformSO_Color, 1, colorBack);
        for (short i = 0;i<activeTasksCount;i++)
            {
                m = getScaleMatrix(SunnyVector3D(shelfsSizesForTasks[i].x,shelfsSizesForTasks[i].y,1)*mapKoef) * getTranslateMatrix(SunnyVector3D((shelfsPositionsForTasks[i].x-shelfsSizesForTasks[i].x/2)*mapKoef+offset,(1280-shelfsPositionsForTasks[i].y)*mapKoef,-5));
                glUniformMatrix4fv(globalModelview[LOS_SolidObject], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(0);
            }
        
        glUniform4fv(uniformSO_Color, 1, color);
        if (p!=selectedStepNum && bufTasksLoaded!=0)
        {
            for (short i = 0;i<activeTasksCount;i++)
            {
                if (activeTasksBuf && activeTasksBuf[i] && tasksNamesBuf[i])
                {
                    m = getScaleMatrix(SunnyVector3D(shelfsSizesForTasks[i].x* LOScore::getTaskProgress(activeTasksBuf[i])/100.,shelfsSizesForTasks[i].y,1)*mapKoef) * getTranslateMatrix(SunnyVector3D((shelfsPositionsForTasks[i].x-shelfsSizesForTasks[i].x/2)*mapKoef+offset,(1280-shelfsPositionsForTasks[i].y)*mapKoef,-5));
                    glUniformMatrix4fv(globalModelview[LOS_SolidObject], 1, GL_FALSE, &(m.front.x));
                    SunnyDrawArrays(0);
                }
            }
        } else
        for (short i = 0;i<activeTasksCount;i++)
        {
            if (taskTextAnimation[i].isAnimated && taskTextAnimation[i].isTextDissapearing())
            {
                m = getScaleMatrix(SunnyVector3D(shelfsSizesForTasks[i].x* taskTextAnimation[i].progress,shelfsSizesForTasks[i].y,1)*mapKoef) * getTranslateMatrix(SunnyVector3D((shelfsPositionsForTasks[i].x-shelfsSizesForTasks[i].x/2)*mapKoef+offset,(1280-shelfsPositionsForTasks[i].y)*mapKoef,-5));
                glUniformMatrix4fv(globalModelview[LOS_SolidObject], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(0);
            } else
            if (activeTasks && activeTasks[i] && tasksNames[i])
            {
                m = getScaleMatrix(SunnyVector3D(shelfsSizesForTasks[i].x* LOScore::getTaskProgress(activeTasks[i])/100.,shelfsSizesForTasks[i].y,1)*mapKoef) * getTranslateMatrix(SunnyVector3D((shelfsPositionsForTasks[i].x-shelfsSizesForTasks[i].x/2)*mapKoef+offset,(1280-shelfsPositionsForTasks[i].y)*mapKoef,-5));
                glUniformMatrix4fv(globalModelview[LOS_SolidObject], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(0);
            }
        }
    
        SHTextureBase::bindTexture(menuTexture);
        bindGameGlobal();
        m = getTranslateMatrix(SunnyVector3D(offset,0,farDist));
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, appearAlpha*stageLockAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(520*mapKoef,(1280-536)*mapKoef,1,1));
        SunnyDrawArrays(LOTaskView_VAO);
        
        //Stars
        if (animateBox.isAnimated)
        {
            for (short i = 0;i<taskViewStarsCount;i++)
                {
                    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(starsPositionsForTasks[i].x*mapKoef,(1280-starsPositionsForTasks[i].y)*mapKoef,animateBox.scale,animateBox.scale));
                    SunnyDrawArrays(LOYellowStar_VAO);
                }
        } else
        {
            short starsCount = LOScore::getStarsCount();
            
            bool q = false;
            bool rendered[taskViewStarsCount];
            for (short i = 0;i<taskViewStarsCount;i++) rendered[i] = false;
            for (short i = taskViewStarsCount-1;i>=0;i--)
            if (starsAnimation[i].isAnimated)
            {
                q = true;
                rendered[i] = true;
                if (i>starsCount) starsCount = i;
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(starsPositionsForTasks[i].x*mapKoef,(1280-starsPositionsForTasks[i].y)*mapKoef,starsAnimation[i].scale,starsAnimation[i].scale));
                SunnyDrawArrays(LOYellowStar_VAO);
            } else
                if (q)
                    break;
            
            for (short i = 0;i<starsCount;i++)
                if (!rendered[i])
                {
                    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(starsPositionsForTasks[i].x*mapKoef,(1280-starsPositionsForTasks[i].y)*mapKoef,1,1));
                    SunnyDrawArrays(LOYellowStar_VAO);
                }
        }
        
        //Borders
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSC]);
            m = getTranslateMatrix(SunnyVector3D(offset,0,farDist));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            
            if (activeTasks)
            for (short i = 0;i<activeTasksCount;i++)
                if (activeTasks[i])
                {
                    if (taskTextAnimation[i].isAnimated)
                    {
                        SunnyVector4D color = SunnyVector4D(255/255.,210/255.,0, appearAlpha*stageLockAlpha)*taskTextAnimation[i].borderAlpha + SunnyVector4D(1,1,1,appearAlpha*stageLockAlpha)*(1-taskTextAnimation[i].borderAlpha);
                        glUniform4fv(uniformTSC_C,1, color);
                    } else
                         glUniform4fv(uniformTSC_C,1, SunnyVector4D(1,1,1, appearAlpha*stageLockAlpha));
                    
                    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(shelfsPositionsForTasks[i].x*mapKoef,(1280-shelfsPositionsForTasks[i].y)*mapKoef,1,1));
                    SunnyDrawArrays(LOTaskBorderTop_VAO+i);
                }
        }
        
        bool anyTasks = false;
        //if (activeTasks)
        {
            const float rectDiminish = 60;
            if (p!=selectedStepNum && bufTasksLoaded!=0)
            {
                for (short i = 0;i<activeTasksCount;i++)
                {
                    SunnyVector4D color = SunnyVector4D(1,1,1,appearAlpha*stageLockAlpha);
                    m = getScaleMatrix(0.3) * getTranslateMatrix(SunnyVector3D(shelfsPositionsForTasks[i].x*mapKoef+offset,(1280-shelfsPositionsForTasks[i].y)*mapKoef,-5));
                    if (tasksNamesBuf[i])
                    {
                        LOFont::writeTextInRect(&m, SunnyVector2D(shelfsSizesForTasks[i].x-rectDiminish,shelfsSizesForTasks[i].y)*mapKoef,LOTA_Center, false, tasksNamesBuf[i],color);
                        anyTasks = true;
                    }
                }

            } else
            for (short i = 0;i<activeTasksCount;i++)
            {
                SunnyVector4D color = SunnyVector4D(1,1,1,appearAlpha*stageLockAlpha);
                m = getScaleMatrix(0.3) * getTranslateMatrix(SunnyVector3D(shelfsPositionsForTasks[i].x*mapKoef+offset,(1280-shelfsPositionsForTasks[i].y)*mapKoef,-5));
                if (taskTextAnimation[i].isAnimated)
                {
                    color.w = appearAlpha*taskTextAnimation[i].progress;
                    if (taskTextAnimation[i].isTextDissapearing())
                    {
                        LOFont::writeTextInRect(&m, SunnyVector2D(shelfsSizesForTasks[i].x-rectDiminish,shelfsSizesForTasks[i].y)*mapKoef,LOTA_Center, false, taskTextAnimation[i].oldText,color);
                        anyTasks = true;
                        continue;
                    }
                }
                if (tasksNames[i])
                {
                    LOFont::writeTextInRect(&m, SunnyVector2D(shelfsSizesForTasks[i].x-rectDiminish,shelfsSizesForTasks[i].y)*mapKoef,LOTA_Center, false, tasksNames[i],color);
                    anyTasks = true;
                }
            }
        }
        
        if (!anyTasks || stageLocked)
        {
            bindGameGlobal();
            sunnyUseShader(globalShaders[LOS_TexturedTSC]);
            SHTextureBase::bindTexture(menuTexture);
            m = getRotationZMatrix(12*M_PI/180)* getTranslateMatrix(SunnyVector3D(478*mapKoef+offset,(1280-628)*mapKoef,-5));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniformTSC_C,1, SunnyVector4D(255/255.,255/255.,255/255., appearAlpha));
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,1,1));
            SunnyDrawArrays(LOLCSnowTable_VAO);
            
            m = getScaleMatrix(10./labelCompleteSize) * getTranslateMatrix(SunnyVector3D(0,0.2,0)) * m;
            
            if (stageLocked)
                LOFont::writeText(&m, LOTA_Center, true, loLabelLocked,whiteOldInnerColor(1),whiteOldOuterColor(1));
            else
                LOFont::writeText(&m, LOTA_Center, true, labelComplete,SunnyVector4D(255/255.,246/255.,54/255.,appearAlpha),SunnyVector4D(255/255.,172/255.,76/255.,appearAlpha));
        }
        
        bindGameGlobal();
        //Percentage
        char * perc = percentage;
        if (p!=selectedStepNum && bufTasksLoaded!=0)
            perc = percentageBuf;
        float size = LOFont::getTimeSize(perc);
        float scale = 0.35;
        m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(836*mapKoef - size*scale/2+offset,(1280-300)*mapKoef,farDist));
        LOFont::writeTime(&m, false, perc, SunnyVector4D(1,1,0,appearAlpha*stageLockAlpha));
        
        //Locked Label
    }
}

void LOTasksView::renderOverlay()
{
    if (!isVisible) return;
    
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(0,0,-2));
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    
    const float mapKoef = mapSize.x/1920;
    
    if (animateBox.isAnimated && animateBox.overlayAlpha != 0)
    {
        for (short i = taskViewStarsCount-1;i>=0;i--)
        {
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,210./255,0,animateBox.overlayAlpha*1.5));
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(starsPositionsForTasks[i].x*mapKoef,(1280-starsPositionsForTasks[i].y)*mapKoef,1,1));
            SunnyDrawArrays(LOOverlaySpells_VAO);
        }
    } else
    {
        bool q = false;
        for (short i = taskViewStarsCount-1;i>=0;i--)
            if (starsAnimation[i].isAnimated)
            {
                q = true;
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,210./255,0,starsAnimation[i].scale*1.5));
                glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(starsPositionsForTasks[i].x*mapKoef,(1280-starsPositionsForTasks[i].y)*mapKoef,1,1));
                SunnyDrawArrays(LOOverlaySpells_VAO);
            } else
                if (q)
                    break;
    }
}

