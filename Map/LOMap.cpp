//
//  LOMap.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOMap.h"
#include "LOGlobalVar.h"

#include "SunnyFunctions.h"
#include "SHTextureBase.h"
#include "iPhone_Strings.h"
#include "LOSmoke.h"
#include "LookOutEngine.h"
#include "LOTrace.h"
#include "LOFallingBall.h"
#include "LOCrashBalls.h"
#include "LOSounds.h"
#include "LOGlobalFunctions.h"
#include "SunnyShaders.h"
#include "LOEvents.h"
#include "LOScore.h"
#include "LOVioletSnowflake.h"
#include "LOTrees.h"
#include "LOSettings.h"

const float closeExplosionDistance = 6;

const int saveMapVersion = 1;

float previewAlpha = 1.0;

SunnyMatrix lastMVMatrix;
void renderHUD();

extern unsigned short snowObjects[];
extern SunnyModelObj * explosionObj;

const float maxMapRotationAngle = 30;

const short greenTipPointsCount = 20;
const float greenTipPeriodTime = 1.5;

//static SunnyVector2D backgroundTexCoords

void LOMap::gameOver(LODieType dieType)
{
    if (gameOvered || mainMenu->levelCompleteShown) return;
    
    levelsStrike = 0;
    LOEvents::OnDeath(dieType);
    
    animateDeath = true;
    deathAnimationFrame = 0;
    
    if (LOSettings::canVibrate())
        loCallBackVibrate();
    
    if (isMultiPlayer)
    {
        bool q = player->crashed;
        if (q)
            for (short i = 0; i<otherPlayersCount; i++)
                if (!otherPlayers[i].player->crashed)
                {
                    q = false;
                    break;
                }
        if (q)
            gameOveredMultiPlayer = true;
    }
    
    gameOvered = true;
    multiPlayerPlayedTime = playingTime;
    
    mainMenu->showGameOver();
}

void LOMap::levelCompleted()
{
    if (mainMenu->levelCompleteShown || gameOvered) return;
    
    if (activeMap->timeCondition>0)
        playingTime = activeMap->timeCondition;
    
    if (cheatProtection2)
        playingTime+= 40 + sRandomf()*30;

    gamePaused = true;
    activeMap->calculateScores();
    
    LOEvents::OnLevelComplete(selectedStepNum, selectedLevelNum, playingTime, earnedStars);
    
    mainMenu->showLevelComplete(playingTime, earnedStars, mapInfo.supposedPlayingTime2, mapInfo.supposedPlayingTime3);
}

LOShortMessage::LOShortMessage()
{
    isAnimated = false;
    message = 0;
}

LOShortMessage::~LOShortMessage()
{
    if (message)
        delete []message;
}

void LOShortMessage::animate(SunnyVector2D pos,const  char * text)
{
    isAnimated = true;
    position = pos;
    message = getLocalizedLabel(text);
    animationTime = 0;
    scale = 0;
    alpha = 0;
}

bool LOShortMessage::update(float deltaTime)
{
    if (isAnimated)
    {
        animationTime += deltaTime;
        const float incTime = 0.4;
        const float decTime = 0.1;
        const float waitTime = 2;
        const float s = 1.3;
        float t = animationTime;
        if (t<incTime)
            scale = s*t/incTime;
        else
        {
            t-=incTime;
            if (t<decTime)
            {
                scale = s + (1.0-s)*(t/decTime);
            } else
            {
                t-=decTime;
                if (t<waitTime)
                {
                    scale = 1;
                } else
                {
                    t-=waitTime;
                    if (t<decTime)
                    {
                        scale = 1 + (s-1)*(t/decTime);
                    } else
                    {
                        t-=decTime;
                        if (t<incTime)
                        {
                            scale = s*(1.0-t/incTime);
                        } else
                        {
                            isAnimated = false;
                        }
                    }
                }
            }
        }
        
        alpha = fminf(1, scale);
    }
    
    return isAnimated;
}

void LOMap::showMessage(SunnyVector2D pos, const char * text)
{
    if (shortMessage)
        delete shortMessage;
    
    shortMessage = new LOShortMessage;
    shortMessage->animate(pos, text);
}

LOMap::LOMap()
{
    mainMenu = new LOMenu;
    
    for (short i = 0;i<maxGoldCoinsCount;i++)
        goldCoins[i] = new LOGoldCoin;
    
    for (short i = 0;i<maxVioletSnowflakesCount;i++)
        violetSnowflakes[i] = new LOVioletSnowflake;
    
    init();
    
    windTimer = getCurrentTime() + sRandomf()*3*60;
    
    playingMusic = 0;
    shortMessage = 0;
    previousSurvivalRecord = 5;
}

LOMap::~LOMap()
{
    delete mainMenu;
    clear();
}

void LOMap::init()
{
    activeSurvivalMap = 0;
    finishVAO = finishVBO = 0;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_TEXTURE_2D);
    targetCount = 5;
    amplitude = 0;
    rightSide = false;
    glLineWidth(2*pointsCoeff);
    
    for (short i = 0;i<maxFireBalls;i++)
        fireBalls[i] = new LOFireBall;
    for (short i = 0;i<maxWaterBalls;i++)
        waterBalls[i] = new LOWaterBall;
    for (short i = 0;i<maxGroundBalls;i++)
        groundBalls[i] = new LOGroundBall;
    for (short i = 0;i<maxAngryBalls;i++)
        angryBalls[i] = new LOAngryBall;
    for (short i = 0;i<maxElectricBalls;i++)
        electricBalls[i] = new LOElectricBall;
    for (short i = 0;i<maxAnomalyBalls;i++)
        anomalyBalls[i] = new LOAnomalyBall;
    
    for (short i = 0;i<maxAngryBalls;i++)
        angryBalls[i]->attachMoveBody();
    
    fireBallsCount = 0;
    waterBallsCount = 0;
    groundBallsCount = 0;
    angryBallsCount = 0;
    electricBallsCount = 0;
    anomalyBallsCount = 0;
    goldCoinsCount = 0;
    violetSnowflakesCount = 0;
    
    animateDeath = false;
    
    activeTasks = new LOTask*[activeTasksCount];
    for (short i = 0;i<activeTasksCount;i++)
        activeTasks[i] = NULL;
}

void LOMap::clear()
{
    if (activeSurvivalMap)
    {
        delete activeSurvivalMap;
        activeSurvivalMap = 0;
    }
    for (short i = 0;i<maxFireBalls;i++)
        delete fireBalls[i];
    for (short i = 0;i<maxWaterBalls;i++)
        delete waterBalls[i];
    for (short i = 0;i<maxGroundBalls;i++)
        delete groundBalls[i];
    for (short i = 0;i<maxAngryBalls;i++)
        delete angryBalls[i];
    for (short i = 0;i<maxElectricBalls;i++)
        delete electricBalls[i];
    
    delete [] activeTasks;
}

void LOMap::explosion(SunnyVector2D pos, bool movePlayer)
{
    SunnyVector2D dir = SunnyVector2D(player->position.pos.x, player->position.pos.y) - pos;
    float len = dir.length();
    
    if (!movePlayer)
    {
        loBlurExplosion(pos,5.0/(1.0 + sqrtf(len)));
        return;
    }
    //if (len>10.0) return;
    loBlurExplosion(pos,sqr(5.0/(1.0 + len)));
    earthShake(fminf(1,sqr(6.0/(1.0 + len))));
    
    if (len<=closeExplosionDistance && !gameOvered)
        LOEvents::OnPlayerPushed();
    
    if (len>3.0) return;
    
    dir *= 20./sqr(len);
    if (dir.length()>player->velocityAdd.length())
        player->velocityAdd = dir;
}

//void LOMap::addExplosion(SunnyMatrix mat, LOExplosionType type)
//{
//    if (explosionsCount<maxExplosition)
//        explosions[explosionsCount++].initWithParams(mat,type);
//}

void LOMap::explosion(SunnyMatrix *mat1,SunnyMatrix *mat2, LOExplosionType type1, LOExplosionType type2)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat1,type1);
    
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat2,type2);
    
    if (mat1 && mat2)
    {
        SunnyVector2D pos;
        pos.x = (mat1->pos.x + mat2->pos.x)/2;
        pos.y = (mat1->pos.y + mat2->pos.y)/2;
        explosion(pos);
    }
}

void LOMap::explosion(SunnyMatrix *mat1, LOExplosionType type1,bool blur)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat1,type1);
    
    if (blur && mat1)
    {
        SunnyVector2D pos;
        pos.x = (mat1->pos.x);
        pos.y = (mat1->pos.y);
        explosion(pos);
    }
}

void LOMap::checkOnCloseExplosion(LOExplosionType ex1, LOExplosionType ex2, SunnyVector2D pos)
{
    if (player->distanceToPoint(pos)<=closeExplosionDistance)
    {
        LOEvents::OnCloseExplosion(ex1, ex2);
    }
}

void LOMap::addSnowExplosion(SunnyMatrix *mat)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat,LOExplosion_Snow);
}

void LOMap::addTeleportExplosion(SunnyMatrix *mat)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat,LOExplosion_Teleportation);
    
    SunnyVector2D pos(mat->pos.x, mat->pos.y);
    const float explodeDistance = 4;
    //explode all balls
    for (short i=0;i<fireBallsCount;i++)
        if (!fireBalls[i]->detach && fireBalls[i]->distanceToPoint(pos)<explodeDistance)
        {
            fireBalls[i]->detach = true;
            explosion(&fireBalls[i]->position, LOExplosion_Fire, false);
            LOTrace::sharedTrace()-> addFireTrace(&fireBalls[i]->position);
            LOCrashBalls::sharedCrashBalls()->addFireBallCrash(&fireBalls[i]->position);
            losPlayFFExplosion((fireBalls[i]->position.pos - player->position.pos).length());
        }
    for (short i=0;i<groundBallsCount;i++)
        if (!groundBalls[i]->detach && groundBalls[i]->distanceToPoint(pos)<explodeDistance)
        {
            groundBalls[i]->detach = true;
            explosion(&groundBalls[i]->position, LOExplosion_Ground, false);
            LOTrace::sharedTrace()->addGroundTrace(&fireBalls[i]->position);
            LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(&fireBalls[i]->position);
            losPlayGGExplosion((fireBalls[i]->position.pos - player->position.pos).length());
        }
    
    for (short i=0;i<waterBallsCount;i++)
        if (!waterBalls[i]->detach && waterBalls[i]->distanceToPoint(pos)<explodeDistance)
        {
            waterBalls[i]->detach = true;
            explosion(&waterBalls[i]->position, LOExplosion_Water, false);
            LOTrace::sharedTrace()->addWaterTrace(&waterBalls[i]->position);
            losPlayWWExplosion((waterBalls[i]->position.pos - player->position.pos).length());
        }
    
    SunnyMatrix m = getIdentityMatrix();
    for (short i=0;i<angryBallsCount;i++)
        if (!angryBalls[i]->detach && angryBalls[i]->distanceToPoint(pos)<explodeDistance)
        {
            angryBalls[i]->detach = true;
            m.pos = angryBalls[i]->position.pos;
            explosion(&m, LOExplosion_Fire, false);
            explodeAngryBallBySpell(LA_Teleport, &m);
            losPlayFFExplosion((angryBalls[i]->position.pos - player->position.pos).length());
        }
    
    explosion(SunnyVector2D(mat->pos.x,mat->pos.y),false);
}

void LOMap::addTeleportExplosionPost(SunnyMatrix *mat)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat,LOExplosion_TeleportationPost);
}

void LOMap::addShieldExplosion(SunnyMatrix *mat)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat,LOExplosion_Shield);
}

void LOMap::addSpeedExplosion(SunnyMatrix *mat)
{
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat,LOExplosion_Speed);
}

void LOMap::earthShake(float scale)
{
    earthShakeFame = 0;
    amplitude = 0.7*scale;
}

void LOMap::applyGlobalColor(float x)
{
    float delta = 1.0 - x;
    globalColor.x = x;
    globalColor.y = 1.0 - delta*0.8;
    globalColor.z = 1.0 - delta*0.5;
//    globalColor = SunnyVector3D(x,fminf(x*1.2,1.0),fminf(x*1.4,1.0));
}

SunnyVector2D LOMap::randPosOnMapBounds(short &bound)
{
    SunnyVector2D pos;
    short r = sRandomi()%100;
    
    if (bound==-1)
    {
        if (r<30)
        {
            bound = 0;
            pos.y = mapSize.y;
            pos.x = sRandomf()*mapSize.x;
        } else
            if (r<60)
            {
                bound = 1;
                pos.y = 0;
                pos.x = sRandomf()*mapSize.x;
            } else
                if (r<80)
                {
                    bound = 2;
                    pos.x = 0;
                    pos.y = sRandomf()*mapSize.y;
                } else
                {
                    bound = 3;
                    pos.x = mapSize.x;
                    pos.y = sRandomf()*mapSize.y;
                }
    } else
    {
        if (r<50)
        {
            if (bound==0)
            {
                bound = 1;
                pos.y = 0;
                pos.x = sRandomf()*mapSize.x;
            } else
            if (bound==1)
            {
                bound = 0;
                pos.y = mapSize.y;
                pos.x = sRandomf()*mapSize.x;
            } else
            if (bound==2)
            {
                bound = 3;
                pos.x = mapSize.x;
                pos.y = sRandomf()*mapSize.y;
            } else
            {
                bound = 2;
                pos.x = 0;
                pos.y = sRandomf()*mapSize.y;
            }
        } else
        {
            if (bound==0)
            {
                if (r<75)
                {
                    bound = 2;
                    pos.x = 0;
                    pos.y = (0.5 - sRandomf()/2)*mapSize.y;
                } else
                {
                    bound = 3;
                    pos.x = mapSize.x;
                    pos.y = (0.5 - sRandomf()/2)*mapSize.y;
                }
            } else
            if (bound==1)
            {
                if (r<75)
                {
                    bound = 2;
                    pos.x = 0;
                    pos.y = (0.5 + sRandomf()/2)*mapSize.y;
                } else
                {
                    bound = 3;
                    pos.x = mapSize.x;
                    pos.y = (0.5 + sRandomf()/2)*mapSize.y;
                }
            } else
            if (bound==2)
            {
                if (r<75)
                {
                    bound = 0;
                    pos.y = mapSize.y;
                    pos.x = (0.5 + sRandomf()/2)*mapSize.x;
                } else
                {
                    bound = 1;
                    pos.y = 0;
                    pos.x = (0.5 + sRandomf()/2)*mapSize.x;
                }
            } else
            {
                if (r<75)
                {
                    bound = 0;
                    pos.y = mapSize.y;
                    pos.x = (0.5 - sRandomf()/2)*mapSize.x;
                } else
                {
                    bound = 1;
                    pos.y = 0;
                    pos.x = (0.5 - sRandomf()/2)*mapSize.x;
                }
            }
        }
    }
    return pos;
}

void LOMap::explodePlayer(LOPlayer *pl, LOCrashModel * model)
{
    earthShake();
        
    SunnyVector2D vel;
    float phi = sRandomf()*2*M_PI;
    vel = SunnyVector2D(cosf(phi), sinf(phi));
    model->prepareToCrash(pl->position,vel);
    pl->setCrashed(true);
    if (explosionsCount<maxExplosition)
    {
        SunnyMatrix m = getIdentityMatrix();
        m.pos.x = pl->getPosition().x;
        m.pos.y = pl->getPosition().y;
        explosions[explosionsCount++].initWithParams(&m,LOExplosion_Snow);
    }
    losPlayFSExplosion((pl->position.pos - player->position.pos).length());
    losPlayFFExplosion((pl->position.pos - player->position.pos).length());
}

