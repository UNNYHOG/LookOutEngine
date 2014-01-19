//
//  LOPlayer.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOPlayer.h"
#include "SunnyOpengl.h"
#include "LOGlobalVar.h"
#include "LOSounds.h"
#include "iphone_strings.h"
#include "SunnyShaders.h"
#include "LOTrace.h"
#include "LOExplosion.h"
#include "LOEvents.h"
#include "LOScore.h"
#include "SunnyFunctions.h"

const float maxSpeedEffectDistance = cellSize/2;
const float speedEffectVelocity = cellSize*3;
const float sandEffectDuration = 5;
const float waterEffectDuration = 3;

SunnyVector2D playerPhysicsPoints[playerPhysicsPointsCount];

extern SunnyVector3D **sharedPowerField;

const float speedKoef = 5;

LOPlayer::LOPlayer()
{
    init();
}

LOPlayer::~LOPlayer()
{
    clear();
}

//void LOPlayer::removeAbility(short aNum)
//{
//    if (ability[aNum].active)
//    {
//        if (ability[aNum].abilityType == SA_P_Speed)
//        {
//            abilitySpeed = 1.0;
//        }
//    }
//    ability[aNum].active = false;
//}
//
//void LOPlayer::setAbility(short aNum, SnowballAbility type, short level)
//{
//    removeAbility(aNum);
//    ability[aNum].active = true;
//    ability[aNum].abilityType = type;
//    ability[aNum].level = level;
//    ability[aNum].cooldown = 0;
//    
//    if (ability[aNum].abilityType == SA_P_Speed)
//    {
//        abilitySpeed = 1.0 + 0.1*ability[aNum].level;
//    }
//}

bool LOPlayer::useShield()
{
    if (survivalShieldWorkTime>=0.001)
    {
        return true;
    }
    if (survivalShield<=0) return false;
    survivalShield--;
    losPlaySpellSound(3);
    survivalShieldWorkTime = 0.3;
    return true;
}

void loPButtonPressed(SunnyButton *btn);

bool LOPlayer::activateAbility(LevelAbility ability, bool loaded)
{
    if (!loaded)
    {
        if (!LOEvents::OnUseAbility(ability))
        {
            LOMenu::getSharedMenu()->pauseTheGame();
            LOMenu::getSharedMenu()->showNotEnoughGoldHint(loPButtonPressed,LOMenu::getSharedMenu()->pauseWindow->getStoreButton());
            return false;//not enough money
        }
        
        activatedSpell = ability;
        spellActivationFrame = 0;
    }
    switch (ability) {
        case LA_Speed:
        {
            for (short i = 0;i<speedPointsCount;i++) speedPoints[i].pos = position.pos;
            if (!loaded)
            if (lastBufVelocity.length()>0.1)
            {
                SunnyMatrix m = sunnyIdentityMatrix;
                m.up = SunnyVector4D(lastBufVelocity.x/speedKoef/2,lastBufVelocity.y/speedKoef/2,0,0);
                m.front = m.up ^ m.right;
                m.pos = SunnyVector4D(position.pos.x, position.pos.y, 0, 1);
                activeMap->addSpeedExplosion(&m);
            }
            abilitySpeed = 1.5;
            speedTime = speedSpellTime;
            sandFriction = 1;
            waterKoef = 0;
            anomalyKoef = 0;
            break;
        }
        case LA_Life:
            survivalShield = 1;
            break;
        case LA_Shield:
            effectShield = 3;
            break;
        case LA_Teleport:
            teleportActive = true;
            break;
        case LA_Magnet:
            snowMagnet = true;
            break;
        case LA_Time:
            snowTime = true;
            break;
        case LA_Doubler:
            snowDoubler = true;
            break;
        case LA_Ressurection:
        {
            reset(true);
            break;
        }
            
        default:
            break;
    }
    
    return true;
}

