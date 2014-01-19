//
//  LOSounds.m
//  LookOut
//
//  Created by Pavel Ignatov on 24.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "LOSounds.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"

const bool useMp3Files = true;

#ifdef __ANDROID__

#include "SunnyOpenAL.h"

const short maxSnowflakeSoundCount = 5;
static OSLContext * oslContext = 0;

//static SunnyALBuffer * bufRollingSnowBallLooped = 0;
//static SunnyALBuffer * bufFireBallLaunch = 0;
//static SunnyALBuffer * bufAngryBallLaunch = 0;
//static SunnyALBuffer * bufAngryBallDig = 0;
const short losExplosionsCount = 12;//FS,FF,FG,FW, GS,GG,GW, WS,WW , SS , E, RobotPreRush
//static SunnyALBuffer * bufExplosion[losExplosionsCount];
//static SunnyALBuffer * bufSnowflakePickUp[maxSnowflakeSoundCount];

const short losLoopedSoundsCount = 12;//fire,ground,water,bonfire,eField,AngryBall,ABLocked,RotatingBlades,MovingBlades,eAttack,AngryRush,ElectricBall
//static SunnyALBuffer * bufLoopedSounds[losLoopedSoundsCount];
const short losMaxAllLoopedSoundsCount = 4;
static short losAllLoopedSounds[losMaxAllLoopedSoundsCount];
static short losAllLoopedSoundsCount = 0;

//static OSLSound *losRollingSnowBallLooped = 0;

static OSLSound * rollingSnowBallSource = 0;
static OSLSound * snowballAndBonfire = 0;
static OSLSound * levelCompleteSource = 0;
static OSLSound * buttonClickSound = 0;
static OSLSound * trapSound = 0;
static OSLSound * snowBallAndBlades = 0;
static OSLSound * iceBlockMoved = 0;
static OSLSound * iceBlockAndIceBlock = 0;

//New Sounds
const short losSpellSoundsCount = 8;//Shield, Life, Speed, Teleport
const short losMagicBoxCount = 3;
static OSLSound * spellSounds[losSpellSoundsCount];
static OSLSound * deathSound = 0;
static OSLSound * goldCoinSound = 0;
static OSLSound * starSound = 0;
static OSLSound * dropSound = 0;
static OSLSound * iceTapSound[2];
static OSLSound * iceBreakSound = 0;
static OSLSound * magicBoxOpenSound[losMagicBoxCount];
static OSLSound * puzzlePartSound[2];
//New Sounds...

float soundsVolumeKoef = 1;

struct LOSObjectSound
{
    void *shapePointer;
    float distance;
    OSLSound * source;
};

const short maxObjectSoundCount = 1;
const short losMinDistance = 1;
const short losMaxDistance = 30;

struct LORobotSound
{
    void *pointer;
    double offset;
} losRobotSounds[maxObjectSoundCount];

LORobotSound losMovingBladesSounds[maxObjectSoundCount];

static LOSObjectSound *losLoopedSounds[losLoopedSoundsCount][maxObjectSoundCount];
static LOSObjectSound *losFireObjectsLaunch[maxObjectSoundCount];
static LOSObjectSound *losAngryObjectsLaunch[maxObjectSoundCount];
static LOSObjectSound *losAngryObjectsDig[maxObjectSoundCount];
static LOSObjectSound *losExplosion[losExplosionsCount][maxObjectSoundCount];
static LOSObjectSound *losSnowflakePickUp[maxSnowflakeSoundCount];

short losCurrentLoopedObjects[losLoopedSoundsCount];
short losLastLoopedObjects[losLoopedSoundsCount];

short losCurrentFireBallLauchCount = 0;
short losCurrentAngryBallLauchCount = 0;
short losCurrentAngryBallDigCount = 0;

float losPiDuration = 0;
short iceBlocksDuration = 0;

float volumeForDist(float a)
{
    a = (200./(a*a) - 200./(losMaxDistance*losMaxDistance));
    if (a>2) a = 2;
    a*=soundsVolumeKoef;
    return a;
}

const char * getFormat(const char * s)
{
#ifdef __ANDROID__
    	return "wav";
#endif
    if (useMp3Files)
        return "mp3";

    return s;
}

OSLSound * hardSound = 0;

void losPrepareHardSound()
{
    if (!hardSound)
    {
        hardSound = new OSLWav(oslContext);
        hardSound->load((char * )getPath("Base/Sounds/ButtonClickV_1", getFormat("aiff")));
        hardSound->setVolume(soundsVolumeKoef);
    }
}



void playTestSound2(OSLSound *source)
{
    source->setLooping(false);
    source->setVolume(1);
    source->play();
}

