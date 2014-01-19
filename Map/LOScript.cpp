//
//  LOScript.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 19.12.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOScript.h"
#include "SunnyOpengl.h"

LOScript::LOScript()
{
    init();
}

LOScript::~LOScript()
{
    clear();
}

void LOScript::init()
{
    complete = false;
    repeat = -1;
    repeatDiminish = 0;
    repeatMinimum = 0;
    repeatValue = repeat;
    
    effectsCount = 0;
    effects = 0;
    
    conditionsCount = 0;
    conditions = 0;
    
    scriptName = 0;
    nextRepeatTime = -1;
    timeForLastScriptCondition = -1;
}

void LOScript::clear()
{
    if (conditionsCount)
    {
        for (short i = 0;i<conditionsCount;i++)
        {
            if (conditions[i].conditionType == SCT_Time)
                delete (SCTimeInfo*)conditions[i].conditionInfo;
            else
            if (conditions[i].conditionType == SCT_Position)
                delete (SCPosInfo*)conditions[i].conditionInfo;
            else
            if (conditions[i].conditionType == SCT_SnowflakePickUp)
                delete (SCSnowflakePickUpInfo*)conditions[i].conditionInfo;
            else
                if (conditions[i].conditionType == SCT_AngryBallDestroyed)
                    delete (SCAngryBallInfo*)conditions[i].conditionInfo;
            else
            if (conditions[i].conditionType == SCT_Script || conditions[i].conditionType == SCT_NScript)
            {
                free(((SCScriptInfo*)conditions[i].conditionInfo)->scriptsNum);
                delete (SCScriptInfo*)conditions[i].conditionInfo;
            } else
                if (conditions[i].conditionType == SCT_SnowBallScale)
                    delete (SCSnowBallScaleInfo*)conditions[i].conditionInfo;
                else
                    if (conditions[i].conditionType == SCT_ButtonCondition)
                        delete (SCButtonInfo*)conditions[i].conditionInfo;
        }
        free(conditions);
    }
    
    if (effectsCount)
    {
        for (short i = 0;i<effectsCount;i++)
        {
            if (effects[i].effectType >= SET_GroundBall && effects[i].effectType <= SET_AnomalyBall)
                delete (SEBall*)effects[i].effectInfo;
            else
            if (effects[i].effectType >= SET_RandomGroundBall && effects[i].effectType <= SET_PlayerAnomalyBall)
                delete (SERandomBall*)effects[i].effectInfo;
            else
            if (effects[i].effectType == SET_GreenTip)
                delete (SEGreenTip*)effects[i].effectInfo;
            else
            if (effects[i].effectType == SET_Message)
            {
                delete[] ((SEMessage*)effects[i].effectInfo)->textMessage;
                delete (SEMessage*)effects[i].effectInfo;
            }
            else
                if (effects[i].effectType == SET_RandomWind || effects[i].effectType == SET_Wind)
                    delete (SEWind*)effects[i].effectInfo;
            else
                if (effects[i].effectType == SET_ChangeElectricField)
                    delete (SEChangeElectic*)effects[i].effectInfo;
            else
            if (effects[i].effectType == SET_ExecuteRandomScript)
            {
                free(((SEExecuteRandomScript*)effects[i].effectInfo)->scriptsNum);
                delete (SEExecuteRandomScript*)effects[i].effectInfo;
            } else
                if (effects[i].effectType == SET_Snowflake)
                {
                    delete (SEPos*)effects[i].effectInfo;
                }
        }
        free(effects);
    }
    
    if (scriptName)
        delete [] scriptName;
    
    init();
}

void LOScript::addCondition(LOScriptCondition condition)
{
    if (conditionsCount) conditions = (LOScriptCondition*)realloc(conditions, sizeof(LOScriptCondition)*(conditionsCount+1));
    else conditions = (LOScriptCondition*)malloc(sizeof(LOScriptCondition));
    conditions[conditionsCount++] = condition;
}