bool LOPlayer::isAbilityActive(LevelAbility ability)
{
    switch (ability) {
        case LA_Speed:
            return abilitySpeed>1.01;
            break;
        case LA_Life:
            return survivalShield != 0;
            break;
        case LA_Shield:
            return effectShield != 0;
            break;
        case LA_Teleport:
            return teleportActive;
            break;
        case LA_Magnet:
            return snowMagnet;
            break;
        case LA_Time:
            return snowTime;
            break;
        case LA_Doubler:
            return snowDoubler;
            break;
        default:
            return false;
            break;
    }
}

bool LOPlayer::isAbilityEnable(LevelAbility ability)
{
    switch (ability) {
        case LA_Speed:
            return !(abilitySpeed>1.01);
            break;
        case LA_Life:
            return !(survivalShield != 0);
            break;
        case LA_Shield:
            return !(effectShield != 0);
            break;
        case LA_Teleport:
            return !(teleportActive);
            break;
        case LA_Magnet:
            return !snowMagnet;
            break;
        case LA_Time:
            return !snowTime;
            break;
        case LA_Doubler:
            return !snowDoubler;
            break;
        default:
            return false;
            break;
    }
}

void LOPlayer::init()
{
    rolledDistance = 0;
    lastReportedDistance = 0;
    
    abilitySpeed = 1;

    alphaX = alphaY = 0;
    
    respawnTime = 0;
    radius = cellSize/2;
    shapeRadius = radius;
    reset();
    
    for (short i = 0;i<speedPointsCount;i++)
        speedPoints[i] = sunnyIdentityMatrix;
    
    float angle = 0;
    float deltaAngle = M_PI*2/playerPhysicsPointsCount;
    for (short i = 0;i<playerPhysicsPointsCount;i++)
    {
        playerPhysicsPoints[i] = SunnyVector2D(cosf(angle),sin(angle));
        angle += deltaAngle;
    }
    
    ressurectionsCount = 0;
}

void LOPlayer::clear()
{
}

void LOPlayer::setAngle(float x, float y)
{
    alphaX = x;
    alphaY = y;
}

void LOPlayer::removePhysics()
{
    body->layers = LAYER_SNOWFLAKE;
    physActive = false;
}

void LOPlayer::attachMoveBody()
{
    if (player==this)
        body->layers = LAYER_HIGH | LAYER_LOW | LAYER_BLADES | LAYER_ELECTRIC | LAYER_ELECTRIC_BALL | LAYER_SNOWFLAKE;
    else
        body->layers = LAYER_LOW | LAYER_ELECTRIC_BALL | LAYER_SNOWFLAKE;
    body->collisionType = COLLISION_PLAYER;
    physActive = true;
}

void LOPlayer::updateRadius(float newR)
{
    float bufR = shapeRadius;
    radius = newR;
    setRadius(radius);
    shapeRadius = bufR;
    
    if (fabsf(radius - shapeRadius) >= 0.01*cellSize/2)
    {
        LOEvents::OnSizeChanged(radius);
        shapeRadius = radius;
    }
}

void LOPlayer::setRadius(float newR)
{
    radius = newR;
    shapeRadius = radius;
    body->radius = newR;
}

bool LOPlayer::updateMultiPlayer()
{
    SunnyVector2D v = SunnyVector2D(alphaX, alphaY)*7.2*abilitySpeed + velocityAdd;
    velocityAdd /= 2;
    
//    if (mapWind.z)
//        v += SunnyVector2D(mapWind.x,mapWind.y);
    
    if (waterKoef>0.0)
    {
        v = lastVelocity*waterKoef + (1.0-waterKoef)*v;
        waterKoef -= deltaTime/waterEffectDuration;
        if (waterKoef<0.0) waterKoef = 0.0;
    }
    
    v *= sandFriction;
    if (sandFriction<1.0)
    {
        sandFriction += deltaTime/sandEffectDuration;
        if (sandFriction>1.0) sandFriction = 1.0;
    }
    
    lastBufVelocity = v;
    
    if (anomalyKoef>0)
    {
        anomalyKoef -= deltaTime/5;
        v += SunnyVector2D(1 - sRandomf()*2,1 - sRandomf()*2)*2;
    }
    
    return false;
}

static const float maxBadDistance = cellSize/2;

