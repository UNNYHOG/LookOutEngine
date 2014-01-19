//
//  LOEvents.cpp
//  LookOut
//
//  Created by Ignatov on 06.08.13.
//
//

#include "LOEvents.h"
#include "LOGlobalVar.h"
#include "GameEvent.h"

bool eventWaterBallsAppeared = false;
bool eventGroundBallsAppeared = false;

void LOEvents::OnWaterBallsAppeared()
{
    eventWaterBallsAppeared = true;
}

void LOEvents::OnGroundBallsAppeared()
{
    eventGroundBallsAppeared = true;
}

void LOEvents::OnMapReset(LOTask * task)
{
    switch (task->taskType) {
            //Can be saved for ressurection:
        case LOTT_Roll:
        case LOTT_SurviveTime:
        case LOTT_CollectSnowflakes:
        case LOTT_CollideGround:
        case LOTT_CollideWater:
        case LOTT_DontMakeDoubleSnowball:
        case LOTT_RollSizeBackAndForth:
        case LOTT_RollWithDoubleSized:
        case LOTT_RollUnderWater:
        case LOTT_DontGiveGroundBallToCollide:
        case LOTT_DontGiveWaterBallToCollide:
        case LOTT_CollectSnowflakesWithDoubleSized:
            //version 2
        case LOTT_KillRobotLife:
        case LOTT_KillRobotTeleport:
        case LOTT_KillRobotGround:
        case LOTT_KillRobotFire:
        case LOTT_DemobilizeRobotWater:
        case LOTT_ChargeRobotElectric:
        case LOTT_KillRobotDoubleSnowball:
            //Can't be saved
        case LOTT_CollideSequenceWGWG:
        case LOTT_RollWithHalfSpeed:
            task->taskProgress = 0;
            break;
        default:
            break;
    }
}

void LOEvents::OnMapReset()
{
    LOScore::saveScores();
    eventWaterBallsAppeared = eventGroundBallsAppeared = false;
    
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        
        if (tasks)
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnMapReset(tasks[i]);
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnMapReset(task);
        GameEvent::checkTaskComplition(task);
    }
}

bool LOEvents::OnUseAbility(LevelAbility ability, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_UseLifeAbility:
            if (ability == LA_Life)
                task->taskProgress++;
            break;
        case LOTT_UseTeleportAbility:
            if (ability == LA_Teleport)
                task->taskProgress++;
            break;
        case LOTT_UseSpeedAbility:
            if (ability == LA_Speed)
                task->taskProgress++;
            break;
        case LOTT_UseShieldAbility:
            if (ability == LA_Shield)
                task->taskProgress++;
            break;
            
        case LOTT_UseDoublerPowerUp:
            if (ability == LA_Doubler)
                task->taskProgress++;
            break;
        case LOTT_UseMagnetPowerUp:
            if (ability == LA_Magnet)
                task->taskProgress++;
            break;
        case LOTT_UseTimePowerUp:
            if (ability == LA_Time)
                task->taskProgress++;
            break;
        default:
            break;
    }
    return true;
}

bool LOEvents::OnUseAbility(LevelAbility ability)
{
    if (!LOScore::abilityWasUsed(ability)) return false;
    
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnUseAbility(ability, tasks[i]);
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnUseAbility(ability, task);
        GameEvent::checkTaskComplition(task);
    }
    return true;
}

void LOEvents::OnCloseExplosion(LOExplosionType ex1, LOExplosionType ex2, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_CloseExplosion:
        {
            if (task->taskInfo==1)//ff
            {
                if (ex1 == LOExplosion_Fire && ex2 == LOExplosion_Fire)
                    task->taskProgress = task->taskInfo;
            } else
                if (task->taskInfo==2)//fg
                {
                    if ((ex1 == LOExplosion_Fire && ex2 == LOExplosion_Ground) || (ex1 == LOExplosion_Ground && ex2 == LOExplosion_Fire))
                        task->taskProgress = task->taskInfo;
                } else
                    if (task->taskInfo==3)//gg
                    {
                        if (ex1 == LOExplosion_Ground && ex2 == LOExplosion_Ground)
                            task->taskProgress = task->taskInfo;
                    } else
                        if (task->taskInfo==4)//fw
                        {
                            if ((ex1 == LOExplosion_Fire && ex2 == LOExplosion_Water) || (ex1 == LOExplosion_Water && ex2 == LOExplosion_Fire))
                                task->taskProgress = task->taskInfo;
                        } else
                            if (task->taskInfo==5)//gw
                            {
                                if ((ex1 == LOExplosion_Ground && ex2 == LOExplosion_Water) || (ex1 == LOExplosion_Water && ex2 == LOExplosion_Ground))
                                    task->taskProgress = task->taskInfo;
                            } else
                                if (task->taskInfo==6)//ww
                                {
                                    if (ex1 == LOExplosion_Water && ex2 == LOExplosion_Water)
                                        task->taskProgress = task->taskInfo;
                                }
            break;
        }
        default:
            break;
    }
}

