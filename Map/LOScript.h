//
//  LOScript.h
//  LookOut
//
//  Created by Pavel Ignatov on 19.12.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOScript_h
#define LookOut_LOScript_h

#include "SunnyVector2D.h"
#include "stdio.h"
#include "SunnyOpengl.h"

#pragma mark -
#pragma mark Conditions

enum LOScriptConditionType
{
    SCT_None = 0,
    SCT_Time,
    SCT_Position,
    SCT_Script,
    SCT_NScript,
    SCT_Snowflakes,
    SCT_SnowBallScale,
    SCT_FinishCondition,
    SCT_RobotsCondition,
    SCT_OnCall,
    SCT_ButtonCondition,
    SCT_SnowflakePickUp,
    SCT_AngryBallDestroyed,
};

struct SCButtonInfo
{
    short buttonNum;
    short buttonState;//0 - down, 1 - up
};

struct SCTimeInfo
{
    float originTimeValue;
    float timeValue;
};

struct SCAngryBallInfo
{
    unsigned char angryballNum;
};

struct SCPosInfo
{
    SunnyVector2D pos;
    float radius;
};

struct SCScriptInfo
{
    short *scriptsNum;
    short scriptsCount;
};

struct SCSnowBallScaleInfo
{
    float scaleValue;
};

struct SCSnowflakePickUpInfo
{
    unsigned char snowflakeNum;
};


struct LOScriptCondition
{
    LOScriptConditionType conditionType;
    void *conditionInfo;
    bool complete;
};

#pragma mark -
#pragma mark Effects

enum LOScriptEffectType
{
    SET_None = 0,
    SET_FireBall,
    SET_WaterBall,
    SET_GroundBall,
    SET_AngryBall,
    SET_ElectricBall,
    SET_AnomalyBall,
    SET_RandomFireBall,
    SET_RandomWaterBall,
    SET_RandomGroundBall,
    SET_RandomAngryBall,//10
    SET_RandomElectricBall,
    SET_RandomAnomalyBall,
    SET_PlayerFireBall,
    SET_PlayerWaterBall,
    SET_PlayerGroundBall,
    SET_PlayerAngryBall,
    SET_PlayerElectricBall,
    SET_PlayerAnomalyBall,
    SET_Message,
    SET_GreenTip,//20
    SET_Wind,
    SET_RandomWind,
    SET_LevelComplete,
    SET_ChangeElectricField,
    SET_ExecuteRandomScript,
    SET_RandomSnowflake,
    SET_Snowflake,
};

struct SEBall //Fire, Water, Ground
{
    SunnyVector2D pos;
    SunnyVector2D vel;
};

struct SEPos //Snowflake
{
    SunnyVector2D pos;
};

struct SERandomBall
{
    float velocityScale;
};

struct SEGreenTip
{
    SunnyVector2D pos;
    float duration;
};

struct SEMessage
{
    SunnyVector2D pos;
    char * textMessage;
};

struct SEChangeElectic
{
    short num;
    short state;
};

struct SEExecuteRandomScript
{
    short scriptsCount;
    short * scriptsNum;
};

struct SEWind
{
    SunnyVector2D direction;
    float scale;
};

struct LOScriptEffect
{
    LOScriptEffectType effectType;
    void *effectInfo;
};

#pragma mark -
#pragma mark Main Class

class LOScript
{
    char * scriptName;//not saving & loading
    float repeat;// -1 if not repeatable
    float repeatValue;
    float repeatDiminish;
    float repeatMinimum;
    bool complete;
    float timeForLastScriptCondition;
    
    LOScriptCondition* conditions;
    short conditionsCount;
    
    LOScriptEffect* effects;
    short effectsCount;
    
    void init();
    void clear();
    float nextRepeatTime;
    
    bool multiPosScript;
public:
    LOScript();
    ~LOScript();
    
    void update();
    void addCondition(LOScriptCondition condition);
    void addEffect(LOScriptEffect effect);
    void save(sunnyFILE *stream);
	void load(sunnyFILE *stream);
    void changeToNewSystem();
    
    friend class LOMapInfo;
};

#endif