void LOPlayer::checkMultiplayerPosition(SunnyVector2D pos)
{
}

void LOPlayer::applyJoystic()
{
    SunnyVector2D v = calculateVelocity();
    float len = (loJoystic.location - getPosition()).length();
    float vLen = v.length();
    
    if (vLen>len)
    {
        alphaX = alphaX/vLen*len;
        alphaY = alphaY/vLen*len;
    }
//    printf("\nLen = %f = %f (%f)",len,vLen,vLen*deltaTime);
}

bool LOPlayer::isHardPowerForDirection(short direction)
{
    const float bigValue = 0.29;
    switch (direction) {
        case 1://left
            return (alphaX<-bigValue);
            break;
        case 2://top
            return (alphaY>bigValue);
            break;
        case 3://right
            return (alphaX>bigValue);
            break;
        case 4://bottom
            return (alphaY<-bigValue);
            break;
            
        default:
            break;
    }
    return false;
}
/*
bool LOPlayer::isHardPowerForDirection(short direction)
{
    const float bigValue = 0.29*100/2;
    
    hasIceContact[direction-1] = true;
    switch (direction) {
        case 1://left
            icePowers[0] += alphaX;
            if (icePowers[0]<-bigValue)
            {
                return true;
            } else
                return false;
            break;
        case 2://top
            icePowers[1] += alphaY;
            if (icePowers[1]>bigValue)
            {
                return true;
            } else
                return false;
            break;
        case 3://right
            icePowers[2] += alphaX;
            if (icePowers[2]>bigValue)
            {
                return true;
            } else
                return false;
            break;
        case 4://bottom
            icePowers[3] += alphaY;
            if (icePowers[3]<-bigValue)
            {
                return true;
            } else
                return false;
            break;
        default:
            break;
    }
    return false;
}*/

SunnyVector2D LOPlayer::calculateVelocity()
{
    if (crashed)
        return SunnyVector2D(0,0);
    SunnyVector2D v = SunnyVector2D(alphaX, alphaY)*5.0*abilitySpeed + velocityAdd;
    v*=speedKoef;
    
    if ((activeMap->mapInfo.flagPosition.x || activeMap->mapInfo.flagPosition.y) && activeMap->mapInfo.snowflakeCondition == 2)
    {
        if ((activeMap->mapInfo.flagPosition - getPosition()).length()<cellSize*2)
        {
            v += (activeMap->mapInfo.flagPosition - getPosition()).normalize();
        }
    }
    
//    if (mapWind.z)
//        v += SunnyVector2D(mapWind.x,mapWind.y);
    
    if (waterKoef>0.0)
        v = lastVelocity*waterKoef + (1.0-waterKoef)*v;
    
    v *= sandFriction;

    return v;
}

bool LOPlayer::update()
{
    if (!physActive && !crashed)
    if (ressurectionTime>0 && realPlayingTime-ressurectionTime > ressurectionImmune)
    {
        attachMoveBody();
    }
    SunnyVector2D v = calculateVelocity();
    lastBufVelocity = v;
    
    velocityAdd /= 4;
    if (waterKoef>0.0)
    {
        waterKoef -= deltaTime/waterEffectDuration;
        if (waterKoef<0.0) waterKoef = 0.0;
    }
    
    if (sandFriction<1.0)
    {
        sandFriction += deltaTime/sandEffectDuration;
        if (sandFriction>1.0) sandFriction = 1.0;
    }
    
    if (anomalyKoef>0)
    {
        anomalyKoef -= deltaTime/5;
        v += SunnyVector2D(1 - sRandomf()*2,1 - sRandomf()*2)*2;
    }
    
    velocity = v;
    if (crashed)
        losSetRollingSnowballSpeed(0);
    else
    {
        //losSetRollingSnowballSpeed(v.length());
        losSetRollingSnowballSpeed(velocity.length()/5);
    }
    
    if (isAbilityActive(LA_Speed))
    {
        speedTime -= deltaTime;
        if (speedTime<=0)
        {
            abilitySpeed = 1;
            activatedSpell = LA_Speed;
            spellActivationFrame = 0;
            losPlaySpellSound(5);
        }
    }
    
    if (survivalShieldWorkTime>0)
    {
        survivalShieldWorkTime -= deltaTime;
    }
    
    for (short i = 0;i<4;i++)
    if (!hasIceContact[i])
    {
        icePowers[i] = 0;
    } else
    {
        hasIceContact[i] = false;
        icePowers[i] -= sunnyZnak(icePowers[i]);
    }
    
    return false;
}

