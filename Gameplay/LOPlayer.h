//
//  LOPlayer.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOPlayer_h
#define LookOut_LOPlayer_h
#include "LOObject.h"

//const float standartMu = 0.3;
const float speedSpellTime = 5;
const float ressurectionImmune = 3;

enum SnowballAbility
{
    SA_None = 0,
    SA_P_Speed,
};

enum LevelAbility
{
    LA_Life = 0,
    LA_Teleport,
    LA_Speed,
    LA_Shield,
    LA_Doubler,
    LA_Magnet,
    LA_Time,
    LA_Ressurection,
    LA_Count,
};

enum LODieType
{
    LOD_FireBall = 0,
    LOD_WaterBall,
    LOD_ElectricBall,
    LOD_AngryBall,
    LOD_Bonfire,
    LOD_ElectricField,
    LOD_Blades,
    LOD_Count,
};

enum LOCollisionType
{
    LOCT_GroundBall = 0,
    LOCT_WaterBall,
    LOCT_AnomalyBall,
    LOCT_Count,
};

struct LOAbility
{
    bool active;
    SnowballAbility abilityType;
    short level;
    float cooldown;
};

const short maxAbilitiesCount = 3;
const short speedPointsCount = 5;

const short playerPhysicsPointsCount = 8;

class LOPlayer : public LOObject
{
    float shapeRadius;
    float radius;
    
    SunnyVector2D velocityAdd;
    SunnyVector2D lastVelocity;
    SunnyVector2D lastBufVelocity;
    
    float anomalyKoef;
    
    //LOAbility ability[maxAbilitiesCount];
    
    SunnyMatrix speedPoints[speedPointsCount];
    
    float abilitySpeed;
    
    short survivalShield;
    
    bool teleportActive;
    
    void init();
    void clear();
    
    SunnyVector2D calculateVelocity();
    
    //spell Activation
    short spellActivationFrame;
    short activatedSpell;
    float speedTime;
    
    float rolledDistance;
    unsigned long lastReportedDistance;
    bool snowMagnet;
    bool snowDoubler;
    bool snowTime;
    
    bool physActive;
    bool hidden;
    float survivalShieldWorkTime;
    float icePowers[4];
    bool hasIceContact[4];
public:
    short effectShield;
    unsigned int ressurectionsCount;
    double ressurectionTime;
    float alphaX,alphaY;
    float waterKoef;
    float sandFriction;
    bool crashed;
    
    LOPlayer();
    ~LOPlayer();
    
    bool useShield();
    void forceTeleport();
    void teleportToLocation(SunnyVector2D pos);
    
    void applyJoystic();
    void setAngle(float x, float y);
    bool update();
    bool updateMultiPlayer();
    bool updatePosition();
    static void renderForMenu(SunnyMatrix *MV, float colorScale = 1, SunnyVector3D color = SunnyVector3D(1,1,1.15));
    void render(SunnyMatrix *MV);
    void renderSpells(SunnyMatrix *MV);
    void renderSpellsUnderPlayer(SunnyMatrix *MV);
    void renderOverlays(SunnyMatrix *MV);
    void attachMoveBody();
    void removePhysics();
    void updateRadius(float newR);
    void setRadius(float newR);
    void reset(bool ressurection = false);
    bool setWaterKoef();
    bool setAnomaly();
    bool setGroundKoef();
    float getRadius();
    float getSnowballScale();
    float getSnowballScale(float radius);
    void setPlayersColor();
    void checkMultiplayerPosition(SunnyVector2D pos);
    float getRessurectionAlpha();
    
//    void setAbility(short aNum, SnowballAbility type, short level);
//    void removeAbility(short aNum);
    bool activateAbility(LevelAbility ability, bool loaded = false);
    bool isAbilityActive(LevelAbility ability);
    bool isAbilityEnable(LevelAbility ability);
    
    bool isHardPowerForDirection(short direction);
    void setCrashed(bool value);
    
    friend class LOMap;
    friend class LOMapInfo;
};

#endif
