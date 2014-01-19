//
//  LOAchievements.h
//  LookOut
//
//  Created by Ignatov on 16.08.13.
//
//

#ifndef __LookOut__LOAchievements__
#define __LookOut__LOAchievements__

#include <iostream>
#include "UnnyNet.h"
#include "LOPlayer.h"

class LOAchievements
{
public:
    static void reportCrashAchievement(LODieType type, int progress);
    static void reportCollideAchievement(LOCollisionType type, int progress);
    static void reportAbilityWasUsed(LevelAbility ability, int progress);
};

#endif /* defined(__LookOut__LOAchievements__) */
