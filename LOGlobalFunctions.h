//
//  LOGlobalFunctions.h
//  LookOut
//
//  Created by Pavel Ignatov on 17.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOGlobalFunctions_h
#define LookOut_LOGlobalFunctions_h

void showGameCenter();
void showLeaderboard();
void activateAccelerometer();
void deactivateAccelerometer();

void gameOverMultiPlayer();
void loadMapAfterAnimation();
void multiPlayerUpdatePosition();
void multiPlayerRespawnBall(LOScriptEffectType type,SunnyVector2D pos, SunnyVector2D vel);
void multiPlayerSnowExploded(LOScriptEffectType type, short ballNum);
void multiPlayerMinutePast();
void multiPlayerWind();
//void multiPlayerAngryBall(SunnyVector2D);


#endif