bool LOPlayer::updatePosition()
{
    if (crashed)
        return false;
    
    const float onePartSize = cellSize/4/4;
    float dist = velocity.length()*deltaTime;
    SunnyVector2D pos = getPosition();
    SunnyVector2D realPos = pos;
    SunnyVector2D v = velocity*deltaTime;
    for (int j = 0;j<6;j++)
    {
        if (v.length()<0.01)
            break;
        
        //bool q = true;
        for (short i = 0;i<playerPhysicsPointsCount;i++)
        {
            SunnyVector2D pp = realPos + playerPhysicsPoints[i]*radius;
            
            short x = pp.x*forceFieldSizeX/mapSize.x;
            short y = forceFieldSizeY - pp.y*forceFieldSizeY/mapSize.y - 1;
            
            if (x>=0 && y>=0 && x<forceFieldSizeX && y<forceFieldSizeY)
            {
                SunnyVector3D force = sharedPowerField[x][y];
                //printf("\n (%d,%d) > force = (%f,%f,%f)",x,y,force.x,force.y,force.z);
                if (force.z>4)
                {
                    float dot = v.x*force.x + v.y*force.y;
                    if (dot<0)
                    {
                        v.x -= dot*force.x;
                        v.y -= dot*force.y;
                        //q = false;
                    }
                } else
                {
                    //v.x += force.x*force.z/8;
                    //v.y += force.y*force.z/8;
                }
            }
        }
        //if (q) realPos = pos;
        
        float len = v.length();
        if (len<0.01)
            break;
        else
        {
            dist = fminf(onePartSize,len);
            realPos = realPos + v*dist/len;
            v -= v*dist/len;
        }
    }
    
    v = realPos;
    
    if (v.x<radius)
        v.x = radius;
    else
        if (v.x>mapSize.x-radius)
            v.x = mapSize.x-radius;
    if (v.y<radius)
        v.y = radius;
    else
        if (v.y>mapSize.y-radius)
            v.y = mapSize.y-radius;
    
    //Make a multiplayer check
    rolledDistance += SunnyVector2D(v.x-position.pos.x, v.y-position.pos.y).length();
    if (rolledDistance-lastReportedDistance>1)
    {
        LOEvents::OnMeterPassed();
        lastReportedDistance++;
    }
    
    SunnyVector3D rotVec = SunnyVector3D(v.x - position.pos.x, v.y - position.pos.y, 0) ^ SunnyVector3D(0,0,-1);
    rotVec = -rotVec/radius;
    float sinTetaAngle = sin(rotVec.x);
    float cosTetaAngle = cos(rotVec.x);
    float sinPhiAngle = sin(-rotVec.y);
    float cosPhiAngle = cos(-rotVec.y);
    
    {
        float scale = 1;
        if (waterKoef>0) scale += waterKoef*5;
        radius += rotVec.length() * 0.001*scale;
        updateRadius(radius);
    }
    
    position = position * SunnyMatrix(SunnyVector4D(1,0,0,0),SunnyVector4D(0,cosTetaAngle,-sinTetaAngle,0),SunnyVector4D(0,sinTetaAngle,cosTetaAngle,0),SunnyVector4D(0,0,0,1));
    position = position * SunnyMatrix(SunnyVector4D(cosPhiAngle,0,-sinPhiAngle,0),SunnyVector4D(0,1,0,0),SunnyVector4D(sinPhiAngle,0,cosPhiAngle,0),SunnyVector4D(0,0,0,1));
    
    SunnyVector2D p = SunnyVector2D(v.x,v.y);
    //Speed effect
    SunnyVector2D l;
    float len;
    for (int i = 0;i<speedPointsCount;i++)
    {
        l = SunnyVector2D(speedPoints[i].pos.x, speedPoints[i].pos.y) - p;
        len = l.length() - speedEffectVelocity*deltaTime;
        if (len<=0)
        {
            speedPoints[i].pos.x = p.x;
            speedPoints[i].pos.y = p.y;
        }
        else
        {
            if (len>maxSpeedEffectDistance)
                len = maxSpeedEffectDistance;
            l.normalize();
            speedPoints[i].pos.x = p.x + l.x*len;
            speedPoints[i].pos.y = p.y + l.y*len;
            p = SunnyVector2D(speedPoints[i].pos.x, speedPoints[i].pos.y);
            
            speedPoints[i].up.x = -l.x;
            speedPoints[i].up.y = -l.y;
            speedPoints[i].front = speedPoints[i].up ^ speedPoints[i].right;
        }
    }
    
    position.pos.x = v.x;
    position.pos.y = v.y;
    position.pos.z = radius;
    
    body->position = SunnyVector2D(position.pos.x, position.pos.y);
    
    return false;
}

