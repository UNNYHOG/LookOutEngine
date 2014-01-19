//
//  LOEvents.h
//  LookOut
//
//  Created by Ignatov on 06.08.13.
//
//

#ifndef __LookOut__LOEvents__
#define __LookOut__LOEvents__

#include <iostream>
#include "LOAchievements.h"
#include "LOPlayer.h"
#include "LOExplosion.h"
#include "LOScore.h"

class LOEvents
{
    static void OnMeterPassed(LOTask * task);
    static void OnSnowflakeCollected(LOTask * task);
    static void OnGoldCoinCollected(LOTask * task);
    static void OnSecondPassed(LOTask * task);
    static void OnDeath(LODieType dieType,LOTask * task);
    static bool OnUseAbility(LevelAbility ability,LOTask * task);
    static void OnCloseExplosion(LOExplosionType ex1, LOExplosionType ex2,LOTask * task);
    static void OnSizeChanged(float radius,LOTask * task);
    static void OnPlayerCollision(LOCollisionType type,LOTask * task);
    static void OnBallCollision(LOCollisionType ballType,LOTask * task);
    static void OnPlayerPushed(LOTask * task);
    
    static void OnWaterBallsAppeared(LOTask * task);
    static void OnGroundBallsAppeared(LOTask * task);
    static void OnMapReset(LOTask * task);
    
    static void OnAngryBallExplode(LOTask * task);
    static void OnAngryBallCollide(LODieType dieType,LOTask * task);
    static void OnAngryBallCollide(LevelAbility ability,LOTask * task);
    static void OnAngryBallCollide(LOCollisionType type,LOTask * task);
    static void OnLevelComplete(float time, unsigned char coins, LOTask * task);
public:
    static void OnMeterPassed();
    static void OnSnowflakeCollected();
    static void OnGoldCoinCollected();
    static void OnSecondPassed();
    static void OnDeath(LODieType dieType);
    static bool OnUseAbility(LevelAbility ability);
    static void OnCloseExplosion(LOExplosionType ex1, LOExplosionType ex2);
    static void OnSizeChanged(float radius);
    static void OnPlayerCollision(LOCollisionType type);
    static void OnBallCollision(LOCollisionType ballType);
    static void OnPlayerPushed();
    
    static void OnWaterBallsAppeared();
    static void OnGroundBallsAppeared();
    static void OnMapReset();
    
    static void OnAngryBallExplode();
    static void OnAngryBallCollide(LODieType dieType);
    static void OnAngryBallCollide(LevelAbility ability);
    static void OnAngryBallCollide(LOCollisionType type);
    static void OnLevelComplete(int stage, int level, float time, unsigned char coins);
};

#endif /* defined(__LookOut__LOEvents__) */