void LOScript::addEffect(LOScriptEffect effect)
{
    if (effectsCount) effects = (LOScriptEffect*)realloc(effects, sizeof(LOScriptEffect)*(effectsCount+1));
    else effects = (LOScriptEffect*)malloc(sizeof(LOScriptEffect));
    effects[effectsCount++] = effect;
}

void LOScript::save(sunnyFILE *stream)
{
	sunnyfwrite(&repeat,sizeof(repeat),1,stream);
    sunnyfwrite(&repeatDiminish,sizeof(repeatDiminish),1,stream);
    sunnyfwrite(&repeatMinimum,sizeof(repeatMinimum),1,stream);
	sunnyfwrite(&conditionsCount,sizeof(conditionsCount),1,stream);
//    printf("\n --> New script");
//    printf("\n Cond %d",conditionsCount);
	for (short i = 0;i<conditionsCount;i++)
	{
//        printf("\n > type = %d",conditions[i].conditionType);
		sunnyfwrite(&conditions[i].conditionType,sizeof(conditions[i].conditionType),1,stream);
		if (conditions[i].conditionType==SCT_Time)
			sunnyfwrite(conditions[i].conditionInfo,sizeof(SCTimeInfo),1,stream);
		else
		if (conditions[i].conditionType==SCT_Position)
			sunnyfwrite(conditions[i].conditionInfo,sizeof(SCPosInfo),1,stream);
		else
		if (conditions[i].conditionType==SCT_Script || conditions[i].conditionType == SCT_NScript)
		{
			SCScriptInfo* info = (SCScriptInfo*)conditions[i].conditionInfo;
			sunnyfwrite(&(info->scriptsCount),sizeof(short),1,stream);
			sunnyfwrite(info->scriptsNum,sizeof(short),info->scriptsCount,stream);
		}
        else
        if (conditions[i].conditionType==SCT_SnowBallScale)
            sunnyfwrite(conditions[i].conditionInfo,sizeof(SCSnowBallScaleInfo),1,stream);
        else
        if (conditions[i].conditionType==SCT_SnowflakePickUp)
            sunnyfwrite(conditions[i].conditionInfo,sizeof(SCSnowflakePickUpInfo),1,stream);
        else
            if (conditions[i].conditionType==SCT_AngryBallDestroyed)
                sunnyfwrite(conditions[i].conditionInfo,sizeof(SCAngryBallInfo),1,stream);
        else
        if (conditions[i].conditionType==SCT_ButtonCondition)
            sunnyfwrite(conditions[i].conditionInfo,sizeof(SCButtonInfo),1,stream);
	}
    
//    printf("\n Eff %d",effectsCount);
	sunnyfwrite(&effectsCount,sizeof(effectsCount),1,stream);
	for (short i = 0;i<effectsCount;i++)
	{
//        printf("\n > type = %d",effects[i].effectType);
		sunnyfwrite(&effects[i].effectType,sizeof(effects[i].effectType),1,stream);
		if (effects[i].effectType>=SET_FireBall && effects[i].effectType<=SET_AnomalyBall)
			sunnyfwrite(effects[i].effectInfo,sizeof(SEBall),1,stream);
		else
        if (effects[i].effectType>=SET_RandomFireBall && effects[i].effectType<=SET_PlayerAnomalyBall)
            sunnyfwrite(effects[i].effectInfo,sizeof(SERandomBall),1,stream);
        else
        if (effects[i].effectType==SET_Snowflake)
            sunnyfwrite(effects[i].effectInfo,sizeof(SEPos),1,stream);
        else
        if (effects[i].effectType==SET_GreenTip)
            sunnyfwrite(effects[i].effectInfo,sizeof(SEGreenTip),1,stream);
        else
		if (effects[i].effectType==SET_Message)
		{
			SEMessage* info = (SEMessage*)effects[i].effectInfo;
			sunnyfwrite(&(info->pos),sizeof(SunnyVector2D),1,stream);
			short len = strlen(info->textMessage);
			sunnyfwrite(&len,sizeof(short),1,stream);
			sunnyfwrite(info->textMessage,sizeof(char),strlen(info->textMessage),stream);
		}else
        if (effects[i].effectType==SET_RandomWind || effects[i].effectType==SET_Wind)
            sunnyfwrite(effects[i].effectInfo,sizeof(SEWind),1,stream);
        else
            if (effects[i].effectType==SET_ChangeElectricField)
                sunnyfwrite(effects[i].effectInfo, sizeof(SEChangeElectic), 1, stream);
        else
            if (effects[i].effectType==SET_ExecuteRandomScript)
            {
                SEExecuteRandomScript *info = (SEExecuteRandomScript*)effects[i].effectInfo;
                sunnyfwrite(&info->scriptsCount, sizeof(short), 1, stream);
                sunnyfwrite(info->scriptsNum, sizeof(short), info->scriptsCount, stream);
                
//                printf("\nExec Rand : %d",info->scriptsCount);
//                for (short i = 0;i<info->scriptsCount;i++)
//                    printf("\n > %d",info->scriptsNum[i]);
            }
	}
}

