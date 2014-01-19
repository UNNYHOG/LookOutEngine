//
//  LOGlobalVar.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOGlobalVar_h
#define LookOut_LOGlobalVar_h
#include "SunnyVector4D.h"

const short maxPlayersCount = 4;
const int loLevelsPerStep = 15;
const int loStepsCount = 4;
const float defaultButtonScale = 0.85;
const float pixelToWorld = 30./1024;
const short freeSpellsCount = 3;
const float smallButtonsScale = 0.76;

extern float globalFontScale;

#define whiteOldInnerColor(a) SunnyVector4D(250,251,253,255*a)/255
#define whiteOldOuterColor(a) SunnyVector4D(161,174,188,255*a)/255
#define whiteOuterColor(a) SunnyVector4D(104,139,169,255*a)/255
#define whiteInnerColor(a) SunnyVector4D(207,233,248,255*a)/255

#define yellowInnerColor(a) SunnyVector4D(260,250,20,255*a)/255
#define yellowOuterColor(a) SunnyVector4D(245,148,69,255*a)/255

#define social_unnyhog_fb "https://www.facebook.com/unnyhog"
#define social_unnyhog_tw "https://www.twitter.com/unnyhog"
#define social_dodge_fb "https://www.facebook.com/dodgenroll"
#define social_dodge_tw "https://www.twitter.com/dodgenroll"
#define social_dodge_tw_id "dodgenroll"
#define social_unnyhog_youtube "http://www.youtube.com/user/unnyhog"
#define social_unnyhog_youtube_id "unnyhog"
#define social_unnyhog_vk "http://www.vk.com/unnyhog"
#define social_dodge_fb_id "458138464284680"
#define social_unnyhog_fb_id "208173649242257"

#include "LOPlayer.h"
#include "LOMenu.h"
#include "LOMap.h"
#include "LOScore.h"
#include "LOFont.h"
#include "SunnyVector2D.h"
#include "SunnyOpengl.h"
#include "SunnyCollisionDetector.h"
#include "LOGroundModel.h"
#include "LOGroundGenerator.h"
#include "LOCrashModel.h"
#include "LOGroundAnimation.h"
#include "LOIceBlockModel.h"
#include "SunnyButton.h"

#define SunnyDrawArrays(a) glDrawArrays(GL_TRIANGLE_STRIP, (a)*4, 4)
#define PixelToWorld(a,b) SunnyVector2D(a/1920.*mapSize.x,(1280.-b)/1280.*mapSize.y)
const short numbersCount = 10;
const float numbersSizes[numbersCount] = {62,42,60,56,54,58,62,50,50,52};
const short numbersSymbolsCount = 8;
const char fontSymbols[numbersSymbolsCount] = {'.', ',', ':','-', '?', '!',  '\'','%'};
const float numbersSymbolsSizes[numbersSymbolsCount] = {48,50,50,92,118,50,42,138};

#define LAYER_LOW (1)
#define LAYER_HIGH (1<<1)
#define LAYER_BLADES (1<<2)
#define LAYER_ELECTRIC (1<<3)
#define LAYER_ELECTRIC_BALL (1<<4)
#define LAYER_SNOWFLAKE (1<<5)

extern bool cheatProtection1;
extern bool cheatProtection2;
extern bool cheatProtection3;

enum LOResetType
{
    RT_None = 0,
    RT_Death,
    RT_LevelComplete,
    RT_Pause,
};

extern LOResetType resetType;

enum COLLISION_TYPE
{
    COLLISION_PLAYER = 0,
    COLLISION_HIGH_SNOW,
    COLLISION_FIREBALL,
    COLLISION_WATERBALL,
    COLLISION_GROUNDBALL,
    COLLISION_SNOWFLAKE,
    COLLISION_VIOLET_SNOWFLAKE,
    COLLISION_GOLD_COIN,
    COLLISION_PUZZLE,
    COLLISION_BONFIRE,
    COLLISION_ROLLING_BLADES,
    COLLISION_ICE_BLOCK,
    COLLISION_ANGRYBALL,
    COLLISION_ELECTRICBALL,
    COLLISION_ELECTRICFIELD,
    COLLISION_LOW_SNOW,
    COLLISION_ANOMALYBALL,
    COLLISION_BUTTON,
    COLLISION_COUNT,
};