void LOMap::explodeFireBall(short num, LOPlayer *pl, LOCrashModel *model)
{
    SunnyMatrix *mat;
    SunnyVector2D vel;
    
    fireBalls[num]->detach = true;
    mat = &(fireBalls[num]->position);
    vel = fireBalls[num]->velocity;
    
    SunnyMatrix m;
    if (explosionsCount<maxExplosition)
    {
        m = *mat;
        m.pos.x = pl->getPosition().x;
        m.pos.y = pl->getPosition().y;
        explosions[explosionsCount++].initWithParams(&m,LOExplosion_Snow);
    }
#ifndef LOOKOUT_GOD_MOD
    if (!pl->useShield())
    {
        model->prepareToCrash(pl->position,(pl->getPosition() - SunnyVector2D(mat->pos.x,mat->pos.y) + vel).normalize());
        pl->setCrashed(true);
        if (pl == player)
        {
            gameOver(LOD_FireBall);
        }
        explosion(mat, LOExplosion_Fire);
    } else
    {
        if (explosionsCount<maxExplosition)
            explosions[explosionsCount++].initWithParams(&m,LOExplosion_Life);
        explosion(mat, LOExplosion_Fire,false);
        explosion(SunnyVector2D(mat->pos.x,mat->pos.y),false);
    }
#endif
    
    LOTrace::sharedTrace()-> addFireTrace(mat);
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat);
    losPlayFSExplosion((pl->position.pos - player->position.pos).length());
    losPlayFFExplosion((pl->position.pos - player->position.pos).length());
}

void LOMap::explodeWaterBall(short num, LOPlayer *pl, LOCrashModel *model)
{
    SunnyMatrix *mat;
    waterBalls[num]->detach = true;
    mat = &(waterBalls[num]->position);
    
    LOExplosionType type = LOExplosion_Water;
    
    if (waterBalls[num]->skipFrame)
    {
        //explosion(mat, LOExplosion_Electric);
        addElectroExplosion(SunnyVector2D(mat->pos.x,mat->pos.y));
        if (explosionsCount<maxExplosition)
            explosions[explosionsCount++].initWithParams(mat,LOExplosion_Water);
        LOTrace::sharedTrace()-> addGroundWaterTrace(mat);
        LOTrace::sharedTrace()-> addFireTrace(mat);
#ifndef LOOKOUT_GOD_MOD
        if (!pl->useShield())
        {
            gameOver(LOD_WaterBall);
            snowBallCrashObj->prepareToCrash(pl->position,(pl->getPosition() - SunnyVector2D(mat->pos.x,mat->pos.y) + waterBalls[num]->velocity).normalize());
            pl->setCrashed(true);
            if (explosionsCount<maxExplosition)
            {
                SunnyMatrix m = *mat;
                m.pos.x = pl->getPosition().x;
                m.pos.y = pl->getPosition().y;
                explosions[explosionsCount++].initWithParams(&m,LOExplosion_Snow);
            }
            losPlayWSExplosion((pl->position.pos - player->position.pos).length());
            losPlayFSExplosion((pl->position.pos - player->position.pos).length());
        } else
        {
            SunnyMatrix mat = waterBalls[num]->position;
            mat.pos = player->position.pos;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Life);
        }
#endif
        return;
    } else
    {
        if (!player->setWaterKoef())
        {
            SunnyMatrix mat1 = *mat;
            mat1.pos.x = player->position.pos.x;
            mat1.pos.y = player->position.pos.y;
            activeMap->addShieldExplosion(&mat1);
            type = LOExplosion_Steam;
        } else
            if (waterBalls[num]->anomaled)
                player->setAnomaly();
    }
    
    if (explosionsCount < maxExplosition)
        explosions[explosionsCount++].initWithParams(mat,type);
    
    LOTrace::sharedTrace()-> addGroundWaterTrace(mat);
    losPlayWSExplosion((pl->position.pos - player->position.pos).length());
}

void LOMap::explodeGroundBall(short num, LOPlayer *pl, LOCrashModel *model)
{
    SunnyMatrix *mat;
    SunnyVector2D vel;
    
    groundBalls[num]->detach = true;
    mat = &(groundBalls[num]->position);
    vel = groundBalls[num]->velocity;
    
    explosion(mat, LOExplosion_Ground, !pl->isAbilityActive(LA_Shield));
    LOTrace::sharedTrace()-> addGroundTrace(mat);
    
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat);
    losPlayGSExplosion((pl->position.pos - player->position.pos).length());
}

void LOMap::collisionPlayerAndPlayer(cpBody * body1, cpBody * body2)
{
    if (player->crashed || (player->body!=body1 && player->body!=body2)) return;
    
    for (short i=0;i<otherPlayersCount;i++)
    if (!otherPlayers[i].player->crashed)
    {
        if (otherPlayers[i].player->body == body1 || otherPlayers[i].player->body == body2)
        {
            SunnyVector2D v = player->getPosition() - otherPlayers[i].player->getPosition();
            v.normalize();
            
            float rr = player->getRadius()/otherPlayers[i].player->getRadius();
            
            float scalar = player->velocity & v;
            player->velocityAdd += scalar*player->velocity/10;
            scalar = otherPlayers[i].player->velocity & v;
            player->velocityAdd += scalar*otherPlayers[i].player->velocity/rr;
            
            scalar = otherPlayers[i].player->velocity & v;
            otherPlayers[i].player->velocityAdd -= scalar*otherPlayers[i].player->velocity/10;
            scalar = player->velocity & v;
            otherPlayers[i].player->velocityAdd -= scalar*player->velocity*rr;
            break;
        }
    }
}

void LOMap::collisionPlayerAndFireBall(cpBody * body1, cpBody * body2)
{
    if (player->crashed) return;
    if (player->body!=body1 && player->body!=body2) return;
    
    short i = 0;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            break;
        }
    
    explodeFireBall(i, player, snowBallCrashObj);
    if (isMultiPlayer)
        multiPlayerSnowExploded(SET_FireBall, i);
}

void LOMap::collisionPlayerAndWaterBall(cpBody * body1, cpBody * body2)
{
    if (player->crashed) return;
    if (player->body!=body1 && player->body!=body2) return;
    
    short i = 0;
//    SunnyMatrix *mat;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
//            mat = &(waterBalls[i]->position);
            break;
        }
    
    explodeWaterBall(i, player, snowBallCrashObj);
    LOEvents::OnPlayerCollision(LOCT_WaterBall);
    
    if (isMultiPlayer)
        multiPlayerSnowExploded(SET_WaterBall, i);
}

void LOMap::collisionPlayerAndGroundBall(cpBody * body1, cpBody * body2)
{
    if (player->crashed) return;
    if (player->body!=body1 && player->body!=body2) return;
    
    short i = 0;
    SunnyMatrix *mat = 0;
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            mat = &(groundBalls[i]->position);
            break;
        }
    if (!mat) return;
    
    SunnyMatrix mat1 = *mat;
    mat1.pos.x = player->position.pos.x;
    mat1.pos.y = player->position.pos.y;
    activeMap->addSnowExplosion(&mat1);
    
    explodeGroundBall(i, player, snowBallCrashObj);
    LOEvents::OnPlayerCollision(LOCT_GroundBall);
    
    if (!player->setGroundKoef())
    {
        activeMap->addShieldExplosion(&mat1);
    }
    
    if (isMultiPlayer)
        multiPlayerSnowExploded(SET_GroundBall, i);
}

void LOMap::explodeAngryballAtLocation(SunnyMatrix * pos)
{
    if (player->isAbilityActive(LA_Magnet))
        UnnyNet::reportAchievement("killAngryUnderMagnet", 100);
    
    LOEvents::OnAngryBallExplode();
    
    LOTrace::sharedTrace()->addAngryBallTrace(pos);
    mapInfo.destroyedAngryBallsCount++;
}

void LOMap::explodeAngryBallByBall(LODieType type, SunnyMatrix * pos)
{
    LOEvents::OnAngryBallCollide(type);
    explodeAngryballAtLocation(pos);
}

void LOMap::collideAngryBallWithBall(LOCollisionType type, SunnyMatrix * pos)
{
    LOEvents::OnAngryBallCollide(type);
    if (type == LOCT_GroundBall)
        explodeAngryballAtLocation(pos);
}

void LOMap::explodeAngryBallBySpell(LevelAbility ability, SunnyMatrix * pos)
{
    LOEvents::OnAngryBallCollide(ability);
    explodeAngryballAtLocation(pos);
}

void LOMap::collisionAngryBallAndFireBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat1 = 0,mat2 = getIdentityMatrix();
    SunnyVector2D vel;
    short i;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            fireBalls[i]->detach = true;
            mat1 = &(fireBalls[i]->position);
            vel = fireBalls[i]->velocity;
            break;
        }
    if (!mat1) return;
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->detach = true;
            mat2.pos = angryBalls[i]->position.pos;
            break;
        }
    
    LOTrace::sharedTrace()-> addFireTrace(mat1);
    explodeAngryBallByBall(LOD_FireBall, &mat2);
    
    explosion(mat1, LOExplosion_Fire);
    explosion(&mat2, LOExplosion_Fire);
    
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat1);
    losPlayFFExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionAngryBallAndWaterBall(cpBody * body1, cpBody * body2)
{
    //earthShake();
    SunnyMatrix *mat1 = 0;//,*mat2;
    short i;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
            waterBalls[i]->detach = true;
            mat1 = &(waterBalls[i]->position);
            break;
        }
    if (!mat1) return;
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->turnOffTime = 7;
            break;
        }
    
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat1,LOExplosion_Water);
    LOTrace::sharedTrace()-> addGroundWaterTrace(mat1);
    
    collideAngryBallWithBall(LOCT_WaterBall,mat1);
    
    losPlayWSExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionAngryBallAndGroundBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat1 = 0,mat2 = getIdentityMatrix();
    SunnyVector2D vel;
    short i;
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            groundBalls[i]->detach = true;
            mat1 = &(groundBalls[i]->position);
            vel = groundBalls[i]->velocity;
            break;
        }
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->detach = true;
            mat2.pos = angryBalls[i]->position.pos;
            break;
        }
    if (!mat1) return;
    explosion(mat1, LOExplosion_Ground);
    explosion(&mat2, LOExplosion_Fire);
    
    LOTrace::sharedTrace()-> addGroundTrace(mat1);
    collideAngryBallWithBall(LOCT_GroundBall,&mat2);
    
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat1);
    losPlayFGExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionAngryBallAndAnomalyBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix mat = getIdentityMatrix();
    short i;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            anomalyBalls[i]->detach = true;
            mat.pos = (anomalyBalls[i]->position.pos + player->position.pos)/2;
            mat.up.x = mat.pos.x - player->position.pos.x;
            mat.up.y = mat.pos.y - player->position.pos.y;
            mat.up.normalize();
            mat.front = mat.up * mat.right;
            break;
        }
    
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->setAnomaly();
            break;
        }
    
    collideAngryBallWithBall(LOCT_GroundBall,&mat);
    
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Anomaly);
}

void LOMap::collisionAngryBallAndBonfire(cpBody * body1, cpBody * body2)
{
    SunnyMatrix mat = getIdentityMatrix();
    SunnyVector2D vel;
    short i;
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->detach = true;
            mat.pos = angryBalls[i]->position.pos;
            break;
        }
    explosion(&mat, LOExplosion_Fire);
    
    explodeAngryBallByBall(LOD_Bonfire,&mat);
    
    losPlayFFExplosion((mat.pos-player->position.pos).length());
}

void LOMap::collisionAngryBallAndElectricField(cpBody * body1, cpBody * body2)
{
//    SunnyMatrix mat = getIdentityMatrix();
    SunnyVector2D vel;
    short i;
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->chargeTheBall();
            break;
        }
    
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking++;
}

void LOMap::collisionAngryBallAndElectricFieldEnded(cpBody * body1, cpBody * body2)
{
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking--;
}

void LOMap::collisionPlayerAndAngryBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix mat = getIdentityMatrix();
    SunnyVector2D vel(0,0);
    short i;
    for (i=0;i<angryBallsCount;i++)
        if (angryBalls[i]->body==body1 || angryBalls[i]->body==body2)
        {
            angryBalls[i]->detach = true;
            mat.pos = angryBalls[i]->position.pos;
            vel = angryBalls[i]->velocity;
            break;
        }
    
#ifndef LOOKOUT_GOD_MOD
    if (!player->crashed)
    {
        if (!player->useShield())
        {
            gameOver(LOD_AngryBall);
            snowBallCrashObj->prepareToCrash(player->position,(player->getPosition() - SunnyVector2D(mat.pos.x,mat.pos.y) + vel).normalize());
            player->setCrashed(true);
            if (explosionsCount<maxExplosition)
            {
                SunnyMatrix m = mat;
                m.pos.x = player->getPosition().x;
                m.pos.y = player->getPosition().y;
                explosions[explosionsCount++].initWithParams(&m,LOExplosion_Snow);
            }
            explodeAngryballAtLocation(&mat);
        } else
        {
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Life);
            
            explodeAngryBallBySpell(LA_Life, &mat);
        }
    } else
        explodeAngryballAtLocation(&mat);
#endif
    explosion(&mat, LOExplosion_Fire);
    losPlayFSExplosion(0);
}

void LOMap::collisionPlayerAndButtonBegan(cpBody * body1, cpBody * body2)
{
    for (short i = 0;i<mapInfo.buttonsCount;i++)
        if (mapInfo.buttons[i].body == body1 || mapInfo.buttons[i].body == body2)
        {
            if (mapInfo.buttons[i].buttonState == LOBS_WaitingUp || mapInfo.buttons[i].buttonState == LOBS_Up)
                mapInfo.buttons[i].buttonState = LOBS_Down;
            break;
        }
}

void LOMap::collisionPlayerAndButtonEnded(cpBody * body1, cpBody * body2)
{
    for (short i = 0;i<mapInfo.buttonsCount;i++)
        if (mapInfo.buttons[i].body == body1 || mapInfo.buttons[i].body == body2)
        if (mapInfo.buttons[i].repeat)
        {
            if (mapInfo.buttons[i].buttonState == LOBS_WaitingDown || mapInfo.buttons[i].buttonState == LOBS_Down)
                mapInfo.buttons[i].buttonState = LOBS_Up;
                break;
        }
}

void LOMap::collisionPlayerAndAnomalyBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix mat = getIdentityMatrix();
    short i;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            anomalyBalls[i]->detach = true;
            mat.pos = (anomalyBalls[i]->position.pos + player->position.pos)/2;
            mat.up.x = mat.pos.x - player->position.pos.x;
            mat.up.y = mat.pos.y - player->position.pos.y;
            mat.up.normalize();
            mat.front = mat.up * mat.right;
            break;
        }
    
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Anomaly);
    
    LOEvents::OnPlayerCollision(LOCT_AnomalyBall);
    
    if (!player->setAnomaly())
    {
        SunnyMatrix mat1 = mat;
        mat1.pos.x = player->position.pos.x;
        mat1.pos.y = player->position.pos.y;
        activeMap->addShieldExplosion(&mat1);
    }
}