extern AAssetManager *mgr;
void losLoadAllSounds()
{
//    if (bufRollingSnowBallLooped) return;

    if (!oslContext)
    	oslContext = new OSLContext(mgr);



    rollingSnowBallSource = new OSLWav(oslContext);
    rollingSnowBallSource->load((char *)getPath("Base/Sounds/SnowballRollV_5",getFormat("aiff")));
	rollingSnowBallSource->setLooping(true);

    snowballAndBonfire = new OSLWav(oslContext);
    snowballAndBonfire->load((char *)getPath("Base/Sounds/Snowball+BonfireV1",getFormat("aiff")));

    levelCompleteSource = new OSLWav(oslContext);
    levelCompleteSource->load((char *)getPath("Base/Sounds/LevelComplete", getFormat("mp3")));

    buttonClickSound = new OSLWav(oslContext);
    buttonClickSound->load((char *)getPath("Base/Sounds/ButtonClickV_3", getFormat("aiff")));

    trapSound = new OSLWav(oslContext);
    trapSound->load((char *)getPath("Base/Sounds/TrappedV_1", getFormat("aiff")));

    snowBallAndBlades = new OSLWav(oslContext);
    snowBallAndBlades->load((char *)getPath("Base/Sounds/Snowball&BladeV_1", getFormat("aiff")));

    iceBlockMoved = new OSLWav(oslContext);
    iceBlockMoved->load((char *)getPath("Base/Sounds/MoveIceBlockV_2", getFormat("aiff")));

    iceBlockAndIceBlock = new OSLWav(oslContext);
    iceBlockAndIceBlock->load((char *)getPath("Base/Sounds/IceBlockintoIceBlockV_1", getFormat("aiff")));

    //New Sounds
    deathSound = new OSLWav(oslContext);
    deathSound->load((char *)getPath("Base/Sounds/GameOverV_1", getFormat("wav")));
    goldCoinSound = new OSLWav(oslContext);
    goldCoinSound->load((char *)getPath("Base/Sounds/GoldCoinsV_1", getFormat("wav")));

    starSound = new OSLWav(oslContext);
    starSound->load((char *)getPath("Base/Sounds/SnowflakePickUpV_2", getFormat("wav")));

    spellSounds[0] = new OSLWav(oslContext);
    spellSounds[0]->load((char *)getPath("Base/Sounds/ShieldActivatedV_1", getFormat("wav")));
    spellSounds[1] = new OSLWav(oslContext);
    spellSounds[1]->load((char *)getPath("Base/Sounds/ShieldDestroyedV_1", getFormat("wav")));

    spellSounds[2] = new OSLWav(oslContext);
    spellSounds[2]->load((char *)getPath("Base/Sounds/LifeAbilityUsedV_1", getFormat("wav")));
    spellSounds[3] = new OSLWav(oslContext);
    spellSounds[3]->load((char *)getPath("Base/Sounds/LifeShieldDestroyedV_1", getFormat("wav")));

    spellSounds[4] = new OSLWav(oslContext);
    spellSounds[4]->load((char *)getPath("Base/Sounds/SpeedAbilityUsedV_1", getFormat("wav")));
    spellSounds[5] = new OSLWav(oslContext);
    spellSounds[5]->load((char *)getPath("Base/Sounds/SpeedAbilityEndedV_1", getFormat("wav")));

    spellSounds[6] = new OSLWav(oslContext);
    spellSounds[6]->load((char *)getPath("Base/Sounds/TeleportActivatedV_1", getFormat("wav")));
    spellSounds[7] = new OSLWav(oslContext);
    spellSounds[7]->load((char *)getPath("Base/Sounds/TeleportV_1", getFormat("wav")));

    deathSound->setVolume(3*soundsVolumeKoef);
    for (short i = 0;i<losSpellSoundsCount;i++)
        spellSounds[i]->setVolume(3*soundsVolumeKoef);

    dropSound = new OSLWav(oslContext);
    dropSound->load((char *)getPath("Base/Sounds/MagicItemV_1", getFormat("wav")));
    iceTapSound[0] = new OSLWav(oslContext);
    iceTapSound[0] ->load((char *)getPath("Base/Sounds/IceCrack1V_1", getFormat("wav")));
    iceTapSound[1] = new OSLWav(oslContext);
    iceTapSound[1]->load((char *)getPath("Base/Sounds/IceCrack2V_1", getFormat("wav")));
    iceBreakSound = new OSLWav(oslContext);
    iceBreakSound->load((char *)getPath("Base/Sounds/IceDestroyedV_1", getFormat("wav")));
    magicBoxOpenSound[0] = new OSLWav(oslContext);
    magicBoxOpenSound[0]->load((char *)getPath("Base/Sounds/MagicBox1V_1", getFormat("wav")));
    magicBoxOpenSound[1] = new OSLWav(oslContext);
    magicBoxOpenSound[1]->load((char *)getPath("Base/Sounds/MagicBox2V_1", getFormat("wav")));
    magicBoxOpenSound[2] = new OSLWav(oslContext);
    magicBoxOpenSound[2]->load((char *)getPath("Base/Sounds/MagicBox3V_1", getFormat("wav")));
    puzzlePartSound[0] = new OSLWav(oslContext);
    puzzlePartSound[0]->load((char *)getPath("Base/Sounds/PuzzleDroppedV_1", getFormat("wav")));
    puzzlePartSound[1] = new OSLWav(oslContext);
    puzzlePartSound[1]->load( (char *)getPath("Base/Sounds/PuzzleConnectedV_1", getFormat("wav")));
    //New Sounds...
//    return;
    for (short j = 0;j<losLoopedSoundsCount;j++)
    for (short i = 0;i<maxObjectSoundCount;i++){
        losLoopedSounds[j][i] = new LOSObjectSound;
        losLoopedSounds[j][i]->source = new OSLWav(oslContext);
//        losLoopedSounds[j][i]->source->loadFromBuffer(bufLoopedSounds[j]);
        losLoopedSounds[j][i]->source->setLooping(true);
        losLoopedSounds[j][i]->shapePointer = 0;
    }
    losLoopedSounds[0][0]->source->load((char *)getPath("Base/Sounds/FireBallLoopV_3","wav"));
    losLoopedSounds[1][0]->source->load((char *)getPath("Base/Sounds/GroundBallLoopV_2","wav"));
    losLoopedSounds[2][0]->source->load((char *)getPath("Base/Sounds/WaterBallLoopV_5b","wav"));
    losLoopedSounds[3][0]->source->load((char *)getPath("Base/Sounds/BonfireV_1","wav"));
    losLoopedSounds[4][0]->source->load((char *)getPath("Base/Sounds/ElectricFieldV_6","wav"));
    losLoopedSounds[5][0]->source->load((char *)getPath("Base/Sounds/RobotV_5b","wav"));
    losLoopedSounds[6][0]->source->load((char *)getPath("Base/Sounds/RobotElectricalLockingV_3","wav"));
    losLoopedSounds[7][0]->source->load((char *)getPath("Base/Sounds/RotatingBladesV_2B","wav"));
    losLoopedSounds[8][0]->source->load((char *)getPath("Base/Sounds/MovingBladesV_1","wav"));
    losLoopedSounds[9][0]->source->load((char *)getPath("Base/Sounds/ElectricBallAttackV_3","wav"));
    losLoopedSounds[10][0]->source->load((char *)getPath("Base/Sounds/RobotRushV_5b","wav"));
    losLoopedSounds[11][0]->source->load((char *)getPath("Base/Sounds/FlyingElectricBallV_6D","wav"));
    for (short j = 0;j<losLoopedSoundsCount;j++)
        for (short i = 0;i<maxObjectSoundCount;i++)
        	losLoopedSounds[j][i]->source->setLoop(true);

    rollingSnowBallSource = new OSLWav(oslContext);
    rollingSnowBallSource->load((char *)getPath("Base/Sounds/SnowballRollV_5",getFormat("aiff")));
    rollingSnowBallSource->setLoop(true);

    for (short i = 0;i<maxObjectSoundCount;i++){
        losFireObjectsLaunch[i] = new LOSObjectSound;
        losFireObjectsLaunch[i]->source = new OSLWav(oslContext);
//        losFireObjectsLaunch[i]->source->loadFromBuffer(bufFireBallLaunch);
        losFireObjectsLaunch[i]->source->load((char *)getPath("Base/Sounds/FireballLaunchV_4","wav"));
        losFireObjectsLaunch[i]->shapePointer = 0;

        losAngryObjectsLaunch[i] = new LOSObjectSound;
        losAngryObjectsLaunch[i]->source =new OSLWav(oslContext);
        losAngryObjectsLaunch[i]->source->load((char *)getPath("Base/Sounds/RobotChargeV_4","wav"));
        losAngryObjectsLaunch[i]->shapePointer = 0;

        losAngryObjectsDig[i] = new LOSObjectSound;
        losAngryObjectsDig[i]->source = new OSLWav(oslContext);
        losAngryObjectsDig[i]->source->load((char *)getPath("Base/Sounds/RobotDigsV_2","wav"));
        losAngryObjectsDig[i]->shapePointer = 0;
    }

    for (short j = 0;j<losExplosionsCount;j++)
    for (short i = 0;i<maxObjectSoundCount;i++) {
        losExplosion[j][i] = new LOSObjectSound;
        losExplosion[j][i]->source = new OSLWav(oslContext);
//        losExplosion[j][i]->source->loadFromBuffer(bufExplosion[j]);
        losExplosion[j][i]->shapePointer = 0;
    }
    losExplosion[0][0]->source->load((char *)getPath("Base/Sounds/Fireball&SnowballWithDebrisV_1","wav"));
    losExplosion[1][0]->source->load((char *)getPath("Base/Sounds/Fireball&FireballWDebrisV_4","wav"));
    losExplosion[2][0]->source->load((char *)getPath("Base/Sounds/Fireball&GroundBallWDebrisV_2","wav"));
    losExplosion[3][0]->source->load((char *)getPath("Base/Sounds/Fireball&WaterballWDebrisV_1","wav"));
    losExplosion[4][0]->source->load((char *)getPath("Base/Sounds/GroundBall&SnowBallWDebrisV_2","wav"));
    losExplosion[5][0]->source->load((char *)getPath("Base/Sounds/Groundball&GroundballWDebrisV_1","wav"));
    losExplosion[6][0]->source->load((char *)getPath("Base/Sounds/Groundball&WaterballWDebrisandRollV_1","wav"));
    losExplosion[7][0]->source->load((char *)getPath("Base/Sounds/Waterball&SnowballWDebrisV_1","wav"));
    losExplosion[8][0]->source->load((char *)getPath("Base/Sounds/Waterball&WaterballWDebrisV_2","wav"));
    losExplosion[9][0]->source->load((char *)getPath("Base/Sounds/SnowWallExplodeV_1","wav"));
    losExplosion[10][0]->source->load((char *)getPath("Base/Sounds/ElectricBallExplodesObjectV_2b","wav"));
    losExplosion[11][0]->source->load((char *)getPath("Base/Sounds/RobotRushLaunchV_4","wav"));

    for (short i = 0;i<maxSnowflakeSoundCount;i++)
    {
        losSnowflakePickUp[i] = new LOSObjectSound;
        losSnowflakePickUp[i]->source = new OSLWav(oslContext);
//        losSnowflakePickUp[i]->source->loadFromBuffer(bufSnowflakePickUp[i]);
        losSnowflakePickUp[i]->shapePointer = 0;
    }
    losSnowflakePickUp[0]->source->load((char *)getPath("Base/Sounds/SnowFlakePickUPV_4","wav"));
    losSnowflakePickUp[1]->source->load((char *)getPath("Base/Sounds/SnowFlakePickUPV_5","wav"));
    losSnowflakePickUp[2]->source->load((char *)getPath("Base/Sounds/SnowFlakePickUPV_6","wav"));
    losSnowflakePickUp[3]->source->load((char *)getPath("Base/Sounds/RobotV_1","wav"));
    losSnowflakePickUp[4]->source->load((char *)getPath("Base/Sounds/SnowflakePickUpV_3","wav"));

//    ALint bufferSize, frequency, bitsPerSample, channels;
//    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_SIZE, &bufferSize);
//    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_FREQUENCY, &frequency);
//    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_CHANNELS, &channels);
//    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_BITS, &bitsPerSample);
//    losPiDuration = ((double)bufferSize)/(frequency*channels*(bitsPerSample/8));
    losPiDuration = ((double) ((OSLWav*)losLoopedSounds[5][0]->source)->header.dataSize)/
    		(((OSLWav*)losLoopedSounds[5][0]->source)->header.bytesPerSec);

    losPrepareHardSound();

    losChangeSoundVolume(soundsVolumeKoef);
//    LOGD("hm");
}








