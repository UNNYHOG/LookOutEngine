//
//  LOMap.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOMap_h
#define LookOut_LOMap_h
#include "LOFireBall.h"
#include "LOWaterBall.h"
#include "LOGroundBall.h"
#include "LOAngryBall.h"
#include "LOElectricBall.h"
#include "LOAnomalyBall.h"
#include "LOMapInfo.h"
#include "SunnyOpengl.h"
//#include "LOGroundGenerator.h"
#include "LOExplosion.h"
#include "LOPlayer.h"
#include "LOCrashModel.h"
#include "LOGlobalFunctions.h"
#include "SunnyButton.h"
#include "LOGoldCoin.h"
#include "LOVioletSnowflake.h"
#include "LOSurvivalMap01.h"
#include "LOSurvivalMap02.h"
#include "LOSurvivalMap03.h"
#include "LOSurvivalMap04.h"
#include "LOPuzzleObject.h"

#define cpBody SC_Circle
#define cpShape SC_Circle

const short maxExplosition = 16;
const short maxGoldCoinsCount = 10;
const short maxVioletSnowflakesCount = 16;

class LOMenu;
struct LOTask;

const short maxElectroExplosions = 16;

struct LOElectroExplosion
{
    SunnyVector2D pos;
    short frames;
};

const short maxSnowFromAngryBallsCount = 300;

struct LOSnowFromAngryBalls
{
    SunnyVector3D velocity;
    SunnyVector3D position;
    float angle;
};

class LOShortMessage
{
    float animationTime;
public:
    LOShortMessage();
    ~LOShortMessage();
    SunnyVector2D position;
    char * message;
    bool isAnimated;
    float scale;
    float alpha;
    void animate(SunnyVector2D pos,const  char * text);
    bool update(float deltaTime);
};

class LOMap
{
    void init();
    void clear();
    void resetLevel();
    
    bool rightSide;
    short frameNum;
    
    LOSurvivalMap * activeSurvivalMap;
    
    LOFireBall *fireBalls[maxFireBalls];
    LOWaterBall *waterBalls[maxWaterBalls];
    LOGroundBall *groundBalls[maxGroundBalls];
    LOAngryBall *angryBalls[maxAngryBalls];
    LOElectricBall *electricBalls[maxElectricBalls];
    LOAnomalyBall * anomalyBalls[maxAnomalyBalls];
    short fireBallsCount;
    short waterBallsCount;
    short visibleWaterBallsCount;
    short groundBallsCount;
    short electricBallsCount;
    short anomalyBallsCount;
    
    short targetCount;
    
    int minuteNumber;
    short finishBlink;
        
    LOExplosion explosions[maxExplosition];
    short explosionsCount;
    
    LOElectroExplosion electroExplosions[maxElectroExplosions];
    short electroExplosionsCount;
    void addElectroExplosion(SunnyVector2D p);
    void renderElectroExplosions(SunnyMatrix *MV);
    
    LOSnowFromAngryBalls snowFromAngryBalls[maxSnowFromAngryBallsCount];
    short snowFromAngryBallsCount;
    long lastMinuteNum;
    void renderShadows(SunnyMatrix *MV);
    LOGoldCoin *goldCoins[maxGoldCoinsCount];
    short goldCoinsCount;
    LOVioletSnowflake *violetSnowflakes[maxVioletSnowflakesCount];
    short violetSnowflakesCount;
    void renderGoldCoins(SunnyMatrix *MV);
    void renderVioletSnowflakes(SunnyMatrix *MV);
    bool tryToDropGoldCoin(SunnyVector2D p);
    bool tryToDropPuzzlePart(SunnyVector2D p);
    
    void levelCompleted();
    
    short deathAnimationFrame;
    bool animateDeath;
    void renderDeathAnimation();
    
    GLuint finishVAO,finishVBO;
    void prepareFinish();
    
    //Tasks
    LOTask **activeTasks;
    unsigned long lastReportedPlayingTime;
    unsigned char playingMusic;
    LOSurvivalMap * createSurvivalMap();
    
    LOShortMessage * shortMessage;
    
    void createVioletSnowflake(SunnyVector2D p, SunnyVector2D v = SunnyVector2D(0,0));
public:
    void explodeAngryballAtLocation(SunnyMatrix * pos);
    void explodeAngryBallByBall(LODieType type, SunnyMatrix * pos);
    void collideAngryBallWithBall(LOCollisionType type, SunnyMatrix * pos);
    void explodeAngryBallBySpell(LevelAbility ability, SunnyMatrix * pos);
    float previousSurvivalRecord;
    bool dropPuzzlePart(SunnyVector2D p, short stepNumber, bool justClose);
    unsigned long survivalSnowflakesCollected,survivalGoldCoinsCollected;
    void saveMap();
    bool loadMap();
    void removeMap();
    void renderFinish(SunnyMatrix *MV);
    LOTask ** loadActiveTasks();
    void destroyVioletSnowflake(LOVioletSnowflake *snowflake);
    void gameOver(LODieType dieType);
    LOMenu * mainMenu;
    