void LOPlayer::setPlayersColor()
{
    static const SunnyVector3D grColor = SunnyVector3D(0.8,0.4,0.0);
    static const SunnyVector3D wColor = SunnyVector3D(0,0.3,1.0);
    static const SunnyVector3D aColor = SunnyVector3D(152,87,234)/255.;
    SunnyVector3D color(1,1,1.15);
    if (waterKoef!=0)
    {
        float k = waterKoef*0.4;
        color = color*(1-k) + k*wColor;
    }
    if (sandFriction!=1)
    {
        float k = sandFriction*0.4+0.6;
        color = color*k + (1-k)*grColor;
    }
    if (anomalyKoef!=0)
    {
        float k = anomalyKoef;
        color = color*(1-k) + k*aColor;
    }
    float alpha = getRessurectionAlpha();
    glUniform4fv(uniform3DS_C, 1, SunnyVector4D(color.x*globalColor.x, color.y*globalColor.y, color.z*globalColor.z, alpha));
}

float LOPlayer::getRessurectionAlpha()
{
    if (!physActive)
    if (ressurectionTime>0 && realPlayingTime-ressurectionTime<ressurectionImmune)
    {
        float alpha = 0.3+0.7*fabsf(sinf((realPlayingTime-ressurectionTime+0.5)*M_PI));
        glEnable(GL_BLEND);
        return alpha;
    }
    return 1;
}

void LOPlayer::renderForMenu(SunnyMatrix *MV, float colorScale,SunnyVector3D color )
{
    sunnyUseShader(globalShaders[LOS_Textured3DSnow]);
    glUniform4fv(uniform3DS_C, 1, SunnyVector4D(color.x*colorScale,color.y*colorScale,color.z*colorScale, 1.0));
    glUniformMatrix4fv(globalModelview[LOS_Textured3DSnow], 1, GL_FALSE, &(MV->front.x));
    SunnyVector4D v = SunnyVector4D(1,1,20,0).normalize()*1.3;
    glUniform3fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "lightPosition"), 1, v);
    snowBallObj->render(true,isVAOSupported);
    glUniform3fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "lightPosition"), 1, customLightPositionGraphic);
}

void LOPlayer::render(SunnyMatrix *MV)
{
    if (hidden) return;
    
    SunnyMatrix m = getScaleMatrix(SunnyVector3D(radius*2, radius*2, radius*2))* position **MV;
    glUniformMatrix4fv(globalModelview[LOS_Textured3DSnow], 1, GL_FALSE, &(m.front.x));
    snowBallObj->render(true,isVAOSupported);
}