void losClearAllSounds()
{
	LOGD("losClearAllSounds()");
    delete starSound;
    delete goldCoinSound;
    delete deathSound;

    for (short i = 0;i<losSpellSoundsCount;i++)
        delete spellSounds[i];

    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        delete losFireObjectsLaunch[i]->source;
        delete losFireObjectsLaunch[i];

        delete losAngryObjectsLaunch[i]->source;
        delete losAngryObjectsLaunch[i];

        delete losAngryObjectsDig[i]->source;
        delete losAngryObjectsDig[i];

        for (short j = 0;j<losLoopedSoundsCount;j++)
        {
            delete losLoopedSounds[j][i]->source;
            delete losLoopedSounds[j][i];
        }
    }


    delete rollingSnowBallSource;
    delete snowballAndBonfire;

    delete oslContext;

    delete buttonClickSound;
    delete trapSound;
    delete snowBallAndBlades;
    delete iceBlockMoved;
    delete iceBlockAndIceBlock;
}

void losPauseSounds(){
	LOGD("losPauseSounds()");
	if(oslContext != NULL)
		oslContext->pause();
}

void losContinueSounds(){
	LOGD("losContinueSounds()");
	if(oslContext != NULL)
		oslContext->resume();
}
#else
#include "SunnyOpenAL.h"

const short maxSnowflakeSoundCount = 5;
static SunnyALContext * alContext = 0;

static SunnyALBuffer * bufRollingSnowBallLooped = 0;
static SunnyALBuffer * bufFireBallLaunch = 0;
static SunnyALBuffer * bufAngryBallLaunch = 0;
static SunnyALBuffer * bufAngryBallDig = 0;
const short losExplosionsCount = 12;//FS,FF,FG,FW, GS,GG,GW, WS,WW , SS , E, RobotPreRush
static SunnyALBuffer * bufExplosion[losExplosionsCount];
static SunnyALBuffer * bufSnowflakePickUp[maxSnowflakeSoundCount];

const short losLoopedSoundsCount = 12;//fire,ground,water,bonfire,eField,AngryBall,ABLocked,RotatingBlades,MovingBlades,eAttack,AngryRush,ElectricBall
static SunnyALBuffer * bufLoopedSounds[losLoopedSoundsCount];
const short losMaxAllLoopedSoundsCount = 4;
static short losAllLoopedSounds[losMaxAllLoopedSoundsCount];
static short losAllLoopedSoundsCount = 0;

static SunnyALSource * rollingSnowBallSource = 0;
static SunnyALSource * snowballAndBonfire = 0;
static SunnyALSource * levelCompleteSource = 0;
static SunnyALSource * buttonClickSound = 0;
static SunnyALSource * trapSound = 0;
static SunnyALSource * snowBallAndBlades = 0;
static SunnyALSource * iceBlockMoved = 0;
static SunnyALSource * iceBlockAndIceBlock = 0;

//New Sounds
const short losSpellSoundsCount = 8;//Shield, Life, Speed, Teleport
const short losMagicBoxCount = 3;
static SunnyALSource * spellSounds[losSpellSoundsCount];
static SunnyALSource * deathSound = 0;
static SunnyALSource * goldCoinSound = 0;
static SunnyALSource * starSound = 0;
static SunnyALSource * dropSound = 0;
static SunnyALSource * iceTapSound[2];
static SunnyALSource * iceBreakSound = 0;
static SunnyALSource * magicBoxOpenSound[losMagicBoxCount];
static SunnyALSource * puzzlePartSound[2];
//New Sounds...

float soundsVolumeKoef = 1;

struct LOSObjectSound
{
    void *shapePointer;
    float distance;
    SunnyALSource * source;
};

const short maxObjectSoundCount = 1;
const short losMinDistance = 1;
const short losMaxDistance = 30;

struct LORobotSound
{
    void *pointer;
    double offset;
} losRobotSounds[maxObjectSoundCount];

LORobotSound losMovingBladesSounds[maxObjectSoundCount];

static LOSObjectSound *losLoopedSounds[losLoopedSoundsCount][maxObjectSoundCount];
static LOSObjectSound *losFireObjectsLaunch[maxObjectSoundCount];
static LOSObjectSound *losAngryObjectsLaunch[maxObjectSoundCount];
static LOSObjectSound *losAngryObjectsDig[maxObjectSoundCount];
static LOSObjectSound *losExplosion[losExplosionsCount][maxObjectSoundCount];
static LOSObjectSound *losSnowflakePickUp[maxSnowflakeSoundCount];

short losCurrentLoopedObjects[losLoopedSoundsCount];
short losLastLoopedObjects[losLoopedSoundsCount];

short losCurrentFireBallLauchCount = 0;
short losCurrentAngryBallLauchCount = 0;
short losCurrentAngryBallDigCount = 0;

float losPiDuration = 0;
short iceBlocksDuration = 0;

float volumeForDist(float a)
{
    a = (200./(a*a) - 200./(losMaxDistance*losMaxDistance));
    if (a>2) a = 2;
    a*=soundsVolumeKoef;
    return a;
}

const char * getFormat(const char * s)
{
#ifdef __ANDROID__
    	return "wav";
#endif
    if (useMp3Files)
        return "mp3";

    return s;
}

SunnyALSource * hardSound = 0;

void losPrepareHardSound()
{
    if (!hardSound)
    {
        hardSound = new SunnyALSource;
        hardSound->loadFromFile(getPath("Base/Sounds/ButtonClickV_1", getFormat("aiff")));
        hardSound->setVolume(soundsVolumeKoef);
    }
}


void playTestSound2(SunnyALSource *source)
{
    source->setLooping(false);
    source->setVolume(1);
    source->play();
}