void LOMap::collisionFireBallAndFireBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat1 = 0,*mat2 = 0;
    bool first = true;
    short i;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            fireBalls[i]->detach = true;
            if (first)
            {
                mat1 = &(fireBalls[i]->position);
                first = false;
            } else
                mat2 = &(fireBalls[i]->position);
        }
    if (!mat1 || !mat2) return;
    explosion(mat1,mat2,LOExplosion_Fire,LOExplosion_Fire);
    checkOnCloseExplosion(LOExplosion_Fire, LOExplosion_Fire, SunnyVector2D((mat1->pos.x+mat2->pos.x)/2, (mat1->pos.y+mat2->pos.y)/2));
    
    LOTrace::sharedTrace()-> addFireTrace(mat1);
    LOTrace::sharedTrace()-> addFireTrace(mat2);
    
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat1);
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat2);
    
    losPlayFFExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionFireBallAndWaterBall(cpBody * body1, cpBody * body2)
{
    //earthShake();
    SunnyMatrix *mat1 = 0,*mat2 = 0;
    SunnyVector2D vel;
    short i;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            fireBalls[i]->detach = true;
            mat1 = &(fireBalls[i]->position);
            vel = fireBalls[i]->velocity;
            break;
        }
    if (!mat1) return;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
            waterBalls[i]->detach = true;
            mat2 = &(waterBalls[i]->position);
            break;
        }
    if (!mat2) return;
    //explosion(mat1,mat2,LOExplosion_Fire,LOExplosion_Water);
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat1,LOExplosion_Steam);
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat2,LOExplosion_Steam);
    checkOnCloseExplosion(LOExplosion_Fire, LOExplosion_Water, SunnyVector2D((mat1->pos.x+mat2->pos.x)/2, (mat1->pos.y+mat2->pos.y)/2));
    
    LOFallingBall::addFallingBallFromFireball(mat1, vel);
    
    LOEvents::OnBallCollision(LOCT_WaterBall);
    
    losPlayFWExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionFireBallAndGroundBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat1 = 0,*mat2 = 0;
    short i;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            fireBalls[i]->detach = true;
            mat1 = &(fireBalls[i]->position);
            break;
        }
    if (!mat1) return;
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            groundBalls[i]->detach = true;
            mat2 = &(groundBalls[i]->position);
            break;
        }
    if (!mat2) return;
    explosion(mat1,mat2,LOExplosion_Fire,LOExplosion_Ground);
    checkOnCloseExplosion(LOExplosion_Fire, LOExplosion_Ground, SunnyVector2D((mat1->pos.x+mat2->pos.x)/2, (mat1->pos.y+mat2->pos.y)/2));
    
    LOTrace::sharedTrace()-> addFireTrace(mat1);
    LOTrace::sharedTrace()-> addGroundTrace(mat2);
    
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat1);
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat2);
    
    LOEvents::OnBallCollision(LOCT_GroundBall);
    
    losPlayFGExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionWaterBallAndWaterBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat1 = 0,*mat2 = 0;
    bool first = true;
    short i;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
            waterBalls[i]->detach = true;
            if (first)
            {
                mat1 = &(waterBalls[i]->position);
                first = false;
            } else
                mat2 = &(waterBalls[i]->position);
        }
    if (!mat1 || !mat2) return;
    
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat1,LOExplosion_Water);
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(mat2,LOExplosion_Water);
    
    checkOnCloseExplosion(LOExplosion_Water, LOExplosion_Water, SunnyVector2D((mat1->pos.x+mat2->pos.x)/2, (mat1->pos.y+mat2->pos.y)/2));
    LOEvents::OnBallCollision(LOCT_WaterBall);
    
    LOTrace::sharedTrace()-> addGroundWaterTrace(mat1);
    LOTrace::sharedTrace()-> addGroundWaterTrace(mat2);
    
    losPlayWWExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionWaterBallAndGroundBall(cpBody * body1, cpBody * body2)
{
    //earthShake();
    SunnyMatrix mat1 = sunnyIdentityMatrix,*mat2 = 0;
    SunnyVector2D vel;
    short i;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
            waterBalls[i]->detach = true;
            mat1 = waterBalls[i]->position;
            break;
        }
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            groundBalls[i]->detach = true;
            mat2 = &(groundBalls[i]->position);
            vel = groundBalls[i]->velocity;
            break;
        }
    if (!mat2) return;
    //explosion(mat1,mat2,LOExplosion_Water,LOExplosion_Ground);
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(&mat1,LOExplosion_Water);
    mat1.pos = (mat1.pos + mat2->pos)/2;
    LOTrace::sharedTrace()-> addGroundWaterTrace(&mat1);
    
    checkOnCloseExplosion(LOExplosion_Water, LOExplosion_Ground, SunnyVector2D(mat1.pos.x, mat1.pos.y));
    LOEvents::OnBallCollision(LOCT_WaterBall);
    LOEvents::OnBallCollision(LOCT_GroundBall);
    
    LOFallingBall::addFallingBallFromGroundball(mat2, vel);
    
    losPlayGWExplosion((mat1.pos-player->position.pos).length());
}

void LOMap::collisionGroundBallAndGroundBall(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat1 = 0,*mat2 = 0;
    bool first = true;
    short i;
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            groundBalls[i]->detach = true;
            if (first)
            {
                mat1 = &(groundBalls[i]->position);
                first = false;
            } else
                mat2 = &(groundBalls[i]->position);
        }
    if (!mat1 || !mat2) return;
    explosion(mat1,mat2,LOExplosion_Ground,LOExplosion_Ground);
    checkOnCloseExplosion(LOExplosion_Ground, LOExplosion_Ground, SunnyVector2D((mat1->pos.x+mat2->pos.x)/2, (mat1->pos.y+mat2->pos.y)/2));
    LOEvents::OnBallCollision(LOCT_GroundBall);
    
    LOTrace::sharedTrace()-> addGroundTrace(mat1);
    LOTrace::sharedTrace()-> addGroundTrace(mat2);
    
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat1);
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat2);
    
    losPlayGGExplosion((mat1->pos-player->position.pos).length());
}

void LOMap::collisionFireBallAndAnomalyBall(cpBody * body1, cpBody * body2)
{
    short i;
    SunnyMatrix mat;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            if (anomalyBalls[i]->detach)
                break;
            anomalyBalls[i]->detach = true;
            mat = anomalyBalls[i]->position;
            mat.up.x = anomalyBalls[i]->velocity.x;
            mat.up.y = anomalyBalls[i]->velocity.y;
            mat.up.normalize();
            mat.front = mat.up * mat.right;
            mat.pos.x -= mat.up.x*cellSize/2;
            mat.pos.y -= mat.up.y*cellSize/2;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Anomaly);
            break;
        }
    
    LOEvents::OnBallCollision(LOCT_AnomalyBall);
    
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
//        if (!fireBalls[i]->anomaled)
        {
            fireBalls[i]->applyAnomaly();
            explosion(&(fireBalls[i]->position), LOExplosion_Fire);
            break;
        }
}

void LOMap::collisionWaterBallAndAnomalyBall(cpBody * body1, cpBody * body2)
{
    short i;
    SunnyMatrix mat;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            if (anomalyBalls[i]->detach)
                break;
            anomalyBalls[i]->detach = true;
            mat = anomalyBalls[i]->position;
            mat.up.x = anomalyBalls[i]->velocity.x;
            mat.up.y = anomalyBalls[i]->velocity.y;
            mat.up.normalize();
            mat.front = mat.up * mat.right;
            mat.pos.x -= mat.up.x*cellSize/2;
            mat.pos.y -= mat.up.y*cellSize/2;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Anomaly);
            break;
        }
    
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
            if (!waterBalls[i]->anomaled)
            {
                waterBalls[i]->anomaled = 1;
//                explosion(fireBalls[i]->position, LOExplosion_Fire);
                break;
            }
    
    LOEvents::OnBallCollision(LOCT_WaterBall);
    LOEvents::OnBallCollision(LOCT_AnomalyBall);
}

void LOMap::collisionGroundBallAndAnomalyBall(cpBody * body1, cpBody * body2)
{
    short i;
    SunnyMatrix mat;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            if (anomalyBalls[i]->detach)
                break;
            anomalyBalls[i]->detach = true;
            mat = anomalyBalls[i]->position;
            mat.up.x = anomalyBalls[i]->velocity.x;
            mat.up.y = anomalyBalls[i]->velocity.y;
            mat.up.normalize();
            mat.front = mat.up * mat.right;
            mat.pos.x -= mat.up.x*cellSize/2;
            mat.pos.y -= mat.up.y*cellSize/2;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Anomaly);
            break;
        }
    
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
            {
                float angle = (0.5-sRandomf())*M_PI_2;
                SunnyVector2D vel;
                vel.x = groundBalls[i]->velocity.x*cosf(angle) - groundBalls[i]->velocity.y*sinf(angle);
                vel.y = groundBalls[i]->velocity.y*cosf(angle) + groundBalls[i]->velocity.x*sinf(angle);
                groundBalls[i]->position.up.x = vel.x;
                groundBalls[i]->position.up.y = vel.y;
                groundBalls[i]->position.up.normalize();
                groundBalls[i]->position.front.x = groundBalls[i]->position.up.y;
                groundBalls[i]->position.front.y = - groundBalls[i]->position.up.x;
                groundBalls[i]->setVelocity(vel);
                explosion(&(groundBalls[i]->position), LOExplosion_Ground);
                break;
            }
    LOEvents::OnBallCollision(LOCT_GroundBall);
    LOEvents::OnBallCollision(LOCT_AnomalyBall);
}

void LOMap::collisionAnomalyBallAndAnomalyBall(cpBody * body1, cpBody * body2)
{
    short i;
    SunnyMatrix mat1 = sunnyIdentityMatrix,mat2 = sunnyIdentityMatrix;
    bool first = true;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            anomalyBalls[i]->detach = true;
            if (first)
            {
                mat1 = anomalyBalls[i]->position;
                first = false;
            } else
                mat2 = anomalyBalls[i]->position;
        }
    
    mat2.pos = mat1.pos = (mat1.pos+mat2.pos)/2;
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(&mat1,LOExplosion_Anomaly);
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(&mat2,LOExplosion_Anomaly);
    
    LOEvents::OnBallCollision(LOCT_AnomalyBall);
}


void LOMap::collisionHighSnowAndFireBall(cpShape * body1, cpShape * body2)
{
    short i;
    SmallSnowPhysics * data;
    if (body1->data)
    {
        data = (SmallSnowPhysics*)body1->data;
    } else
    {
        data = (SmallSnowPhysics*)body2->data;
    }

    SunnyMatrix *mat = 0;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            if (fireBalls[i]->detach)
                return;
            fireBalls[i]->detach = true;
            mat = &(fireBalls[i]->position);
            break;
        }
    if (!mat) return;
    mapInfo.explodeSnow(data->num,mat);
    
    explosion(mat, LOExplosion_Fire);
 
    LOTrace::sharedTrace()-> addFireTrace(mat);
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat);
    
    losPlayFFExplosion((mat->pos-player->position.pos).length());
    losPlaySSExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionHighSnowAndWaterBall(cpShape * body1, cpShape * body2)
{
    short i;
    SunnyMatrix *mat = 0;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
            if (waterBalls[i]->detach)
                return;
            waterBalls[i]->detach = true;
            mat = &(waterBalls[i]->position);
            break;
        }
    if (!mat) return;
    explosion(mat, LOExplosion_Water);
    losPlayWWExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionHighSnowAndAnomalyBall(cpShape * body1, cpShape * body2)
{
    short i;
    SunnyMatrix mat;
    for (i=0;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->body==body1 || anomalyBalls[i]->body==body2)
        {
            if (anomalyBalls[i]->detach)
                return;
            anomalyBalls[i]->detach = true;
            mat = anomalyBalls[i]->position;
            mat.up.x = anomalyBalls[i]->velocity.x;
            mat.up.y = anomalyBalls[i]->velocity.y;
            mat.up.normalize();
            mat.front = mat.up * mat.right;
            mat.pos.x -= mat.up.x*cellSize/2;
            mat.pos.y -= mat.up.y*cellSize/2;
            break;
        }
    if (explosionsCount<maxExplosition)
        explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Anomaly);
}

void LOMap::collisionHighSnowAndGroundBall(cpShape * body1, cpShape * body2)
{
    short i;
    SmallSnowPhysics * data;
    if (body1->data)
    {
        data = (SmallSnowPhysics*)body1->data;
    } else
    {
        data = (SmallSnowPhysics*)body2->data;
    }
    
    SunnyMatrix *mat = 0;
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            if (groundBalls[i]->detach)
                return;
            groundBalls[i]->detach = true;
            mat = &(groundBalls[i]->position);
            break;
        }
    if (!mat) return;
    mapInfo.explodeSnow(data->num,mat);
    explosion(mat, LOExplosion_Ground);
    
    LOTrace::sharedTrace()-> addGroundTrace(mat);
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat);
    //LOCrashBalls::sharedCrashBalls()->addSnowWallCrash(&mat1);
    
    losPlayFGExplosion((mat->pos-player->position.pos).length());
    losPlaySSExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionPlayerAndSnowflake(cpShape * shape1, cpShape * shape2)
{
    if (player->crashed) return;
    
    SmallSnowPhysics * p = (SmallSnowPhysics*)shape1->data;
    if (!p)
        p = (SmallSnowPhysics*)shape2->data;
    short i =  (p - mapInfo.snowflakesPhysics);
    mapInfo.snowflakesPhysics[i].waitForRemove = true;
    mapInfo.lastSnowFlakeTime = realPlayingTime;
    losPlaySnowflakePickUp();
    
    tryToDropPuzzlePart(mapInfo.snowflakesPhysics[i].circleBody->position);
}

void LOMap::collisionPlayerAndVioletSnowflake(cpShape * shape1, cpShape * shape2)
{
    if (player->crashed) return;
    
    LOVioletSnowflake * p = (LOVioletSnowflake*)shape1->data;
    if (!p)
        p = (LOVioletSnowflake*)shape2->data;
    p->collectSnowflake();
    p->prepareToDissapear();
    
    mapInfo.lastSnowFlakeTime = realPlayingTime;
    losPlaySnowflakePickUp();
    survivalSnowflakesCollected++;
    if (tryToDropGoldCoin(p->getPosition()))
        survivalGoldCoinsCollected++;
}

void LOMap::collisionPlayerAndGoldCoin(cpShape * shape1, cpShape * shape2)
{
    if (player->crashed) return;
    
    LOGoldCoin * p = (LOGoldCoin*)shape1->data;
    if (!p)
        p = (LOGoldCoin*)shape2->data;
    
    p->collectCoin();
    losPlaySnowflakePickUp();
    
    for (short i = 0;i<goldCoinsCount-1;i++)
        if (goldCoins[i] == p)
        {
            goldCoins[i] = goldCoins[goldCoinsCount-1];
            goldCoins[goldCoinsCount-1] = p;
            break;
        }
    goldCoinsCount--;
}

void LOMap::collisionPlayerAndBonfire(cpShape * shape1, cpShape * shape2)
{
    if (!player->crashed)
    {
        SmallSnowPhysics * data;
        if (shape1->data)
            data = (SmallSnowPhysics*)shape1->data;
        else
            data = (SmallSnowPhysics*)shape2->data;
        
        if (!mapInfo.bonfires[data->num].isActive()) return;
        mapInfo.bonfires[data->num].extinguishFire();
        
        if (!player->useShield())
        {
            gameOver(LOD_Bonfire);
            
            snowBallCrashObj->prepareToCrashFromBonfire(player->position,player->velocity);
            player->setCrashed(true);
            if (explosionsCount<maxExplosition)
            {
                SunnyMatrix mat = getIdentityMatrix();
                mat.pos = player->position.pos;
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Steam);
            }
            
            losPlaySnowballBonfire();
        } else
        {
            SunnyMatrix mat = getIdentityMatrix();
            mat.pos = player->position.pos;
            mat.up.x = mat.pos.x - mapInfo.bonfires[data->num].position.x;
            mat.up.y = mat.pos.y - mapInfo.bonfires[data->num].position.y;
            mat.up.normalize();
            mat.front = mat.up ^ mat.right;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Life);
        }
    }
}

void LOMap::collisionPlayerAndRollingBlades(cpShape * shape1, cpShape * shape2)
{
    if (!player->crashed)
    {
        if (!player->useShield())
        {
            gameOver(LOD_Blades);
            losPlaySnowBallAndBlades();
            snowBallCrashObj->prepareToCrashFromBonfire(player->position,player->velocity);
            player->setCrashed(true);
            if (explosionsCount<maxExplosition)
            {
                SunnyMatrix mat = getIdentityMatrix();
                mat.pos = player->position.pos;
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Steam);
            }
        } else
        {
            SunnyMatrix mat = getIdentityMatrix();
            mat.pos = player->position.pos;
            float angle = sRandomf()*M_PI*2;
            mat.up.x = cosf(angle);
            mat.up.y = sinf(angle);
            mat.front = mat.up ^ mat.right;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Life);
        }
    }
}