const int LOIceBlock_VAO = (0);
const int LOBonfire_VAO = (LOIceBlock_VAO+1);
const int LOBackground_VAO = (LOBonfire_VAO+18);
const int LOWarnTriangles_VAO = (LOBackground_VAO+1);
const int LOShadows_VAO = (LOWarnTriangles_VAO+9);
const int LOObject_VAO = (LOShadows_VAO+12);
const int LOTraces_VAO = (LOObject_VAO+60);
const int LOWaterBall_VAO = (LOTraces_VAO+16);
const int LORollingBlades_VAO = (LOWaterBall_VAO+22);
const int LOSnowflake_VAO = (LORollingBlades_VAO+16);
const int LOSmoke_VAO = (LOSnowflake_VAO+3);
const int LOPreview_VAO = (LOSmoke_VAO+17);
const int LOButton_VAO = (LOPreview_VAO+14);
const int LODeath_VAO = (LOButton_VAO+3);
const int LOJoystic_VAO = (LODeath_VAO+3);
const int LONumbers_VAO = (LOJoystic_VAO+1);
const int LONumbersSymbols_VAO = (LONumbers_VAO + numbersCount*2);
const int LOOverlaySnowflake_VAO = (LONumbersSymbols_VAO + numbersSymbolsCount*2);
const int LOOverlayFireball_VAO = (LOOverlaySnowflake_VAO + 1);
const int LOOverlayElectricity_VAO = (LOOverlayFireball_VAO + 1);
const int LOOverlaySpells_VAO = (LOOverlayElectricity_VAO + 1);
const int LOOverlayLifeSpell_VAO = (LOOverlaySpells_VAO + 1);
const int LOGameplayButtons_VAO = (LOOverlayLifeSpell_VAO + 1);
const int LOSpellButtons_VAO = (LOGameplayButtons_VAO+1);
const int LOSpellTeleport_VAO = (LOSpellButtons_VAO+14);
const int LOSpellShield_VAO = (LOSpellTeleport_VAO+1);
const int LOSpellLife_VAO = (LOSpellShield_VAO+1);
const int LOSpellSpeed_VAO = (LOSpellLife_VAO+2);
const int LOSnowExplosionShadow_VAO = (LOSpellSpeed_VAO + 1);
const int LOMapPreviewVAO = (LOSnowExplosionShadow_VAO + 2);
const int LOLSArrowVAO = (LOMapPreviewVAO + 1);
const int LOLSDotVAO = (LOLSArrowVAO + 1);
const int LOLSTopBorderVAO = (LOLSDotVAO + 1);
const int LOLSPlayButtonVAO = (LOLSTopBorderVAO + 2);
const int LOLCBorderVAO = (LOLSPlayButtonVAO + 1);
const int LOLCButtonsVAO = (LOLCBorderVAO+4);
const int LOLCSnowTable_VAO = (LOLCButtonsVAO + 7);
const int LOLCClock_VAO = (LOLCSnowTable_VAO + 1);
const int LOLCForest_VAO = (LOLCClock_VAO + 1);
const int LOGoldCoin_VAO = (LOLCForest_VAO + 1);
const int LOVioletSnowflake_VAO = (LOGoldCoin_VAO + 1);
const int LOPauseCross_VAO = (LOVioletSnowflake_VAO + 1);
const int LOMusicButtonOn_VAO = (LOPauseCross_VAO + 1);
const int LOMusicButtonOff_VAO = (LOMusicButtonOn_VAO + 1);
const int LOBigButton_VAO = (LOMusicButtonOff_VAO + 1);
const int LOTaskView_VAO = (LOBigButton_VAO + 1);
const int LOYellowStar_VAO = (LOTaskView_VAO + 1);
const int LOTaskBorderTop_VAO = (LOYellowStar_VAO + 1);
const int LOTaskBorderMiddle_VAO = (LOTaskBorderTop_VAO + 1);
const int LOTaskBorderBottom_VAO = (LOTaskBorderMiddle_VAO + 1);
const int LOSocialButtonBack_VAO = (LOTaskBorderBottom_VAO + 1);
const int LOSocialButtonFB_VAO = (LOSocialButtonBack_VAO + 1);
const int LOSocialButtonTW_VAO = (LOSocialButtonFB_VAO + 1);
const int LOSocialButtonVK_VAO = (LOSocialButtonTW_VAO + 1);
const int LOShareFB_VAO = (LOSocialButtonVK_VAO + 1);
const int LOMagicBoxMagic_VAO = (LOShareFB_VAO + 1);
const int LOStoreButton_VAO = (LOMagicBoxMagic_VAO + 3);
const int LOFreeStuff_VAO = (LOStoreButton_VAO + 6);
const int LOMainMenu_VAO = (LOFreeStuff_VAO + 8);
const int LOOkButtonBack_VAO = (LOMainMenu_VAO + 2);
const int LOPuzzleButton_VAO = (LOOkButtonBack_VAO + 1);
const int LOSurvival_VAO = (LOPuzzleButton_VAO + 1);
const int LOSettingsButton_VAO = (LOSurvival_VAO + 1);
const int LOVibration_VAO = (LOSettingsButton_VAO + 1);
const int LOGameCenter_VAO = (LOVibration_VAO + 2);
const int LOSounds_VAO = (LOGameCenter_VAO + 2);
const int LOJoysticAcc_VAO = (LOSounds_VAO + 2);
const int LOLeftRightHand_VAO = (LOJoysticAcc_VAO + 2);
const int LOMessageBorders_VAO = (LOLeftRightHand_VAO + 2);
const int LOVioletSnowflakeInGame_VAO = (LOMessageBorders_VAO + 3);
const int LOIcePriceButton_VAO = (LOVioletSnowflakeInGame_VAO + 2);
const int LONewRecordImage_VAO = (LOIcePriceButton_VAO + 1);
const int LOCount_VAO = (LONewRecordImage_VAO + 1);