void LOScript::load(sunnyFILE *stream)
{
	clear();

    //printf("\n --> New script");
	sunnyfread(&repeat,sizeof(repeat),1,stream);
    sunnyfread(&repeatDiminish,sizeof(repeatDiminish),1,stream);
    sunnyfread(&repeatMinimum,sizeof(repeatMinimum),1,stream);
    repeatValue = repeat;
	sunnyfread(&conditionsCount,sizeof(conditionsCount),1,stream);
	conditions = (LOScriptCondition*)malloc(sizeof(LOScriptCondition)*conditionsCount);
    //printf("\n Cond %d",conditionsCount);
	for (short i = 0;i<conditionsCount;i++)
	{
		sunnyfread(&conditions[i].conditionType,sizeof(conditions[i].conditionType),1,stream);
        //printf("\n > type = %d",conditions[i].conditionType);
		if (conditions[i].conditionType==SCT_Time)
		{
			conditions[i].conditionInfo = new SCTimeInfo;
			sunnyfread(conditions[i].conditionInfo,sizeof(SCTimeInfo),1,stream);
		}
		else
		if (conditions[i].conditionType==SCT_Position)
		{
			conditions[i].conditionInfo = new SCPosInfo;
			sunnyfread(conditions[i].conditionInfo,sizeof(SCPosInfo),1,stream);
		}
        else
		if (conditions[i].conditionType==SCT_Script || conditions[i].conditionType==SCT_NScript)
		{
			conditions[i].conditionInfo = new SCScriptInfo;
			SCScriptInfo* info = (SCScriptInfo*)conditions[i].conditionInfo;
			sunnyfread(&(info->scriptsCount),sizeof(short),1,stream);
			info->scriptsNum = (short*)malloc(sizeof(short)*info->scriptsCount);
			sunnyfread(info->scriptsNum,sizeof(short),info->scriptsCount,stream);
		} 
        else
        if (conditions[i].conditionType==SCT_SnowBallScale)
        {
            conditions[i].conditionInfo = new SCSnowBallScaleInfo;
            sunnyfread(conditions[i].conditionInfo,sizeof(SCSnowBallScaleInfo),1,stream);
        } else
        if (conditions[i].conditionType==SCT_SnowflakePickUp)
        {
            conditions[i].conditionInfo = new SCSnowflakePickUpInfo;
            sunnyfread(conditions[i].conditionInfo,sizeof(SCSnowflakePickUpInfo),1,stream);
        } else
            if (conditions[i].conditionType==SCT_AngryBallDestroyed)
            {
                conditions[i].conditionInfo = new SCAngryBallInfo;
                sunnyfread(conditions[i].conditionInfo,sizeof(SCAngryBallInfo),1,stream);
            } else
        if (conditions[i].conditionType==SCT_ButtonCondition)
        {
            conditions[i].conditionInfo = new SCButtonInfo;
            sunnyfread(conditions[i].conditionInfo,sizeof(SCButtonInfo),1,stream);
        }
	}

	sunnyfread(&effectsCount,sizeof(effectsCount),1,stream);
	effects = (LOScriptEffect*)malloc(sizeof(LOScriptEffect)*effectsCount);
	for (short i = 0;i<effectsCount;i++)
	{
		sunnyfread(&effects[i].effectType,sizeof(effects[i].effectType),1,stream);
		if (effects[i].effectType>=SET_FireBall && effects[i].effectType<=SET_AnomalyBall)
		{
			effects[i].effectInfo = new SEBall;
			sunnyfread(effects[i].effectInfo,sizeof(SEBall),1,stream);
		}
		else
        if (effects[i].effectType>=SET_RandomFireBall && effects[i].effectType<=SET_PlayerAnomalyBall)
        {
            effects[i].effectInfo = new SERandomBall;
            sunnyfread(effects[i].effectInfo,sizeof(SERandomBall),1,stream);
        }
        else
        if (effects[i].effectType==SET_Snowflake)
        {
            effects[i].effectInfo = new SEPos;
            sunnyfread(effects[i].effectInfo,sizeof(SEPos),1,stream);
        }
        else
        if (effects[i].effectType==SET_GreenTip)
        {
            effects[i].effectInfo = new SEGreenTip;
            sunnyfread(effects[i].effectInfo,sizeof(SEGreenTip),1,stream);
        }
        else
		if (effects[i].effectType==SET_Message)
		{
			effects[i].effectInfo = new SEMessage;
			SEMessage* info = (SEMessage*)effects[i].effectInfo;
			sunnyfread(&(info->pos),sizeof(SunnyVector2D),1,stream);
			short len;
			sunnyfread(&len,sizeof(short),1,stream);
			info->textMessage = new char[len+1];
			sunnyfread(info->textMessage,sizeof(char),len,stream);
            info->textMessage[len] = '\0';
		}
        else
        if (effects[i].effectType==SET_RandomWind || effects[i].effectType==SET_Wind)
        {
            effects[i].effectInfo = new SEWind;
            sunnyfread(effects[i].effectInfo,sizeof(SEWind),1,stream);
        }
        else
            if (effects[i].effectType==SET_ChangeElectricField)
            {
                effects[i].effectInfo = new SEChangeElectic;
                sunnyfread(effects[i].effectInfo,sizeof(SEChangeElectic),1,stream);
            }
        else
            if (effects[i].effectType == SET_ExecuteRandomScript)
            {
                effects[i].effectInfo = new SEExecuteRandomScript;
                SEExecuteRandomScript * info = (SEExecuteRandomScript*)effects[i].effectInfo;
                sunnyfread(&info->scriptsCount,sizeof(short),1,stream);
                info->scriptsNum = (short*)malloc(sizeof(short)*info->scriptsCount);
                sunnyfread(info->scriptsNum,sizeof(short),info->scriptsCount,stream);
            }
	}
}

void LOScript::changeToNewSystem()
{
    static const float newSystemKoef = 2.5;
    for (short i = 0;i<conditionsCount;i++)
	{
        if (conditions[i].conditionType==SCT_Position)
        {
            SCPosInfo *info = (SCPosInfo*)conditions[i].conditionInfo;
            info->pos *= newSystemKoef;
        }
    }
    
    for (short i = 0;i<effectsCount;i++)
	{
		if (effects[i].effectType>=SET_FireBall && effects[i].effectType<=SET_AnomalyBall)
        {
            SEBall * info = (SEBall*)effects[i].effectInfo;
            info->pos *= newSystemKoef;
        } else
            if (effects[i].effectType==SET_Snowflake)
            {
                SEPos * info = (SEPos*)effects[i].effectInfo;
                info->pos *= newSystemKoef;
            }
    }
}

void LOScript::update()
{
    
}