//
//  LOGlobalVar.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOExternMethods.h"

SunnyMatrix initialMatrix = getIdentityMatrix();
SunnyMatrix initialMatrixJoystic = getIdentityMatrix();

LOLastAction lastAction = LOLA_None;
short actionParametr;
bool actionAbilities[3];
LOJoystic loJoystic;
LOControlType controlType;

float backCornerSize = 0;
float backSideSize = 0;
float backBorderSize = 0;

float globalFontScale = 1;

GLuint solidVAO, solidVBO;

unsigned short abilityVSPrice[LA_Count] = {120,60,30,90,0,0,0,200};
unsigned short abilityGoldPrice[LA_Count] = {2,1,1,1,6,5,10,2};

float pixelToScale;

bool cheatProtection1 = false;
bool cheatProtection2 = false;
bool cheatProtection3 = false;

//extern
bool levelIsLoading = false;
bool isRetinaDisplay = false;
bool isPadDevice = false;
bool isFastDevice = false;
bool isVerySlowDevice = false;
float pointsCoeff = 1.0;
float padInterfaceScale = 1;
///

//LODieType dieType;
float verticalMapAdd = 0;
float horizontalMapAdd = 0;
short mapActivationFrameNum = 0;
short mapActivationStarted = 0;
bool drawPreview = false;

short selectedStepNum = 0;
short selectedLevelNum = 0;

double deltaTime = 0;
double playingTime = 0;
double realPlayingTime = 0;
bool gameOvered = false;
bool gameOveredMultiPlayer = false;
bool gamePaused = true;

char *loLabelKilo = 0;
char *loLabelOk = 0;
char *loLabelLoading = 0;
char *loLabelLocked = 0;
char *loLabelShare = 0;
char *loLabelNewRecord = 0;

short earnedStars = 0;

bool messageShowed;

float menuBallTranslateX = 0;
int menuBallTranslateV = 0;
bool isSurvival = false;
bool isServer = false;
bool isMultiPlayer = false;
short myMultiPlayerNumber = 0;

SunnyModelObj * snowBorders = 0;
SunnyModelObj * snowBallObj = 0;
SunnyModelObj * angryBallObj = 0;
SunnyModelObj * goldCoinObj = 0;
LOCrashModel * fireBallObj = 0;
LOCrashModel * snowWallObj = 0;
LOGroundAnimation * groundBallObj = 0;
LOCrashModel * snowBallCrashObj = 0;
LOCrashModel * snowBallCrashObjForMenu = 0;
LOIceBlockModel * iceBlockModel = 0;
SunnyVector2D screenSize;
SunnyModelObj * electricField = 0;

LOPlayer * player = 0;
double multiPlayerPlayedTime = 0;
LOOtherPlayer * otherPlayers = 0;
short otherPlayersCount = 0;
LOMap * activeMap = 0;
SunnyVector4D mapWind = SunnyVector4D(0,0,-1,0);
SunnyVector2D newMapWind;
SunnyVector2D rotationAngle;
double windTimer;

GLuint objectsTexture = 0;
GLuint fontTexture = 0;
GLuint menuTexture = 0;
GLuint particleTextures[LOP_Count];

//GLuint standartFaceVBO = 0;
GLuint textureForRender = 0;
GLuint textureFramebuffer = 0;
GLuint depthRenderbuffer = 0;
GLint texturebufferWidth;
GLint texturebufferHeight;

float amplitude;
short earthShakeFame;

//Bonuses
float velocityScale = 1.0;
double velocityScaleTime = 0;
float timeVelocityScale = 1.0;

bool menuMode = true;
unsigned short levelsStrike = 0;

LOGroundGenerator *groundModel = 0;
SunnyVector4D lightPosition;
bool physicsPaused;