const float defaultVelocity = 7.;

const SunnyMatrix sunnyIdentityMatrix = getIdentityMatrix();

enum LOLastAction
{
    LOLA_None = 0,
    LOLA_LoadMap,
    LOLA_LoadSurvMap,
    LOLA_LoadEmptyMap,
};

extern LOLastAction lastAction;
extern short actionParametr;
extern bool actionAbilities[];

extern GLuint solidVAO, solidVBO;

extern float backCornerSize;
extern float backSideSize;
extern float backBorderSize;

struct LOJoystic
{
    void *touch;
    SunnyVector2D location;
    SunnyVector2D direction;
};

extern LOJoystic loJoystic;

enum LOControlType
{
    LOC_Joystic = 0,
    LOC_Touches,
    LOC_Accelerometer,
    LOC_Count,
};

extern LOControlType controlType;

extern unsigned short abilityVSPrice[];
extern unsigned short abilityGoldPrice[];

extern float pixelToScale;

//extern LODieType dieType;
extern SunnyMatrix initialMatrix,initialMatrixJoystic;

extern bool levelIsLoading;
extern bool isRetinaDisplay;
extern bool isPadDevice;
extern bool isFastDevice;
extern bool isVerySlowDevice;
extern float pointsCoeff;
extern float padInterfaceScale;
//
extern float verticalMapAdd;
extern float horizontalMapAdd;

const short mapActivationFramesCount = 10;
extern short mapActivationFrameNum;
extern short mapActivationStarted;
extern bool drawPreview;

extern bool messageShowed;

extern short selectedStepNum;
extern short selectedLevelNum;

const float menuBallScale = 5;

const SunnyVector2D mapSize = SunnyVector2D(30,20);
const SunnyVector4D customLightPosition = SunnyVector4D(1,1,0.7,0).normalize();
const SunnyVector4D customLightPositionGraphic = SunnyVector4D(1,1,4,0).normalize();
const float mapScale = 0.9375;

const double cellSize = 20./320.*mapSize.y;
const float textureToMapSize = 2048./640*mapSize.y;