void LOMap::collisionPlayerAndElectricField(cpShape * shape1, cpShape * shape2)
{
    if (!player->crashed)
    {
        if (!player->useShield())
        {
            gameOver(LOD_ElectricField);
            snowBallCrashObj->prepareToCrashFromBonfire(player->position,player->velocity);
            player->setCrashed(true);
            addElectroExplosion(player->getPosition());
            losPlayEExplosion(0);
            if (explosionsCount<maxExplosition)
            {
                SunnyMatrix mat = getIdentityMatrix();
                mat.pos = player->position.pos;
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Steam);
            }
        } else
        {
            SunnyMatrix mat = getIdentityMatrix();
            mat.pos = player->position.pos;
            float angle = sRandomf()*M_PI*2;
            mat.up.x = cosf(angle);
            mat.up.y = sinf(angle);
            mat.front = mat.up ^ mat.right;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Life);
        }
    }
}


void LOMap::collisionPlayerAndElectricBallBegan(cpBody * body1, cpBody * body2)
{
    short i;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            if (player->body==body2 && player->crashed) return;
            if (isMultiPlayer)
                for (short i = 0; i<otherPlayersCount; i++)
                    if (otherPlayers[i].player->body==body2 && otherPlayers[i].player->crashed) return;
            electricBalls[i]->addTarget(body2);
            break;
        } else
        if (electricBalls[i]->body==body2)
        {
            if (player->body==body1 && player->crashed) return;
            if (isMultiPlayer)
                for (short i = 0; i<otherPlayersCount; i++)
                    if (otherPlayers[i].player->body==body1 && otherPlayers[i].player->crashed) return;
            electricBalls[i]->addTarget(body1);            
            break;
        }
}

void LOMap::collisionPlayerAndElectricBallEnded(cpBody * body1, cpBody * body2)
{
    short i;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->removeTarget(body2);
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->removeTarget(body1);            
                break;
            }
#ifndef LOOKOUT_GOD_MOD    
    if (!player->crashed && (player->body==body1 || player->body==body2))
    {
        if (!player->useShield())
        {
            gameOver(LOD_ElectricBall);
            snowBallCrashObj->prepareToCrashFromBonfire(player->position,player->velocity);
            player->setCrashed(true);
            addElectroExplosion(player->getPosition());
            SunnyMatrix mat = getIdentityMatrix();
            mat.pos = player->position.pos;
            LOTrace::sharedTrace()-> addElectricTrace(&mat);
            losPlayEExplosion(0);
            if (explosionsCount<maxExplosition)
            {
                SunnyMatrix mat = getIdentityMatrix();
                mat.pos = player->position.pos;
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Steam);
            }
        } else
        {
            SunnyMatrix mat = getIdentityMatrix();
            mat.pos = player->position.pos;
            float angle = sRandomf()*M_PI*2;
            mat.up.x = cosf(angle);
            mat.up.y = sinf(angle);
            mat.front = mat.up ^ mat.right;
            if (explosionsCount<maxExplosition)
                explosions[explosionsCount++].initWithParams(&mat,LOExplosion_Life);
        }
    }
#endif    
}

void LOMap::collisionFireBallAndElectricBallBegan(cpBody * body1, cpBody * body2)
{
    short i;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->addTarget(body2);
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->addTarget(body1);
                break;
            }
}

void LOMap::collisionFireBallAndElectricBallEnded(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat = 0;
    //SunnyMatrix mat2;
    short i,j;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            //mat2 = electricBalls[i].position;
            for (j=0;j<fireBallsCount;j++)
                if (fireBalls[j]->body==body2)
                {
                    electricBalls[i]->removeTarget(body2);
                    mat = &(fireBalls[j]->position);
                    fireBalls[j]->detach = true;
                    break;
                }
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                //mat2 = electricBalls[i].position;
                for (j=0;j<fireBallsCount;j++)
                    if (fireBalls[j]->body==body1)
                    {
                        electricBalls[i]->removeTarget(body1);
                        mat = &(fireBalls[j]->position);
                        fireBalls[j]->detach = true;
                        break;
                    }
                break;
            }
    if (!mat) return;
    explosion(mat, LOExplosion_Fire);
    //explosion(mat, LOExplosion_Electric);
    addElectroExplosion(SunnyVector2D(mat->pos.x,mat->pos.y));
    LOTrace::sharedTrace()-> addFireTrace(mat);
    LOTrace::sharedTrace()-> addElectricTrace(mat);
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat);
    
    losPlayFFExplosion((mat->pos-player->position.pos).length());
    losPlayEExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionWaterBallAndElectricBallBegan(cpBody * body1, cpBody * body2)
{
    short i,j;
    SunnyVector2D p(0,0);
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            for (j=0;j<waterBallsCount;j++)
                if (waterBalls[j]->body==body2)
                {
                    electricBalls[i]->addTarget(body2);
                    waterBalls[j]->skipFrame = true;
                    p = waterBalls[j]->getPosition();
                    break;
                }
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                for (j=0;j<waterBallsCount;j++)
                    if (waterBalls[j]->body==body1)
                    {
                        electricBalls[i]->addTarget(body1);
                        waterBalls[j]->skipFrame = true;
                        p = waterBalls[j]->getPosition();
                        break;
                    }
                break;
            }
    addElectroExplosion(p);
}

void LOMap::collisionWaterBallAndElectricBallEnded(cpBody * body1, cpBody * body2)
{
    short i;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->removeTarget(body2);
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->removeTarget(body1);
                break;
            }
}

void LOMap::collisionGroundBallAndElectricBallBegan(cpBody * body1, cpBody * body2)
{
    short i;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->addTarget(body2);
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->addTarget(body1);
                break;
            }
}

void LOMap::collisionGroundBallAndElectricBallEnded(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat = 0;//,mat2;
    short i,j;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            //mat2 = electricBalls[i].position;
            for (j=0;j<groundBallsCount;j++)
                if (groundBalls[j]->body==body2)
                {
                    electricBalls[i]->removeTarget(body2);
                    mat = &(groundBalls[j]->position);
                    groundBalls[j]->detach = true;
                    break;
                }
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                //mat2 = electricBalls[i].position;
                for (j=0;j<groundBallsCount;j++)
                    if (groundBalls[j]->body==body1)
                    {
                        electricBalls[i]->removeTarget(body1);
                        mat = &(groundBalls[j]->position);
                        groundBalls[j]->detach = true;
                        break;
                    }
                break;
            }
    if (!mat) return;
    explosion(mat, LOExplosion_Ground);
    addElectroExplosion(SunnyVector2D(mat->pos.x,mat->pos.y));
    LOTrace::sharedTrace()-> addGroundTrace(mat);
    LOTrace::sharedTrace()-> addElectricTrace(mat);
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat);
    
    losPlayFGExplosion((mat->pos-player->position.pos).length());
    losPlayEExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionAngryBallAndElectricBallBegan(cpBody * body1, cpBody * body2)
{
    short i,j;
    SunnyVector2D p(0,0);
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            for (j=0;j<angryBallsCount;j++)
                if (angryBalls[j]->body==body2)
                {
                    electricBalls[i]->addTarget(body2);
                    angryBalls[j]->chargeTheBall();
                    p = angryBalls[j]->getPosition();
                    break;
                }
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                for (j=0;j<angryBallsCount;j++)
                    if (angryBalls[j]->body==body1)
                    {
                        electricBalls[i]->addTarget(body1);
                        angryBalls[j]->chargeTheBall();
                        p = angryBalls[j]->getPosition();
                        break;
                    }
                break;
            }
    addElectroExplosion(p);
}

void LOMap::collisionAngryBallAndElectricBallEnded(cpBody * body1, cpBody * body2)
{
    short i;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->removeTarget(body2);
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->removeTarget(body1);
                break;
            }
}

void LOMap::collisionElectricBallAndElectricBallBegan(cpBody * body1, cpBody * body2)
{
    short i,j;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->addTarget(body2);
            for (j=0;j<electricBallsCount;j++)
                if (electricBalls[j]->body==body2)
                {
                    electricBalls[j]->addTarget(body1);
                    break;
                }
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->addTarget(body1);
                for (j=0;j<electricBallsCount;j++)
                    if (electricBalls[j]->body==body1)
                    {
                        electricBalls[j]->addTarget(body2);
                        break;
                    }
                break;
            }
}

void LOMap::collisionAngryBallChargedBegan(cpBody * body1, cpBody * body2)
{
    SunnyMatrix mat = getIdentityMatrix();
    LOAngryBall * ball1 = 0;
    LOAngryBall * ball2 = 0;
    for (short j=0;j<angryBallsCount;j++)
        if (angryBalls[j]->body==body1 || angryBalls[j]->body==body2)
        {
            if (angryBalls[j]->chargeInfo.charged)
            {
                angryBalls[j]->detach = true;
                mat.pos = angryBalls[j]->position.pos;
                
                explodeAngryballAtLocation(&mat);
                explosion(&mat, LOExplosion_Fire);
                losPlayFFExplosion((mat.pos-player->position.pos).length());
            }
            
            if (ball1)
            {
                ball2 = angryBalls[j];
                break;
            }
            else
                ball1 = angryBalls[j];
        }
    
    if (ball1 && ball2)
    {
        if (ball1->detach != ball2->detach)
        {
            if (ball1->detach)
            {
                mat.pos = ball2->position.pos;
                ball2->detach = true;
            }
            else
            {
                mat.pos = ball1->position.pos;
                ball1->detach = true;
            }
            explodeAngryballAtLocation(&mat);
            
            UnnyNet::reportAchievement("angryVSangry", 100);
        }
        SunnyVector2D front = ball2->getPosition() - ball1->getPosition();
        front.normalize();
        
        float proj = front & ball1->velocity;
        if (proj>0)
            ball1->velocity -= proj*front;
        
        proj = (-front) & ball2->velocity;
        if (proj>0)
            ball2->velocity += proj*front;
    }
}

void LOMap::collisionElectricBallAndElectricBallEnded(cpBody * body1, cpBody * body2)
{
    short i,j;
    for (i=0;i<electricBallsCount;i++)
        if (electricBalls[i]->body==body1)
        {
            electricBalls[i]->removeTarget(body2);
            for (j=0;j<electricBallsCount;j++)
                if (electricBalls[j]->body==body2)
                {
                    electricBalls[j]->removeTarget(body1);
                    break;
                }
            break;
        } else
            if (electricBalls[i]->body==body2)
            {
                electricBalls[i]->removeTarget(body1);
                for (j=0;j<electricBallsCount;j++)
                    if (electricBalls[j]->body==body1)
                    {
                        electricBalls[j]->removeTarget(body2);
                        break;
                    }
                break;
            }
}


void LOMap::collisionFireBallAndElectricField(cpBody * body1, cpBody * body2)
{
//    LORollingBlades * el = (LORollingBlades *)body1->data;
//    if (!el) el = (LORollingBlades *)body2->data;
//    el->attaking++;
    SunnyMatrix* mat = 0;
    SunnyVector2D vel;
    short i;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            fireBalls[i]->detach = true;
            mat = &(fireBalls[i]->position);
            vel = fireBalls[i]->velocity;
            break;
        }
    
//    LORollingBlades * el = (LORollingBlades *)body1->data;
//    if (!el) el = (LORollingBlades *)body2->data;
//    el->attaking--;
    if (!mat) return;
    LOTrace::sharedTrace()-> addFireTrace(mat);
    explosion(mat, LOExplosion_Fire);
    addElectroExplosion(SunnyVector2D(mat->pos.x,mat->pos.y));
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat);
    losPlayFFExplosion((mat->pos-player->position.pos).length());
    losPlayEExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionFireBallAndElectricFieldEnded(cpBody * body1, cpBody * body2)
{
    return;
    SunnyMatrix *mat;
    SunnyVector2D vel;
    short i;
    for (i=0;i<fireBallsCount;i++)
        if (fireBalls[i]->body==body1 || fireBalls[i]->body==body2)
        {
            fireBalls[i]->detach = true;
            mat = &(fireBalls[i]->position);
            vel = fireBalls[i]->velocity;
            break;
        }
    
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking--;
    
    LOTrace::sharedTrace()-> addFireTrace(mat);
    explosion(mat, LOExplosion_Fire);
    addElectroExplosion(SunnyVector2D(mat->pos.x,mat->pos.y));
    LOCrashBalls::sharedCrashBalls()->addFireBallCrash(mat);
    losPlayFFExplosion((mat->pos-player->position.pos).length());
    losPlayEExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionGroundBallAndElectricField(cpBody * body1, cpBody * body2)
{
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking++;
    LOEvents::OnBallCollision(LOCT_GroundBall);
}

void LOMap::collisionGroundBallAndElectricFieldEnded(cpBody * body1, cpBody * body2)
{
    SunnyMatrix *mat = 0;
    SunnyVector2D vel;
    short i;
    for (i=0;i<groundBallsCount;i++)
        if (groundBalls[i]->body==body1 || groundBalls[i]->body==body2)
        {
            groundBalls[i]->detach = true;
            mat = &(groundBalls[i]->position);
            vel = groundBalls[i]->velocity;
            break;
        }
    if (!mat) return;
    
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking--;
    
    LOTrace::sharedTrace()-> addGroundTrace(mat);
    explosion(mat, LOExplosion_Ground);
    addElectroExplosion(SunnyVector2D(mat->pos.x,mat->pos.y));
    LOCrashBalls::sharedCrashBalls()->addGroundBallCrash(mat);
    losPlayFGExplosion((mat->pos-player->position.pos).length());
    losPlayEExplosion((mat->pos-player->position.pos).length());
}

void LOMap::collisionWaterBallAndElectricField(cpBody * body1, cpBody * body2)
{
    SunnyMatrix mat;
    SunnyVector2D vel;
    short i;
    for (i=0;i<waterBallsCount;i++)
        if (waterBalls[i]->body==body1 || waterBalls[i]->body==body2)
        {
            waterBalls[i]->skipFrame = true;
            addElectroExplosion(waterBalls[i]->getPosition());
            break;
        }
    
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking++;
    
    LOEvents::OnBallCollision(LOCT_WaterBall);
}

void LOMap::collisionWaterBallAndElectricFieldEnded(cpBody * body1, cpBody * body2)
{
    LORollingBlades * el = (LORollingBlades *)body1->data;
    if (!el) el = (LORollingBlades *)body2->data;
    el->attaking--;
}

void LOMap::collisionPlayerAndIceBlock(cpBody * body1, cpBody * body2)
{
    SunnyVector2D pos;
    LOIceBlock * block;
    block = (LOIceBlock*)body1->data;
    if (!block)
    {
        block = (LOIceBlock*)body2->data;
        pos = body1->position;
    } else
    {
        pos = body2->position;
    }
    
    block->tryToMoveFromPoint(pos);
}

void LOMap::collisionPlayerAndIceBlockEnded(cpBody * body1, cpBody * body2)
{
    LOIceBlock * block;
    block = (LOIceBlock*)body1->data;
    if (!block)
        block = (LOIceBlock*)body2->data;
    block->removeContact();
}

void LOMap::createRandomFireBall(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);        
    SunnyVector2D pos2;
    short ran = sRandomi()%targetCount;
    if (ran == 0 && !isMultiPlayer)
    {
        pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    } else
    {
        pos2 = randPosOnMapBounds(bound);
    }
    
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createFireBall(pos, pos2);
}

void LOMap::createRandomWaterBall(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    short ran = sRandomi()%targetCount;
    if (ran == 0 && !isMultiPlayer)
    {
        pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    } else
    {
        pos2 = randPosOnMapBounds(bound);
    }
    
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createWaterBall(pos, pos2);
}

void LOMap::createRandomGroundBall(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    short ran = sRandomi()%targetCount;
    if (ran == 0 && !isMultiPlayer)
    {
        pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    } else
    {
        pos2 = randPosOnMapBounds(bound);
    }
    
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createGroundBall(pos, pos2);
}

void LOMap::createRandomElectricBall(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    short ran = sRandomi()%targetCount;
    if (ran == 0 && !isMultiPlayer)
    {
        pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    } else
    {
        pos2 = randPosOnMapBounds(bound);
    }
    
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createElectricBall(pos, pos2);
}