void LOPlayer::renderSpellsUnderPlayer(SunnyMatrix *MV)
{
    if (hidden) return;
    if (isAbilityActive(LA_Speed))
    {
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        
        float scale = radius/(cellSize/2);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));

        SunnyMatrix m;
        short i = 4;
        glUniform1f(uniformTSA_A, 0.15);
        m = speedPoints[i] * *MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSpellSpeed_VAO);
        
        i = 3;
        glUniform1f(uniformTSA_A, 0.30);
        m = speedPoints[i] * *MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSpellSpeed_VAO);
        
        i = 2;
        glUniform1f(uniformTSA_A, 0.55);
        m = speedPoints[i] * *MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSpellSpeed_VAO);
        
        i = 1;
        glUniform1f(uniformTSA_A, 0.70);
        m = speedPoints[i] * *MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSpellSpeed_VAO);
        
        i = 0;
        glUniform1f(uniformTSA_A, 1);
        m = speedPoints[i] * *MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSpellSpeed_VAO);
    }
}

void LOPlayer::renderSpells(SunnyMatrix *MV)
{
    if (hidden) return;
    float scale = radius/(cellSize/2);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
    glUniform1f(uniformTSA_A, 1);
    
    SunnyMatrix mat = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y,0)) **MV, m;
    
    if (isAbilityActive(LA_Life))
    {
        float s = scale*(1+0.15*sin(realPlayingTime*2));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, s, s));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat.front.x));
        SunnyDrawArrays(LOSpellLife_VAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
    }
    
    if (isAbilityActive(LA_Shield))
    {
        for (short i = 0;i<effectShield;i++)
        {
            m = getRotationZMatrix(realPlayingTime*2+i*2*M_PI/effectShield) * mat;
            m = getTranslateMatrix(SunnyVector3D(-31*0.75/640*mapSize.y*scale,0,0))*m;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOSpellShield_VAO);
        }
        //glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
    }
}

void LOPlayer::renderOverlays(SunnyMatrix *MV)
{
    if (hidden) return;
    float scale = radius/(cellSize/2);
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(position.pos.x,position.pos.y,scale,scale));
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
    
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(MV->front.x));
    
    const float halfPeriod = 0.15;
    if (playingTime>halfPeriod*6)
    {
        if (!physActive && !crashed)
        {
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,getRessurectionAlpha()));
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
        }
        SunnyDrawArrays(LOOverlaySnowflake_VAO);
    }
    else
    {
        float s = fabsf(sinf(playingTime*M_PI_2/halfPeriod))*2;
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(position.pos.x,position.pos.y,scale*(1+s*0.25),scale*(1+s*0.25)));
        if (playingTime>halfPeriod*5)
        {
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,s));
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
        } else
        {
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,s));
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
        }
        
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
    }
    
    if (isAbilityActive(LA_Life))
    {
        SunnyDrawArrays(LOSpellLife_VAO+1);
    }
    
    if (isAbilityActive(LA_Shield))
    {
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
        SunnyMatrix mat = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y,0)) **MV, m;
        for (short i = 0;i<effectShield;i++)
        {
            m = getRotationZMatrix(realPlayingTime*2+i*2*M_PI/effectShield) * mat;
            m = getTranslateMatrix(SunnyVector3D(-31*0.75/640*mapSize.y*scale,0,0))*m;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
        }
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(MV->front.x));
    }
    
    if (isAbilityActive(LA_Teleport))
    {
         //SunnyDrawArrays(LOSpellTeleport_VAO+1);
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(position.pos.x,position.pos.y,scale*0.7,scale*0.7));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,210./255,0,0.75+0.25*sin(realPlayingTime*5)));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
    
    //Activation
    if (activatedSpell>=1)
    {
        SunnyVector4D color;
        switch (activatedSpell) {
            case LA_Life:
                color = SunnyVector4D(248./255,151./255,36./255,1.0);
                break;
            case LA_Teleport:
                color = SunnyVector4D(255./255,243./255,67./255,1.0);
                break;
            case LA_Speed:
                color = SunnyVector4D(186./255,255./255,0./255,1.0);
                break;
            case LA_Shield:
                color = SunnyVector4D(158./255,227./255,255./255,1.0);
                break;
            default:
                break;
        }
        
        const short spellFrames = 10;
        
        if (spellActivationFrame>1)
        {
            color.w = ((float)(spellFrames - (spellActivationFrame-1)))/spellFrames;
            scale *= (1 + (1-color.w));
        }
        
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(position.pos.x,position.pos.y,scale,scale));
        glUniform4fv(uniformOverlayTSA_C, 1, color);
        SunnyDrawArrays(LOOverlaySpells_VAO);
        
        if (spellActivationFrame==0)
            SunnyDrawArrays(LOOverlaySpells_VAO);
        spellActivationFrame++;
        if (spellActivationFrame==spellFrames+1)
            activatedSpell = 0;
    }
}