const short explosionSnowRadius = 3;
const float standartWindVelocity = 1;

extern SunnyModelObj * snowBorders;
extern SunnyModelObj * snowBallObj;
extern SunnyModelObj * angryBallObj;
extern SunnyModelObj * goldCoinObj;
extern LOCrashModel* fireBallObj;
extern LOCrashModel * snowWallObj;
extern LOGroundAnimation * groundBallObj;
extern LOCrashModel * snowBallCrashObj;
extern LOCrashModel * snowBallCrashObjForMenu;
extern LOIceBlockModel * iceBlockModel;
extern SunnyVector2D screenSize;
extern SunnyModelObj * electricField;

extern double deltaTime;
extern double playingTime;
extern double realPlayingTime;
extern bool gameOvered;
extern bool gameOveredMultiPlayer;
extern bool gamePaused;
extern float goldCoinImageWidth;
extern float snowflakeImageWidth;
extern float yellowStarImageWidth;

extern short earnedStars;

extern float menuBallTranslateX;
extern int menuBallTranslateV;
extern bool isSurvival;
extern bool isServer;
extern bool isMultiPlayer;
extern short myMultiPlayerNumber;

extern LOPlayer * player;
extern double multiPlayerPlayedTime;
struct LOOtherPlayer
{
    LOPlayer * player;
    LOCrashModel * snowBallCrashObj;
    char playerId[256];
    short playerNum;
    double multiPlayerPlayedTime;
};

extern LOOtherPlayer * otherPlayers;
extern short otherPlayersCount;
const SunnyVector3D multiPlayersColors[maxPlayersCount] = {SunnyVector3D(102,45,145)/255.,SunnyVector3D(147,255,0)/255,SunnyVector3D(255,255,0)/255.,SunnyVector3D(255,35,165)/255.};

extern LOMap * activeMap;

extern SunnyVector4D mapWind; //(x,y,scale,random)
extern SunnyVector2D newMapWind;
extern SunnyVector2D rotationAngle;
extern double windTimer;

extern GLuint objectsTexture,menuTexture,fontTexture;
extern GLuint particleTextures[];

extern int puzzleTextureID;

//extern GLuint testExplosionTexture;
//extern GLuint groundTexture;

//extern GLuint standartFaceVBO;
extern GLuint textureForRender;
extern GLuint textureFramebuffer;
extern GLuint depthRenderbuffer;
extern GLint texturebufferWidth;
extern GLint texturebufferHeight;

extern float amplitude;
extern short earthShakeFame;

extern float velocityScale;
extern double velocityScaleTime;
extern float timeVelocityScale;

extern bool menuMode;
extern unsigned short levelsStrike;

extern char * loLabelKilo;
extern char * loLabelOk;
extern char * loLabelLoading;
extern char * loLabelLocked;
extern char * loLabelShare;
extern char * loLabelNewRecord;

//extern LOGroundModel * groundModel;
extern LOGroundGenerator *groundModel;
extern SunnyVector4D lightPosition;
extern bool physicsPaused;

#ifndef VAO_NOT_SUPPORTED
extern GLuint gameGlobalVAO;
#endif
extern GLuint gameGlobalVBO;
void bindGameGlobal();
extern float orientationScale;
extern float survivalLeftTime;

#define GAMEMODE_CLASSIC (1)
#define GAMEMODE_HARDCORE (2)
#define GAMEMOVE_ARCADE (3)

enum ArcadeMode
{
    AM_Normal = 0,
    AM_Snowflakes,
    AM_CatchUp,
};
extern int gameMode;
extern ArcadeMode arcadeGameMode;

enum LOParticles
{
    LOP_Paillette = 0,
    LOP_Rock01,
    LOP_Rock02,
    LOP_Snowflake01,
    LOP_Snowflake02,
    LOP_Snowflake03,
    LOP_Spark,
    LOP_Count,
};


enum LOButtonSoundsTag
{
    LOBT_None = 0,
    LOBT_Custom,
    LOBT_Ressurection,
    LOBT_SpellShield,
    LOBT_SpellLife,
    LOBT_SpellSpeed,
    LOBT_SpellTeleport,
    LOBT_DropSound,
};

