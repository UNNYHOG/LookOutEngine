//
//  LOAchievements.cpp
//  LookOut
//
//  Created by Ignatov on 16.08.13.
//
//

#include "LOAchievements.h"
#include "LOGlobalVar.h"

void LOAchievements::reportCrashAchievement(LODieType type, int progress)
{
    char achId[128];
    
    if (player->isAbilityActive(LA_Shield))
    {
        UnnyNet::reportAchievement("wrongButton", 100);
    }
    
    if (player->ressurectionTime>0 && realPlayingTime - player->ressurectionTime <= 3 + ressurectionImmune)
    {
        UnnyNet::reportAchievement("deadAgain", 100);
    }
    
    strcpy(achId, "crash.");
    switch (type) {
        case LOD_FireBall:
        {
            strcat(achId, "fire");
            progress = 100*progress/100;
            break;
        }
        case LOD_Bonfire:
        {
            strcat(achId, "bonfire");
            progress = 100*progress/50;
            break;
        }
        case LOD_Blades:
        {
            strcat(achId, "blades");
            progress = 100*progress/50;
            break;
        }
        case LOD_ElectricField:
        {
            strcat(achId, "electricField");
            progress = 100*progress/50;
            break;
        }
        case LOD_ElectricBall:
        {
            strcat(achId, "electric");
            progress = 100*progress/50;
            break;
        }
        case LOD_AngryBall:
        {
            strcat(achId, "angry");
            progress = 100*progress/50;
            
            if (player->isAbilityActive(LA_Magnet))
            {
                UnnyNet::reportAchievement("angryMagnet", 100);
            }
            break;
        }
        
        default:
            return;
    }
    
    UnnyNet::reportAchievement(achId, progress);
}

void LOAchievements::reportCollideAchievement(LOCollisionType type, int progress)
{
    char achId[128];
    strcpy(achId, "crash.");
    switch (type) {
        case LOCT_GroundBall:
        {
            strcat(achId, "ground");
            progress = 100*progress/100;
            break;
        }
        case LOCT_WaterBall:
        {
            strcat(achId, "water");
            progress = 100*progress/100;
            break;
        }
        case LOCT_AnomalyBall:
        {
            strcat(achId, "anomaly");
            progress = 100*progress/100;
            break;
        }
            
        default:
            return;
    }
    UnnyNet::reportAchievement(achId, progress);
}

void LOAchievements::reportAbilityWasUsed(LevelAbility ability, int progress)
{
    char achId[128];
    strcpy(achId, "spell.");
    switch (ability) {
        case LA_Life:
        {
            strcat(achId, "life");
            progress = 100*progress/100;
            break;
        }
        case LA_Teleport:
        {
            strcat(achId, "teleport");
            progress = 100*progress/100;
            break;
        }
        case LA_Speed:
        {
            strcat(achId, "speed");
            progress = 100*progress/100;
            if (player->getSnowballScale()>=2)
                UnnyNet::reportAchievement("speedUp2x", 100);
            if (player->sandFriction<=0.5)
                UnnyNet::reportAchievement("speedUpGround", 100);
            break;
        }
        case LA_Shield:
        {
            strcat(achId, "shield");
            progress = 100*progress/100;
            break;
        }
        case LA_Doubler:
        {
            strcat(achId, "doubler");
            progress = 100*progress/50;
            break;
        }
        case LA_Magnet:
        {
            strcat(achId, "magnet");
            progress = 100*progress/50;
            break;
        }
        case LA_Ressurection:
        {
            strcat(achId, "ressurect");
            progress = 100*progress/50;
            if (!isSurvival)
                UnnyNet::reportAchievement("campaignRessurection", 100);
            else
                if (playingTime>=5*60)
                    UnnyNet::reportAchievement("ressurect5min", 100);
            break;
        }
        case LA_Time:
        {
            strcat(achId, "time");
            progress = 100*progress/50;
            break;
        }
            
        default:
            return;
    }
    
    UnnyNet::reportAchievement(achId, progress);
}