#ifndef VAO_NOT_SUPPORTED
GLuint gameGlobalVAO = 0;
#endif
GLuint gameGlobalVBO = 0;
void bindGameGlobal()
{
    if (isVAOSupported)
        glBindVertexArray(gameGlobalVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, gameGlobalVBO);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

float orientationScale = 1;

float survivalLeftTime = 0;
int gameMode = 0;//false;
ArcadeMode arcadeGameMode = AM_Normal;

SunnyVector4D mapZoomBounds;

bool isVAOSupported = true;
SunnyMatrix projectionMatrix;
GLuint globalShaders[LOS_Count];
GLuint globalModelview[LOS_Count];
SunnyVector3D globalColor(1,1,1);

float goldCoinImageWidth = 0;
float snowflakeImageWidth = 0;
float yellowStarImageWidth = 0;
//LOScore *loScore = 0;

//
//GLuint uniform3DS_L,uniform3D_L;
GLuint uniformA_A,uniformTSA_TR, uniformTSA_A, uniformTSC_TR, uniformTSC_C,uniform3DNL_C, uniform3D_C, uniform3D_TTR, uniformSP, uniformSP_size, uniformFS_size, uniform3D_A;
GLuint uniform3DS_C,uniform3DSS_C,uniform3DSS_H, uniformA_Tex_A, uniformA_Tex_Tex, uniformGP_color, uniformCA_color;
GLuint uniformOverlayTSA_TR, uniformOverlayTSA_C;
GLuint uniformTSAMiddle_TR,uniformTSAMiddle_A;
GLuint uniform3D_LightPos,uniform3DNL_Tex,uniform3DSW_C;
GLuint uniformSO_Color,uniform3D_FR_Color,uniform3D_FB_Color,uniform3D_FB_BackColor,uniform3D_FR_TS,uniform3D_FB_TS;
GLuint uniformTS_TR,uniformTS_Params;

GLuint uniformSP_Global;
void multiPlayerRespawnBall(LOScriptEffectType type,SunnyVector2D pos, SunnyVector2D vel){}
void multiPlayerSnowExploded(LOScriptEffectType type, short ballNum){}
void multiPlayerMinutePast(){}
void multiPlayerWind(){}

char * getLocalizedLabel(const char * label)
{
    const char * l = getLocalizedString(label);
    int len = (int)strlen(l);
    char * str = new char[len+1];
    strcpy(str, l);
    return str;
}

char * getLocalizedLabel(const char * label, char * buf)
{
    const char * l = getLocalizedString(label);
    strcpy(buf, l);
    return buf;
}

LOResetType resetType = RT_None;


#ifdef __ANDROID__
#include "MyJni.h"
void (*sucks)(const char*);
extern JniNativeCallListener listener;
const char * getLocalizedString(const char * locKey)
{
	return listener.callGetLocalizedString(locKey);
}

void showLeaderboard()
{
    
}

void gameOverMultiPlayer(){}
void loadMapAfterAnimation(){}
void multiPlayerUpdatePosition(){}

void LOSetUserIdForFlurry(const char * str)
{
    
}

void LOSendFlurryMessageString(const char * eventName, ...)
{
    char ** keyValue = (char**)malloc(sizeof(char*));
    int count = 0;
    char* str;
	va_list vl;
	va_start(vl,eventName);
	LOGD("a");
	do {
		str=va_arg(vl,char*);
		if (str == NULL) break;
        if (count)
            keyValue = (char**)realloc(keyValue, sizeof(char*)*(count+1));
        keyValue[count] = new char [strlen(str)+1];
        strcpy(keyValue[count], str);
        count++;
	} while (true);
	va_end(vl);
	LOGD("b");
    //Send here to Java
    listener.callSendFlurryMessageString(eventName, count, keyValue);
    // OK, bro
    for (short i = 0;i<count;i++)
        delete []keyValue[i];
    free(keyValue);
}
void LOSendFlurryMessageInt(const char * eventName, ...)
{
    char ** keys = (char**)malloc(sizeof(char*));
    int *values = (int*)malloc(sizeof(int));
    int count = 0;
    bool isItKey = true;
    char* str;
	va_list vl;
	va_start(vl,eventName);
	do {
        if (isItKey)
        {
            str=va_arg(vl,char*);
            if (str == NULL) break;
            if (count)
            {
                keys = (char**)realloc(keys,sizeof(char*)*(count+1));
                values = (int*)realloc( values, sizeof(int)*(count+1));
            }
            keys[count] = new char[strlen(str)+1];
            strcpy(keys[count], str);
            isItKey = !isItKey;
        }
        else
            {
                int v = va_arg(vl,int);
                values[count] = v;
                count++;
                isItKey = !isItKey;
            }
	} while (true);
	va_end(vl);
    
    //Send here to Java
    listener.callSendFlurryMessageInt(eventName, count, keys, values);
    for (short i = 0;i<count;i++)
        delete []keys[i];
    free(keys);
    free(values);
}

#endif

void scheduleLocalNotification(float delaySec, const char * textBody, const char * alertAction, const char * soundName)
{
#ifdef IPHONE_OS
    scheduleLocalNotificationIOS(delaySec, textBody, alertAction, soundName);
#endif
}

void cancelAllLocalNotifications()
{
#ifdef IPHONE_OS
    cancelAllLocalNotificationsIOS();
#endif
}


#ifdef MAC_OS
void LOSendFlurryMessageString(const char * eventName, ...){}
void LOSendFlurryMessageInt(const char * eventName, ...){}
#endif

void activateAccelerometer()
{
    
}

void deactivateAccelerometer()
{
    
}

void loCallbackSetMusic(const char * fileName, unsigned short startPosition){
#ifdef IPHONE_OS
    loCallbackSetMusicIOS(fileName,startPosition);
#endif
    
#ifdef __ANDROID__
    listener.callSetMusic(fileName,startPosition);
#endif
}

void loCallbackSetMusicPosition(unsigned short startPosition){
#ifdef IPHONE_OS
    loCallbackSetMusicPositionIOS(startPosition);
#endif
    
#ifdef __ANDROID__
    listener.callSetMusicPosition(startPosition);
#endif
}


void loCallBackVibrate(){
#ifdef IPHONE_OS
    loCallBackVibrateIOS();
#endif
    
#ifdef __ANDROID__
    listener.callVibrate();
#endif
}

void loOpenURL(const char * url){
#ifdef IPHONE_OS
    loOpenURLIOS(url);
#endif

#ifdef __ANDROID__
	listener.callOpenURL(url);
#endif
}

void loOpenTWFunPage(const char * url, const char * pageID){
#ifdef __ANDROID__
	listener.callOpenTWFunPage(url,pageID);
#endif
}

void loOpenUTFunPage(const char * url, const char * pageID){
#ifdef IPHONE_OS
    loOpenUTFunPageIOS(url,pageID);
#endif

#ifdef __ANDROID__
	listener.callOpenUTFunPage(url, pageID);
#endif
}

void loOpenFBFunPage(const char * url, const char * pageID){
#ifdef IPHONE_OS
    loOpenFBFunPageIOS(url,pageID);
#endif

#ifdef __ANDROID__
	listener.callOpenFBFunPage(url, pageID);
#endif
}

void loShareFacebookLC(const int levelNumber, const char* episodeName, const float levelTime, const char* messageName, const char * messageCaption, const char * messageDesc){
#ifdef IPHONE_OS
    loShareFacebookLCIOS(levelNumber,episodeName,levelTime,messageName,messageCaption,messageDesc);
#endif
    
#ifdef __ANDROID__
	listener.callShareFacebookLC(levelNumber, episodeName,  levelTime,  messageName,  messageCaption,  messageDesc);
#endif
}

void loShareFacebookSurvival(const char* episodeName, const float levelTime, const char* messageName, const char * messageCaption, const char * messageDesc){
#ifdef IPHONE_OS
    loShareFacebookSurvivalIOS(episodeName,levelTime,messageName,messageCaption,messageDesc);
#endif
    
#ifdef __ANDROID__
	listener.callShareFacebookSurvival( episodeName,  levelTime,  messageName,  messageCaption,  messageDesc);
#endif
}

void loMakeFBConnection(void (*success)(const char*)){
#ifdef IPHONE_OS
    loMakeFBConnectionIOS(success);
#endif
#ifdef __ANDROID__
    sucks = success;
	listener.callMakeFBConnection();
#endif
}

void showGameCenter(){
#ifdef IPHONE_OS
    showGameCenterIOS();
#endif
#ifdef __ANDROID__
    listener.callShowGameCenter();
#endif
}

void loRateTheGame(){
#ifdef IPHONE_OS
    loRateTheGameIOS();
#endif
#ifdef __ANDROID__
	listener.callRateTheGame();
#endif
}

float musicVolumeKoef = 1;
float currentVolumeOfMusic = globalMusicVolume;

void losChangeMusicVolumeIOS();

void losSetMusicVolume(float volume)
{
    currentVolumeOfMusic = volume;
#ifdef IPHONE_OS
    losChangeMusicVolumeIOS();
#endif
    
#ifdef __ANDROID__
    listener.callSetMusicVolume(currentVolumeOfMusic);
#endif
}

void loExitTheApp()
{
#ifdef __ANDROID__
	listener.callExitTheApp();
#endif
}

void loBackButtonPressed()
{
    activeMap->mainMenu->backButtonPressed();
}

void losChangeVolumeOfTheMusic()
{
    if (activeMap->isLevelComplete())
    {
        if (musicVolumeKoef>0.1)
        {
            musicVolumeKoef-=0.1;
            losSetMusicVolume(currentVolumeOfMusic);
        }
    } else
    {
        if (musicVolumeKoef<1)
        {
            musicVolumeKoef+=0.1;
            losSetMusicVolume(currentVolumeOfMusic);
        }
    }
}


double loginTime = 0;
unsigned long loginTimeServer = 0;

unsigned long getCurrentServerTime()
{
    return (unsigned long)(getCurrentTime() - loginTime + loginTimeServer);
}