void losLoadAllSounds()
{
    if (bufRollingSnowBallLooped) return;

    if (!alContext)
        alContext = new SunnyALContext;

    //Looped
    bufLoopedSounds[0] = new SunnyALBuffer;
    bufLoopedSounds[0]->loadFromFile(getPath("Base/Sounds/FireBallLoopV_3","wav"));
    bufLoopedSounds[1] = new SunnyALBuffer;
    bufLoopedSounds[1]->loadFromFile(getPath("Base/Sounds/GroundBallLoopV_2","wav"));
    bufLoopedSounds[2] = new SunnyALBuffer;
    bufLoopedSounds[2]->loadFromFile(getPath("Base/Sounds/WaterBallLoopV_5b","wav"));
    bufLoopedSounds[3] = new SunnyALBuffer;
    bufLoopedSounds[3]->loadFromFile(getPath("Base/Sounds/BonfireV_1",getFormat("aiff")));
    bufLoopedSounds[4] = new SunnyALBuffer;
    bufLoopedSounds[4]->loadFromFile(getPath("Base/Sounds/ElectricFieldV_6",getFormat("aiff")));
    bufLoopedSounds[5] = new SunnyALBuffer;
    bufLoopedSounds[5]->loadFromFile(getPath("Base/Sounds/RobotV_5b",getFormat("aiff")));
    bufLoopedSounds[6] = new SunnyALBuffer;
    bufLoopedSounds[6]->loadFromFile(getPath("Base/Sounds/RobotElectricalLockingV_3",getFormat("aiff")));
    bufLoopedSounds[7] = new SunnyALBuffer;
    bufLoopedSounds[7]->loadFromFile(getPath("Base/Sounds/RotatingBladesV_2B",getFormat("wav")));
    bufLoopedSounds[8] = new SunnyALBuffer;
    bufLoopedSounds[8]->loadFromFile(getPath("Base/Sounds/MovingBladesV_1",getFormat("aiff")));
    bufLoopedSounds[9] = new SunnyALBuffer;
    bufLoopedSounds[9]->loadFromFile(getPath("Base/Sounds/ElectricBallAttackV_3",getFormat("aiff")));
    bufLoopedSounds[10] = new SunnyALBuffer;
    bufLoopedSounds[10]->loadFromFile(getPath("Base/Sounds/RobotRushV_5b",getFormat("aiff")));
    bufLoopedSounds[11] = new SunnyALBuffer;
    bufLoopedSounds[11]->loadFromFile(getPath("Base/Sounds/FlyingElectricBallV_6D",getFormat("aiff")));
    //...
    
    bufRollingSnowBallLooped = new SunnyALBuffer;
     bufRollingSnowBallLooped->loadFromFile(getPath("Base/Sounds/SnowballRollV_5",getFormat("aiff")));
    
    bufFireBallLaunch = new SunnyALBuffer;
    bufFireBallLaunch->loadFromFile(getPath("Base/Sounds/FireballLaunchV_4",getFormat("aiff")));
    bufAngryBallLaunch = new SunnyALBuffer;
    bufAngryBallLaunch->loadFromFile(getPath("Base/Sounds/RobotChargeV_4",getFormat("aiff")));
    bufAngryBallDig = new SunnyALBuffer;
    bufAngryBallDig->loadFromFile(getPath("Base/Sounds/RobotDigsV_2",getFormat("aiff")));
    
    bufExplosion[0] = new SunnyALBuffer;
    bufExplosion[0]->loadFromFile(getPath("Base/Sounds/Fireball&SnowballWithDebrisV_1",getFormat("aiff")));
    bufExplosion[1] = new SunnyALBuffer;
    bufExplosion[1]->loadFromFile(getPath("Base/Sounds/Fireball&FireballWDebrisV_4",getFormat("aif")));
    bufExplosion[2] = new SunnyALBuffer;
    bufExplosion[2]->loadFromFile(getPath("Base/Sounds/Fireball&GroundBallWDebrisV_2",getFormat("aif")));
    bufExplosion[3] = new SunnyALBuffer;
    bufExplosion[3]->loadFromFile(getPath("Base/Sounds/Fireball&WaterballWDebrisV_1",getFormat("aiff")));
    bufExplosion[4] = new SunnyALBuffer;
    bufExplosion[4]->loadFromFile(getPath("Base/Sounds/GroundBall&SnowBallWDebrisV_2",getFormat("aif")));
    bufExplosion[5] = new SunnyALBuffer;
    bufExplosion[5]->loadFromFile(getPath("Base/Sounds/Groundball&GroundballWDebrisV_1",getFormat("aif")));
    bufExplosion[6] = new SunnyALBuffer;
    bufExplosion[6]->loadFromFile(getPath("Base/Sounds/Groundball&WaterballWDebrisandRollV_1",getFormat("aif")));
    bufExplosion[7] = new SunnyALBuffer;
    bufExplosion[7]->loadFromFile(getPath("Base/Sounds/Waterball&SnowballWDebrisV_1",getFormat("aif")));
    bufExplosion[8] = new SunnyALBuffer;
    bufExplosion[8]->loadFromFile(getPath("Base/Sounds/Waterball&WaterballWDebrisV_2",getFormat("aif")));
    bufExplosion[9] = new SunnyALBuffer;
    bufExplosion[9]->loadFromFile(getPath("Base/Sounds/SnowWallExplodeV_1",getFormat("aiff")));
    bufExplosion[10] = new SunnyALBuffer;
    bufExplosion[10]->loadFromFile(getPath("Base/Sounds/ElectricBallExplodesObjectV_2b",getFormat("aiff")));
    bufExplosion[11] = new SunnyALBuffer;
    bufExplosion[11]->loadFromFile(getPath("Base/Sounds/RobotRushLaunchV_4",getFormat("aiff")));
    
    bufSnowflakePickUp[0] = new SunnyALBuffer;
    bufSnowflakePickUp[0]->loadFromFile(getPath("Base/Sounds/SnowFlakePickUPV_4",getFormat("wav")));
    bufSnowflakePickUp[1] = new SunnyALBuffer;
    bufSnowflakePickUp[1]->loadFromFile(getPath("Base/Sounds/SnowFlakePickUPV_5",getFormat("wav")));
    bufSnowflakePickUp[2] = new SunnyALBuffer;
    bufSnowflakePickUp[2]->loadFromFile(getPath("Base/Sounds/SnowFlakePickUPV_6",getFormat("wav")));
    bufSnowflakePickUp[3] = new SunnyALBuffer;
    bufSnowflakePickUp[3]->loadFromFile(getPath("Base/Sounds/RobotV_1",getFormat("aiff")));
    bufSnowflakePickUp[4] = new SunnyALBuffer;
    bufSnowflakePickUp[4]->loadFromFile(getPath("Base/Sounds/SnowflakePickUpV_3",getFormat("aif")));
    
    rollingSnowBallSource = new SunnyALSource;
    rollingSnowBallSource->loadFromBuffer(bufRollingSnowBallLooped);
	rollingSnowBallSource->setLooping(true);
    
    snowballAndBonfire = new SunnyALSource;
    snowballAndBonfire->loadFromFile(getPath("Base/Sounds/Snowball+BonfireV1",getFormat("aiff")));
    
    levelCompleteSource = new SunnyALSource;
    levelCompleteSource->loadFromFile(getPath("Base/Sounds/LevelComplete", getFormat("mp3")));
    
    buttonClickSound = new SunnyALSource;
    buttonClickSound->loadFromFile(getPath("Base/Sounds/ButtonClickV_3", getFormat("aiff")));
    
    trapSound = new SunnyALSource;
    trapSound->loadFromFile(getPath("Base/Sounds/TrappedV_1", getFormat("aiff")));
    
    snowBallAndBlades = new SunnyALSource;
    snowBallAndBlades->loadFromFile(getPath("Base/Sounds/Snowball&BladeV_1", getFormat("aiff")));
    
    iceBlockMoved = new SunnyALSource;
    iceBlockMoved->loadFromFile(getPath("Base/Sounds/MoveIceBlockV_2", getFormat("aiff")));

    iceBlockAndIceBlock = new SunnyALSource;
    iceBlockAndIceBlock->loadFromFile(getPath("Base/Sounds/IceBlockintoIceBlockV_1", getFormat("aiff")));
    
    //New Sounds
    deathSound = new SunnyALSource;
    deathSound->loadFromFile(getPath("Base/Sounds/GameOverV_1", getFormat("wav")));
    goldCoinSound = new SunnyALSource;
    goldCoinSound->loadFromFile(getPath("Base/Sounds/GoldCoinsV_1", getFormat("wav")));
    
    starSound = new SunnyALSource;
    starSound->loadFromFile(getPath("Base/Sounds/SnowflakePickUpV_2", getFormat("wav")));
    
    spellSounds[0] = new SunnyALSource;
    spellSounds[0]->loadFromFile(getPath("Base/Sounds/ShieldActivatedV_1", getFormat("wav")));
    spellSounds[1] = new SunnyALSource;
    spellSounds[1]->loadFromFile(getPath("Base/Sounds/ShieldDestroyedV_1", getFormat("wav")));
    
    spellSounds[2] = new SunnyALSource;
    spellSounds[2]->loadFromFile(getPath("Base/Sounds/LifeAbilityUsedV_1", getFormat("wav")));
    spellSounds[3] = new SunnyALSource;
    spellSounds[3]->loadFromFile(getPath("Base/Sounds/LifeShieldDestroyedV_1", getFormat("wav")));
    
    spellSounds[4] = new SunnyALSource;
    spellSounds[4]->loadFromFile(getPath("Base/Sounds/SpeedAbilityUsedV_1", getFormat("wav")));
    spellSounds[5] = new SunnyALSource;
    spellSounds[5]->loadFromFile(getPath("Base/Sounds/SpeedAbilityEndedV_1", getFormat("wav")));
    
    spellSounds[6] = new SunnyALSource;
    spellSounds[6]->loadFromFile(getPath("Base/Sounds/TeleportActivatedV_1", getFormat("wav")));
    spellSounds[7] = new SunnyALSource;
    spellSounds[7]->loadFromFile(getPath("Base/Sounds/TeleportV_1", getFormat("wav")));
    
    deathSound->setVolume(3*soundsVolumeKoef);
    for (short i = 0;i<losSpellSoundsCount;i++)
        spellSounds[i]->setVolume(3*soundsVolumeKoef);
    
    dropSound = new SunnyALSource(getPath("Base/Sounds/MagicItemV_1", getFormat("wav")));
    iceTapSound[0] = new SunnyALSource(getPath("Base/Sounds/IceCrack1V_1", getFormat("wav")));
    iceTapSound[1] = new SunnyALSource(getPath("Base/Sounds/IceCrack2V_1", getFormat("wav")));
    iceBreakSound = new SunnyALSource(getPath("Base/Sounds/IceDestroyedV_1", getFormat("wav")));
    magicBoxOpenSound[0] = new SunnyALSource(getPath("Base/Sounds/MagicBox1V_1", getFormat("wav")));
    magicBoxOpenSound[1] = new SunnyALSource(getPath("Base/Sounds/MagicBox2V_1", getFormat("wav")));
    magicBoxOpenSound[2] = new SunnyALSource(getPath("Base/Sounds/MagicBox3V_1", getFormat("wav")));
    puzzlePartSound[0] = new SunnyALSource(getPath("Base/Sounds/PuzzleDroppedV_1", getFormat("wav")));
    puzzlePartSound[1] = new SunnyALSource(getPath("Base/Sounds/PuzzleConnectedV_1", getFormat("wav")));
    //New Sounds...
    
    for (short j = 0;j<losLoopedSoundsCount;j++)
    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        losLoopedSounds[j][i] = new LOSObjectSound;
        losLoopedSounds[j][i]->source = new SunnyALSource;
        losLoopedSounds[j][i]->source->loadFromBuffer(bufLoopedSounds[j]);
        losLoopedSounds[j][i]->source->setLooping(true);
        losLoopedSounds[j][i]->shapePointer = 0;
    }
    
    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        losFireObjectsLaunch[i] = new LOSObjectSound;
        losFireObjectsLaunch[i]->source = new SunnyALSource;
        losFireObjectsLaunch[i]->source->loadFromBuffer(bufFireBallLaunch);
        losFireObjectsLaunch[i]->shapePointer = 0;
        
        losAngryObjectsLaunch[i] = new LOSObjectSound;
        losAngryObjectsLaunch[i]->source = new SunnyALSource;
        losAngryObjectsLaunch[i]->source->loadFromBuffer(bufAngryBallLaunch);
        losAngryObjectsLaunch[i]->shapePointer = 0;
        
        losAngryObjectsDig[i] = new LOSObjectSound;
        losAngryObjectsDig[i]->source = new SunnyALSource;
        losAngryObjectsDig[i]->source->loadFromBuffer(bufAngryBallDig);
        losAngryObjectsDig[i]->shapePointer = 0;
    }
    
    for (short j = 0;j<losExplosionsCount;j++)
    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        losExplosion[j][i] = new LOSObjectSound;
        losExplosion[j][i]->source = new SunnyALSource;
        losExplosion[j][i]->source->loadFromBuffer(bufExplosion[j]);
        losExplosion[j][i]->shapePointer = 0;
    }
    
    for (short i = 0;i<maxSnowflakeSoundCount;i++)
    {
        losSnowflakePickUp[i] = new LOSObjectSound;
        losSnowflakePickUp[i]->source = new SunnyALSource;
        losSnowflakePickUp[i]->source->loadFromBuffer(bufSnowflakePickUp[i]);
        losSnowflakePickUp[i]->shapePointer = 0;
    }
    
    ALint bufferSize, frequency, bitsPerSample, channels;
    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_SIZE, &bufferSize);
    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_FREQUENCY, &frequency);
    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_CHANNELS, &channels);    
    alGetBufferi(bufLoopedSounds[5]->alcBuffer, AL_BITS, &bitsPerSample);
    losPiDuration = ((double)bufferSize)/(frequency*channels*(bitsPerSample/8));
    
    losPrepareHardSound();
    
    losChangeSoundVolume(soundsVolumeKoef);
}