void LOEvents::OnCloseExplosion(LOExplosionType ex1, LOExplosionType ex2)
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnCloseExplosion(ex1, ex2, tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnCloseExplosion(ex1, ex2, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnPlayerPushed(LOTask * task)
{
    switch (task->taskType) {
        case LOTT_BePushed:
            task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnPlayerPushed()
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnPlayerPushed(tasks[i]);
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnPlayerPushed(task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnDeath(LODieType dieType, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_CollideSequenceWGWG:
        case LOTT_RollWithHalfSpeed:
            task->taskProgress = 0;
            break;
        case LOTT_ExplodeFromFire:
            if (dieType == LOD_FireBall)
                task->taskProgress++;
            break;
        case LOTT_ExplodeFromAngry:
            if (dieType == LOD_AngryBall)
                task->taskProgress++;
            break;
        case LOTT_ExplodeFromElectric:
            if (dieType == LOD_ElectricBall)
                task->taskProgress++;
            break;
        case LOTT_DeathTime:
            if (playingTime>=task->taskInfo && playingTime<=task->taskInfo+5)
                task->taskProgress = task->taskInfo;
            break;
        default:
            break;
    }
}

void LOEvents::OnDeath(LODieType dieType)
{
    LOScore::playerDied(dieType);
    if (isSurvival)
    {
        if (player->ressurectionTime<1)
            LOScore::addSurvivalGame(selectedStepNum);
        LOTask **tasks = activeMap->activeTasks;
        
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnDeath(dieType, tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnDeath(dieType, task);
        GameEvent::checkTaskComplition(task);
    }

}

void LOEvents::OnMeterPassed(LOTask * task)
{
    switch (task->taskType) {
        case LOTT_Roll:
        case LOTT_RollTotal:
            task->taskProgress++;
            break;
        case LOTT_RollUnderWater:
            if (player->waterKoef>0.3)
                task->taskProgress++;
            break;
        case LOTT_RollWithDoubleSized:
            if (player->getSnowballScale()>=2)
                task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnMeterPassed()
{
    LOScore::addRollMeters(1);
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnMeterPassed(tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnMeterPassed(task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnSnowflakeCollected(LOTask * task)
{
    switch (task->taskType) {
        case LOTT_CollectSnowflakes:
        case LOTT_CollectSnowflakesTotal:
            task->taskProgress++;
            break;
        case LOTT_CollectSnowflakesWithDoubleSized:
            if (player->getSnowballScale()>=2)
                task->taskProgress++;
            break;
        case LOTT_PickUpSnowflakeUnderWater:
            if (player->waterKoef>0.3)
                task->taskProgress++;
            break;
        case LOTT_PickUpSnowflakeUnderGround:
            if (player->sandFriction<0.7)
                task->taskProgress++;
            break;
            
        default:
            break;
    }
}

void LOEvents::OnSnowflakeCollected()
{
    LOScore::addSnowflake(1);
    
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnSnowflakeCollected(tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnSnowflakeCollected(task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnGoldCoinCollected()
{
    LOScore::addGoldCoin(1);
}

void LOEvents::OnBallCollision(LOCollisionType ballType, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_DontGiveGroundBallToCollide:
            if (ballType==LOCT_GroundBall)
                task->taskProgress = 0;
            break;
        case LOTT_DontGiveWaterBallToCollide:
            if (ballType==LOCT_WaterBall)
                task->taskProgress = 0;
            break;
        default:
            break;
    }
}

void LOEvents::OnBallCollision(LOCollisionType ballType)
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnBallCollision(ballType, tasks[i]);
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnBallCollision(ballType, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnAngryBallExplode(LOTask * task)
{
    switch (task->taskType) {
        case LOTT_KillRobotDoubleSnowball:
            if (!player->crashed && player->getSnowballScale()>=2)
                task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnAngryBallExplode()
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnAngryBallExplode(tasks[i]);
                
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnAngryBallExplode(task);
        GameEvent::checkTaskComplition(task);
    }

}

void LOEvents::OnAngryBallCollide(LODieType dieType, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_KillRobotFire:
            if (dieType==LOD_FireBall)
                task->taskProgress++;
            break;
        case LOTT_ChargeRobotElectric:
            if (dieType==LOD_ElectricBall)
                task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnAngryBallCollide(LODieType dieType)
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnAngryBallCollide(dieType, tasks[i]);
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnAngryBallCollide(dieType, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnAngryBallCollide(LevelAbility ability, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_KillRobotLife:
            if (ability==LA_Life)
                task->taskProgress++;
            break;
        case LOTT_KillRobotTeleport:
            if (ability==LA_Teleport)
                task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnAngryBallCollide(LevelAbility ability)
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnAngryBallCollide(ability, tasks[i]);
                
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnAngryBallCollide(ability, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnAngryBallCollide(LOCollisionType type, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_KillRobotGround:
            if (type==LOCT_GroundBall)
                task->taskProgress++;
            break;
        case LOTT_DemobilizeRobotWater:
            if (type==LOCT_WaterBall)
                task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnAngryBallCollide(LOCollisionType type)
{
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
            if (tasks[i] && !tasks[i]->complete)
            {
                OnAngryBallCollide(type, tasks[i]);
                
                if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                    activeMap->taskComplete(tasks[i]);
            }
    }
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnAngryBallCollide(type, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnPlayerCollision(LOCollisionType type, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_CollideGround:
            if (type==LOCT_GroundBall)
                task->taskProgress++;
            break;
        case LOTT_CollideWater:
            if (type==LOCT_WaterBall)
                task->taskProgress++;
            break;
        case LOTT_CollideSequenceWGWG:
            if ((type==LOCT_WaterBall && task->taskProgress%2==0) || (type==LOCT_GroundBall && task->taskProgress%2==1))
                task->taskProgress++;
            else
                if (task->taskProgress!=1 || type!=LOCT_WaterBall)
                    task->taskProgress = 0;
        default:
            break;
    }
}

void LOEvents::OnPlayerCollision(LOCollisionType type)
{
    LOScore::playerCollided(type);
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnPlayerCollision(type, tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnPlayerCollision(type, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnSizeChanged(float radius, LOTask * task)
{
    switch (task->taskType) {
        case LOTT_RollSize:
            task->taskProgress = task->taskInfo * (player->getSnowballScale(radius)-1) / (task->taskInfo/100-1);
            break;
        case LOTT_DontMakeDoubleSnowball:
            if (radius >= cellSize/2*1.5)
                task->taskProgress = 0;
            break;
        case LOTT_RollSizeBackAndForth:
        {
            int p;
            if (task->taskProgress*2<task->taskInfo)
            {
                p = task->taskInfo * (player->getSnowballScale(radius)-1) / (task->taskInfo/100-1);
                if (p>=task->taskInfo)
                    task->taskProgress = task->taskInfo/2;
                else
                    task->taskProgress = p/2;
            } else//return to normal size
            {
                if ((player->getSnowballScale(radius)-1)<0.01)
                    task->taskProgress = task->taskInfo;
                else
                {
                    p = task->taskInfo * (player->getSnowballScale(radius)-1) / (task->taskInfo/100-1);
                    p = task->taskInfo - p;
                    p = task->taskInfo/2 + p/2;
                    p = fmaxf(task->taskInfo/2, p);
                    if (p>task->taskProgress)
                        task->taskProgress = p;
                }
            }
        }
        default:
            break;
    }
}

void LOEvents::OnSizeChanged(float radius)
{
    LOScore::checkRadius(radius);
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnSizeChanged(radius, tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnSizeChanged(radius, task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnSecondPassed(LOTask * task)
{
    switch (task->taskType) {
        case LOTT_DontMakeDoubleSnowball:
            if (player->getSnowballScale() < 1.5)
                task->taskProgress++;
            else
                task->taskProgress = 0;
            break;
        case LOTT_SurviveTime:
        case LOTT_PlayTime:
            task->taskProgress++;
            break;
        case LOTT_RollWithHalfSpeed:
            if (player->getVelocity().length()>=4)
                task->taskProgress++;
            else
                task->taskProgress = 0;
        case LOTT_DontGiveWaterBallToCollide:
            if (eventWaterBallsAppeared)
                task->taskProgress++;
            break;
        case LOTT_DontGiveGroundBallToCollide:
            if (eventGroundBallsAppeared)
                task->taskProgress++;
            break;
        default:
            break;
    }
}

void LOEvents::OnSecondPassed()
{
    LOScore::addPlayingTime(1);
    
    if (isSurvival)
    {
        LOTask **tasks = activeMap->activeTasks;
        for (short i = 0;i<activeTasksCount;i++)
        if (tasks[i] && !tasks[i]->complete)
        {
            OnSecondPassed(tasks[i]);
            if (tasks[i]->taskProgress>=tasks[i]->taskInfo)
                activeMap->taskComplete(tasks[i]);
        }
    }
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnSecondPassed(task);
        GameEvent::checkTaskComplition(task);
    }
}

void LOEvents::OnLevelComplete(float time, unsigned char coins,LOTask * task)
{
    switch (task->taskType) {
        case LOTT_CompleteLevelWithCoins:
                task->taskProgress = coins;
            break;
        case LOTT_CompleteLevelWithTime:
            if (task->taskInfo<=time)
                task->taskProgress = task->taskInfo;
            break;
        default:
            break;
    }
}

void LOEvents::OnLevelComplete(int stage, int level, float time, unsigned char coins)
{
    LOScore::levelComplete(selectedStepNum, selectedLevelNum, playingTime, earnedStars);
    
    LOTask * task = GameEvent::getAvailableTask();
    if (task)
    {
        OnLevelComplete(time,coins,task);
        GameEvent::checkTaskComplition(task);
    }
}

