#ifndef _LO_SOUNDS_
#define _LO_SOUNDS_
#include "SunnyVector2D.h"

void losStopBackgroundMusic();
void losPauseSounds();
void losContinueSounds();

void losLoadAllSounds();
void losClearAllSounds();
void losSetPlayerPosition(SunnyVector2D pos);

void losPreUpdateSounds();
void losUpdateSounds();
void losStopGameSounds();
void losPlayGameSounds();
void losSetRollingSnowballSpeed(float v);
void losAddFireBallSound(void *pointer, float distance);
void losAddGroundBallSound(void *pointer, float distance);
void losAddWaterBallSound(void *pointer, float distance);
void losAddBonfireSound(void *pointer, float distance);
bool losIsElectricFieldPlaying(void *pointer);
void losAddElectricFieldSound(void *pointer, float distance);
void losAddAngryBallSound(void *pointer, float distance,float offset);
void losAddAngryBallLockedSound(void *pointer, float distance);
void losAddRotatingBladesSound(void *pointer, float distance);
void losAddMovingBladesSound(void *pointer, float distance,float offse);
void losAddElectricAttackSound(void *pointer, float distance);
void losAddAngryRushBallSound(void *pointer, float distance);
void losAddElectricBalllSound(void *pointer, float distance);
void losAddFireBallLaunchSound(void *pointer, float distance);
void losAddAngryBallLaunchSound(void *pointer, float distance);
void losAddAngryBallDigSound(void *pointer, float distance);
void losPlayFSExplosion(float distance);
void losPlayFFExplosion(float distance);
void losPlayFGExplosion(float distance);
void losPlayFWExplosion(float distance);
void losPlayGSExplosion(float distance);
void losPlayGGExplosion(float distance);
void losPlayGWExplosion(float distance);
void losPlayWSExplosion(float distance);
void losPlayWWExplosion(float distance);
void losPlaySSExplosion(float distance);
void losPlayEExplosion(float distance);
void losPlayAngryPreRush(float distance);
void losPlaySnowballBonfire();
void losPlaySnowflakePickUp();
void losPlayAchievementSound();
void losPlayNewSurvivalRecord();
void losPlayFinishPoint();

void losResetSounds();
void losPauseSoundsForMenu();
void losUnpauseSoundsFromMenu();

void losPlayRessurection();
void losPlayDeathSecond();

//New Sounds
void losPlayDropSound();
void losPlayIceTapSound();
void losPlayIceBreakSound();
void losPlayPuzzleDropSound();
void losPlayPuzzleConnectedSound();
void losPlayMagicBoxOpenedSound(short num);
//New Sounds...

//void losPlayWind(bool play);
//void losWindVolume(float volume);
void losPlayLevelComplete();
void losChangeVolumeOfTheMusic();

void losChangeSoundVolume(float volume);

void losPlaySpellSound(short num);
void losPlaySnowballDeathSound();
void losPlayStarSound(bool forceStop = false);
void losPlayGoldCoinSound();

void losPlayIceBlockMoved();
void losPlayIceBlockAndIceBlock();
void losPlaySnowBallAndBlades();
void losPlayTrapSound();
void losPlayButtonClick();

void losPrepareHardSound();
void losPlayHardSound();
void losRemoveHardSound();

void playTestSound(short num);
#endif