float LOPlayer::getRadius()
{
    return radius;
}

float LOPlayer::getSnowballScale()
{
    return getSnowballScale(radius);
//    return radius/(cellSize/2);
}

float LOPlayer::getSnowballScale(float r)
{
    return powf(r/(cellSize/2), 2);
}

void LOPlayer::forceTeleport()
{
    hidden = true;
    
    SunnyMatrix m = sunnyIdentityMatrix;
    m.pos.x = position.pos.x;
    m.pos.y = position.pos.y;
    activeMap->addTeleportExplosionPost(&m);
    losPlaySpellSound(7);
}

void LOPlayer::teleportToLocation(SunnyVector2D pos)
{
    if (!teleportActive) return;
    
    const float min = 1;
    if (pos.x<min) pos.x = min;
    if (pos.y<min) pos.y = min;
    if (pos.x>mapSize.x-min) pos.x = mapSize.x-min;
    if (pos.y>mapSize.y-min) pos.y = mapSize.y-min;
    
    SunnyMatrix m = sunnyIdentityMatrix;
    m.pos.x = position.pos.x;
    m.pos.y = position.pos.y;
    activeMap->addTeleportExplosionPost(&m);
    
    setPosition(pos);
    
    teleportActive = false;
    
    m.pos.x = position.pos.x;
    m.pos.y = position.pos.y;
    LOTrace::sharedTrace()->addTeleportationPostEffect(&m);
    activeMap->addTeleportExplosion(&m);
    
    losPlaySpellSound(7);
}

bool LOPlayer::setGroundKoef()
{
    radius -= 0.1;
    if (radius<cellSize/2)
        radius=cellSize/2;
    updateRadius(radius);

    if (effectShield>0)
    {
        effectShield--;
        losPlaySpellSound(1);
        return false;
    }
    sandFriction = 0.0;
    
    return true;
}

bool LOPlayer::setWaterKoef()
{
    if (effectShield>0)
    {
        effectShield--;
        losPlaySpellSound(1);
        return false;
    }
    waterKoef = 1.0;
    lastVelocity = lastBufVelocity;
    return true;
}

bool LOPlayer::setAnomaly()
{
    if (effectShield>0)
    {
        effectShield--;
        losPlaySpellSound(1);
        return false;
    }
    anomalyKoef = 1.0;
    return true;
}

void LOPlayer::setCrashed(bool value)
{
    crashed = value;
    if (crashed)
    {
        alphaX = alphaY = 0;
        removePhysics();
    }
}

void LOPlayer::reset(bool ressurection)
{
    survivalShieldWorkTime = 0;
    hidden = false;
    if (ressurection)
    {
        ressurectionTime = realPlayingTime;
        ressurectionsCount++;
    }
    else
    {
        snowMagnet = snowDoubler = snowTime = false;
        ressurectionTime = 0;
        ressurectionsCount = 0;
        attachMoveBody();
    }
    speedTime = 0;
    activatedSpell = -1;
    crashed = false;
    velocityAdd = SunnyVector2D(0,0);
    velocity = lastVelocity = SunnyVector2D(0,0);
    sandFriction = 1.0;
    waterKoef = 0.0;
    anomalyKoef = 0;
    
    abilitySpeed = 1;
    
    survivalShield = 0;
    effectShield = 0;
    teleportActive = false;
    
//    position = getIdentityMatrix();
    
    loJoystic.location = getPosition();
    loJoystic.direction = SunnyVector2D(0,0);

    for (short i = 0;i<4;i++)
    {
        icePowers[i] = 0;
        hasIceContact[i] = false;
    }
}