    void showMessage(SunnyVector2D pos, const char * text);
    void drawShortMessage();
    
    LOMapInfo mapInfo;
    void addSnowFromAngryBall(SunnyVector2D pos,SunnyVector2D vel);
    void renderSnowFromAngryBallsUnderPlayer(SunnyMatrix *MV);
    void renderSnowFromAngryBalls(SunnyMatrix *MV);
    void updateSnowFromAngryBalls();
    short angryBallsCount;
    float timeCondition;
    
    LOMap();
    ~LOMap();
    
    void calculateScores();
    bool isLevelComplete();
    
    void update();
    void renderWarningTriangles(SunnyMatrix *MV);
    void renderES2();
    void renderOverlay();
    void renderPreview();
    void resetMap();
    void earthShake(float scale = 1);
    
    void renderFinish();
    
    void checkOnCloseExplosion(LOExplosionType ex1, LOExplosionType ex2, SunnyVector2D pos);
    
    void explosion(SunnyVector2D pos, bool movePlayer = true);
    void explosion(SunnyMatrix *mat1,SunnyMatrix *mat2, LOExplosionType type1, LOExplosionType type2);
    void explosion(SunnyMatrix *mat1,LOExplosionType type1,bool blur = true);
    void addSnowExplosion(SunnyMatrix *mat);
    void addTeleportExplosion(SunnyMatrix *mat);
    void addTeleportExplosionPost(SunnyMatrix *mat);
    void addShieldExplosion(SunnyMatrix *mat);
    void addSpeedExplosion(SunnyMatrix *mat);
    
    void load(const char* fileName);
    void prepareSurvivalMap();
    void loadPreview(const char* fileName);
    void usePreviewInsteadOfLoading();
    SunnyVector2D movePlayer(SunnyVector2D oldPosition, SunnyVector2D velocity);
    
    void renderBallsShadows(SunnyMatrix *MV);
    void renderGoldCoinsShadows(SunnyMatrix *MV);
    
    void createRandomVioletSnowflake(float velocityScale);
    void createRandomFireBall(float velocityScale);
    void createRandomWaterBall(float velocityScale);
    void createRandomGroundBall(float velocityScale);
    void createRandomElectricBall(float velocityScale);
    void createRandomAnomalyBall(float velocityScale);
    void createFireBall(SunnyVector2D pos, SunnyVector2D vel);
    void createWaterBall(SunnyVector2D pos, SunnyVector2D vel);
    void createGroundBall(SunnyVector2D pos, SunnyVector2D vel);
    void createElectricBall(SunnyVector2D pos, SunnyVector2D vel);
    void createAnomalyBall(SunnyVector2D pos, SunnyVector2D vel);
    void createRandomFireBallToPlayer(float velocityScale);
    void createRandomWaterBallToPlayer(float velocityScale);
    void createRandomGroundBallToPlayer(float velocityScale);
    void createRandomElectricBallToPlayer(float velocityScale);
    void createRandomAnomalyBallToPlayer(float velocityScale);
    
    void createRandomAngryBall();
    void createBufAngryBall(LOBufAngryBall pos);
    void createAngryBall(SunnyVector2D pos);
    
    void explodePlayer(LOPlayer *pl, LOCrashModel * model);
    void explodeFireBall(short num, LOPlayer *pl, LOCrashModel * model);
    void explodeWaterBall(short num, LOPlayer *pl, LOCrashModel * model);
    void explodeGroundBall(short num, LOPlayer *pl, LOCrashModel * model);
    
    void collisionPlayerAndPlayer(cpBody * body1, cpBody * body2);
    void collisionPlayerAndFireBall(cpBody * body1, cpBody * body2);
    void collisionPlayerAndWaterBall(cpBody * body1, cpBody * body2);
    void collisionPlayerAndGroundBall(cpBody * body1, cpBody * body2);
    void collisionPlayerAndAngryBall(cpBody * body1, cpBody * body2);
    void collisionPlayerAndAnomalyBall(cpBody * body1, cpBody * body2);
    