void LOMap::createRandomAnomalyBall(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    short ran = sRandomi()%targetCount;
    if (ran == 0 && !isMultiPlayer)
    {
        pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    } else
    {
        pos2 = randPosOnMapBounds(bound);
    }
    
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createAnomalyBall(pos, pos2);
}

void LOMap::createRandomFireBallToPlayer(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createFireBall(pos, pos2);
}

void LOMap::createRandomWaterBallToPlayer(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createWaterBall(pos, pos2);
}

void LOMap::createRandomGroundBallToPlayer(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createGroundBall(pos, pos2);
}

void LOMap::createRandomElectricBallToPlayer(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createElectricBall(pos, pos2);
}

void LOMap::createRandomAnomalyBallToPlayer(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createAnomalyBall(pos, pos2);
}

void LOMap::createFireBall(SunnyVector2D pos, SunnyVector2D vel)
{
//    createGroundBall(pos, vel);
//    return;
    if (fireBallsCount>=maxFireBalls) return;
    if (cheatProtection1) vel*=4;
    fireBalls[fireBallsCount++]->initWithParams(pos, vel * defaultVelocity);
    if (isMultiPlayer && isServer) multiPlayerRespawnBall(SET_FireBall,pos,vel);
}

void LOMap::createWaterBall(SunnyVector2D pos, SunnyVector2D vel)
{
    if (waterBallsCount>=maxWaterBalls) return;
    if (cheatProtection1) vel*=4;
    waterBalls[waterBallsCount++]->initWithParams(pos, vel * defaultVelocity);
    if (isMultiPlayer && isServer) multiPlayerRespawnBall(SET_WaterBall,pos,vel);
}

void LOMap::createGroundBall(SunnyVector2D pos, SunnyVector2D vel)
{
    if (groundBallsCount>=maxGroundBalls) return;
    if (cheatProtection1) vel*=4;
    groundBalls[groundBallsCount++]->initWithParams(pos, vel * defaultVelocity);
    if (isMultiPlayer && isServer) multiPlayerRespawnBall(SET_GroundBall,pos,vel);
}

void LOMap::createElectricBall(SunnyVector2D pos, SunnyVector2D vel)
{
    if (electricBallsCount>=maxElectricBalls) return;
    if (cheatProtection1) vel*=4;
    electricBalls[electricBallsCount++]->initWithParams(pos, vel * defaultVelocity);
    if (isMultiPlayer && isServer) multiPlayerRespawnBall(SET_ElectricBall,pos,vel);
}

void LOMap::createAnomalyBall(SunnyVector2D pos, SunnyVector2D vel)
{
    if (anomalyBallsCount>=maxAnomalyBalls) return;
    if (cheatProtection1) vel*=4;
    anomalyBalls[anomalyBallsCount++]->initWithParams(pos, vel * defaultVelocity);
    if (isMultiPlayer && isServer) multiPlayerRespawnBall(SET_AnomalyBall,pos,vel);
}

void LOMap::createBufAngryBall(LOBufAngryBall pos)
{
    if (angryBallsCount>=maxAngryBalls) return;
    angryBalls[angryBallsCount++]->initBufWithParams(pos);
}

void LOMap::createRandomAngryBall()
{
    if (angryBallsCount>=maxAngryBalls) return;
    
    SunnyVector2D pos;
    pos.x = sRandomf()*(mapSize.x - 6) + 3;
    pos.y = sRandomf()*(mapSize.y - 6) + 3;
    if (pos.y<mapSize.x/3 && pos.y<mapSize.y/2)
    {
        if (sRandomi()%2)
            pos.x = sRandomf()*(mapSize.x*2/3 - 3) + mapSize.x/3;
        else
            pos.y = sRandomf()*(mapSize.y/2 - 3) + mapSize.y/2;
    }
//    pos.type = 0;
    angryBalls[angryBallsCount++]->initWithParams(pos);
}

void LOMap::createAngryBall(SunnyVector2D pos)
{
    if (angryBallsCount>=maxAngryBalls) return;
    angryBalls[angryBallsCount++]->initWithParams(pos);
    if (isMultiPlayer && isServer) multiPlayerRespawnBall(SET_AngryBall,pos,SunnyVector2D(0,0));
}

void LOMap::addSnowFromAngryBall(SunnyVector2D pos,SunnyVector2D vel)
{
    if (snowFromAngryBallsCount>=maxSnowFromAngryBallsCount) return;
    
    snowFromAngryBalls[snowFromAngryBallsCount].position = SunnyVector3D(pos.x,pos.y,1);
    snowFromAngryBalls[snowFromAngryBallsCount].velocity = SunnyVector3D(vel.x,vel.y,sRandomf());
    snowFromAngryBalls[snowFromAngryBallsCount].angle = sRandomf()*360;
    snowFromAngryBallsCount++;
}

void LOMap::renderSnowFromAngryBallsUnderPlayer(SunnyMatrix *MV)
{
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    
    float scale = 0.5;
    SunnyMatrix m;
    for (short i = 0;i<snowFromAngryBallsCount;i++)
    {
        if (snowFromAngryBalls[i].position.z<=0)
        {
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(1,1,1,1 + snowFromAngryBalls[i].position.z));
            
            m = getTranslateMatrix(SunnyVector3D(snowFromAngryBalls[i].position.x, snowFromAngryBalls[i].position.y, 0)) * *MV;
            m = getScaleMatrix(SunnyVector3D(scale,scale,scale)) * m;
            m = getRotationZMatrix(snowFromAngryBalls[i].angle*M_PI/180) * m;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            explosionObj->renderObject(snowObjects[i%4]);
        }
    }
}

void LOMap::renderSnowFromAngryBalls(SunnyMatrix *MV)
{
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    float scale;
    SunnyMatrix m;
    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.y,1));
    for (short i = 0;i<snowFromAngryBallsCount;i++)
    {
        if (snowFromAngryBalls[i].position.z>0)
        {
            scale = 0.5 + snowFromAngryBalls[i].position.z;
            m = getTranslateMatrix(SunnyVector3D(snowFromAngryBalls[i].position.x, snowFromAngryBalls[i].position.y, snowFromAngryBalls[i].position.z)) **MV;
            m = getScaleMatrix(SunnyVector3D(scale, scale, scale))*m;
            m = getRotationZMatrix(snowFromAngryBalls[i].angle*M_PI/180 ) *m;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            explosionObj->renderObject(snowObjects[i%4]);
        }
    }
}

void LOMap::updateSnowFromAngryBalls()
{
    for (short i = 0;i<snowFromAngryBallsCount;i++)
    {
        if (snowFromAngryBalls[i].position.z>0)
        {
            snowFromAngryBalls[i].position += snowFromAngryBalls[i].velocity*deltaTime;
            snowFromAngryBalls[i].position.z -= 3*deltaTime*deltaTime;
            snowFromAngryBalls[i].velocity.z -= deltaTime*6;
        } else
            snowFromAngryBalls[i].position.z -= deltaTime;
        
        if (snowFromAngryBalls[i].position.z<=-1)
        {
            snowFromAngryBalls[i] = snowFromAngryBalls[snowFromAngryBallsCount-1];
            snowFromAngryBallsCount--;
            i--;
            continue;
        }
    }
}

void LOMap::updateInfoForSounds()
{
    if (gamePaused) return;
    
    SunnyVector2D playerPos = player->getPosition();
    
    for (short i = 0;i<fireBallsCount;i++)
        if (fireBalls[i]->status == LO_Active)
        {
            losAddFireBallSound(fireBalls[i]->body, fireBalls[i]->distanceToPoint(playerPos));
        }
    
    for (short i = 0;i<groundBallsCount;i++)
        if (groundBalls[i]->status == LO_Active)
        {
            losAddGroundBallSound(groundBalls[i]->body, groundBalls[i]->distanceToPoint(playerPos));
        }
    
    for (short i = 0;i<waterBallsCount;i++)
        if (waterBalls[i]->status == LO_Active)
        {
            if (waterBalls[i]->skipFrame)
                losAddAngryBallLockedSound(waterBalls[i]->body, waterBalls[i]->distanceToPoint(playerPos));
            else
                losAddWaterBallSound(waterBalls[i]->body, waterBalls[i]->distanceToPoint(playerPos));
        }
    
    for (short i = 0;i<mapInfo.bonfiresCount;i++)
        losAddBonfireSound(mapInfo.bonfiresPhysics[i].circleBody, (mapInfo.bonfires[i].position-playerPos).length());
    
    for (short i = 0;i<mapInfo.electricFieldsCount;i++)
        if (mapInfo.electricFields[i].charged)
        {
            losAddElectricFieldSound(&mapInfo.electricFields[i], (mapInfo.electricFields[i].position - playerPos).length());
        }
    
    for (short i = 0;i<angryBallsCount;i++)
        if (!angryBalls[i]->chargeInfo.charged)
        {
            if (angryBalls[i]->turnOffTime<=0)
            {
                if (angryBalls[i]->respawnTime<=0)
                    losAddAngryBallSound(angryBalls[i]->body, angryBalls[i]->distanceToPoint(playerPos),angryBalls[i]->state/30.);
            } else
            {
                losAddAngryBallLockedSound(angryBalls[i]->body, angryBalls[i]->distanceToPoint(playerPos));
            }
        } else
            if (angryBalls[i]->chargeInfo.chargedTime>=2.0)
                losAddAngryRushBallSound(angryBalls[i]->body, angryBalls[i]->distanceToPoint(playerPos));
    
    for (short i = 0;i<mapInfo.rollingBladesCount;i++)
        losAddRotatingBladesSound(&mapInfo.rollingBlades[i], (mapInfo.rollingBlades[i].position - playerPos).length());
    
    for (short i = 0;i<mapInfo.bladesCount;i++)
        losAddMovingBladesSound(&mapInfo.blades[i], (mapInfo.blades[i].position - playerPos).length(),mapInfo.blades[i].startAngle);
    
    for (short i = 0;i<electricBallsCount;i++)
    {
        if (electricBalls[i]->attackCount)
            losAddElectricAttackSound(electricBalls[i]->body, electricBalls[i]->distanceToPoint(playerPos));
        losAddElectricBalllSound(electricBalls[i]->body, electricBalls[i]->distanceToPoint(playerPos));
    }
}

void LOMap::calculateScores()
{
    if (playingTime < mapInfo.supposedPlayingTime3)
        earnedStars = 3;
    else
    if (playingTime < mapInfo.supposedPlayingTime2)
        earnedStars = 2;
    else
        earnedStars = 1;
}

bool LOMap::isLevelComplete()
{
    return mapInfo.levelComplete;
}

void LOMap::update()
{
    if (resetType != RT_None)
    {
        switch (resetType) {
            case RT_Death:
            {
                LOMenu::getSharedMenu()->showGameOverButtons(false);
                LOMenu::getSharedMenu()->showGameSpellButtons(true);
                loResetMap();
                LOMenu::getSharedMenu()->dontShowTutorial();
                
                LOPriceType type;
                unsigned short price;
                
                SunnyButton ** buttons = LOMenu::getSharedMenu()->getDeathWindow()->buttons;
                
                if (buttons[LODWB_Spell_Doubler]->active)
                {
                    type = LOScore::getPrice(LA_Doubler, price);
                    if (type!=LOPT_Free) buttons[LODWB_Spell_Doubler]->active = false;
                    player->activateAbility(LA_Doubler);
                }
                if (buttons[LODWB_Spell_Magnet]->active)
                {
                    type = LOScore::getPrice(LA_Magnet, price);
                    if (type!=LOPT_Free) buttons[LODWB_Spell_Magnet]->active = false;
                    player->activateAbility(LA_Magnet);
                }
                if (buttons[LODWB_Spell_Time]->active)
                {
                    type = LOScore::getPrice(LA_Time, price);
                    if (type!=LOPT_Free) buttons[LODWB_Spell_Time]->active = false;
                    player->activateAbility(LA_Time);
                }
                break;
            }
            case RT_LevelComplete:
            {
                gamePaused = false;
                LOMenu::getSharedMenu()->showLevelCompleteButtons(false);
                LOMenu::getSharedMenu()->showGameSpellButtons(true);
                loResetMap();
                LOMenu::getSharedMenu()->dontShowTutorial();
                break;
            }
            case RT_Pause:
            {
                LOMenu::getSharedMenu()->showGamePlayButtons(false);
                LOMenu::getSharedMenu()->showGameSpellButtons(true);
                gamePaused = false;
                loResetMap();
                LOMenu::getSharedMenu()->dontShowTutorial();
                break;
            }
                
            default:
                break;
        }
        
        resetType = RT_None;
    }
    
    if (playingTime<0.5)
        LOMenu::getSharedMenu()->updateSpells();

    if (playingTime - lastReportedPlayingTime>=1)
    {
        LOEvents::OnSecondPassed();
        lastReportedPlayingTime++;
        
        if (getCurrentTime()>windTimer && mapWind.z<=0)
        {
            float windPower = (0.2+0.8*sRandomf());
            float angle = sRandomf()*M_PI*2;
            newMapWind.x = windPower*cosf(angle);
            newMapWind.y = windPower*sinf(angle);
            mapWind.z = 1;
            mapWind.w = 20 + 60*sRandomf();
        }
    }
    
    if (shortMessage)
        if (!shortMessage->update(deltaTime))
        {
            delete shortMessage;
            shortMessage = 0;
        }
    
    mainMenu->update();
    
    losChangeVolumeOfTheMusic();
    
    if (mapInfo.levelComplete)
    {
        levelCompleted();
        //losSetRollingSnowballSpeed(0);
    }
    
    if (isSurvival && activeSurvivalMap)
        activeSurvivalMap->update();
    
    short i;
    
    if (!physicsPaused)
    {
        player->update();
        if (isMultiPlayer)
            for (short i = 0;i<otherPlayersCount;i++)
                if (!otherPlayers[i].player->crashed)
                    otherPlayers[i].player->updateMultiPlayer();
    }
//    else
//        losSetRollingSnowballSpeed(0);
    
    LOFallingBall::update();
    LOCrashBalls::sharedCrashBalls()->update();
    
    for (short i = 0;i<goldCoinsCount;i++)
        goldCoins[i]->update();
    
    for (short i = 0;i<violetSnowflakesCount;i++)
        if (!violetSnowflakes[i]->update())
        {
            destroyVioletSnowflake(violetSnowflakes[i]);
        };
    
    //FireBalls
    for (i = fireBallsCount-1;i>=0; i--)
        if (fireBalls[i]->update())
        {
            fireBalls[i]->detachPhysics();
            if (i!=fireBallsCount-1)
            {
                LOFireBall * buf = fireBalls[i];
                fireBalls[i] = fireBalls[fireBallsCount-1];
                fireBalls[fireBallsCount-1] = buf;
            }
            fireBallsCount--;
        }
    
    //WaterBalls
    for (i = waterBallsCount-1;i>=0; i--)
        if (waterBalls[i]->update())
        {
            waterBalls[i]->detachPhysics();
            if (i!=waterBallsCount-1)
            {
                LOWaterBall * buf = waterBalls[i];
                waterBalls[i] = waterBalls[waterBallsCount-1];
                waterBalls[waterBallsCount-1] = buf;
            }
            waterBallsCount--;
        } 
    
    //GroundBalls
    for (i = groundBallsCount-1;i>=0; i--)
        if (groundBalls[i]->update())
        {
            groundBalls[i]->detachPhysics();
            if (i!=groundBallsCount-1)
            {
                LOGroundBall * buf = groundBalls[i];
                groundBalls[i] = groundBalls[groundBallsCount-1];
                groundBalls[groundBallsCount-1] = buf;
            }
            groundBallsCount--;
        }
    //AngryBalls
    if (!physicsPaused)
    for (i = angryBallsCount-1;i>=0; i--)
        if (angryBalls[i]->update())
        {
            if (mapInfo.enableDropSnowflakeFromAngryball)
                mapInfo.createSnowflakeAtPoint(angryBalls[i]->getPosition());
            
            if (isSurvival)
            {
                if (selectedStepNum == 3)
                {
                    short count = fminf(12, maxVioletSnowflakesCount-violetSnowflakesCount);
                    printf("\n ==>create %d",count);
                    float deltaAngle = M_PI*2/count;
                    float angle = 0;
                    SunnyVector2D p = angryBalls[i]->getPosition();
                    SunnyVector2D v;
                    for (short i = 0;i<count;i++)
                    {
                        v = SunnyVector2D(sinf(angle), cosf(angle))/2;
                        angle += deltaAngle;
                        createVioletSnowflake(p,v);
                    }
                }
            }

            angryBalls[i]->detachPhysics();
            if (i!=angryBallsCount-1)
            {
                LOAngryBall * buf = angryBalls[i];
                angryBalls[i] = angryBalls[angryBallsCount-1];
                angryBalls[angryBallsCount-1] = buf;
            }
            angryBallsCount--;
        }
    //ElectricBalls
    for (i = electricBallsCount-1;i>=0; i--)
        if (electricBalls[i]->update())
        {
            electricBalls[i]->detachPhysics();
            if (i!=electricBallsCount-1)
            {
                LOElectricBall * buf = electricBalls[i];
                electricBalls[i] = electricBalls[electricBallsCount-1];
                electricBalls[electricBallsCount-1] = buf;
            }
            electricBallsCount--;
        }
    //AnomalyBalls
    for (i = anomalyBallsCount-1;i>=0; i--)
        if (anomalyBalls[i]->update())
        {
            anomalyBalls[i]->detachPhysics();
            if (i!=anomalyBallsCount-1)
            {
                LOAnomalyBall * buf = anomalyBalls[i];
                anomalyBalls[i] = anomalyBalls[anomalyBallsCount-1];
                anomalyBalls[anomalyBallsCount-1] = buf;
            }
            anomalyBallsCount--;
        }
    
    for (i = explosionsCount-1; i>=0; i--)
        if (explosions[i].update(deltaTime))
        {
            explosions[i] = explosions[explosionsCount-1];
            explosionsCount--;
        }
    
    if (!physicsPaused)
    {
        player->updatePosition();

        if (!player->crashed)
             LOTrace::sharedTrace()->addRandomPlayerTrace(SunnyVector2D(player->position.pos.x,player->position.pos.y),player->getRadius()/(cellSize/2));

        if (isMultiPlayer)
        {
            for (short i = 0;i<otherPlayersCount;i++)
            if (!otherPlayers[i].player->crashed)
            {
                otherPlayers[i].player->updatePosition();
                LOTrace::sharedTrace()->addRandomOtherPlayerTrace(SunnyVector2D(otherPlayers[i].player->position.pos.x,otherPlayers[i].player->position.pos.y),otherPlayers[i].player->getRadius()/(cellSize/2));
            }
        }
        
        scStep(deltaTime);
        
        player->setPosition(player->body->position);
        if (isMultiPlayer)
            for (short i = 0;i<otherPlayersCount;i++)
                otherPlayers[i].player->setPosition(otherPlayers[i].player->body->position);
        
        for (short  i = 0;i<angryBallsCount;i++)
            angryBalls[i]->updatePosition();
        updateSnowFromAngryBalls();
    }
    mapInfo.update();
    if (!physicsPaused)
        LOSmoke::sharedSmoke()->update();
    LOTrace::sharedTrace()->update();
    
    SunnyMatrix mat = getIdentityMatrix();
    float angle = -rotationAngle.x*maxMapRotationAngle/180.*M_PI*0.3;
    mat.front.x = mat.right.z = cosf(angle);
    mat.front.z = sinf(angle);
    mat.right.x =-mat.front.z;
    lightPosition = mat*customLightPosition;
    
    mat = getIdentityMatrix();
    angle = -rotationAngle.y*maxMapRotationAngle/180.*M_PI*0.3;
    mat.up.y = mat.right.z = cosf(angle);
    mat.up.z = sinf(angle);
    mat.right.y =-mat.up.z;
    lightPosition = mat*lightPosition;
    
    visibleWaterBallsCount = 0;
    if (isFastDevice)
    for (short i = 0;i<waterBallsCount;i++)
    {
        if (waterBalls[i]->status == LO_Active)
        {
            waterBalls[i]->setPositionForNum(visibleWaterBallsCount);
            visibleWaterBallsCount++;
        }
    }
}