enum LOShaders
{
    LOS_Textured = 0,
    LOS_Textured3D,
    LOS_Textured3DNoLight,
    LOS_Textured3DSnow,
    LOS_Textured3DSnowWalls,
    LOS_Textured3DSnowShadow,
    LOS_Textured3DC,
    LOS_Textured3DA,
    LOS_TexturedC,
    LOS_TexturedA_Tex,
    LOS_TexturedTSA,
    LOS_TexturedTS_Animation,
    LOS_TexturedTSA_Middle,
    LOS_TexturedTSC,
    LOS_SpecularPoints,
    LOS_FallingSnow,
    LOS_Sparks,
    LOS_GroundParticles,
    LOS_ColoredAlpha,
    LOS_SolidObject,
    LOS_OverlayTSA,
    LOS_FontRegular,
    LOS_FontBold,
    LOS_Count,
};

extern SunnyVector4D mapZoomBounds;

extern bool isVAOSupported;
extern SunnyMatrix projectionMatrix;
extern GLuint globalShaders[];
extern GLuint globalModelview[];
extern SunnyVector3D globalColor;
//extern LOScore *loScore;

//extern GLuint uniform3DS_L,uniform3D_L;
extern GLuint uniformA_A, uniformTSA_TR, uniformTSA_A, uniformTSC_TR, uniformTSC_C,uniform3DNL_C, uniform3D_C, uniform3D_TTR,uniformSP, uniformSP_size, uniformFS_size, uniform3D_A;
extern GLuint uniform3DS_C,uniform3DSS_C,uniform3DSS_H, uniformA_Tex_A, uniformA_Tex_Tex, uniformGP_color, uniformCA_color;
extern GLuint uniformOverlayTSA_TR, uniformOverlayTSA_C;
extern GLuint uniformTSAMiddle_TR,uniformTSAMiddle_A;
extern GLuint uniform3D_LightPos,uniform3DNL_Tex,uniform3DSW_C;
extern GLuint uniformSO_Color,uniform3D_FR_Color,uniform3D_FB_Color,uniform3D_FB_BackColor,uniform3D_FR_TS,uniform3D_FB_TS;
extern GLuint uniformTS_TR,uniformTS_Params;

extern GLuint uniformSP_Global;


void loRenderPriceLabel(SunnyMatrix *mat, LOPriceType priceType, int price, float appearAlpha = 1, LOTextAlign align = LOTA_Center, float scaleCoin = 1);
char * getLocalizedLabel(const char * label);
char * getLocalizedLabel(const char * label, char * buf);

//Notifications
void cancelAllLocalNotifications();
void scheduleLocalNotification(float delaySec, const char * textBody, const char * alertAction, const char * soundName);

void loCallbackSetMusic(const char * fileName, unsigned short startPosition);
void loCallbackSetMusicPosition(unsigned short startPosition);
void loCallBackVibrate();
void loOpenURL(const char * url);
void loOpenTWFunPage(const char * url, const char * pageID);
void loOpenUTFunPage(const char * url, const char * pageID);
void loOpenFBFunPage(const char * url, const char * pageID);
void loRateTheGame();
void loBackButtonPressed();
void loExitTheApp();

void LOSetUserIdForFlurry(const char * str);
void LOSendFlurryMessageString(const char * eventName, ...);
void LOSendFlurryMessageInt(const char * eventName, ...);

const float globalMusicVolume = 1;
extern float musicVolumeKoef;
extern float currentVolumeOfMusic;
void losSetMusicVolume(float volume);
void loShareFacebookLC(const int levelNumber, const char* episodeName, const float levelTime, const char* messageName, const char * messageCaption, const char * messageDesc);
void loShareFacebookSurvival(const char* episodeName, const float levelTime, const char* messageName, const char * messageCaption, const char * messageDesc);
void loMakeFBConnection(void (*success)(const char*));


//Server
extern double loginTime;
extern unsigned long loginTimeServer;
unsigned long getCurrentServerTime();
 
#endif