void losClearAllSounds()
{
    delete starSound;
    delete goldCoinSound;
    delete deathSound;

    for (short i = 0;i<losSpellSoundsCount;i++)
        delete spellSounds[i];

    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        delete losFireObjectsLaunch[i]->source;
        delete losFireObjectsLaunch[i];

        delete losAngryObjectsLaunch[i]->source;
        delete losAngryObjectsLaunch[i];

        delete losAngryObjectsDig[i]->source;
        delete losAngryObjectsDig[i];

        for (short j = 0;j<losLoopedSoundsCount;j++)
        {
            delete losLoopedSounds[j][i]->source;
            delete losLoopedSounds[j][i];
        }
    }

    for (short j = 0;j<losLoopedSoundsCount;j++)
        delete bufLoopedSounds[j];
    delete bufRollingSnowBallLooped;
    delete bufFireBallLaunch;
    delete bufAngryBallLaunch;
    delete bufAngryBallDig;
    for (short i = 0;i<losExplosionsCount;i++)
        delete bufExplosion[i];

    for (short i = 0;i<maxSnowflakeSoundCount;i++)
        delete bufSnowflakePickUp[i];

    delete rollingSnowBallSource;
    delete snowballAndBonfire;

    delete alContext;

    delete buttonClickSound;
    delete trapSound;
    delete snowBallAndBlades;
    delete iceBlockMoved;
    delete iceBlockAndIceBlock;
}

void losPauseSounds()
{
    if (alContext)
    {
        alcMakeContextCurrent(0);
        alContext->suspend();
    }
}

void losContinueSounds()
{
    if (alContext)
    {
        alContext->makeCurrent();
        alContext->process();
    }
}
#endif

void losPlayHardSound()
{
    if (hardSound)
        hardSound->play();
}

void losRemoveHardSound()
{
    delete hardSound;
    hardSound = 0;
}