float getAlphaFromTime(float time)
{
    time = customRespawnTime-time;
    if (time<customRespawnTime*0.3)
        return time/(customRespawnTime*0.3);
    
    if (time<customRespawnTime*0.4)
        return 1;
    
    if (time<customRespawnTime*0.7)
        return (time-0.4*customRespawnTime)/(customRespawnTime*0.3);
    
    if (time<customRespawnTime*0.9)
        return 1;
    
    return (customRespawnTime - time)/(0.1*customRespawnTime);
}

void LOMap::renderWarningTriangles(SunnyMatrix *MV)
{
    bindGameGlobal();
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, cellSize, 1, 1));
    SunnyMatrix mat;
    
    for (short i = 0 ;i<fireBallsCount;i++)
        if (fireBalls[i]->status == LO_Respawn)
        {
            mat = fireBalls[i]->position * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat.front.x));
            glUniform1f(uniformTSA_A, getAlphaFromTime(fireBalls[i]->respawnTime));//sinf(fireBalls[i]->respawnTime*4));
            glDrawArrays(GL_TRIANGLE_STRIP, LOWarnTriangles_VAO*4, 4);
        }
    for (short i = 0 ;i<waterBallsCount;i++)
        if (waterBalls[i]->status == LO_Respawn)
        {
            mat = waterBalls[i]->position * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat.front.x));
            glUniform1f(uniformTSA_A, getAlphaFromTime(waterBalls[i]->respawnTime));//sinf(fireBalls[i]->respawnTime*4));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+1)*4, 4);
        }
    for (short i = 0 ;i<groundBallsCount;i++)
        if (groundBalls[i]->status == LO_Respawn)
        {
            mat = groundBalls[i]->position * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat.front.x));
            glUniform1f(uniformTSA_A, getAlphaFromTime(groundBalls[i]->respawnTime));//sinf(fireBalls[i]->respawnTime*4));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+2)*4, 4);
        }
    for (short i = 0 ;i<electricBallsCount;i++)
        if (electricBalls[i]->status == LO_Respawn)
        {
            mat = electricBalls[i]->position * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat.front.x));
            glUniform1f(uniformTSA_A, getAlphaFromTime(electricBalls[i]->respawnTime));//sinf(fireBalls[i]->respawnTime*4));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+3)*4, 4);
        }
    for (short i = 0 ;i<anomalyBallsCount;i++)
        if (anomalyBalls[i]->status == LO_Respawn)
        {
            mat = anomalyBalls[i]->position * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat.front.x));
            glUniform1f(uniformTSA_A, getAlphaFromTime(anomalyBalls[i]->respawnTime));//sinf(fireBalls[i]->respawnTime*4));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+8)*4, 4);
        }
}

void LOMap::renderBallsShadows(SunnyMatrix *MV)
{
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1));

    if (groundBallsCount)
    {
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 0, 1, 1));
        for (short i = 0;i<groundBallsCount;i++)
            groundBalls[i]->renderShadow(MV);
    }
    if (waterBallsCount)
    {
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(MV->front.x));
        for (short i = 0;i<waterBallsCount;i++)
            waterBalls[i]->renderShadow();
    }
}
void LOMap::renderPreview()
{
    float blend = 1;
    if (mapActivationStarted<=1)
    {
    }
    else
    {
        blend = (1.5*mapActivationFramesCount-mapActivationFrameNum)/(0.5*mapActivationFramesCount);
    }
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    SunnyMatrix mat = getIdentityMatrix();
    
//    //sunny
    mat = getTranslateMatrix(SunnyVector3D((1030+69.*2)/1920.*mapSize.x,(1280-(67*2-56))/1280.*mapSize.y,-10));
//    mat = getRotationZMatrix(-0.0689595502007)*mat;
    mat = getScaleMatrix(SunnyVector3D(334./960,334./960,1))*mat;
//    //sunny...
    mat = getTranslateMatrix(SunnyVector3D(0,-mapSize.y,0))*mat;
    
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, blend*previewAlpha);
    
    mapInfo.renderPreview(&mat,blend*previewAlpha);
}

void LOMap::renderAngryBallsShadows(SunnyMatrix *MV)
{
    if (!angryBallsCount) return;
    
    SunnyMatrix mat = getIdentityMatrix();
    float r = cellSize/4*1.2;
    mat.up.y = -2*lightPosition.y*(r/lightPosition.z);
    mat.up.x = -2*lightPosition.x*(r/lightPosition.z);
    mat.front = mat.up ^ mat.right;
    mat.front.normalize();
    mat.front *= r/cellSize*4;
    
    SunnyMatrix m;
    for (short i = 0;i<angryBallsCount;i++)
    {
        mat.pos.y = angryBalls[i]->position.pos.y + mat.up.y/4;
        mat.pos.x = angryBalls[i]->position.pos.x + mat.up.x/4;
            
        m = mat**MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        r = 1.0 - angryBalls[i]->respawnTime/4;
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 0, r, r));
        glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,r));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO)*4, 4);
    }
}

void LOMap::renderAngryBallsLight(SunnyMatrix *MV)
{
    for (short  i = 0;i<angryBallsCount;i++)
        angryBalls[i]->renderLight(MV);
}

void LOMap::renderWaterCharges(SunnyMatrix *MV)
{
    for (short i = 0; i<waterBallsCount; i++)
        if (waterBalls[i]->skipFrame)
            waterBalls[i]->renderCharge(MV);
}

void LOMap::renderShadows(SunnyMatrix *MV)
{
    if (!isFastDevice) return;
    
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,2,2));
    glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));

    SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(-0.5,-0.5,0)) * *MV;
    SunnyMatrix m;
    for (short i = 0;i<fireBallsCount;i++)
        if (fireBalls[i]->status == LO_Active)
        {
            m = fireBalls[i]->position * m1;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            glDrawArrays(GL_TRIANGLE_STRIP,(LOSmoke_VAO+13)*4,4);
        }
    
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,1,1));
    for (short i =0; i<explosionsCount; i++)
        explosions[i].renderShadow(MV);
    //glUniform1f(uniformTSA_A, 1);
}

void LOMap::renderFinish()
{
    if (isVAOSupported)
        glBindVertexArray(finishVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, finishVBO);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    short snows = ((int)(20./mapInfo.snowflakesCount+0.99))*mapInfo.snowflakesCount;
    snows = snows*mapInfo.collectedSnowflakes/mapInfo.snowflakesCount;
    glDrawArrays(GL_TRIANGLE_FAN, 0, snows+2);
}

void LOMap::prepareFinish()
{
    if (mapInfo.snowflakesCount==0) return;
    
    if (finishVBO)
    {
        glDeleteBuffers(1, &finishVBO);
        glDeleteVertexArrays(1, &finishVAO);
    }
    
    const float z = 1.0/640*mapSize.y/2;
    float physRadius = 74*z;
    
    short snows = ((int)(20./mapInfo.snowflakesCount+0.99))*mapInfo.snowflakesCount;
    
    float angle = 2*M_PI/snows;
    short pCount = 2 + snows;
    SunnyVector4D *points = new SunnyVector4D[pCount];
    points[0] = SunnyVector4D(0,0,(830+37)/2048.,(270+37)/2048.);
    
    float nowAngle = M_PI_2;
    for (short i = 0 ;i<=snows;i++)
    {
        points[1+i] = SunnyVector4D(physRadius*cosf(nowAngle),physRadius*sinf(nowAngle),points[0].z + 37./2048*cosf(nowAngle),points[0].w - 37./2048*sinf(nowAngle));
        nowAngle -= angle;
    }
    
    if (isVAOSupported)
    {
        glGenVertexArrays(1, &finishVAO);
        glBindVertexArray(finishVAO);
    }
    glGenBuffers(1, &finishVBO);
    glBindBuffer(GL_ARRAY_BUFFER, finishVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SunnyVector4D)*pCount, &points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete []points;
//    addPointsToArray(SunnyVector4D(830,270,74,74), LOShadows_VAO+8, points,false);
}

void LOMap::renderDeathAnimation()
{
    if (!animateDeath) return;
    
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    deathAnimationFrame++;
    float scale = 1+deathAnimationFrame/200.;
    if (deathAnimationFrame<3)
    {
        glUniform1f(uniformTSA_A, deathAnimationFrame/3.);
        scale *= (4-deathAnimationFrame);
    } else
    {
        if (deathAnimationFrame>=33)
            glUniform1f(uniformTSA_A, (43-deathAnimationFrame)*0.1);
        else
            glUniform1f(uniformTSA_A, 1);
    }
    
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y/2,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
    SunnyDrawArrays(LODeath_VAO);
    
    if (deathAnimationFrame>=43)
        animateDeath = false;
}

void LOMap::renderOverlay()
{
    sunnyUseShader(globalShaders[LOS_OverlayTSA]);
    if (!mainMenu->isTutorial() && !menuMode)
    {
        SunnyMatrix m;
        
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
        mapInfo.renderOverlay(&lastMVMatrix);
        
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,2,2));
        for (short i = 0;i<waterBallsCount;i++)
            if (waterBalls[i]->status == LO_Respawn)
                waterBalls[i]->renderOverlay(&lastMVMatrix);

        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1,1));
        for (short i = 0;i<waterBallsCount;i++)
            if (waterBalls[i]->status == LO_Active)
                waterBalls[i]->renderOverlay(&lastMVMatrix);

        for (short i = 0;i<fireBallsCount;i++)
            if (fireBalls[i]->status == LO_Respawn)
            {
                SunnyMatrix m = fireBalls[i]->position * lastMVMatrix;
                glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(LOOverlayFireball_VAO);
            } else
            {
                 fireBalls[i]->renderOverlay(&lastMVMatrix);
            }
        
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1.3,1.3));
        for (short i = 0;i<electricBallsCount;i++)
            if (electricBalls[i]->status == LO_Respawn)
            {
                SunnyMatrix m = electricBalls[i]->position * lastMVMatrix;
                glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(LOOverlayElectricity_VAO);
            } else
            {
                electricBalls[i]->renderOverlay(&lastMVMatrix);
            }
        
        if (!menuMode)
        {
            if (!player->crashed)
                player->renderOverlays(&lastMVMatrix);
            if (isMultiPlayer)
            {
                for (short i = 0;i<otherPlayersCount;i++)
                    if (!otherPlayers[i].player->crashed)
                        otherPlayers[i].player->renderOverlays(&lastMVMatrix);
            }
        }
        
        for (short i = 0;i<explosionsCount;i++)
            explosions[i].renderOverlay(&lastMVMatrix);
        
        for (short i = 0;i<goldCoinsCount;i++)
            goldCoins[i]->renderOverlay(&lastMVMatrix);
        
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(lastMVMatrix.front.x));
        for (short i = 0;i<violetSnowflakesCount;i++)
            violetSnowflakes[i]->renderOverlay(&lastMVMatrix);
    }
    
    mainMenu->renderOverlay();
}

bool LOMap::dropPuzzlePart(SunnyVector2D p, short stepNumber, bool justClose)
{
    short number = LOScore::addPuzzlePart(stepNumber);
    if (number<0) return false;
    
    LOMenu::getSharedMenu()->showPuzzleTutorial(p, number, justClose);
    gamePaused = true;
    losPlayLevelComplete();
    
    return true;
}

bool LOMap::tryToDropPuzzlePart(SunnyVector2D p)
{
    if (mapInfo.collectedSnowflakes != mapInfo.puzzleDropSnowflake) return false;
    if (!LOScore::canDropPuzzle(selectedStepNum,selectedLevelNum)) return false;
    
    if (dropPuzzlePart(p, selectedStepNum,false))
    {
        return true;
    } else
        return false;
}

