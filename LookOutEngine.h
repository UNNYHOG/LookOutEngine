//
//  LookOutEngine.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LookOutEngine_h
#define LookOut_LookOutEngine_h

#include "SunnyVector2D.h"
#include "SunnyMatrix.h"
#include "LOScript.h"
#include "LOMessages.h"
#include "LOGlobalFunctions.h"

void loResize(SunnyVector2D screen);

void loInit(SunnyVector2D screen, bool isFast, bool isVAO = true);
//void loMakeFrameBufferTexture(GLint &w, GLint &h);
void loClear();
void loClearEngine();
void loForcePause();
void loRestoreEngine();
void loLoadMap(short num, bool instantly = false);
void loLoadPreview(short num);
void loLoadEmptyMap(bool instantly = false);
void loLoadSurvivalMap(short num, bool instantly = false, bool doubler = false, bool magnet = false, bool time = false);
void loLoadMap(const char* fileName, bool menu = false);
void loUpdate(double delta);
//void loRenderFramebufferTexture();
//void loRenderSlowDevice();
void loRender();
void loAccelerometer(float sx = 0,float sy = 0,float sz = -1);
void loResetMap();
void loBlurExplosion(SunnyVector2D pos, float power);
void loCalibrate(bool accelerometer);

int loGetCollectedSnowflakes();

void addPointsToArray(SunnyVector4D texture,short num, SunnyVector4D *&points, bool inverse = true, SunnyVector2D textureSize = SunnyVector2D(2048,2048));
void translatePointsInArray(SunnyVector2D texture,short num, SunnyVector4D *&points);

//void loMultiplayerSetPosition(SunnyVector2D pos, SunnyVector2D vel, float radius, bool crashed, float w, float s, SunnyVector2D alpha, const char* playerId);
void loMultiplayerSetPosition(MessageUpdatePosition * pos, const char* playerId);
void loMultiplayerSnowExploded(LOScriptEffectType type, short num, const char * playerId);
void loMultiplayerRespawn(LOScriptEffectType type,SunnyVector2D pos, SunnyVector2D vel);

void loTouchBegan(void * touch, float x, float y);
void loTouchMoved(void * touch, float x, float y);
void loTouchEnded(void * touch, float x, float y);
void loSetJoysticPosition(SunnyVector2D position);

void loRenderJoystic(void * touch, SunnyVector2D position, SunnyVector2D direction);

#endif