void playTestSound(short num)
{
    if (num<losLoopedSoundsCount)
    {
        playTestSound2(losLoopedSounds[num][0]->source);
    } else
        if (num==losLoopedSoundsCount)
        {
            playTestSound2(losFireObjectsLaunch[0]->source);
        } else
            if (num==losLoopedSoundsCount+1)
            {
                playTestSound2(losAngryObjectsLaunch[0]->source);
            } else
                if (num==losLoopedSoundsCount+2)
                {
                    playTestSound2(losAngryObjectsDig[0]->source);
                } else
                {
                    num-=losLoopedSoundsCount+3;
                    if (num<losExplosionsCount)
                        playTestSound2(losExplosion[num][0]->source);
                    else//27
                    {
                        num-=losExplosionsCount;
                        if (num<maxSnowflakeSoundCount)
                            playTestSound2(losSnowflakePickUp[num]->source);
                        else//31
                        {
                            num-=maxSnowflakeSoundCount;
                            if (num==0)
                            {
                                losPrepareHardSound();
                                losPlayHardSound();
                            } else
                                if (num==1)
                                    playTestSound2(rollingSnowBallSource);
                                else
                                    if (num==2)
                                        playTestSound2(snowballAndBonfire);
                                    else
                                        if (num==3)
                                            playTestSound2(levelCompleteSource);
                                        else
                                            if (num==4)
                                                playTestSound2(buttonClickSound);
                                            else
                                                if (num==5)
                                                    playTestSound2(trapSound);
                                                else
                                                    if (num==6)
                                                        playTestSound2(snowBallAndBlades);
                                                    else
                                                        if (num==7)
                                                            playTestSound2(iceBlockMoved);
                                                        else
                                                            if (num==8)
                                                                playTestSound2(iceBlockAndIceBlock);
                        }
                    }
                }
}

void losPlayDropSound()
{
    dropSound->stop();
    dropSound->play();
}

void losPlayIceTapSound()
{
    short num = sRandomi()%2;
    iceTapSound[num]->stop();
    iceTapSound[num]->play();
}

void losPlayIceBreakSound()
{
    iceBreakSound->stop();
    iceBreakSound->play();
}
void losPlayPuzzleDropSound()
{
    puzzlePartSound[0]->stop();
    puzzlePartSound[0]->play();
}

void losPlayPuzzleConnectedSound()
{
    puzzlePartSound[1]->stop();
    puzzlePartSound[1]->play();
}

void losPlayMagicBoxOpenedSound(short num)
{
    magicBoxOpenSound[num]->stop();
    magicBoxOpenSound[num]->play();
}

void losPlaySpellSound(short num)
{
    spellSounds[num]->stop();
    spellSounds[num]->play();
}

void losPlaySnowballDeathSound()
{
    deathSound->play();
}

void losPlayGoldCoinSound()
{
    goldCoinSound->play();
}

void losPlayStarSound(bool forceStop)
{
    if (forceStop)
    {
        starSound->stop();
        starSound->play();
    } else
    if (!starSound->isPlaying())
        starSound->play();
}

void losPreUpdateSounds()
{
    for (short  i = 0;i<losLoopedSoundsCount;i++)
        losCurrentLoopedObjects[i] = 0;
}

void losPlaySnowballBonfire()
{
    snowballAndBonfire->play();
}

void losPlaySnowflakePickUp()
{
    losSnowflakePickUp[0]->source->stop();
    losSnowflakePickUp[0]->source->play();
}

void losPlayAchievementSound()
{
    losSnowflakePickUp[2]->source->stop();
    losSnowflakePickUp[2]->source->play();
}

void losPlayNewSurvivalRecord()
{
    losSnowflakePickUp[3]->source->stop();
    losSnowflakePickUp[3]->source->play();
}

void losChangeSoundVolume(float volume)
{

    soundsVolumeKoef = volume;
    for (short i = 0;i<maxSnowflakeSoundCount;i++)
        losSnowflakePickUp[i]->source->setVolume(soundsVolumeKoef);
    trapSound->setVolume(soundsVolumeKoef*2);
    buttonClickSound->setVolume(soundsVolumeKoef*0.5);
    snowballAndBonfire->setVolume(soundsVolumeKoef);
    levelCompleteSource->setVolume(soundsVolumeKoef);
    snowBallAndBlades->setVolume(soundsVolumeKoef);
    iceBlockMoved->setVolume(soundsVolumeKoef);
    iceBlockAndIceBlock->setVolume(soundsVolumeKoef);

    hardSound->setVolume(soundsVolumeKoef);

    //New Sounds
    for (short i = 0;i<losMagicBoxCount;i++)
        magicBoxOpenSound[i]->setVolume(soundsVolumeKoef);

    for (short i = 0;i<2;i++)
        puzzlePartSound[i]->setVolume(soundsVolumeKoef);

    for (short i = 0;i<2;i++)
        iceTapSound[i]->setVolume(soundsVolumeKoef);

    iceBreakSound->setVolume(soundsVolumeKoef);
    dropSound->setVolume(soundsVolumeKoef);
    starSound->setVolume(soundsVolumeKoef);
    goldCoinSound->setVolume(soundsVolumeKoef);
    deathSound->setVolume(3*soundsVolumeKoef);
    for (short i = 0;i<losSpellSoundsCount;i++)
        spellSounds[i]->setVolume(3*soundsVolumeKoef);
}

void losPlayRessurection()
{
    losSnowflakePickUp[4]->source->stop();
    losSnowflakePickUp[4]->source->play();
}

void losPlayDeathSecond()
{
    losSnowflakePickUp[1]->source->stop();
    losSnowflakePickUp[1]->source->play();
}

void losPlayFinishPoint()
{
    losSnowflakePickUp[1]->source->stop();
    losSnowflakePickUp[1]->source->play();
}

void losPlayLevelComplete()
{
    levelCompleteSource->play();
}

void addExplosionSound(float distance, LOSObjectSound **p, float scaleVolume = 1.0)
{
    if (distance>=losMaxDistance) return;
    if (distance<losMinDistance) distance = losMinDistance;
    
    for (short i = 0;i<maxObjectSoundCount;i++)
        if (!p[i]->source->isPlaying())
        {
            p[i]->source->setVolume(volumeForDist(distance)*scaleVolume);
            p[i]->source->play();
            p[i]->distance = distance;
            return;
        }
    
    for (short i = 0; i<maxObjectSoundCount; i++)
    {
        if (p[i]->distance > distance)
        {
            p[i]->source->stop();
            p[i]->source->setVolume(volumeForDist(distance)*scaleVolume);
            p[i]->source->play();
            p[i]->distance = distance;
            break;
        }
    }
}

void losPlayFSExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[0]);
}

void losPlayFFExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[1],2.0);
}

void losPlayFGExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[2]);
}

void losPlayFWExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[3]);
}

void losPlayGSExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[4]);
}

void losPlayGGExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[5]);
}

void losPlayGWExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[6]);
}

void losPlayWSExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[7]);
}

void losPlayWWExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[8]);
}

void losPlaySSExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[9]);
}

void losPlayEExplosion(float distance)
{
    addExplosionSound(distance, losExplosion[10]);
}

void losPlayAngryPreRush(float distance)
{
    addExplosionSound(distance, losExplosion[11]);
}

void losAddFireBallLaunchSound(void *pointer, float distance)
{
    if (distance>=losMaxDistance) return;
    
    if (losCurrentFireBallLauchCount < maxObjectSoundCount)
    {
        losFireObjectsLaunch[losCurrentFireBallLauchCount]->shapePointer = pointer;
        losFireObjectsLaunch[losCurrentFireBallLauchCount]->distance = distance;
        losFireObjectsLaunch[losCurrentFireBallLauchCount]->source->play();
        losCurrentFireBallLauchCount++;
    } else
    {
        for (short i = 0;i<losCurrentFireBallLauchCount;i++)
            if (losFireObjectsLaunch[i]->distance>distance)
            {
                losFireObjectsLaunch[i]->distance = distance;
                losFireObjectsLaunch[i]->shapePointer = pointer;
                losFireObjectsLaunch[i]->source->stop();
                losFireObjectsLaunch[i]->source->play();
                break;
            }
    }
}