bool LOMap::tryToDropGoldCoin(SunnyVector2D p)
{
    if (goldCoinsCount>=maxGoldCoinsCount) return false;
    
    if (sRandomf()>=0.01) return false;
    
    losPlayGoldCoinSound();
    goldCoins[goldCoinsCount]->initWithPosition(p);
    goldCoinsCount++;
    
    return true;
}

void LOMap::destroyVioletSnowflake(LOVioletSnowflake *snowflake)
{
    for (short i = 0;i<violetSnowflakesCount;i++)
        if (violetSnowflakes[i] == snowflake)
        {
            if (i!=violetSnowflakesCount-1)
            {
                violetSnowflakes[i] = violetSnowflakes[violetSnowflakesCount-1];
                violetSnowflakes[violetSnowflakesCount-1] = snowflake;
            }
            violetSnowflakesCount--;
            break;
        }
}

void LOMap::createRandomVioletSnowflake(float velocityScale)
{
    short bound = -1;
    SunnyVector2D pos = randPosOnMapBounds(bound);
    SunnyVector2D pos2;
    short ran = sRandomi()%targetCount;
    if (ran == 0 && !isMultiPlayer)
    {
        pos2 = SunnyVector2D(player->position.pos.x,player->position.pos.y);
    } else
    {
        pos2 = randPosOnMapBounds(bound);
    }
    
    pos2 = pos2 - pos;
    pos2.normalize();
    pos2 *= velocityScale;
    createVioletSnowflake(pos, pos2);
}

void LOMap::createVioletSnowflake(SunnyVector2D p, SunnyVector2D v)
{
    if (violetSnowflakesCount>=maxVioletSnowflakesCount) return;
    
    violetSnowflakes[violetSnowflakesCount]->initWithPosition(p,v* defaultVelocity);
    violetSnowflakesCount++;
}

void LOMap::renderGoldCoinsShadows(SunnyMatrix *MV)
{
    if (!goldCoinsCount) return;
    
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    for (short i = 0;i<goldCoinsCount;i++)
    {
        goldCoins[i]->renderShadow(MV);
    }
}

void LOMap::renderGoldCoins(SunnyMatrix *MV)
{
    if (!goldCoinsCount) return;

    for (short i = 0;i<goldCoinsCount;i++)
        goldCoins[i]->render(MV);
}

void LOMap::renderVioletSnowflakes(SunnyMatrix *MV)
{
    if (!violetSnowflakesCount) return;
    
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    SHTextureBase::bindTexture(objectsTexture);
    glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, 1, 1));
    for (short i = 0;i<violetSnowflakesCount;i++)
        violetSnowflakes[i]->render(MV);
}

void LOMap::renderFinish(SunnyMatrix *MV)
{
    if (mapInfo.flagPosition.x==0 && mapInfo.flagPosition.y==0) return;
    
    sunnyUseShader(globalShaders[LOS_TexturedC]);
    glDisable(GL_DEPTH_TEST);
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(mapInfo.flagPosition.x, mapInfo.flagPosition.y, 0)) **MV;
    glUniformMatrix4fv(globalModelview[LOS_TexturedC], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformA_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+7)*4, 4);
    
    if (!mapInfo.levelComplete && mapInfo.snowflakeCondition == 1)
    {
        renderFinish();
        bindGameGlobal();
    }
    
    m = getRotationZMatrix(playingTime*20/180*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedC], 1, GL_FALSE, &(m.front.x));
    if (!mapInfo.levelComplete)
    {
        if (mapInfo.snowflakeCondition != 1)
        {
            glUniform4fv(uniformA_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, sin(playingTime*2)*0.5 + 0.5));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+4)*4, 4);
        }
    }
    else
    {
        glUniform4fv(uniformA_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+5)*4, 4);
    }
    
    if (mapInfo.startBlinkTime>=0)
    {
        double t = getCurrentTime() - mapInfo.startBlinkTime;
        int c = t/0.5;
        if (c<4)
        {
            t -= c*0.5;
            glUniform4fv(uniformA_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,3*t));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOWarnTriangles_VAO+6)*4, 4);
        } else
            mapInfo.startBlinkTime = -1;
    }
}

void LOMap::renderES2()
{
    SunnyMatrix m;
    SunnyMatrix MV = getIdentityMatrix();
    MV.pos = SunnyVector4D(-mapSize.x/2, -mapSize.y/2, 0,1);
    
    MV = MV*getRotationXMatrix(-rotationAngle.y*20/180*M_PI);// * MV;
    MV = MV*getRotationYMatrix(-rotationAngle.x*20/180*M_PI);// * MV;
    SunnyMatrix tr = getIdentityMatrix();
    if (menuMode)
        tr.front.x = tr.up.y = tr.right.z = mapScale*1.5;
    else
        tr.front.x = tr.up.y = tr.right.z = mapScale;
    MV = MV*tr;
    
    tr = getIdentityMatrix();
    tr.pos = SunnyVector4D(mapSize.x/2, mapSize.y/2, -300, 1);
    if (amplitude>0.1)
    {
        earthShakeFame++;
        if (earthShakeFame%2 == 0)
            amplitude /= 1.3;
        
        tr.pos.x += amplitude*sinf(earthShakeFame*M_PI_2);
        rightSide = !rightSide;
    }
    MV = MV*tr;
    
if (!LOMenu::getSharedMenu()->isLevelCompletedShown())
{
    bindGameGlobal();
    //sunnyUseShader(globalShaders[LOS_Textured]);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    SHTextureBase::bindTexture(objectsTexture);
    
    SunnyVector3D lightPosition = SunnyVector3D(1,1,1.5).normalize();
    float color = 0.3 + 0.7*fmaxf(lightPosition.z,0);
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(MV.front.x));
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,1,1));
    glUniform4fv(uniformTSC_C,1,SunnyVector4D(color*globalColor.x,color*1.01*globalColor.y,color*1.1*globalColor.z, 1));
    SunnyDrawArrays(LOBackground_VAO);
    glEnable(GL_BLEND);
    
//    bindGameGlobal();
if (!menuMode)
{
    mapInfo.render(&MV);
    
    renderVioletSnowflakes(&MV);
    
    //снег, под шаром
    if (explosionsCount || snowFromAngryBallsCount)
    {
        LOExplosion::prepareForRender();
        renderSnowFromAngryBallsUnderPlayer(&MV);
        for (short i =0; i<explosionsCount; i++)
            explosions[i].renderUnderPlayer(&MV);
    }
    
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
    sunnyUseShader(globalShaders[LOS_Textured3DSnow]);
    player->setPlayersColor();
    if (menuMode)
    {
        
    } else
        if (player->crashed)
        {
            snowBallCrashObj->update();
            snowBallCrashObj->render(&MV);
        } else
            player->render(&MV);
    
//    LOTrees::render(&MV);

    if (isMultiPlayer)
    {
        for (short i = 0;i<otherPlayersCount;i++)
        {
            otherPlayers[i].player->setPlayersColor();
            if (otherPlayers[i].player->crashed)
            {
                otherPlayers[i].snowBallCrashObj->update();
                otherPlayers[i].snowBallCrashObj->render(&MV);
            } else
            {
                otherPlayers[i].player->render(&MV);
            }
        }
    }
    glEnable(GL_BLEND);
    renderGoldCoins(&MV);
    glDisable(GL_BLEND);
    SHTextureBase::bindTexture(objectsTexture);
    
    sunnyUseShader(globalShaders[LOS_Textured3D]);
    glUniform4fv(uniform3D_C, 1, SunnyVector4D(globalColor.x, globalColor.y, globalColor.z, 1));

    if (angryBallsCount)
    {
        angryBallObj->prepareForRender(isVAOSupported);
        for (short  i = 0;i<angryBallsCount;i++)
            angryBalls[i]->render(&MV);
    }
    
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    mapInfo.renderAfterPlayer(&MV);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    sunnyUseShader(globalShaders[LOS_Textured3D]);
    glUniform4fv(uniform3D_C, 1, SunnyVector4D(1, 1, 1, 1.0));
    SunnyMatrix MV1 = getTranslateMatrix(SunnyVector3D(0,0,1)) * MV;
    groundBallObj->prepareForRender();
    for (short i = 0; i<groundBallsCount; i++)
        groundBalls[i]->render(&MV1);
    LOFallingBall::render(&MV1);
    LOCrashBalls::sharedCrashBalls()->render(&MV1);
    //glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    glUniform1f(uniformTSA_A, 1);
    if (!isFastDevice)
    {
        glEnable(GL_BLEND);
        for (short i = 0;i<waterBallsCount;i++)
            waterBalls[i]->render(&MV);
        renderWaterCharges(&MV);
    } else
        if (visibleWaterBallsCount)
        {
            LOWaterBall::render(&MV,visibleWaterBallsCount);
            renderWaterCharges(&MV);
        }
        else
            glEnable(GL_BLEND);
    if (groundBallsCount)
    {
        if (isVAOSupported)
            glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        sunnyUseShader(globalShaders[LOS_GroundParticles]);
        SHTextureBase::bindTexture(particleTextures[LOP_Rock01]);
        glUniformMatrix4fv(globalModelview[LOS_GroundParticles], 1, GL_FALSE, &(MV.front.x));
        glUniform4fv(uniformGP_color, 1, SunnyVector4D(65./255, 50./255, 60./255, 1.0));
        for (short i = 0; i<groundBallsCount; i++) {
            groundBalls[i]->renderParts();
        }
        SHTextureBase::bindTexture(objectsTexture);
    }

    renderShadows(&MV);
    if (snowFromAngryBallsCount)
    {
        LOExplosion::prepareForRender();
        renderSnowFromAngryBalls(&MV);
    }

    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    if (anomalyBallsCount)
    {
        //glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1));
        for (short i = 0;i<anomalyBallsCount;i++)
            anomalyBalls[i]->render(&MV);
    }
    
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(MV.front.x));
    LOSmoke::sharedSmoke()->render();
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,1,1));
    //glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x, globalColor.y, globalColor.z, 1));
    glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1, 1, 1, 1));
    for (short i = 0; i<fireBallsCount; i++)
        fireBalls[i]->renderLight(&MV);
    glDisable(GL_DEPTH_TEST);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    for (short i =0; i<fireBallsCount; i++)
        fireBalls[i]->render(&MV);
    mapInfo.renderAfterBalls(&MV);
    
    SHTextureBase::bindTexture(objectsTexture);
    if (explosionsCount)
    {
        sunnyUseShader(globalShaders[LOS_Textured3DA]);
        LOExplosion::prepareForRender();
        for (short i =0; i<explosionsCount; i++)
            explosions[i].render(&MV);
    }
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, 1);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    if (electricBallsCount || electroExplosionsCount)
    {
        for (short i = 0;i<electricBallsCount;i++)
            electricBalls[i]->render(&MV);
        renderElectroExplosions(&MV);
    }
}//!menu
    
    
    {
        //side fades
        SHTextureBase::bindTexture(objectsTexture);
        glDisable(GL_DEPTH_TEST);
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SunnyMatrix m = sunnyIdentityMatrix;
        m.pos.z = -2;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, 1);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+8)*4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+9)*4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+10)*4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+11)*4, 4);
        
        //fade
        m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y/2,-1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, 1);
        
        SHTextureBase::bindTexture(menuTexture);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
        SunnyDrawArrays(LODeath_VAO+1);
        
        SHTextureBase::bindTexture(objectsTexture);
        //corners
        //glDisable(GL_DEPTH_TEST);
        
        if (LOGroundGenerator::needShadowsOnSides())//for iphone5
        {
//            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
//            SunnyDrawArrays(LODeath_VAO+2);
//            
//            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,-1,1));
//            SunnyDrawArrays(LODeath_VAO+2);
        }

        /*sunnyUseShader(globalShaders[LOS_Textured]);//corners
        float sc = 1.25;
        m = getScaleMatrix(SunnyVector3D( sc,sc,1)) * getTranslateMatrix(SunnyVector3D(46./960.*mapSize.x,605./640.*mapSize.y,1))*MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured], 1, GL_FALSE, &(m .front.x));
        SunnyDrawArrays(LOButton_VAO+2);
        m = getScaleMatrix(SunnyVector3D(-sc,sc,1)) * getTranslateMatrix(SunnyVector3D(914./960.*mapSize.x,605./640.*mapSize.y,1))*MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured], 1, GL_FALSE, &(m .front.x));
        SunnyDrawArrays(LOButton_VAO+2);
        m = getScaleMatrix(SunnyVector3D(sc,-sc,1)) * getTranslateMatrix(SunnyVector3D(46./960.*mapSize.x,35./640.*mapSize.y,1))*MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured], 1, GL_FALSE, &(m .front.x));
        SunnyDrawArrays(LOButton_VAO+2);
        m = getScaleMatrix(SunnyVector3D(-sc,-sc,1)) * getTranslateMatrix(SunnyVector3D(914./960.*mapSize.x,35./640.*mapSize.y,1))*MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured], 1, GL_FALSE, &(m .front.x));
        SunnyDrawArrays(LOButton_VAO+2);*/
        
        if (menuMode && !LOMainWindow::getSharedWindow()->isVisible)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniform1f(uniformTSA_A, 1);
            //Borders
            SHTextureBase::bindTexture(menuTexture);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 2, 2));
            m = getRotationZMatrix(93.5/180*M_PI)* getTranslateMatrix(SunnyVector3D(mapSize.x*0.3,mapSize.y*0.9,-1));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOLSTopBorderVAO);
            
            m = getRotationZMatrix(93.5/180*M_PI)* getTranslateMatrix(SunnyVector3D(mapSize.x*0.8,mapSize.y*0.1,-1));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOLSTopBorderVAO+1);
            //Borders...
        }
        
        renderDeathAnimation();
    }
    renderWarningTriangles(&MV);
}
    if (drawPreview)
        activeMap->renderPreview();
    renderHUD();
//    renderWarningTriangles(&MV);
    
    mapInfo.renderFallingSnow(&MV);
    
    drawShortMessage();
    
    bindGameGlobal();
    
    LOTrees::render(&MV);
    glDisable(GL_BLEND);
    
    lastMVMatrix = MV;
}

void LOMap::drawShortMessage()
{
    if (!shortMessage) return;
    
    SunnyMatrix m = getScaleMatrix(shortMessage->scale*0.5) * getTranslateMatrix(SunnyVector3D(shortMessage->position.x,shortMessage->position.y,-5));
    LOFont::writeText(&m, LOTA_Center, true, shortMessage->message, yellowInnerColor(shortMessage->alpha), yellowOuterColor(shortMessage->alpha));
}

void LOMap::addElectroExplosion(SunnyVector2D p)
{
    if (electroExplosionsCount>=maxElectroExplosions) return;
    
    electroExplosions[electroExplosionsCount].pos = p;
    electroExplosions[electroExplosionsCount].frames = 3;
    electroExplosionsCount++;
}

void LOMap::renderElectroExplosions(SunnyMatrix *MV)
{
    SunnyMatrix m;
    for (short i = 0;i<electroExplosionsCount;i++)
    {
        m = getTranslateMatrix(SunnyVector3D(electroExplosions[i].pos.x, electroExplosions[i].pos.y,0)) **MV;
        m = getRotationZMatrix(sRandomf()*2*M_PI)*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+53)*4, 4);
        electroExplosions[i].frames--;
        if (electroExplosions[i].frames<=0)
        {
            electroExplosions[i] = electroExplosions[electroExplosionsCount-1];
            electroExplosionsCount--;
            i--;
            continue;
        }
    }
}