    void collisionPlayerAndButtonBegan(cpBody * body1, cpBody * body2);
    void collisionPlayerAndButtonEnded(cpBody * body1, cpBody * body2);
    
    void collisionAngryBallAndFireBall(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndWaterBall(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndGroundBall(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndAnomalyBall(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndBonfire(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndElectricField(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndElectricFieldEnded(cpBody * body1, cpBody * body2);
    
    void collisionFireBallAndFireBall(cpBody * body1, cpBody * body2);
    void collisionFireBallAndWaterBall(cpBody * body1, cpBody * body2);
    void collisionFireBallAndGroundBall(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndWaterBall(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndGroundBall(cpBody * body1, cpBody * body2);
    void collisionGroundBallAndGroundBall(cpBody * body1, cpBody * body2);
    
    void collisionFireBallAndAnomalyBall(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndAnomalyBall(cpBody * body1, cpBody * body2);
    void collisionGroundBallAndAnomalyBall(cpBody * body1, cpBody * body2);
    void collisionAnomalyBallAndAnomalyBall(cpBody * body1, cpBody * body2);
    
    void collisionHighSnowAndFireBall(cpShape * body1, cpShape * body2);
    void collisionHighSnowAndWaterBall(cpShape * body1, cpShape * body2);
    void collisionHighSnowAndAnomalyBall(cpShape * body1, cpShape * body2);
    void collisionHighSnowAndGroundBall(cpShape * body1, cpShape * body2);
    void collisionPlayerAndSnowflake(cpShape * shape1, cpShape * shape2);
    void collisionPlayerAndVioletSnowflake(cpShape * shape1, cpShape * shape2);
    void collisionPlayerAndGoldCoin(cpShape * shape1, cpShape * shape2);
    void collisionPlayerAndBonfire(cpShape * shape1, cpShape * shape2);
    void collisionPlayerAndRollingBlades(cpShape * shape1, cpShape * shape2);
    void collisionPlayerAndElectricField(cpShape * shape1, cpShape * shape2);
    
    void collisionFireBallAndElectricField(cpBody * body1, cpBody * body2);
    void collisionFireBallAndElectricFieldEnded(cpBody * body1, cpBody * body2);
    void collisionGroundBallAndElectricField(cpBody * body1, cpBody * body2);
    void collisionGroundBallAndElectricFieldEnded(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndElectricField(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndElectricFieldEnded(cpBody * body1, cpBody * body2);
    void collisionPlayerAndIceBlock(cpBody * body1, cpBody * body2);
    void collisionPlayerAndIceBlockEnded(cpBody * body1, cpBody * body2);
    
    void collisionPlayerAndElectricBallBegan(cpBody * body1, cpBody * body2);
    void collisionPlayerAndElectricBallEnded(cpBody * body1, cpBody * body2);
    void collisionFireBallAndElectricBallBegan(cpBody * body1, cpBody * body2);
    void collisionFireBallAndElectricBallEnded(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndElectricBallBegan(cpBody * body1, cpBody * body2);
    void collisionWaterBallAndElectricBallEnded(cpBody * body1, cpBody * body2);
    void collisionGroundBallAndElectricBallBegan(cpBody * body1, cpBody * body2);
    void collisionGroundBallAndElectricBallEnded(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndElectricBallBegan(cpBody * body1, cpBody * body2);
    void collisionAngryBallAndElectricBallEnded(cpBody * body1, cpBody * body2);
    void collisionElectricBallAndElectricBallBegan(cpBody * body1, cpBody * body2);
    void collisionElectricBallAndElectricBallEnded(cpBody * body1, cpBody * body2);

    void collisionAngryBallChargedBegan(cpBody * body1, cpBody * body2);
    
    void setRotationAngle(SunnyVector2D v);
    SunnyVector2D getRotationAngle();
    
    void createGroundModel(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount],unsigned char * heightMap);
    bool canMoveIceBlock(SunnyVector2D pos, short direction);
    
    void updateInfoForSounds();
    void renderAngryBallsShadows(SunnyMatrix *MV);
    void renderAngryBallsLight(SunnyMatrix *MV);
    void renderWaterCharges(SunnyMatrix *MV);
    
    void minutePast();
    
    void sendAngryBallsForMultiplayer();
    void checkObjects();
    
    int getCollectedSnowflakes();
    
    void taskComplete(LOTask *task);
    void applyGlobalColor(float x);
    
    SunnyVector2D randPosOnMapBounds(short &bound);
    
    friend class LOEvents;
    friend class LOSurvivalMap01;
    friend class LOSurvivalMap03;
    friend class LOSurvivalMap04;
};

#endif