void losAddAngryBallLaunchSound(void *pointer, float distance)
{
    if (distance>=losMaxDistance) return;
    
    if (losCurrentAngryBallLauchCount < maxObjectSoundCount)
    {
        losAngryObjectsLaunch[losCurrentAngryBallLauchCount]->shapePointer = pointer;
        losAngryObjectsLaunch[losCurrentAngryBallLauchCount]->distance = distance;
        losAngryObjectsLaunch[losCurrentAngryBallLauchCount]->source->play();
        losCurrentAngryBallLauchCount++;
    } else
    {
        for (short i = 0;i<losCurrentAngryBallLauchCount;i++)
            if (losAngryObjectsLaunch[i]->distance>distance)
            {
                losAngryObjectsLaunch[i]->distance = distance;
                losAngryObjectsLaunch[i]->shapePointer = pointer;
                losAngryObjectsLaunch[i]->source->stop();
                losAngryObjectsLaunch[i]->source->play();
                break;
            }
    }
}

void losAddAngryBallDigSound(void *pointer, float distance)
{
    if (distance>=losMaxDistance) return;
    
    if (losCurrentAngryBallDigCount < maxObjectSoundCount)
    {
        losAngryObjectsDig[losCurrentAngryBallDigCount]->shapePointer = pointer;
        losAngryObjectsDig[losCurrentAngryBallDigCount]->distance = distance;
        losAngryObjectsDig[losCurrentAngryBallDigCount]->source->play();
        losCurrentAngryBallDigCount++;
    } else
    {
        for (short i = 0;i<losCurrentAngryBallDigCount;i++)
            if (losAngryObjectsDig[i]->distance>distance)
            {
                losAngryObjectsDig[i]->distance = distance;
                losAngryObjectsDig[i]->shapePointer = pointer;
                losAngryObjectsDig[i]->source->stop();
                losAngryObjectsDig[i]->source->play();
                break;
            }
    }
}

short losAddBallSound(void *pointer, float distance, LOSObjectSound **p, short &cur)
{
    if (distance>=losMaxDistance) return -1;
    
    for (short i = cur;i<maxObjectSoundCount;i++)
        if (p[i]->shapePointer == pointer)
        {
            p[i]->distance = distance;
            if (i!=cur)
            {
                LOSObjectSound * buf = p[i];
                p[i] = p[cur];
                p[cur] = buf;
            }
            cur++;
            return cur-1;
        }
    
    if (cur<maxObjectSoundCount)
    {
        p[cur]->shapePointer = pointer;
        p[cur]->distance = distance;
        cur++;
        return cur-1;
    } else
    {
        for (short i = 0;i<maxObjectSoundCount;i++)
        if (p[i]->distance>distance*1.1)
        {
            p[i]->distance = distance;
            p[i]->shapePointer = pointer;
            return i;
            break;
        }
    }
    return -1;
}

void losAddFireBallSound(void *pointer, float distance)
{
    for (short i = 0;i<losCurrentFireBallLauchCount;i++)
    if (losFireObjectsLaunch[i]->shapePointer == pointer)
    {
        losFireObjectsLaunch[i]->distance = distance;
        return;
    }
    losAddBallSound(pointer, distance, losLoopedSounds[0],losCurrentLoopedObjects[0]);
}

void losAddGroundBallSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance, losLoopedSounds[1],losCurrentLoopedObjects[1]);
}

void losAddWaterBallSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance,  losLoopedSounds[2],losCurrentLoopedObjects[2]);
}

void losAddBonfireSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance*3,  losLoopedSounds[3],losCurrentLoopedObjects[3]);
}

bool losIsElectricFieldPlaying(void *pointer)
{
    for (short i = 0;i<losCurrentLoopedObjects[4];i++)
        if (losLoopedSounds[4][i]->shapePointer==pointer)
            return true;
    return false;
}

void losAddElectricFieldSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance*3,  losLoopedSounds[4],losCurrentLoopedObjects[4]);
}

void losAddAngryBallSound(void *pointer, float distance, float offset)
{
    short n = losAddBallSound(pointer, distance*2,  losLoopedSounds[5],losCurrentLoopedObjects[5]);
    if (n!=-1)
        losRobotSounds[n].offset = offset;
}

void losAddAngryBallLockedSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance,  losLoopedSounds[6],losCurrentLoopedObjects[6]);
}

void losAddRotatingBladesSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance,  losLoopedSounds[7],losCurrentLoopedObjects[7]);
}

void losAddMovingBladesSound(void *pointer, float distance,float offset)
{
    short n = losAddBallSound(pointer, distance,  losLoopedSounds[8],losCurrentLoopedObjects[8]);
    if (n!=-1)
    {
        if (offset<=deltaTime)
            losMovingBladesSounds[n].pointer = 0;
        losMovingBladesSounds[n].offset = offset;
    }
}

void losAddElectricAttackSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance,  losLoopedSounds[9],losCurrentLoopedObjects[9]);
}

void losAddElectricBalllSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance,  losLoopedSounds[11],losCurrentLoopedObjects[11]);
}

void losAddAngryRushBallSound(void *pointer, float distance)
{
    losAddBallSound(pointer, distance/1.5, losLoopedSounds[10],losCurrentLoopedObjects[10]);
}