void LOMap::resetMap()
{
    survivalSnowflakesCollected = survivalGoldCoinsCollected = 0;
    if (shortMessage)
    {
        delete shortMessage;
        shortMessage = 0;
    }
    if (playingMusic != mapInfo.musicSettings.number)
    {
        playingMusic = mapInfo.musicSettings.number;
        //change the music
        char name[256];
        strcpy(name, "Base/Music/SoundTrack0");
        short len = strlen(name);
        name[len] = '0' + playingMusic;
        name[len+1] = '\0';
        strcat(name, ".mp3");
        loCallbackSetMusic(name,mapInfo.musicSettings.startTime);
    } else
    if (mapInfo.musicSettings.forceChangeTimer || mapInfo.musicSettings.reset)
    {
        loCallbackSetMusicPosition(mapInfo.musicSettings.startTime);
    }
    LOEvents::OnMapReset();
    
    applyGlobalColor(mapInfo.dayTime);
    
    realPlayingTime = playingTime = 0;
    lastReportedPlayingTime = 0;
    loCalibrate(false);
    loJoystic.touch = NULL;
    loAccelerometer();
    gameOvered = false;
    gameOveredMultiPlayer = false;

    finishBlink = -1;
    lastMinuteNum = 0;
    losResetSounds();
    snowFromAngryBallsCount = 0;
    electroExplosionsCount = 0;
    timeCondition = -1;
    for (short i = 0;i<fireBallsCount; i++)                                                                                             
        fireBalls[i]->detachPhysics();
    
    for (short i = 0;i<waterBallsCount; i++)
        waterBalls[i]->detachPhysics();

    for (short i = 0;i<groundBallsCount; i++)
        groundBalls[i]->detachPhysics();
    
    for (short i = 0;i<angryBallsCount; i++)
        angryBalls[i]->detachPhysics();
    
    for (short i = 0;i<electricBallsCount; i++)
    {
        electricBalls[i]->body->detachAllCollisions(false);
        electricBalls[i]->detachPhysics();
    }
    
    for (short i = 0;i<anomalyBallsCount; i++)
        anomalyBalls[i]->detachPhysics();
    
    fireBallsCount = 0;
    waterBallsCount = 0;
    visibleWaterBallsCount = 0;
    groundBallsCount = 0;
    angryBallsCount = 0;
    electricBallsCount = 0;
    anomalyBallsCount = 0;
    explosionsCount = 0;
    velocityScale = 1.0;
    velocityScaleTime = 0.0;
    timeVelocityScale = 0.8;
    minuteNumber = 0;
    for (short i = 0;i<goldCoinsCount;i++)
        goldCoins[i]->collectCoin(false);
    for (short i = 0;i<violetSnowflakesCount;i++)
        violetSnowflakes[i]->collectSnowflake(false);
    
    goldCoinsCount = 0;
    violetSnowflakesCount = 0;

    mapInfo.reset();
    rotationAngle = SunnyVector2D(0,0);
    if (!isMultiPlayer)
        player->setPosition(mapInfo.respPoint);
    player->reset();
    
    if (activeSurvivalMap)
        activeSurvivalMap->reset();
    
    LOSmoke::sharedSmoke()->reset();
    LOTrace::sharedTrace()->reset();
    LOCrashBalls::sharedCrashBalls()->reset();
    LOFallingBall::reset();
    
    mapInfo.calcBigSnowAndMakeShadows();
    
    if (isSurvival)
    {
        previousSurvivalRecord = LOScore::getSurvivalRecord(selectedStepNum);
        if (previousSurvivalRecord<3)
            previousSurvivalRecord = 5*60;
    }
}

void LOMap::load(const char* fileName)
{
    if (activeSurvivalMap)
    {
        delete activeSurvivalMap;
        activeSurvivalMap = 0;
    }
    
    //isSurvival = false;
    mapInfo.load(fileName);
    resetMap();
    
    prepareFinish();
    if (isVerySlowDevice)
        checkObjects();
}

void LOMap::removeMap()
{
    const char * fileName = getHomePath("lastmap.loset");
    remove(fileName);
//    printf("\n map was removed : %d",code);
}

bool LOMap::loadMap()
{
    const char * fileName = getHomePath("lastmap.loset");
    
    FILE * stream = fopen(fileName,"rb");
    if (!stream)
    {
        printf("\n file not found : %s",fileName);
        return false;
    }
    int version;
    fread(&version, sizeof(version), 1, stream);
    //MapType
    fread(&isSurvival, sizeof(isSurvival), 1, stream);
    if (!isSurvival)
    {
        fclose(stream);
        return false;
    }
    
    fread(&selectedStepNum, sizeof(selectedStepNum), 1, stream);
    fread(&selectedLevelNum, sizeof(selectedLevelNum), 1, stream);
    
    LOMenu::setCurrentPage(selectedStepNum);

    if (isSurvival)
    {
        loLoadSurvivalMap(selectedStepNum,true);
        activeSurvivalMap->loadMap(stream);
    } else
    {
        loLoadMap(selectedStepNum*loLevelsPerStep + selectedLevelNum,true);
    }
    
    //player
    SunnyVector2D p;
    fread(&p, sizeof(p), 1, stream);
    player->setPosition(p);
    //Abilities
    for (short i = 0;i<LA_Count-1;i++)
    {
        bool q;
        fread(&q, sizeof(q), 1, stream);
        if (q)
            player->activateAbility((LevelAbility)i,true);
    }
    fread(&player->effectShield,sizeof(player->effectShield),1,stream);
    SunnyButton ** survivalButtons = LOMenu::getSharedMenu()->getSurvivalButtons();
    survivalButtons[LOSB_SpellDoubler]->active = player->isAbilityActive(LA_Doubler);
    survivalButtons[LOSB_SpellMagnet]->active = player->isAbilityActive(LA_Magnet);
    survivalButtons[LOSB_SpellTime]->active = player->isAbilityActive(LA_Time);

    //ressurection
    fread(&player->ressurectionsCount, sizeof(player->ressurectionsCount), 1, stream);
    //time
    fread(&playingTime, sizeof(playingTime), 1, stream);
    fread(&realPlayingTime, sizeof(realPlayingTime), 1, stream);
    //EnemyBalls
    short count;
    fread(&count, sizeof(count), 1, stream);
    for (short i = 0;i<count;i++)
    {
        SunnyVector2D p,v;
        fread(&p, sizeof(p), 1, stream);
        fread(&v, sizeof(v), 1, stream);
        createFireBall(p, v/defaultVelocity);
    }
    fread(&count, sizeof(count), 1, stream);
    for (short i = 0;i<count;i++)
    {
        SunnyVector2D p,v;
        fread(&p, sizeof(p), 1, stream);
        fread(&v, sizeof(v), 1, stream);
        createGroundBall(p, v/defaultVelocity);
    }
    fread(&count, sizeof(count), 1, stream);
    for (short i = 0;i<count;i++)
    {
        SunnyVector2D p,v;
        fread(&p, sizeof(p), 1, stream);
        fread(&v, sizeof(v), 1, stream);
        createWaterBall(p, v/defaultVelocity);
    }
    fread(&count, sizeof(count), 1, stream);
    for (short i = 0;i<count;i++)
    {
        SunnyVector2D p,v;
        fread(&p, sizeof(p), 1, stream);
        fread(&v, sizeof(v), 1, stream);
        createElectricBall(p, v/defaultVelocity);
    }
    fread(&count, sizeof(count), 1, stream);
    for (short i = 0;i<count;i++)
    {
        SunnyVector2D p,v;
        fread(&p, sizeof(p), 1, stream);
        fread(&v, sizeof(v), 1, stream);
        createAnomalyBall(p, v/defaultVelocity);
    }
    if (selectedStepNum>=3)
    {
        fread(&count, sizeof(count), 1, stream);
        for (short i = 0;i<count;i++)
        {
            SunnyVector2D p;
            fread(&p, sizeof(p), 1, stream);
            createAngryBall(p);
        }
    }
    
    fclose(stream);
    
    removeMap();
    LOMenu::getSharedMenu()->showMainMenu(false);
    loForcePause();
    return true;
}

void LOMap::saveMap()
{
    const char * fileName = getHomePath("lastmap.loset");
    
    FILE * stream = fopen(fileName,"wb");
    if (!stream)
    {
        printf("\n file not found : %s",fileName);
        return;
    }
    fwrite(&saveMapVersion, sizeof(saveMapVersion), 1, stream);
    //MapType
    fwrite(&isSurvival, sizeof(isSurvival), 1, stream);
    fwrite(&selectedStepNum, sizeof(selectedStepNum), 1, stream);
    fwrite(&selectedLevelNum, sizeof(selectedLevelNum), 1, stream);
    
    if (isSurvival)
    {
        activeSurvivalMap->saveMap(stream);
    } else
    {
        
    }
    
    //player
    SunnyVector2D p = player->getPosition();
    fwrite(&p, sizeof(p), 1, stream);
    //Abilities
    for (short i = 0;i<LA_Count-1;i++)
    {
        bool q = player->isAbilityActive((LevelAbility)i);
        fwrite(&q, sizeof(q), 1, stream);
    }
    fwrite(&player->effectShield,sizeof(player->effectShield),1,stream);
    //ressurection
    fwrite(&player->ressurectionsCount, sizeof(player->ressurectionsCount), 1, stream);
    //time
    fwrite(&playingTime, sizeof(playingTime), 1, stream);
    fwrite(&realPlayingTime, sizeof(realPlayingTime), 1, stream);
    //EnemyBalls
    fwrite(&fireBallsCount, sizeof(fireBallsCount), 1, stream);
    for (short i = 0;i<fireBallsCount;i++)
    {
        SunnyVector2D p = fireBalls[i]->getPosition();
        SunnyVector2D v = fireBalls[i]->getVelocity();
        fwrite(&p, sizeof(p), 1, stream);
        fwrite(&v, sizeof(v), 1, stream);
    }
    fwrite(&groundBallsCount, sizeof(groundBallsCount), 1, stream);
    for (short i = 0;i<groundBallsCount;i++)
    {
        SunnyVector2D p = groundBalls[i]->getPosition();
        SunnyVector2D v = groundBalls[i]->getVelocity();
        fwrite(&p, sizeof(p), 1, stream);
        fwrite(&v, sizeof(v), 1, stream);
    }
    fwrite(&waterBallsCount, sizeof(waterBallsCount), 1, stream);
    for (short i = 0;i<waterBallsCount;i++)
    {
        SunnyVector2D p = waterBalls[i]->getPosition();
        SunnyVector2D v = waterBalls[i]->getVelocity();
        fwrite(&p, sizeof(p), 1, stream);
        fwrite(&v, sizeof(v), 1, stream);
    }
    fwrite(&electricBallsCount, sizeof(electricBallsCount), 1, stream);
    for (short i = 0;i<electricBallsCount;i++)
    {
        SunnyVector2D p = electricBalls[i]->getPosition();
        SunnyVector2D v = electricBalls[i]->getVelocity();
        fwrite(&p, sizeof(p), 1, stream);
        fwrite(&v, sizeof(v), 1, stream);
    }
    fwrite(&anomalyBallsCount, sizeof(anomalyBallsCount), 1, stream);
    for (short i = 0;i<anomalyBallsCount;i++)
    {
        SunnyVector2D p = anomalyBalls[i]->getPosition();
        SunnyVector2D v = anomalyBalls[i]->getVelocity();
        fwrite(&p, sizeof(p), 1, stream);
        fwrite(&v, sizeof(v), 1, stream);
    }
    fwrite(&angryBallsCount, sizeof(angryBallsCount), 1, stream);
    for (short i = 0;i<angryBallsCount;i++)
    {
        SunnyVector2D p = angryBalls[i]->getPosition();
        fwrite(&p, sizeof(p), 1, stream);
    }
    
    fclose(stream);
}

LOTask ** LOMap::loadActiveTasks()
{
    LOScore::loadActiveTasks(selectedStepNum, activeTasks);
    return activeTasks;
}

LOSurvivalMap * LOMap::createSurvivalMap()
{
    if (selectedStepNum==0)
        activeSurvivalMap = new LOSurvivalMap01;
    else
        if (selectedStepNum==1)
            activeSurvivalMap = new LOSurvivalMap02;
        else
            if (selectedStepNum==2)
                activeSurvivalMap = new LOSurvivalMap03;
            else
                activeSurvivalMap = new LOSurvivalMap04;
    
    return activeSurvivalMap;
}

void LOMap::prepareSurvivalMap()
{
    //isSurvival = true;
    
    if (activeSurvivalMap)
        delete activeSurvivalMap;

    createSurvivalMap();
    
    if (isVerySlowDevice)
        checkObjects();
}

void LOMap::usePreviewInsteadOfLoading()
{
    mapInfo.usePreviewInsteadOfLoading();
}

void LOMap::loadPreview(const char *fileName)
{
    mapInfo.loadPreview(fileName);
}

SunnyVector2D LOMap::movePlayer(SunnyVector2D oldPosition, SunnyVector2D velocity)
{
    return mapInfo.movePlayer(oldPosition, velocity);
}

void LOMap::setRotationAngle(SunnyVector2D v)
{
    float koeff = 0.8;
//    if (controlType!=LOC_Accelerometer)
//        koeff = 0;
    rotationAngle = rotationAngle*koeff + v*(1-koeff);
}

SunnyVector2D LOMap::getRotationAngle()
{
    return rotationAngle;
}

void LOMap::createGroundModel(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount],unsigned char * heightMap)
{
    groundModel->createNewGroundModel(smallSnow,heightMap);
}

bool LOMap::canMoveIceBlock(SunnyVector2D pos, short direction)
{
    return mapInfo.canMoveIceBlock(pos, direction);
}

void LOMap::minutePast()
{
    mapInfo.minutePast();
}

void LOMap::sendAngryBallsForMultiplayer()
{
    for (short i = 0;i<angryBallsCount;i++)
        if (angryBalls[i]->multiplayerTargetNum == myMultiPlayerNumber)
        {
            
        }
}

int LOMap::getCollectedSnowflakes()
{
    return mapInfo.collectedSnowflakes;
}

void LOMap::taskComplete(LOTask *task)
{
        LOMenu::getSharedMenu()->animateStar();
    task->complete = true;
    
    char achId[128];
    strcpy(achId, "allTasks");
    int len = (int)strlen(achId);
    achId[len] = '1'+selectedStepNum;
    achId[len+1] = '\0';
    UnnyNet::reportAchievement(achId, LOScore::getCompletedTasksCount(selectedStepNum)*100/LOScore::getTasksCount(selectedStepNum));
    
    short c = 0;
    for (short i = 0;i<activeTasksCount;i++)
        if (activeTasks[i] && activeTasks[i]->complete)
            c++;
    if (c==activeTasksCount)
    {
        UnnyNet::reportAchievement("3tasks", 100);
    }
}

void LOMap::checkObjects()
{
    if (mapInfo.iceBlocksCount)
    {
        if (!iceBlockModel)
        {
            iceBlockModel = new LOIceBlockModel;
            if (isVerySlowDevice)
                iceBlockModel->loadFromSobjFile(getPath("Base/Objects/IceCube_Low","sh3do"));
            else
                iceBlockModel->loadFromSobjFile(getPath("Base/Objects/IceCube","sh3do"));
            iceBlockModel->prepareForIceBlock();
            iceBlockModel->makeVBO(true,isVAOSupported,false);
        }
    } else
        if (iceBlockModel) 
        {
            delete iceBlockModel;
            iceBlockModel = 0;
        }
    
    //if (mapInfo.highShadowPointsCount)
    {
        if (!snowWallObj)
        {
            snowWallObj = new LOCrashModel;
            if (isVerySlowDevice)
                snowWallObj->loadFromSobjFile(getPath("Base/Objects/snowCrash_02_Low","sh3do"));
            else
                snowWallObj->loadFromSobjFile(getPath("Base/Objects/snowCrash_02","sh3do"));
            snowWallObj->prepareModels();
            snowWallObj->makeVBO(true,isVAOSupported,false);
        }
    }/* else
        if (snowWallObj)
        {
            delete snowWallObj;
            snowWallObj = 0;
        }*/
    
    if (mapInfo.electricFieldsCount)
    {
        if (!electricField)
        {
            electricField = new SunnyModelObj;
            electricField->loadFromSobjFile(getPath("Base/Objects/Electricity","sh3do"));
            electricField->makeVBO(true,isVAOSupported,false);
        }
    } else
        if (electricField)
        {
            delete electricField;
            electricField = 0;
        }
}