void losUpdateSounds()
{
    if (iceBlockMoved->isPlaying())
    {
        iceBlocksDuration--;
        if (iceBlocksDuration<=0)
            iceBlockMoved->stop();
    }
    for (short j = 0;j<losExplosionsCount;j++)
    for (short i = 0;i<maxObjectSoundCount;i++)
         if (losExplosion[j][i]->source->isPlaying())
         {
             losExplosion[j][i]->distance+=0.5;
         }
    //Fire
    for (short i = 0;i<losCurrentFireBallLauchCount;i++)
    if (losFireObjectsLaunch[i]->source->isPlaying())
    {
        float d = losFireObjectsLaunch[i]->distance;
        if (d<losMinDistance) d = losMinDistance;
        losFireObjectsLaunch[i]->source->setVolume(volumeForDist(d));
    } else
    {
        losFireObjectsLaunch[i]->shapePointer = 0;
        losCurrentFireBallLauchCount--;
        if (i!=losCurrentFireBallLauchCount)
        {
            LOSObjectSound * buf = losFireObjectsLaunch[i];
            losFireObjectsLaunch[i] = losFireObjectsLaunch[losCurrentFireBallLauchCount];
            losFireObjectsLaunch[losCurrentFireBallLauchCount] = buf;
            i--;
        }
    }
    
    //Angry
    for (short i = 0;i<losCurrentAngryBallLauchCount;i++)
        if (losAngryObjectsLaunch[i]->source->isPlaying())
        {
            float d = losAngryObjectsLaunch[i]->distance;
            if (d<losMinDistance) d = losMinDistance;
            losAngryObjectsLaunch[i]->source->setVolume(volumeForDist(d));
        } else
        {
            losAngryObjectsLaunch[i]->shapePointer = 0;
            losCurrentAngryBallLauchCount--;
            if (i!=losCurrentAngryBallLauchCount)
            {
                LOSObjectSound * buf = losAngryObjectsLaunch[i];
                losAngryObjectsLaunch[i] = losAngryObjectsLaunch[losCurrentAngryBallLauchCount];
                losAngryObjectsLaunch[losCurrentAngryBallLauchCount] = buf;
                i--;
            }
        }
    for (short i = 0;i<losCurrentAngryBallDigCount;i++)
        if (losAngryObjectsDig[i]->source->isPlaying())
        {
            float d = losAngryObjectsDig[i]->distance;
            if (d<losMinDistance) d = losMinDistance;
            losAngryObjectsDig[i]->source->setVolume(volumeForDist(d));
        } else
        {
            losAngryObjectsDig[i]->shapePointer = 0;
            losCurrentAngryBallDigCount--;
            if (i!=losCurrentAngryBallDigCount)
            {
                LOSObjectSound * buf = losAngryObjectsDig[i];
                losAngryObjectsDig[i] = losAngryObjectsDig[losCurrentAngryBallDigCount];
                losAngryObjectsDig[losCurrentAngryBallDigCount] = buf;
                i--;
            }
        }
    
    //...
    
    for (short i = 0;i<losCurrentLoopedObjects[5];i++)
        if (losLoopedSounds[5][i]->shapePointer != losRobotSounds[i].pointer)
        {
            losRobotSounds[i].pointer = losLoopedSounds[5][i]->shapePointer;
            losLoopedSounds[5][i]->source->setOffset(losRobotSounds[i].offset*losPiDuration);
        }
    for (short i = losCurrentLoopedObjects[5];i<maxObjectSoundCount;i++)
        losRobotSounds[i].pointer = 0;

    for (short i = 0;i<losCurrentLoopedObjects[8];i++)
        if (losLoopedSounds[8][i]->shapePointer != losMovingBladesSounds[i].pointer)
        {
            losMovingBladesSounds[i].pointer = losLoopedSounds[8][i]->shapePointer;
            if (losMovingBladesSounds[i].offset>0.5)
                losLoopedSounds[8][i]->source->setOffset(0.5);
            else
                losLoopedSounds[8][i]->source->setOffset(losMovingBladesSounds[i].offset);
        }
    for (short i = losCurrentLoopedObjects[8];i<maxObjectSoundCount;i++)
        losMovingBladesSounds[i].pointer = 0;
    
    //losAllLoopedSounds
    losAllLoopedSoundsCount = 0;
    for (short j = 0;j<losLoopedSoundsCount;j++)
    {
        for (short i = 0;i<losCurrentLoopedObjects[j];i++)
        {
            if (losAllLoopedSoundsCount >= losMaxAllLoopedSoundsCount)
            {
                if (losLoopedSounds[j][i]->distance < losLoopedSounds[losAllLoopedSounds[losAllLoopedSoundsCount-1]][0]->distance)
                    losAllLoopedSounds[losAllLoopedSoundsCount-1] = j;
            } else
            {
                losAllLoopedSounds[losAllLoopedSoundsCount] = j;
                losAllLoopedSoundsCount++;
            }
            
            //упорядочиваем
            bool q = true;
            while (q) {
                q = false;
                for (short k=0;k<losAllLoopedSoundsCount-1;k++)
                {
                    if (losLoopedSounds[losAllLoopedSounds[k]][0]->distance > losLoopedSounds[losAllLoopedSounds[k+1]][0]->distance)
                    {
                        short buf = losAllLoopedSounds[k];
                        losAllLoopedSounds[k] = losAllLoopedSounds[k+1];
                        losAllLoopedSounds[k+1] = buf;
                    }
                }
            }
        }
    }
    
    for (short j = 0;j<losLoopedSoundsCount;j++)
    {
        bool q = false;
        for (short k = 0;k<losAllLoopedSoundsCount;k++)
            if (losAllLoopedSounds[k]==j)
            {
                q = true;
                break;
            }
        
        if (!q) losCurrentLoopedObjects[j] = 0;
        
        for (short i = 0;i<losCurrentLoopedObjects[j];i++)
        {
            float d = losLoopedSounds[j][i]->distance;
            if (d<losMinDistance) d = losMinDistance;
            losLoopedSounds[j][i]->source->setVolume(volumeForDist(d));
        }
        
        for (short i = losCurrentLoopedObjects[j];i<losLastLoopedObjects[j];i++)
            losLoopedSounds[j][i]->source->stop();
        for (short i = losLastLoopedObjects[j];i<losCurrentLoopedObjects[j];i++)
            losLoopedSounds[j][i]->source->play();
        losLastLoopedObjects[j] = losCurrentLoopedObjects[j];
    }

    
//    for (short j = 0;j<losLoopedSoundsCount;j++)
//    {
//        for (short i = 0;i<losCurrentLoopedObjects[j];i++)
//        {
//            float d = losLoopedSounds[j][i]->distance;
//            if (d<losMinDistance) d = losMinDistance;
//            losLoopedSounds[j][i]->source->setVolume(volumeForDist(d));
//        }
//        
//        for (short i = losCurrentLoopedObjects[j];i<losLastLoopedObjects[j];i++)
//            losLoopedSounds[j][i]->source->stop();
//        for (short i = losLastLoopedObjects[j];i<losCurrentLoopedObjects[j];i++)
//            losLoopedSounds[j][i]->source->play();
//        losLastLoopedObjects[j] = losCurrentLoopedObjects[j];
//    }
}

void losResetSounds()
{
    losStopGameSounds();
    losPreUpdateSounds();
    for (short j = 0;j<losLoopedSoundsCount;j++)
        losLastLoopedObjects[j] = 0;
    
    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        losRobotSounds[i].pointer = 0;
        losMovingBladesSounds[i].pointer = 0;
    }
}

void losStopGameSounds()
{
    rollingSnowBallSource->stop();
    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        losFireObjectsLaunch[i]->source->stop();
        losAngryObjectsLaunch[i]->source->stop();
        for (short j = 0;j<losLoopedSoundsCount;j++)
            losLoopedSounds[j][i]->source->stop();
    }
    for (short j = 0;j<losLoopedSoundsCount;j++)
        losCurrentLoopedObjects[j] = 0;
}

void losPlayGameSounds()
{
    rollingSnowBallSource->setVolume(0);
    rollingSnowBallSource->play();
}

void losSetRollingSnowballSpeed(float v)
{
    rollingSnowBallSource->setVolume(v*soundsVolumeKoef/2);
    if (v!=0 && !rollingSnowBallSource->isPlaying())
        rollingSnowBallSource->play();
}

void losSetPlayerPosition(SunnyVector2D pos)
{
//    printf("\n (%f, %f)",pos.x,pos.y);
//    gameListener->setPosition(SunnyVector3D(pos.x, pos.y, 0));
}



void losPauseSoundsForMenu()
{
    rollingSnowBallSource->pause();
    for (short i = 0;i<maxObjectSoundCount;i++)
    {
        losFireObjectsLaunch[i]->source->pause();
        losAngryObjectsLaunch[i]->source->pause();
        for (short j = 0;j<losLoopedSoundsCount;j++)
            losLoopedSounds[j][i]->source->pause();
    }
}

void losUnpauseSoundsFromMenu()
{
    rollingSnowBallSource->play();
    for (short i = 0;i<losCurrentFireBallLauchCount;i++)
        losFireObjectsLaunch[i]->source->play();
    for (short i = 0;i<losCurrentAngryBallLauchCount;i++)
        losAngryObjectsLaunch[i]->source->play();
    for (short i = 0;i<losCurrentAngryBallDigCount;i++)
        losAngryObjectsDig[i]->source->play();
    for (short j = 0;j<losLoopedSoundsCount;j++)
    for (short i = 0;i<losCurrentLoopedObjects[j];i++)
        losLoopedSounds[j][i]->source->play();
}

void losPlayIceBlockMoved()
{
    if (!iceBlockMoved->isPlaying())
//    iceBlockMoved->stop();
        iceBlockMoved->play();
    
    iceBlocksDuration = 10;
}

void losPlayIceBlockAndIceBlock()
{
    //iceBlockAndIceBlock->stop();
    if (!iceBlockAndIceBlock->isPlaying())
        iceBlockAndIceBlock->play();
}

void losPlaySnowBallAndBlades()
{
    snowBallAndBlades->stop();
    snowBallAndBlades->play();
}

void losPlayTrapSound()
{
    trapSound->stop();
    trapSound->play();
}

void losPlayButtonClick()
{
    buttonClickSound->stop();
    buttonClickSound->play();
}


