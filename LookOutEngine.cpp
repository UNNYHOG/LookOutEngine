//
//  LookOutEngine.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOGlobalVar.h"
#include "LookOutEngine.h"
#include "SunnyOpengl.h"
#include "SHTextureBase.h"
#include "iPhone_Strings.h"
#include "LOScore.h"

#include "SunnyCollisionDetector.h"

#include "LOBonfire.h"
#include "SHTexture.h"
#include "LORollingBlades.h"
#include "LOSounds.h"
#include "SunnyShaders.h"
#include "SunnyFunctions.h"
#include "LOTrees.h"
#include "LOFont.h"
#include "LOSettings.h"

void loRenderToFrameBuffer();

SunnyVector3D lastAcc(0,0,-1);

GLuint textureVertsVAO = 0;
GLuint textureVertsVBO = 0;
GLuint textureIndVBO = 0;

float maxJoysticDistance = 0.1;
SunnyVector2D joysticCustomPosition[2];

SunnyVector2D blurPosition;
float blurPower = 0;

void collisionPlayerAndPlayerBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndPlayer((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndFireBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndFireBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndWaterBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndWaterBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndGroundBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndGroundBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndAngryBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndAngryBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndButtonBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndButtonBegan((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndButtonEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndButtonEnded((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndFireBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndFireBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndWaterBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndWaterBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndGroundBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndGroundBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndBonfireBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndBonfire((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndElectricFieldBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndElectricField((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndElectricFieldEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndElectricFieldEnded((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionFireBallAndFireBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndFireBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionFireBallAndWaterBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndWaterBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionFireBallAndGroundBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndGroundBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionFireBallAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionWaterBallAndWaterBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndWaterBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionWaterBallAndGroundBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndGroundBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionWaterBallAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionGroundBallAndGroundBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionGroundBallAndGroundBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionGroundBallAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionGroundBallAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAnomalyBallAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAnomalyBallAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionElectricBallAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
//    return 0;
}

void collisionHighSnowAndFireBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionHighSnowAndFireBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionHighSnowAndWaterBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionHighSnowAndWaterBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionHighSnowAndGroundBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionHighSnowAndGroundBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionHighSnowAndAnomalyBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionHighSnowAndAnomalyBall((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndSnowflakeBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndSnowflake((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndVioletSnowflakeBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndVioletSnowflake((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndGoldCoinBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndGoldCoin((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndBonfireBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndBonfire((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndRollingBladesBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndRollingBlades((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndElectricFieldBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndElectricField((SC_Circle*)body1,(SC_Circle*)body2);
}

//Electric Ball
void collisionPlayerAndElectricBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndElectricBallBegan((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionPlayerAndElectricBallEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndElectricBallEnded((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionFireBallAndElectricBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndElectricBallBegan((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionFireBallAndElectricBallEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndElectricBallEnded((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionWaterBallAndElectricBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndElectricBallBegan((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionWaterBallAndElectricBallEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndElectricBallEnded((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionGroundBallAndElectricBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionGroundBallAndElectricBallBegan((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionGroundBallAndElectricBallEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionGroundBallAndElectricBallEnded((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionAngryBallAndElectricBallEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndElectricBallEnded((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionAngryBallAndElectricBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallAndElectricBallBegan((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionElectricBallAndElectricBallEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionElectricBallAndElectricBallEnded((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionElectricBallAndElectricBallBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionElectricBallAndElectricBallBegan((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionAngryBallChargedBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionAngryBallChargedBegan((SC_Circle*)body1,(SC_Circle*)body2);
}

//El Ball...
//fire
void collisionFireBallAndElectricFieldBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndElectricField((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionFireBallAndElectricFieldEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionFireBallAndElectricFieldEnded((SC_Circle*)body1,(SC_Circle*)body2);
}

//ground
void collisionGroundBallAndElectricFieldBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionGroundBallAndElectricField((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionGroundBallAndElectricFieldEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionGroundBallAndElectricFieldEnded((SC_Circle*)body1,(SC_Circle*)body2);
}
//water
void collisionWaterBallAndElectricFieldBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndElectricField((SC_Circle*)body1,(SC_Circle*)body2);
}
void collisionWaterBallAndElectricFieldEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionWaterBallAndElectricFieldEnded((SC_Circle*)body1,(SC_Circle*)body2);
}

//anomaly
void collisionAnomalyBallAndElectricFieldBegan(SC_Body * body1, SC_Body * body2)
{
//    activeMap->collisionWaterBallAndElectricField(arb->body_a_private,arb->body_b_private);
}
void collisionAnomalyBallAndElectricFieldEnded(SC_Body * body1, SC_Body * body2)
{
//    activeMap->collisionWaterBallAndElectricFieldEnded(arb->body_a_private,arb->body_b_private);
}

void collisionPlayerAndIceBlockBegan(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndIceBlock((SC_Circle*)body1,(SC_Circle*)body2);
}

void collisionPlayerAndIceBlockEnded(SC_Body * body1, SC_Body * body2)
{
    activeMap->collisionPlayerAndIceBlockEnded((SC_Circle*)body1,(SC_Circle*)body2);
}

int loGetCollectedSnowflakes()
{
    return activeMap->getCollectedSnowflakes();
}

void addPointsToArray(SunnyVector4D texture,short num, SunnyVector4D *&points, bool inverse, SunnyVector2D textureSize)
{
    const float z = 1.0/640*mapSize.y/2;//(2048./textureSize.y);
    const float tx = 1.0/textureSize.x;
    const float ty = 1.0/textureSize.y;
    if (inverse)
    {
        points[num*4+0] = SunnyVector4D(+texture.z*z,-texture.w*z,texture.x*tx,texture.y*ty);
        points[num*4+1] = SunnyVector4D(+texture.z*z,+texture.w*z,texture.x*tx,(texture.y+texture.w)*ty);
        points[num*4+2] = SunnyVector4D(-texture.z*z,-texture.w*z,(texture.x+texture.z)*tx,texture.y*ty);
        points[num*4+3] = SunnyVector4D(-texture.z*z,+texture.w*z,(texture.x+texture.z)*tx,(texture.y+texture.w)*ty);
    } else
    {
        points[num*4+0] = SunnyVector4D(-texture.z*z,+texture.w*z,texture.x*tx,texture.y*ty);
        points[num*4+1] = SunnyVector4D(-texture.z*z,-texture.w*z,texture.x*tx,(texture.y+texture.w)*ty);
        points[num*4+2] = SunnyVector4D(+texture.z*z,+texture.w*z,(texture.x+texture.z)*tx,texture.y*ty);
        points[num*4+3] = SunnyVector4D(+texture.z*z,-texture.w*z,(texture.x+texture.z)*tx,(texture.y+texture.w)*ty);
    }
}

void translatePointsInArray(SunnyVector2D texture,short num, SunnyVector4D *&points)
{
    const float z = 1.0/640*mapSize.y/2;
    texture *= z;
    points[num*4+0].x += texture.x;points[num*4+0].y += texture.y;
    points[num*4+1].x += texture.x;points[num*4+1].y += texture.y;
    points[num*4+2].x += texture.x;points[num*4+2].y += texture.y;
    points[num*4+3].x += texture.x;points[num*4+3].y += texture.y;
}

void scalePointsSize(SunnyVector2D scale,short num,SunnyVector4D *&points)
{
    for (short i = 0;i<4;i++)
    {
        points[num*4+i].x *= scale.x;
        points[num*4+i].y *= scale.y;
    }
}

void scalePointsToSize(SunnyVector2D newSize,short num,SunnyVector4D *&points)
{
    for (short i = 0;i<4;i++)
    {
//        printf("\n Scale = (%f, %f)", newSize.x/points[num*4+i].x,newSize.y/points[num*4+i].y);
        points[num*4+i].x = sunnyZnak(points[num*4+i].x)*newSize.x;
        points[num*4+i].y = sunnyZnak(points[num*4+i].y)*newSize.y;
    }
}

void loMakeFrameBufferTexture(GLint &w, GLint &h)
{
    int textureSizeX = screenSize.x;
    int textureSizeY = screenSize.y;
    int i;
    
    if((textureSizeX != 1) && (textureSizeX & (textureSizeX - 1))) {
		i = 1;
		while(i < textureSizeX)
			i *= 2;
		textureSizeX = i;
	}
	
	if((textureSizeY != 1) && (textureSizeY & (textureSizeY - 1))) {
		i = 1;
		while (i < textureSizeY)
			i *= 2;
		textureSizeY = i;
	}
    
    textureForRender = makeTextureWithData(0, kTexture2DPixelFormat_RGB888, textureSizeX,textureSizeY);
    w = textureSizeX;
    h = textureSizeY;
    
    if (textureVertsVBO == 0)
    {
        SunnyVector4D vert[] = {
            SunnyVector4D(-horizontalMapAdd,mapSize.y+verticalMapAdd, 0, screenSize.y/textureSizeY),
            SunnyVector4D(-horizontalMapAdd,-verticalMapAdd, 0,0),
            SunnyVector4D(mapSize.x+horizontalMapAdd,mapSize.y+verticalMapAdd, screenSize.x/textureSizeX,screenSize.y/textureSizeY),
            SunnyVector4D(mapSize.x+horizontalMapAdd,-verticalMapAdd, screenSize.x/textureSizeX,0)
        };
        if (isVAOSupported)
        {
            glGenVertexArrays(1, &textureVertsVAO);
            glBindVertexArray(textureVertsVAO);
        }
        glGenBuffers(1, &textureVertsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, textureVertsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SunnyVector4D)*4, &vert[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &textureIndVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureIndVBO);
        unsigned short num[] = {0,1,2,3};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*4, num, GL_STATIC_DRAW);
        if (isVAOSupported)
            glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void loMakeGlobalVAO()
{
    if (gameGlobalVBO) return;
    
    SunnyVector4D *points;
    const short objectsCount = LOCount_VAO;
    points = new SunnyVector4D[objectsCount*4];
    /////////1 Ice Block
    addPointsToArray(SunnyVector4D(562,128,78,80), LOIceBlock_VAO, points,false);
    translatePointsInArray(SunnyVector2D(-12,-28), LOIceBlock_VAO, points);
    
    ///////////18 Bonfire
    {
        addPointsToArray(SunnyVector4D(440,350,68,68), LOBonfire_VAO+0, points);
        addPointsToArray(SunnyVector4D(440,286,30,32), LOBonfire_VAO+1, points);
        addPointsToArray(SunnyVector4D(440,318,30,32), LOBonfire_VAO+2, points);
        addPointsToArray(SunnyVector4D(440,254,30,32), LOBonfire_VAO+3, points);
        addPointsToArray(SunnyVector4D(280,266,160,164), LOBonfire_VAO+4, points);
        for (short i = 0;i<12;i++)
            addPointsToArray(SunnyVector4D(724+(i%4)*44,60+(i/4)*44,44,44), LOBonfire_VAO+5+i, points);
        addPointsToArray(SunnyVector4D(380,220,46,46), LOBonfire_VAO+17, points);
    }

    ///////////1 background
    {
        float w = 970./2./640*mapSize.y;//mapScale;
        float h = 650./2./640*mapSize.y;//mapScale;
        points[LOBackground_VAO*4  ] = SunnyVector4D(mapSize.x/2-w,mapSize.y/2+h,0,(2048.-650)/2048.);
        points[LOBackground_VAO*4+1] = SunnyVector4D(mapSize.x/2-w,mapSize.y/2-h,0,1);
        points[LOBackground_VAO*4+2] = SunnyVector4D(mapSize.x/2+w,mapSize.y/2+h,970./2048.,(2048.-650)/2048.);
        points[LOBackground_VAO*4+3] = SunnyVector4D(mapSize.x/2+w,mapSize.y/2-h,970./2048.,1.);
    }
    
    //////////9 warnTriangles
    {
        addPointsToArray(SunnyVector4D(826,508,68,90), LOWarnTriangles_VAO+0, points);
        addPointsToArray(SunnyVector4D(830,424,56,84), LOWarnTriangles_VAO+1, points);
        addPointsToArray(SunnyVector4D(826,690,58,72), LOWarnTriangles_VAO+2, points);
        addPointsToArray(SunnyVector4D(826,598,64,92), LOWarnTriangles_VAO+3, points);
        addPointsToArray(SunnyVector4D(728,192,100,100), LOWarnTriangles_VAO+4, points);
        addPointsToArray(SunnyVector4D(728,292,100,100), LOWarnTriangles_VAO+5, points);
        addPointsToArray(SunnyVector4D(830,194,74,74), LOWarnTriangles_VAO+6, points);
        addPointsToArray(SunnyVector4D(716,0,60,60), LOWarnTriangles_VAO+7, points,false);
        addPointsToArray(SunnyVector4D(830,780,58,82), LOWarnTriangles_VAO+8, points);
    }
    
    //////////12 shadows
    {
        //ball
        addPointsToArray(SunnyVector4D(872,0,42,42), LOShadows_VAO, points);
        //parts
        addPointsToArray(SunnyVector4D(668,302,20,20), LOShadows_VAO+1, points);
        //ground ball
        addPointsToArray(SunnyVector4D(914,0,52,52), LOShadows_VAO+2, points);
        //water ball
        addPointsToArray(SunnyVector4D(966,0,56,56), LOShadows_VAO+3, points);
        //under player
        addPointsToArray(SunnyVector4D(546,542,60,60), LOShadows_VAO+4, points);
        //player shadow menu
        addPointsToArray(SunnyVector4D(1818,1090,230,306), LOShadows_VAO+5, points);
        //собираются перед шарами при падении
        addPointsToArray(SunnyVector4D(238,386,34,22), LOShadows_VAO+6, points,false);
        //тень от башен
        addPointsToArray(SunnyVector4D(902,148,38,36), LOShadows_VAO+7, points,false);
        translatePointsInArray(SunnyVector2D(0,9), LOShadows_VAO+7, points);
        //take snowflake affect
        addPointsToArray(SunnyVector4D(830,270,74,74), LOShadows_VAO+8, points,false);
        //Multiplayer
        addPointsToArray(SunnyVector4D(1882,532,56,56), LOShadows_VAO+9, points,false);
        //MinutePast
        addPointsToArray(SunnyVector4D(1960,502,86,86), LOShadows_VAO+10, points,false);
        addPointsToArray(SunnyVector4D(1952,406,94,94), LOShadows_VAO+11, points,false);
    }
    
    /////////60 LOObject 
    {
        const short fireballfix = 18;
        for (short i = 0;i<20;i++)
        {
            addPointsToArray(SunnyVector4D(0+(i%7)*70.,442+(i/7)*154.,70,154.), LOObject_VAO+i, points);
            translatePointsInArray(SunnyVector2D(0,-80+fireballfix),LOObject_VAO+i,points);
        }
        //need to add 21 objects
        
        //блик
        addPointsToArray(SunnyVector4D(490,722,60,164), LOObject_VAO+41, points);
        addPointsToArray(SunnyVector4D(490,886,60,164), LOObject_VAO+42, points);
        //water ball
        addPointsToArray(SunnyVector4D(688,302,40,40), LOObject_VAO+43, points);
        //electricity ball
        addPointsToArray(SunnyVector4D(724,776,100,100), LOObject_VAO+44, points);
        addPointsToArray(SunnyVector4D(724,676,100,100), LOObject_VAO+45, points);
        addPointsToArray(SunnyVector4D(724,576,100,100), LOObject_VAO+46, points);
        addPointsToArray(SunnyVector4D(624,776,100,100), LOObject_VAO+47, points);
        addPointsToArray(SunnyVector4D(624,676,100,100), LOObject_VAO+48, points);
        addPointsToArray(SunnyVector4D(624,576,100,100), LOObject_VAO+49, points);
        //el. ball attack
        addPointsToArray(SunnyVector4D(614,876,64,116), LOObject_VAO+50, points,false);
        addPointsToArray(SunnyVector4D(550,936,64,116), LOObject_VAO+51, points,false);
        addPointsToArray(SunnyVector4D(550,820,64,116), LOObject_VAO+52, points,false);
        for (short i = 50;i<=52;i++)
            translatePointsInArray(SunnyVector2D(0,116-60), LOObject_VAO+i, points);
        //el. explosion
        addPointsToArray(SunnyVector4D(680,878,84,84), LOObject_VAO+53, points,false);
        //anomalyball
        addPointsToArray(SunnyVector4D(764,1012,44,46), LOObject_VAO+54, points);
        
        //Fast fireball
        addPointsToArray(SunnyVector4D(836,1332,70,60), LOObject_VAO+55, points);
        addPointsToArray(SunnyVector4D(836,1262,72,66), LOObject_VAO+56, points);
        //Anomaly Electricity
        addPointsToArray(SunnyVector4D(844,984,32,28), LOObject_VAO+57, points);
        addPointsToArray(SunnyVector4D(848,1014,24,24), LOObject_VAO+58, points);
        addPointsToArray(SunnyVector4D(842,1044,38,38), LOObject_VAO+59, points);
    }
    
    ////////////16 Traces
    {
        addPointsToArray(SunnyVector4D(112,398,42,42), LOTraces_VAO, points);
        addPointsToArray(SunnyVector4D(196,398,42,42), LOTraces_VAO+1, points);
        addPointsToArray(SunnyVector4D(154,398,42,42), LOTraces_VAO+2, points);
        //fire posteffect
        addPointsToArray(SunnyVector4D(574,254,94,120), LOTraces_VAO+3, points);
        translatePointsInArray(SunnyVector2D(0,-40), LOTraces_VAO+3, points);
        //water posteffect
        addPointsToArray(SunnyVector4D(1742,0,100,104), LOTraces_VAO+4, points);
        //ground posteffect
        addPointsToArray(SunnyVector4D(1742,0,106,104), LOTraces_VAO+5, points);
        //snow ball parts
        addPointsToArray(SunnyVector4D(668,254,20,48), LOTraces_VAO+6, points);
        addPointsToArray(SunnyVector4D(688,254,20,48), LOTraces_VAO+7, points);
        addPointsToArray(SunnyVector4D(708,254,20,48), LOTraces_VAO+8, points);
        //ground water posteffect
        addPointsToArray(SunnyVector4D(1722,498,156,158), LOTraces_VAO+9, points);
        //el. post effect
        addPointsToArray(SunnyVector4D(554,82,46,42), LOTraces_VAO+10, points);
        //angyBall resp
        addPointsToArray(SunnyVector4D(1724,706,54,54), LOTraces_VAO+11, points);
        //AngryBall explosion
        addPointsToArray(SunnyVector4D(584,1128,110,110), LOTraces_VAO+12, points);
        //iceCube trace
        addPointsToArray(SunnyVector4D(1686,1336 ,60 ,60), LOTraces_VAO+13, points,false);
        //Snowwalls
        addPointsToArray(SunnyVector4D(1832,250 ,72 ,72), LOTraces_VAO+14, points,false);
        //Teleportation
        addPointsToArray(SunnyVector4D(1804,388 ,134 ,122), LOTraces_VAO+15, points,false);
    }
    
    ///////////22 WaterBalls
    if (!isFastDevice)
    {
        for (short i = 0;i<3;i++)
            for (short j = 0;j<6;j++)
            {
                addPointsToArray(SunnyVector4D(592+j*40,1268+i*40,40,40), LOWaterBall_VAO+i*6+j, points, false);
            }
    } else
    {
        const float size = cellSize/2;
        const float rx = mapSize.x/mapScale;
        const float ry = mapSize.y/mapScale;
        const float sx = screenSize.x/rx/texturebufferWidth;
        const float sy = screenSize.y/ry/texturebufferHeight;
        const float cx = (rx - mapSize.x)/2;
        const float cy = (ry - mapSize.y)/2;
        
        points[LOWaterBall_VAO*4] = SunnyVector4D(0,0,0,0);
        float angle;
        for (short i = 1;i<22;i++)
        {
            angle = (i-1)*2*M_PI/(22-2);
            points[LOWaterBall_VAO*4+i] = SunnyVector4D(size*cosf(angle),size*sinf(angle),(cx + 1.5*size + size/2*cosf(angle))*sx,(cy+size/2*sinf(angle))*sy);
        }
    }
    
    ///////////18 RollingBlades
    {
        addPointsToArray(SunnyVector4D(512,94,42,42), LORollingBlades_VAO, points);
        addPointsToArray(SunnyVector4D(558,210,42,42), LORollingBlades_VAO+1, points);
        addPointsToArray(SunnyVector4D(510,202,46,52), LORollingBlades_VAO+2, points);
        addPointsToArray(SunnyVector4D(426,120,34,134), LORollingBlades_VAO+3, points);
        addPointsToArray(SunnyVector4D(510,138,50,64), LORollingBlades_VAO+4, points);
        addPointsToArray(SunnyVector4D(460,104,50,150), LORollingBlades_VAO+5, points);
        
        translatePointsInArray(SunnyVector2D(0,115), LORollingBlades_VAO+3, points);
        translatePointsInArray(SunnyVector2D(0,115), LORollingBlades_VAO+5, points);
        
        //ложбина для пил
        addPointsToArray(SunnyVector4D(622,224,16,28), LORollingBlades_VAO+6, points,false);
        addPointsToArray(SunnyVector4D(602,224,20,28), LORollingBlades_VAO+7, points,false);
        addPointsToArray(SunnyVector4D(638,224,18,28), LORollingBlades_VAO+8, points,false);
        translatePointsInArray(SunnyVector2D(-20,0), LORollingBlades_VAO+7, points);
        translatePointsInArray(SunnyVector2D( 18,0), LORollingBlades_VAO+8, points);
        
        //Angry Ball Light
        addPointsToArray(SunnyVector4D(642,142,80,80), LORollingBlades_VAO+9, points,false);
        addPointsToArray(SunnyVector4D(552,604,72,72), LORollingBlades_VAO+10, points,false);
        addPointsToArray(SunnyVector4D(552,748,72,72), LORollingBlades_VAO+11, points,false);
        addPointsToArray(SunnyVector4D(552,674,72,72), LORollingBlades_VAO+12, points,false);
        
        //Angry ball charging
        addPointsToArray(SunnyVector4D(636,1054,72,72), LORollingBlades_VAO+13, points,false);
        addPointsToArray(SunnyVector4D(492,1054,72,72), LORollingBlades_VAO+14, points,false);
        addPointsToArray(SunnyVector4D(564,1054,72,72), LORollingBlades_VAO+15, points,false);
        
        //AngryIndicator
        addPointsToArray(SunnyVector4D(614,992,60,60), LORollingBlades_VAO+16, points,false);
        //AngryBall Apear
        addPointsToArray(SunnyVector4D(1870,202,48,48), LORollingBlades_VAO+17, points,false);
    }
    
    ////////3 Snowflakes
    {
        addPointsToArray(SunnyVector4D(534,254,38,38), LOSnowflake_VAO, points);
        addPointsToArray(SunnyVector4D(470,254,64,64), LOSnowflake_VAO+1, points);
        addPointsToArray(SunnyVector4D(546,292,28,130), LOSnowflake_VAO+2, points);
    }
    
    ///////17 Smoke
    {
        addPointsToArray(SunnyVector4D(224      ,0,82,82), LOSmoke_VAO, points);
        addPointsToArray(SunnyVector4D(224+82   ,0,82,82), LOSmoke_VAO+1, points);
        addPointsToArray(SunnyVector4D(224+81*2 ,0,82,82), LOSmoke_VAO+2, points);
        addPointsToArray(SunnyVector4D(224+82*3 ,0,82,82), LOSmoke_VAO+3, points);
        addPointsToArray(SunnyVector4D(224+82*4 ,0,82,82), LOSmoke_VAO+4, points);
        addPointsToArray(SunnyVector4D(224+82*5 ,0,82,82), LOSmoke_VAO+5, points);
        //fire undertexture
        addPointsToArray(SunnyVector4D(490+4,1176 ,92 - 4 ,220), LOSmoke_VAO+6, points);
        translatePointsInArray(SunnyVector2D(4,-65 + 20), LOSmoke_VAO+6, points);
        //ground smoke
        addPointsToArray(SunnyVector4D(1614,132 ,54 ,54), LOSmoke_VAO+7, points);
        //borderShadow
        //addPointsToArray(SunnyVector4D(224+82*5 ,0,82,82), LOSmoke_VAO+5, points);
        
        float step = mapSize.x*58./960.;//затемнения по сторонам
        points[(LOSmoke_VAO+8)*4]   = SunnyVector4D(-horizontalMapAdd,mapSize.y+verticalMapAdd,2040/2048.,(346+58)/2048.);
        points[(LOSmoke_VAO+8)*4+1] = SunnyVector4D(-horizontalMapAdd,-verticalMapAdd,(2040+6)/2048.,(346+58)/2048.);
        points[(LOSmoke_VAO+8)*4+2] = SunnyVector4D(-horizontalMapAdd + step,mapSize.y+verticalMapAdd,2040./2048.,346/2048.);
        points[(LOSmoke_VAO+8)*4+3] = SunnyVector4D(-horizontalMapAdd + step,-verticalMapAdd,(2040+6)/2048.,346/2048.);
        
        points[(LOSmoke_VAO+9)*4]   = SunnyVector4D(-horizontalMapAdd, mapSize.y+verticalMapAdd,2040/2048.,(346+58)/2048.);
        points[(LOSmoke_VAO+9)*4+1] = SunnyVector4D(-horizontalMapAdd, mapSize.y+verticalMapAdd-step,2040./2048.,346./2048.);
        points[(LOSmoke_VAO+9)*4+2] = SunnyVector4D(mapSize.x+horizontalMapAdd, mapSize.y+verticalMapAdd,(2040+6)/2048.,(346+58)/2048.);
        points[(LOSmoke_VAO+9)*4+3] = SunnyVector4D(mapSize.x+horizontalMapAdd, mapSize.y+verticalMapAdd-step,2040/2048.,346/2048.);
        
        points[(LOSmoke_VAO+10)*4]   = SunnyVector4D(mapSize.x+horizontalMapAdd-step,mapSize.y+verticalMapAdd,2040/2048.,(346)/2048.);
        points[(LOSmoke_VAO+10)*4+1] = SunnyVector4D(mapSize.x+horizontalMapAdd-step,-verticalMapAdd,(2040+6)/2048.,(346)/2048.);
        points[(LOSmoke_VAO+10)*4+2] = SunnyVector4D(mapSize.x+horizontalMapAdd,mapSize.y+verticalMapAdd,2040./2048.,(346+58)/2048.);
        points[(LOSmoke_VAO+10)*4+3] = SunnyVector4D(mapSize.x+horizontalMapAdd,-verticalMapAdd,(2040+6)/2048.,(346+58)/2048.);
        
        points[(LOSmoke_VAO+11)*4]   = SunnyVector4D(-horizontalMapAdd, -verticalMapAdd+step,2040/2048.,(346)/2048.);
        points[(LOSmoke_VAO+11)*4+1] = SunnyVector4D(-horizontalMapAdd, -verticalMapAdd,2040./2048.,(346.+58)/2048.);
        points[(LOSmoke_VAO+11)*4+2] = SunnyVector4D(mapSize.x+horizontalMapAdd, -verticalMapAdd+step,(2040+6)/2048.,(346)/2048.);
        points[(LOSmoke_VAO+11)*4+3] = SunnyVector4D(mapSize.x+horizontalMapAdd, -verticalMapAdd,2040/2048.,(346+58)/2048.);
        //Explosion shadow
        addPointsToArray(SunnyVector4D(900,56 ,90 ,90), LOSmoke_VAO+12, points);
        addPointsToArray(SunnyVector4D(1750,1244 ,64 ,148), LOSmoke_VAO+13, points);
        translatePointsInArray(SunnyVector2D(0,-130),LOSmoke_VAO+13,points);
        
        //Anomaly Ball Smoke
        addPointsToArray(SunnyVector4D(810,1016,28,30), LOSmoke_VAO+14, points);
        addPointsToArray(SunnyVector4D(810,986,28,28), LOSmoke_VAO+15, points);
        addPointsToArray(SunnyVector4D(782,986,26,24), LOSmoke_VAO+16, points);
    }
    
    /////// 14 Preview
    {
        addPointsToArray(SunnyVector4D(2016,1936,14,14), LOPreview_VAO, points);
        addPointsToArray(SunnyVector4D(2032,1936,14,14), LOPreview_VAO+1, points);
        
        addPointsToArray(SunnyVector4D(2016,1952,14,14), LOPreview_VAO+2, points);
        addPointsToArray(SunnyVector4D(2032,1952,14,14), LOPreview_VAO+3, points);
        
        addPointsToArray(SunnyVector4D(2016,1968,14,14), LOPreview_VAO+4, points);
        addPointsToArray(SunnyVector4D(2032,1968,14,14), LOPreview_VAO+5, points);
        
        addPointsToArray(SunnyVector4D(2016,1984,14,14), LOPreview_VAO+6, points);
        addPointsToArray(SunnyVector4D(2032,1984,14,14), LOPreview_VAO+7, points);
        
        addPointsToArray(SunnyVector4D(2016,2000,14,14), LOPreview_VAO+8, points);
        addPointsToArray(SunnyVector4D(2032,2000,14,14), LOPreview_VAO+9, points);
        
        addPointsToArray(SunnyVector4D(2016,2016,14,14), LOPreview_VAO+10, points);
        addPointsToArray(SunnyVector4D(2032,2016,14,14), LOPreview_VAO+11, points);
        
        addPointsToArray(SunnyVector4D(2016,2032,14,14), LOPreview_VAO+12, points);
        
        const float v = cellSize/2;
        for (short i = 0;i<13;i++)
        {
            points[(LOPreview_VAO+i)*4].x = points[(LOPreview_VAO+i)*4].y = 0;
            points[(LOPreview_VAO+i)*4+1].x = 0;points[(LOPreview_VAO+i)*4+1].y = -v;
            points[(LOPreview_VAO+i)*4+2].x = v;points[(LOPreview_VAO+i)*4+2].y = 0;
            points[(LOPreview_VAO+i)*4+3].x = v;points[(LOPreview_VAO+i)*4+3].y = -v;
        }
        
        addPointsToArray(SunnyVector4D(964,922,672,464), LOPreview_VAO+13, points);
    }
    
    //Buttons 3
    {
        addPointsToArray(SunnyVector4D(676,962,52,50), LOButton_VAO, points);
        addPointsToArray(SunnyVector4D(710,1012,48,44), LOButton_VAO+1, points);
        //Borders
        addPointsToArray(SunnyVector4D(1954,1396,92,86), LOButton_VAO+2, points,false);
        translatePointsInArray(SunnyVector2D(92-34*2-1 - 45,-86+35*2+2 +35), LOButton_VAO+2, points);
    }
    
    //Death 3
    {
        SunnyVector2D halfSize = SunnyVector2D(mapSize.x+horizontalMapAdd*2, mapSize.y + verticalMapAdd*2)/2;
        SunnyVector2D cen = SunnyVector2D(0,0);//mapSize/2;
        
        points[(LODeath_VAO+0)*4]   = SunnyVector4D(cen.x-halfSize.x,cen.y+halfSize.y,970./2048.,(2048-650)/2048.);
        points[(LODeath_VAO+0)*4+1] = SunnyVector4D(cen.x-halfSize.x,cen.y-halfSize.y,970./2048.,(2048)/2048.);
        points[(LODeath_VAO+0)*4+2] = SunnyVector4D(cen.x+halfSize.x,cen.y+halfSize.y,970./2048.*2,(2048-650)/2048.);
        points[(LODeath_VAO+0)*4+3] = SunnyVector4D(cen.x+halfSize.x,cen.y-halfSize.y,970./2048.*2,2048./2048.);
        
        points[(LODeath_VAO+1)*4]   = SunnyVector4D(cen.x-halfSize.x,cen.y+halfSize.y,0,(1280.)/2048.);
        points[(LODeath_VAO+1)*4+1] = SunnyVector4D(cen.x-halfSize.x,cen.y-halfSize.y,0,(1280.+768)/2048.);
        points[(LODeath_VAO+1)*4+2] = SunnyVector4D(cen.x+halfSize.x,cen.y+halfSize.y,1024./2048.,(1280.)/2048.);
        points[(LODeath_VAO+1)*4+3] = SunnyVector4D(cen.x+halfSize.x,cen.y-halfSize.y,1024./2048.,(1280.+768)/2048.);
        
        float w = 180./screenSize.x*halfSize.x*2;
        points[(LODeath_VAO+2)*4]   = SunnyVector4D(cen.x-halfSize.x,cen.y+halfSize.y,1944./2048,1492./2048.);
        points[(LODeath_VAO+2)*4+1] = SunnyVector4D(cen.x-halfSize.x,cen.y-halfSize.y,1944./2048,(1492.+2)/2048.);
        points[(LODeath_VAO+2)*4+2] = SunnyVector4D(cen.x-halfSize.x+w,cen.y+halfSize.y,(1944.+180)/2048.,(1492.)/2048.);
        points[(LODeath_VAO+2)*4+3] = SunnyVector4D(cen.x-halfSize.x+w,cen.y-halfSize.y,(1944.+180)/2048.,(1492.+2)/2048.);
    }
    
    //Joystic 2
    {
        addPointsToArray(SunnyVector4D(1854,1198,198,198), LOJoystic_VAO, points,false);
        
        float size = maxJoysticDistance*(mapSize.y+verticalMapAdd*2);
        scalePointsToSize(SunnyVector2D(size,size),LOJoystic_VAO,points);
    }
    
    //Numbers 26
    {
        for (int i = 0;i<2;i++)
        {
            addPointsToArray(SunnyVector4D(478     ,80*i,numbersSizes[0] ,80), LONumbers_VAO + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 + 62,80*i,numbersSizes[1] ,80), LONumbers_VAO+1 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +104,80*i,numbersSizes[2] ,80), LONumbers_VAO+2 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +164,80*i,numbersSizes[3] ,80), LONumbers_VAO+3 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +220,80*i,numbersSizes[4] ,80), LONumbers_VAO+4 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +274,80*i,numbersSizes[5] ,80), LONumbers_VAO+5 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +332,80*i,numbersSizes[6] ,80), LONumbers_VAO+6 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +394,80*i,numbersSizes[7] ,80), LONumbers_VAO+7 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +444,80*i,numbersSizes[8] ,80), LONumbers_VAO+8 + numbersCount*i, points, false);
            addPointsToArray(SunnyVector4D(478 +494,80*i,numbersSizes[9] ,80), LONumbers_VAO+9 + numbersCount*i, points, false);
            
            addPointsToArray(SunnyVector4D(2678,386 +180*i,numbersSymbolsSizes[0],166)/2, LONumbersSymbols_VAO + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(2734,386 +180*i,numbersSymbolsSizes[1],166)/2, LONumbersSymbols_VAO+1 + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(2790,386 +180*i,numbersSymbolsSizes[2],166)/2, LONumbersSymbols_VAO+2 + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(2850,386 +180*i,numbersSymbolsSizes[3],166)/2, LONumbersSymbols_VAO+3 + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(2950,386 +180*i,numbersSymbolsSizes[4],166)/2, LONumbersSymbols_VAO+4 + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(3080,386 +180*i,numbersSymbolsSizes[5],166)/2, LONumbersSymbols_VAO+5 + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(3134,386 +180*i,numbersSymbolsSizes[6],166)/2, LONumbersSymbols_VAO+6 + numbersSymbolsCount*i, points, false);
            addPointsToArray(SunnyVector4D(3182,386 +180*i,numbersSymbolsSizes[7],166)/2, LONumbersSymbols_VAO+7 + numbersSymbolsCount*i, points, false);
        }
    }
    
    //Overlayers 3
    {
        addPointsToArray(SunnyVector4D(1122,1272,126,124), LOOverlaySnowflake_VAO, points, false);
        addPointsToArray(SunnyVector4D(914,1188,208,208), LOOverlayFireball_VAO, points, false);
        addPointsToArray(SunnyVector4D(1250,1254,136,142), LOOverlayElectricity_VAO, points, false);
        addPointsToArray(SunnyVector4D(1388,1258,140,136), LOOverlaySpells_VAO, points, false);
        addPointsToArray(SunnyVector4D(426,1282,40,38), LOOverlayLifeSpell_VAO, points, false);
    }
    
    //GamePlay Buttons 1
    {
        addPointsToArray(SunnyVector4D(562,160,144,126), LOGameplayButtons_VAO, points, false);
//        addPointsToArray(SunnyVector4D(706,160,108,108), LOGameplayButtons_VAO+1, points, false);
//        addPointsToArray(SunnyVector4D(412,160,150,132), LOGameplayButtons_VAO+2, points, false);
    }
    
    //Spells 8
    {
        //Shield
        addPointsToArray(SunnyVector4D(1480,198+152*0,168,152), LOSpellButtons_VAO+0, points, false);
        addPointsToArray(SunnyVector4D(1650,198+92*0,92,92), LOSpellButtons_VAO+0+4, points, false);
        //Speed
        addPointsToArray(SunnyVector4D(1480,198+152*1,168,152), LOSpellButtons_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(1650,480,72,48), LOSpellButtons_VAO+1+4, points, false);
        //Portal
        addPointsToArray(SunnyVector4D(1480,198+152*2,168,152), LOSpellButtons_VAO+2, points, false);
        addPointsToArray(SunnyVector4D(1650,198+92*1,92,92), LOSpellButtons_VAO+2+4, points, false);
        //Life
        addPointsToArray(SunnyVector4D(1480,198+152*3,168-2,152), LOSpellButtons_VAO+3, points, false);
        addPointsToArray(SunnyVector4D(1650,198+92*2,92,92), LOSpellButtons_VAO+3+4, points, false);
        //magnet
        addPointsToArray(SunnyVector4D(2548,406,322,324)/2, LOSpellButtons_VAO+8, points, false);
        addPointsToArray(SunnyVector4D(2282,492,140,142)/2, LOSpellButtons_VAO+9, points, false);
        //time
        addPointsToArray(SunnyVector4D(2556,780,322,324)/2, LOSpellButtons_VAO+10, points, false);
        addPointsToArray(SunnyVector4D(2290,700,110,102)/2, LOSpellButtons_VAO+11, points, false);
        //snowflake
        addPointsToArray(SunnyVector4D(2556,1142,322,324)/2, LOSpellButtons_VAO+12, points, false);
        addPointsToArray(SunnyVector4D(2236,882,216,218)/2, LOSpellButtons_VAO+13, points, false);
    }
    
    //InGameSpells
    {
        //Teleport
        addPointsToArray(SunnyVector4D(122,1080,190,188), LOSpellTeleport_VAO, points, false);
        //Shield
        const float sScale = 0.75;
        addPointsToArray(SunnyVector4D(1754,406,48,70), LOSpellShield_VAO, points, false);
        scalePointsSize(SunnyVector2D(sScale,sScale), LOSpellShield_VAO, points);
        //Life
        addPointsToArray(SunnyVector4D(1648,628,76,72), LOSpellLife_VAO, points, false);
        scalePointsSize(SunnyVector2D(sScale,sScale), LOSpellLife_VAO, points);
        addPointsToArray(SunnyVector4D(1120,1130,134,122), LOSpellLife_VAO+1, points, false);
        scalePointsSize(SunnyVector2D(sScale,sScale), LOSpellLife_VAO+1, points);
        //Speed
        addPointsToArray(SunnyVector4D(1650,584,34,36), LOSpellSpeed_VAO, points, false);
        //Snow Explosion Shadow
        addPointsToArray(SunnyVector4D(1806,194,40,54), LOSnowExplosionShadow_VAO, points, false);
        addPointsToArray(SunnyVector4D(1827,328,82,42), LOSnowExplosionShadow_VAO+1, points, false);
    }
    
    addPointsToArray(SunnyVector4D(3040,3010,1056,820)/2, LOMapPreviewVAO, points, false);
    translatePointsInArray(SunnyVector2D(1056,-820)/2, LOMapPreviewVAO, points);
    
    addPointsToArray(SunnyVector4D(1697,349,118,168)/2, LOLSArrowVAO, points, false);
    addPointsToArray(SunnyVector4D(1651,375,46,42)/2, LOLSDotVAO, points, false);
    
    //Lines
    addPointsToArray(SunnyVector4D(2854,2066,32,1734)/2, LOLSTopBorderVAO, points, false);
    addPointsToArray(SunnyVector4D(2906,2066,32,1734)/2, LOLSTopBorderVAO+1, points, false);
    
    addPointsToArray(SunnyVector4D(426,18,386,418)/2, LOLSPlayButtonVAO, points, false);
    
    //Level Complete
    {
        addPointsToArray(SunnyVector4D(0,2052,1920,238)/2, LOLCBorderVAO, points, false);
        translatePointsInArray(SunnyVector2D(0,-230)/2, LOLCBorderVAO, points);
        addPointsToArray(SunnyVector4D(0,2466,1920,90)/2, LOLCBorderVAO+1, points, false);
        translatePointsInArray(SunnyVector2D(0,90)/2, LOLCBorderVAO+1, points);
        addPointsToArray(SunnyVector4D(952,2300,968,84)/2, LOLCBorderVAO+2, points, false);
        translatePointsInArray(SunnyVector2D(140,-84)/2, LOLCBorderVAO+2, points);
        addPointsToArray(SunnyVector4D(40,2410,968,48)/2, LOLCBorderVAO+3, points, false);
        translatePointsInArray(SunnyVector2D(-140,-48)/2, LOLCBorderVAO+3, points);
        
        addPointsToArray(SunnyVector4D(6,436,346,346)/2, LOLCButtonsVAO, points, false);
        addPointsToArray(SunnyVector4D(356,550,242,230)/2, LOLCButtonsVAO+1, points, false);
        addPointsToArray(SunnyVector4D(870,592,252,194)/2, LOLCButtonsVAO+2, points, false);
        addPointsToArray(SunnyVector4D(612,556,240,230)/2, LOLCButtonsVAO+3, points, false);
        addPointsToArray(SunnyVector4D(1158,556,214,228)/2, LOLCButtonsVAO+4, points, false);
        addPointsToArray(SunnyVector4D(1416,578,250,212)/2, LOLCButtonsVAO+5, points, false);//Control
        addPointsToArray(SunnyVector4D(2138,558,228,230)/2, LOLCButtonsVAO+6, points, false);//Back Btn
        
        addPointsToArray(SunnyVector4D(2068,3828,1062,266)/2, LOLCSnowTable_VAO, points, false);
        LOLevelComplete::setTableWidth(points[LOLCSnowTable_VAO*4+3].x - points[LOLCSnowTable_VAO*4].x);
        addPointsToArray(SunnyVector4D(958,352,162,160)/2, LOLCClock_VAO, points, false);
        addPointsToArray(SunnyVector4D(0,1626,1920,422)/2, LOLCForest_VAO, points, false);
        translatePointsInArray(SunnyVector2D(0,422)/2, LOLCForest_VAO, points);
        
        addPointsToArray(SunnyVector4D(1860,364,110,132)/2, LOGoldCoin_VAO, points, false);
        goldCoinImageWidth = fabsf(points[LOGoldCoin_VAO*4+3].x - points[LOGoldCoin_VAO*4].x)*1.3;
        addPointsToArray(SunnyVector4D(3936,3930,140,134)/2, LOVioletSnowflake_VAO, points, false);
        snowflakeImageWidth = fabsf(points[LOVioletSnowflake_VAO*4+3].x - points[LOVioletSnowflake_VAO*4].x)*1.3;
        addPointsToArray(SunnyVector4D(836,436,68,70)/2, LOPauseCross_VAO, points, false);
        
        addPointsToArray(SunnyVector4D(1712,588,172,192)/2, LOMusicButtonOn_VAO, points, false);
        addPointsToArray(SunnyVector4D(1906,588,188,192)/2, LOMusicButtonOff_VAO, points, false);
        addPointsToArray(SunnyVector4D(2060,3460,778,338)/2, LOBigButton_VAO, points, false);
        
        addPointsToArray(SunnyVector4D(3062,2108,1032,896)/2, LOTaskView_VAO, points, false);
        addPointsToArray(SunnyVector4D(1938,2452,108,104)/2, LOYellowStar_VAO, points, false);
        yellowStarImageWidth = fabsf(points[LOYellowStar_VAO*4+3].x - points[LOYellowStar_VAO*4].x)*1.3;
        addPointsToArray(SunnyVector4D(3252,1590,840,142)/2, LOTaskBorderTop_VAO, points, false);
        addPointsToArray(SunnyVector4D(3248,1748,844,154)/2, LOTaskBorderMiddle_VAO, points, false);
        addPointsToArray(SunnyVector4D(3258,1910,836,162)/2, LOTaskBorderBottom_VAO, points, false);
    }
    //Level Complete...
    
    //Social
    {
        addPointsToArray(SunnyVector4D(12,790,292,298)/2, LOSocialButtonBack_VAO, points, false);
        addPointsToArray(SunnyVector4D(320,812,86,174)/2, LOSocialButtonFB_VAO, points, false);
        addPointsToArray(SunnyVector4D(434,812,172,122)/2, LOSocialButtonTW_VAO, points, false);
        addPointsToArray(SunnyVector4D(640,812,96,120)/2, LOSocialButtonVK_VAO, points, false);
        addPointsToArray(SunnyVector4D(2058,2483,532,160)/2, LOShareFB_VAO, points, false);
        
        addPointsToArray(SunnyVector4D(2428,62,274,226)/2, LOMagicBoxMagic_VAO, points, false);
        addPointsToArray(SunnyVector4D(2822,30,236,292)/2, LOMagicBoxMagic_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(3164,8,152,342)/2, LOMagicBoxMagic_VAO+2, points, false);
    }
    
    //Store 6
    {
        addPointsToArray(SunnyVector4D(2056,2948,442,498)/2, LOStoreButton_VAO, points, false);
        addPointsToArray(SunnyVector4D(838,820,238,174)/2, LOStoreButton_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(1120,836,348,158)/2, LOStoreButton_VAO+2, points, false);
        addPointsToArray(SunnyVector4D(1526,820,322,300)/2, LOStoreButton_VAO+3, points, false);
        addPointsToArray(SunnyVector4D(1910,830,368,280)/2, LOStoreButton_VAO+4, points, false);
        addPointsToArray(SunnyVector4D(2342,808,386,332)/2, LOStoreButton_VAO+5, points, false);
    }
    
    //Free stuff
    {
        addPointsToArray(SunnyVector4D(3172,3880,746,214)/2, LOFreeStuff_VAO, points, false);
        addPointsToArray(SunnyVector4D(1926,2318,90,108)/2, LOFreeStuff_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(1926,2186,128,100)/2, LOFreeStuff_VAO+2, points, false);
        addPointsToArray(SunnyVector4D(1924,2080,148,98)/2, LOFreeStuff_VAO+3, points, false);
        addPointsToArray(SunnyVector4D(1928,1938,122,122)/2, LOFreeStuff_VAO+4, points, false);
        addPointsToArray(SunnyVector4D(1930,1796,118,136)/2, LOFreeStuff_VAO+5, points, false);
        addPointsToArray(SunnyVector4D(846,356,58,58)/2, LOFreeStuff_VAO+6, points, false);
        addPointsToArray(SunnyVector4D(842,270,70,76)/2, LOFreeStuff_VAO+7, points, false);
    }
    
    //Main Menu
    {
        addPointsToArray(SunnyVector4D(2618,0,1082,2048)/2, LOMainMenu_VAO, points, false, SunnyVector2D(2048,1024));
        addPointsToArray(SunnyVector4D(18,1490,1920,532)/2, LOMainMenu_VAO+1, points, false, SunnyVector2D(2048,1024));
    }
    
    //Settings
    {
        addPointsToArray(SunnyVector4D(2062,2652,606,288)/2, LOOkButtonBack_VAO, points, false);
        addPointsToArray(SunnyVector4D(2036,354,186,174)/2, LOPuzzleButton_VAO, points, false);
        addPointsToArray(SunnyVector4D(1124,318,276,228)/2, LOSurvival_VAO, points, false);
        
        addPointsToArray(SunnyVector4D(2386,534,256,250)/2, LOSettingsButton_VAO, points, false);
        addPointsToArray(SunnyVector4D(320,1020,188,166)/2, LOSounds_VAO, points, false);
        addPointsToArray(SunnyVector4D(324,1206,180,140)/2, LOSounds_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(546,1020,456,134)/2, LOLeftRightHand_VAO, points, false);
        addPointsToArray(SunnyVector4D(546,1216,456,134)/2, LOLeftRightHand_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(1028,1020,284,252)/2, LOVibration_VAO, points, false);
        addPointsToArray(SunnyVector4D(1068,1300,174,152)/2, LOVibration_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(1350,1152,522,204)/2, LOJoysticAcc_VAO, points, false);
        addPointsToArray(SunnyVector4D(1352,1358,520,204)/2, LOJoysticAcc_VAO+1, points, false);
        //ach
        addPointsToArray(SunnyVector4D(8,1108,188,168)/2, LOGameCenter_VAO, points, false);
        //lead
        addPointsToArray(SunnyVector4D(524,1370,244,178)/2, LOGameCenter_VAO+1, points, false);
        
        addPointsToArray(SunnyVector4D(2570,3182,222-4,222-4)/2, LOMessageBorders_VAO, points, false);
        addPointsToArray(SunnyVector4D(2570,3380+2,174,6)/2, LOMessageBorders_VAO+1, points, false);
        addPointsToArray(SunnyVector4D(2770,3182,6,174)/2, LOMessageBorders_VAO+2, points, false);
        translatePointsInArray(SunnyVector2D(222-4,-(222-4))/2, LOMessageBorders_VAO, points);
        translatePointsInArray(SunnyVector2D(174,0)/2, LOMessageBorders_VAO+1, points);
        translatePointsInArray(SunnyVector2D(0,-174)/2, LOMessageBorders_VAO+2, points);
        backCornerSize = fabsf(points[LOMessageBorders_VAO*4+3].x - points[LOMessageBorders_VAO*4].x);
        backSideSize = fabsf(points[(LOMessageBorders_VAO+1)*4+3].y - points[(LOMessageBorders_VAO+1)*4].y);
        backBorderSize = fabsf(points[(LOMessageBorders_VAO+1)*4+3].x - points[(LOMessageBorders_VAO+1)*4].x);
        //LOMusicButtonOn_VAO
        //LOMusicButtonOff_VAO
    }
    //Violet Snowflake
    {
        addPointsToArray(SunnyVector4D(1866,2096,294,274)/2, LOVioletSnowflakeInGame_VAO, points, false);
        addPointsToArray(SunnyVector4D(1892,1810,240,236)/2, LOVioletSnowflakeInGame_VAO+1, points, false);
        
        addPointsToArray(SunnyVector4D(2676,2640,164,382)/2, LOIcePriceButton_VAO, points, false);
        addPointsToArray(SunnyVector4D(310,1356,184,160)/2, LONewRecordImage_VAO, points, false);
    }
    
    if (isVAOSupported)
    {
        glGenVertexArrays(1, &gameGlobalVAO);
        glBindVertexArray(gameGlobalVAO);
    }
	glGenBuffers(1, &gameGlobalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gameGlobalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SunnyVector4D)*objectsCount*4, points, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] points;
}

void loLoadAllObjects()
{
    electricField = new SunnyModelObj;
    electricField->loadFromSobjFile(getPath("Base/Objects/Electricity","sh3do"));
	electricField->makeVBO(true,isVAOSupported,false);
    
    snowWallObj = new LOCrashModel;
    if (isVerySlowDevice)
        snowWallObj->loadFromSobjFile(getPath("Base/Objects/snowCrash_02_Low","sh3do"));
    else
        snowWallObj->loadFromSobjFile(getPath("Base/Objects/snowCrash_02","sh3do"));
    snowWallObj->prepareModels();
	snowWallObj->makeVBO(true,isVAOSupported,false);
    
    iceBlockModel = new LOIceBlockModel;
    if (isVerySlowDevice)
        iceBlockModel->loadFromSobjFile(getPath("Base/Objects/IceCube_Low","sh3do"));
    else
        iceBlockModel->loadFromSobjFile(getPath("Base/Objects/IceCube","sh3do"));
    iceBlockModel->prepareForIceBlock();
    iceBlockModel->makeVBO(true,isVAOSupported,false);
}

void loClearAllObjects()
{
    delete iceBlockModel;
    delete snowWallObj;
    delete electricField;
}

void loMakeShaders()
{
    globalShaders[LOS_FontRegular] = sunnyLoadShader("Base/Shaders/FontRegular", "Base/Shaders/FontRegular", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_FontRegular]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_FontRegular], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_FontRegular] = glGetUniformLocation(globalShaders[LOS_FontRegular], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_FontRegular], "textureSampler"),0);
    uniform3D_FR_Color = glGetUniformLocation(globalShaders[LOS_FontRegular], "color");
    uniform3D_FR_TS = glGetUniformLocation(globalShaders[LOS_FontRegular], "translateScale");
    
    globalShaders[LOS_FontBold] = sunnyLoadShader("Base/Shaders/FontBold", "Base/Shaders/FontBold", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_FontBold]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_FontBold], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_FontBold] = glGetUniformLocation(globalShaders[LOS_FontBold], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_FontBold], "textureSampler"),0);
    uniform3D_FB_Color = glGetUniformLocation(globalShaders[LOS_FontBold], "color");
    uniform3D_FB_BackColor = glGetUniformLocation(globalShaders[LOS_FontBold], "backColor");
    uniform3D_FB_TS = glGetUniformLocation(globalShaders[LOS_FontBold], "translateScale");
    
    globalShaders[LOS_Textured] = sunnyLoadShader("Base/Shaders/TexturedObject", "Base/Shaders/TexturedObject", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_Textured]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured] = glGetUniformLocation(globalShaders[LOS_Textured], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Textured], "textureSampler"),0);
    
    globalShaders[LOS_Textured3D] = sunnyLoadShader("Base/Shaders/TexturedObject3D", "Base/Shaders/TexturedObject3D", "vertex","mapCoord","normal", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3D]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3D], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3D] = glGetUniformLocation(globalShaders[LOS_Textured3D], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Textured3D], "textureSampler"),0);
    uniform3D_C = glGetUniformLocation(globalShaders[LOS_Textured3D], "color");
    uniform3D_LightPos = glGetUniformLocation(globalShaders[LOS_Textured3D], "lightPosition");
    glUniform3fv(uniform3D_LightPos, 1, customLightPositionGraphic);
    
    globalShaders[LOS_Textured3DNoLight] = sunnyLoadShader("Base/Shaders/TexturedObject3DNoLight", "Base/Shaders/TexturedObject3DNoLight", "vertex","mapCoord", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3DNoLight]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3DNoLight], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3DNoLight] = glGetUniformLocation(globalShaders[LOS_Textured3DNoLight], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Textured3DNoLight], "textureSampler"),0);
    uniform3DNL_C = glGetUniformLocation(globalShaders[LOS_Textured3DNoLight], "color");
    uniform3DNL_Tex = glGetUniformLocation(globalShaders[LOS_Textured3DNoLight], "translateOnTexture");
    glUniform2fv(uniform3DNL_Tex, 1, SunnyVector2D(0, 0));
    
    globalShaders[LOS_Textured3DSnowWalls] = sunnyLoadShader("Base/Shaders/TexturedObject3DSnowWalls", "Base/Shaders/TexturedObject3DSnowWalls", "vertex","normal", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3DSnowWalls]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnowWalls], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3DSnowWalls] = glGetUniformLocation(globalShaders[LOS_Textured3DSnowWalls], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Textured3DSnowWalls], "textureSampler"),0);
    uniform3DSW_C = glGetUniformLocation(globalShaders[LOS_Textured3DSnowWalls], "color");
    
    globalShaders[LOS_Textured3DSnow] = sunnyLoadShader("Base/Shaders/TexturedObject3DSnow", "Base/Shaders/TexturedObject3DSnow", "vertex","normal", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3DSnow]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3DSnow] = glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "modelviewMatrix");
    uniform3DS_C = glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "color");
    glUniform3fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "lightPosition"), 1, customLightPositionGraphic);
    
    globalShaders[LOS_Textured3DSnowShadow] = sunnyLoadShader("Base/Shaders/TexturedObject3DSnowShadow", "Base/Shaders/TexturedObject3DSnowShadow", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3DSnowShadow]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnowShadow], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3DSnowShadow] = glGetUniformLocation(globalShaders[LOS_Textured3DSnowShadow], "modelviewMatrix");
    uniform3DSS_C = glGetUniformLocation(globalShaders[LOS_Textured3DSnowShadow], "color");
    uniform3DSS_H = glGetUniformLocation(globalShaders[LOS_Textured3DSnowShadow], "heightScale");
    
    globalShaders[LOS_Textured3DC] = sunnyLoadShader("Base/Shaders/TexturedObject3DColor", "Base/Shaders/TexturedObject3DColor", "vertex","mapCoord", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3DC]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3DC], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3DC] = glGetUniformLocation(globalShaders[LOS_Textured3DC], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Textured3DC], "textureSampler"),0);
    uniform3D_TTR = glGetUniformLocation(globalShaders[LOS_Textured3DC], "texTranslate");
    
    globalShaders[LOS_Textured3DA] = sunnyLoadShader("Base/Shaders/TexturedObject3DAlpha", "Base/Shaders/TexturedObject3DAlpha", "vertex","mapCoord", NULL);
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Textured3DA], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_Textured3DA] = glGetUniformLocation(globalShaders[LOS_Textured3DA], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Textured3DA], "textureSampler"),0);
    uniform3D_A = glGetUniformLocation(globalShaders[LOS_Textured3DA], "color");
    
    globalShaders[LOS_TexturedC] = sunnyLoadShader("Base/Shaders/TexturedObject", "Base/Shaders/TexturedObjectTSC", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_TexturedC]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_TexturedC], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_TexturedC] = glGetUniformLocation(globalShaders[LOS_TexturedC], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_TexturedC], "textureSampler"),0);
    uniformA_A = glGetUniformLocation(globalShaders[LOS_TexturedC], "color");
    
    globalShaders[LOS_TexturedA_Tex] = sunnyLoadShader("Base/Shaders/TexturedObjectA_Tex", "Base/Shaders/TexturedObjectTSA", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_TexturedA_Tex]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_TexturedA_Tex], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_TexturedA_Tex] = glGetUniformLocation(globalShaders[LOS_TexturedA_Tex], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_TexturedA_Tex], "textureSampler"),0);
    uniformA_Tex_A = glGetUniformLocation(globalShaders[LOS_TexturedA_Tex], "alpha");
    uniformA_Tex_Tex = glGetUniformLocation(globalShaders[LOS_TexturedA_Tex], "translateOnTexture");
    
    globalShaders[LOS_TexturedTSA] = sunnyLoadShader("Base/Shaders/TexturedObjectTSA", "Base/Shaders/TexturedObjectTSA", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_TexturedTSA], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_TexturedTSA] = glGetUniformLocation(globalShaders[LOS_TexturedTSA], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_TexturedTSA], "textureSampler"),0);
    uniformTSA_TR = glGetUniformLocation(globalShaders[LOS_TexturedTSA], "translateScale");
    uniformTSA_A = glGetUniformLocation(globalShaders[LOS_TexturedTSA], "alpha");
    
    globalShaders[LOS_TexturedTS_Animation] = sunnyLoadShader("Base/Shaders/TexturedObjectTS_Animation", "Base/Shaders/TexturedObjectTS_Animation", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_TexturedTS_Animation]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_TexturedTS_Animation], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_TexturedTS_Animation] = glGetUniformLocation(globalShaders[LOS_TexturedTS_Animation], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_TexturedTS_Animation], "textureSampler"),0);
    uniformTS_TR = glGetUniformLocation(globalShaders[LOS_TexturedTS_Animation], "translateScale");
    uniformTS_Params = glGetUniformLocation(globalShaders[LOS_TexturedTS_Animation], "animationParams");
    
    globalShaders[LOS_TexturedTSA_Middle] = sunnyLoadShader("Base/Shaders/TexturedObjectTSA", "Base/Shaders/TexturedObjectTSA_Middle", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_TexturedTSA_Middle]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_TexturedTSA_Middle], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_TexturedTSA_Middle] = glGetUniformLocation(globalShaders[LOS_TexturedTSA_Middle], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_TexturedTSA_Middle], "textureSampler"),0);
    uniformTSAMiddle_TR = glGetUniformLocation(globalShaders[LOS_TexturedTSA_Middle], "translateScale");
    uniformTSAMiddle_A = glGetUniformLocation(globalShaders[LOS_TexturedTSA_Middle], "alpha");
    
    globalShaders[LOS_TexturedTSC] = sunnyLoadShader("Base/Shaders/TexturedObjectTSA", "Base/Shaders/TexturedObjectTSC", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_TexturedTSC], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_TexturedTSC] = glGetUniformLocation(globalShaders[LOS_TexturedTSC], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_TexturedTSC], "textureSampler"),0);
    uniformTSC_TR = glGetUniformLocation(globalShaders[LOS_TexturedTSC], "translateScale");
    uniformTSC_C = glGetUniformLocation(globalShaders[LOS_TexturedTSC], "color");
    
    globalShaders[LOS_SpecularPoints] = sunnyLoadShader("Base/Shaders/SpecularPoints", "Base/Shaders/SpecularPoints", "vertex", "normal", NULL);
    sunnyUseShader(globalShaders[LOS_SpecularPoints]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_SpecularPoints], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    glUniform1i(glGetUniformLocation(globalShaders[LOS_SpecularPoints], "textureSampler"),0);
    globalModelview[LOS_SpecularPoints] = glGetUniformLocation(globalShaders[LOS_SpecularPoints], "modelviewMatrix");
    uniformSP = glGetUniformLocation(globalShaders[LOS_SpecularPoints], "cameraVector");
    uniformSP_size = glGetUniformLocation(globalShaders[LOS_SpecularPoints], "pointSize");
    uniformSP_Global = glGetUniformLocation(globalShaders[LOS_SpecularPoints], "globalColor");
    
    globalShaders[LOS_FallingSnow] = sunnyLoadShader("Base/Shaders/FallingSnow", "Base/Shaders/FallingSnow", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_FallingSnow]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_FallingSnow], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    glUniform1i(glGetUniformLocation(globalShaders[LOS_FallingSnow], "textureSampler"),0);
    globalModelview[LOS_FallingSnow] = glGetUniformLocation(globalShaders[LOS_FallingSnow], "modelviewMatrix");
    uniformFS_size = glGetUniformLocation(globalShaders[LOS_FallingSnow], "pointSize");
    
    globalShaders[LOS_Sparks] = sunnyLoadShader("Base/Shaders/Sparks", "Base/Shaders/Sparks", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_Sparks]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_Sparks], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    glUniform1i(glGetUniformLocation(globalShaders[LOS_Sparks], "textureSampler"),0);
    globalModelview[LOS_Sparks] = glGetUniformLocation(globalShaders[LOS_Sparks], "modelviewMatrix");
    
    globalShaders[LOS_GroundParticles] = sunnyLoadShader("Base/Shaders/GroundParticles", "Base/Shaders/GroundParticles", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_GroundParticles]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_GroundParticles], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    glUniform1i(glGetUniformLocation(globalShaders[LOS_GroundParticles], "textureSampler"),0);
    globalModelview[LOS_GroundParticles] = glGetUniformLocation(globalShaders[LOS_GroundParticles], "modelviewMatrix");
    uniformGP_color = glGetUniformLocation(globalShaders[LOS_GroundParticles], "color");
    
    globalShaders[LOS_ColoredAlpha] = sunnyLoadShader("Base/Shaders/ColoredAlpha", "Base/Shaders/ColoredAlpha", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_ColoredAlpha]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_ColoredAlpha], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_ColoredAlpha] = glGetUniformLocation(globalShaders[LOS_ColoredAlpha], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_ColoredAlpha], "textureSampler"),0);
    uniformCA_color = glGetUniformLocation(globalShaders[LOS_ColoredAlpha], "color");
    
    globalShaders[LOS_SolidObject] = sunnyLoadShader("Base/Shaders/SolidObject", "Base/Shaders/SolidObject", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_SolidObject]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_SolidObject], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_SolidObject] = glGetUniformLocation(globalShaders[LOS_SolidObject], "modelviewMatrix");
    uniformSO_Color = glGetUniformLocation(globalShaders[LOS_SolidObject], "color");
    
    globalShaders[LOS_OverlayTSA] = sunnyLoadShader("Base/Shaders/OverlayTSA", "Base/Shaders/OverlayTSA", "vertex", NULL);
    sunnyUseShader(globalShaders[LOS_OverlayTSA]);
    glUniformMatrix4fv(glGetUniformLocation(globalShaders[LOS_OverlayTSA], "projMatrix"), 1, GL_FALSE, &(projectionMatrix.front.x));
    globalModelview[LOS_OverlayTSA] = glGetUniformLocation(globalShaders[LOS_OverlayTSA], "modelviewMatrix");
    glUniform1i(glGetUniformLocation(globalShaders[LOS_OverlayTSA], "dstSampler"),0);
    glUniform1i(glGetUniformLocation(globalShaders[LOS_OverlayTSA], "textureSampler"),1);
    uniformOverlayTSA_TR = glGetUniformLocation(globalShaders[LOS_OverlayTSA], "translateScale");
    uniformOverlayTSA_C = glGetUniformLocation(globalShaders[LOS_OverlayTSA], "color");
    
    SunnyVector2D k = SunnyVector2D(screenSize.x/texturebufferWidth,screenSize.y/texturebufferHeight);
    glUniform2fv(glGetUniformLocation(globalShaders[LOS_OverlayTSA], "textureKoeff"), 1, k);
}

void makeFrameBuffer()
{
    loMakeFrameBufferTexture(texturebufferWidth,texturebufferHeight);
    
    if (isFastDevice)
    {
        glGenFramebuffers(1, &textureFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, textureFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureForRender, 0);
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texturebufferWidth, texturebufferHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }
}

void loCancelAllNotifications()
{
    cancelAllLocalNotifications();
}

void loScheduleAllNotifications()
{
    const float oneDay = 60*60*24;
    
    scheduleLocalNotification(2*oneDay, "PUSH_2_DAYS", "PUSH_ACTION", "Base/Sounds/Fireball&FireballWDebrisV_4.mp3");
    scheduleLocalNotification(3*oneDay, "PUSH_3_DAYS", "PUSH_ACTION", "Base/Sounds/GameOverV_1.mp3");
    scheduleLocalNotification(4*oneDay, "PUSH_4_DAYS", "PUSH_ACTION", "Base/Sounds/GoldCoinsV_1.mp3");
    scheduleLocalNotification(5*oneDay, "PUSH_5_DAYS", "PUSH_ACTION", "Base/Sounds/LevelComplete.mp3");
    scheduleLocalNotification(6*oneDay, "PUSH_6_DAYS", "PUSH_ACTION", "Base/Sounds/MagicBox1V_1.mp3");
    scheduleLocalNotification(7*oneDay, "PUSH_1_WEEK", "PUSH_ACTION", "Base/Sounds/SnowflakePickUpV_3.mp3");
    scheduleLocalNotification(14*oneDay, "PUSH_2_WEEKS", "PUSH_ACTION", "Base/Sounds/IceDestroyedV_1.mp3");
    scheduleLocalNotification(30*oneDay, "PUSH_MONTH", "PUSH_ACTION", "Base/Sounds/FireBallLoopV_3.wav");
//    scheduleLocalNotification(10, "PUSH_2_DAYS", "PUSH_ACTION", "Base/Sounds/Fireball&FireballWDebrisV_4.mp3");
//    scheduleLocalNotification(20, "PUSH_3_DAYS", "PUSH_ACTION", "Base/Sounds/GameOverV_1.mp3");
//    scheduleLocalNotification(30, "PUSH_4_DAYS", "PUSH_ACTION", "Base/Sounds/GoldCoinsV_1.mp3");
//    scheduleLocalNotification(40, "PUSH_5_DAYS", "PUSH_ACTION", "Base/Sounds/LevelComplete.mp3");
//    scheduleLocalNotification(50, "PUSH_6_DAYS", "PUSH_ACTION", "Base/Sounds/MagicBox1V_1.mp3");
//    scheduleLocalNotification(60, "PUSH_1_WEEK", "PUSH_ACTION", "Base/Sounds/SnowflakePickUpV_3.mp3");
//    scheduleLocalNotification(70, "PUSH_2_WEEKS", "PUSH_ACTION", "Base/Sounds/IceDestroyedV_1.mp3");
//    scheduleLocalNotification(80, "PUSH_MONTH", "PUSH_ACTION", "Base/Sounds/FireBallLoopV_3.wav");
}

void loClearEngine()
{
    loScheduleAllNotifications();
    
    if (!menuMode && !player->crashed && isSurvival)
        activeMap->saveMap();
    
    loForcePause();
    LOScore::saveScores();
    
    //FrameBuffer
#ifndef __ANDROID__
    glDeleteFramebuffers(1, &textureFramebuffer);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    glDeleteTextures(1, &textureForRender);
#endif
    
    //Shaders
    sunnyDeletePrograms();
    
    //Textures
    SHTextureBase::deactivateTextures();
}

void loForcePause()
{
    if (!menuMode)
        LOMenu::getSharedMenu()->pauseTheGame();
}

void loRestoreEngine()
{
    loCancelAllNotifications();
    //FrameBuffer
    makeFrameBuffer();

    //Shaders
    loMakeShaders();

    //Textures
    SHTextureBase::activateTextures();
    
    activeMap->removeMap();
}

void loResize(SunnyVector2D screen)
{
    screenSize = screen;
    float menuTranslationX = (screenSize.x - 96*screenSize.y/64)/2;
    if (menuTranslationX<0) menuTranslationX = 0;
    
    verticalMapAdd = (screenSize.y - screenSize.x/1.5)/2*mapSize.y/(screenSize.x/1.5);
    if (verticalMapAdd<0) verticalMapAdd = 0;
    horizontalMapAdd = menuTranslationX/(screenSize.x-menuTranslationX*2)*mapSize.x;
    mapZoomBounds = SunnyVector4D(-horizontalMapAdd, mapSize.x+horizontalMapAdd, -verticalMapAdd, mapSize.y + verticalMapAdd);

    float distFromBorder = 0.1*(mapZoomBounds.w-mapZoomBounds.z);
    float d = (maxJoysticDistance)*(mapZoomBounds.w-mapZoomBounds.z) + distFromBorder;
    joysticCustomPosition[0] = SunnyVector2D(d - horizontalMapAdd, d - verticalMapAdd);
    joysticCustomPosition[1] = SunnyVector2D(mapSize.x - joysticCustomPosition[0].x, joysticCustomPosition[0].y);

    if (textureForRender)
    {
        glDeleteFramebuffers(1, &textureFramebuffer);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        glDeleteTextures(1, &textureForRender);

        makeFrameBuffer();
        pointsCoeff = screen.y/320;
        
        mat4f_LoadOrtho(mapZoomBounds.x, mapZoomBounds.y, mapZoomBounds.z, mapZoomBounds.w, 1, 500, &projectionMatrix[0][0]);
        pixelToScale = (mapZoomBounds.w - mapZoomBounds.z)/2/640;//screenSize.y;
        
        for (short i = 0; i<LOS_Count; i++) {
            sunnyUseShader(globalShaders[i]);
            int loc = glGetUniformLocation(globalShaders[i], "projMatrix");
            glUniformMatrix4fv(loc, 1, GL_FALSE, &(projectionMatrix.front.x));
        }
        
        //sunnySetProjectionMatrix(projectionMatrix, SunnyVector4D(-horizontalMapAdd, mapSize.x + horizontalMapAdd, -verticalMapAdd, mapSize.y + verticalMapAdd));
    }
}

void loInit(SunnyVector2D screen, bool isFast, bool isVAO)
{
    isPadDevice = (screen.y/screen.x>0.7);

    loCancelAllNotifications();
    loJoystic.touch = NULL;
    isFastDevice = isFast;
    screenSize = screen;
    isVAOSupported = isVAO;
    
    char * fontSize = getLocalizedLabel("GLOBAL_SCALE");
    globalFontScale = atof(fontSize);
    delete [] fontSize;
    
    glCullFace(GL_BACK);
    
    UnnyNet::init("com.unnyhog.dodge");
    
    LOScore::init();
    float menuTranslationX = (screenSize.x - 96*screenSize.y/64)/2;
    if (menuTranslationX<0) menuTranslationX = 0;
    
    verticalMapAdd = (screenSize.y - screenSize.x/1.5)/2*mapSize.y/(screenSize.x/1.5);
    if (verticalMapAdd<0) verticalMapAdd = 0;
    horizontalMapAdd = menuTranslationX/(screenSize.x-menuTranslationX*2)*mapSize.x;
    mapZoomBounds = SunnyVector4D(-horizontalMapAdd, mapSize.x+horizontalMapAdd, -verticalMapAdd, mapSize.y + verticalMapAdd);
    
    if (screenSize.y>=750)
        maxJoysticDistance *= 0.7;
    
    float distFromBorder = 0.1*(mapZoomBounds.w-mapZoomBounds.z);
    float d = (maxJoysticDistance)*(mapZoomBounds.w-mapZoomBounds.z) + distFromBorder;
    joysticCustomPosition[0] = SunnyVector2D(d - horizontalMapAdd, d - verticalMapAdd);
    joysticCustomPosition[1] = SunnyVector2D(mapSize.x - joysticCustomPosition[0].x, joysticCustomPosition[0].y);
    
    makeFrameBuffer();
    
    pointsCoeff = screen.y/320;
    
    mat4f_LoadOrtho(mapZoomBounds.x, mapZoomBounds.y, mapZoomBounds.z, mapZoomBounds.w, 1, 500, &projectionMatrix[0][0]);
    pixelToScale = (mapZoomBounds.w - mapZoomBounds.z)/2/640;//screenSize.y;
    
    loMakeShaders();
    loMakeGlobalVAO();
    SHTextureBase::setAlphaFix(true);
    objectsTexture = SHTextureBase::loadTexture("Base/GameTextures/ObjectsTexture.png");
    menuTexture = SHTextureBase::loadTexture("Base/GameTextures/MenuTexture.png");
    particleTextures[LOP_Paillette] = SHTextureBase::loadTexture("Base/GameTextures/paillette.png");
    particleTextures[LOP_Rock01] = SHTextureBase::loadTexture("Base/GameTextures/rock01.png");
    particleTextures[LOP_Rock02] = SHTextureBase::loadTexture("Base/GameTextures/paillette.png");
    particleTextures[LOP_Snowflake01] = SHTextureBase::loadTexture("Base/GameTextures/snowflake01.png");
    particleTextures[LOP_Snowflake02] = SHTextureBase::loadTexture("Base/GameTextures/snowflake02.png");
    particleTextures[LOP_Snowflake03] = SHTextureBase::loadTexture("Base/GameTextures/snowflake03.png");
    particleTextures[LOP_Spark] = SHTextureBase::loadTexture("Base/GameTextures/spark.png");
    LOPuzzle::loadTexture();
    
    //Kilo
    loLabelKilo = getLocalizedLabel("KILO");
    loLabelOk = getLocalizedLabel("Ok");
    loLabelLoading = getLocalizedLabel("Loading");
    loLabelLocked = getLocalizedLabel("LOCKED");
    loLabelShare = getLocalizedLabel("Share");
    loLabelNewRecord = getLocalizedLabel("New_Record");
    
    losLoadAllSounds();
    LOFont::prepareFont();
    
    sunnySetVAOSupported(isVAOSupported);
    sunnySetProjectionMatrix(projectionMatrix, SunnyVector4D(-horizontalMapAdd, mapSize.x + horizontalMapAdd, -verticalMapAdd, mapSize.y + verticalMapAdd));
    
    LOExplosion::prepareExplosions();
    player = new LOPlayer;
    
    groundModel = new LOGroundGenerator;
    
    if (!isVerySlowDevice)
        loLoadAllObjects();
    snowBorders = new SunnyModelObj;
    snowBorders->loadFromSobjFile(getPath("Base/Objects/Snow","sh3do"));// loadFromSobjFile(getPath("Base/Objects/Snow","sh3do"));
	snowBorders->makeVBO(true,isVAOSupported,true);
    
    snowBallObj = new SunnyModelObj;
    snowBallObj->loadFromSobjFile(getPath("Base/Objects/snowball","sh3do"));
	snowBallObj->makeVBO(true,isVAOSupported,true);
    
    snowBallCrashObj = new LOCrashModel;
    if (isVerySlowDevice)
        snowBallCrashObj->loadFromSobjFile(getPath("Base/Objects/snowball_crash_Low","sh3do"));
    else
        snowBallCrashObj->loadFromSobjFile(getPath("Base/Objects/snowball_crash","sh3do"));
    snowBallCrashObj->prepareModels();
	snowBallCrashObj->makeVBO(true,isVAOSupported,true);
    
    snowBallCrashObjForMenu = new LOCrashModel;
    if (isVerySlowDevice)
        snowBallCrashObjForMenu->loadFromSobjFile(getPath("Base/Objects/snowball_crash_Low","sh3do"));
    else
        snowBallCrashObjForMenu->loadFromSobjFile(getPath("Base/Objects/snowball_crash","sh3do"));
    snowBallCrashObjForMenu->prepareModels();
	snowBallCrashObjForMenu->makeVBO(true,isVAOSupported,true);
    
    angryBallObj = new SunnyModelObj;
    if (isVerySlowDevice)
        angryBallObj->loadFromSobjFile(getPath("Base/Objects/Angryball_Low","sh3do"));
    else
        angryBallObj->loadFromSobjFile(getPath("Base/Objects/Angryball","sh3do"));
	angryBallObj->makeVBO(true,isVAOSupported,false);
    LOGoldCoin::prepareModel();
    fireBallObj = new LOCrashModel;
    if (isVerySlowDevice)
        fireBallObj->loadFromSobjFile(getPath("Base/Objects/Fireball_Low","sh3do"));
    else
        fireBallObj->loadFromSobjFile(getPath("Base/Objects/Fireball","sh3do"));
    fireBallObj->prepareModels();
	fireBallObj->makeVBO(true,isVAOSupported,false);
    
    groundBallObj = new LOGroundAnimation;
    if (isVerySlowDevice)
        groundBallObj->loadFromSobjFile(getPath("Base/Objects/GroundBall_Low","sh3do"));
    else
        groundBallObj->loadFromSobjFile(getPath("Base/Objects/GroundBall","sh3do"));
    groundBallObj->prepareLOGroundAnimation();
	groundBallObj->makeVBO(true,isVAOSupported,false);
    LOTrees::prepareModel();
    
    LOMagicBox::prepareModels();

    lightPosition = customLightPositionGraphic;
    glEnable(GL_DEPTH_TEST);
    
    scInit(10,COLLISION_COUNT);
    player->attachPhysics();
    player->attachMoveBody();
    
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_PLAYER, collisionPlayerAndPlayerBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_FIREBALL, collisionPlayerAndFireBallBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_WATERBALL, collisionPlayerAndWaterBallBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_GROUNDBALL, collisionPlayerAndGroundBallBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_ANGRYBALL, collisionPlayerAndAngryBallBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_ANOMALYBALL, collisionPlayerAndAnomalyBallBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_BUTTON, collisionPlayerAndButtonBegan, collisionPlayerAndButtonEnded);
    
    scSetCollisionHandler(COLLISION_FIREBALL, COLLISION_FIREBALL, collisionFireBallAndFireBallBegan, 0);
    scSetCollisionHandler(COLLISION_FIREBALL, COLLISION_WATERBALL, collisionFireBallAndWaterBallBegan, 0);
    scSetCollisionHandler(COLLISION_FIREBALL, COLLISION_GROUNDBALL, collisionFireBallAndGroundBallBegan, 0);
    scSetCollisionHandler(COLLISION_WATERBALL, COLLISION_WATERBALL, collisionWaterBallAndWaterBallBegan, 0);
    scSetCollisionHandler(COLLISION_WATERBALL, COLLISION_GROUNDBALL, collisionWaterBallAndGroundBallBegan, 0);
    scSetCollisionHandler(COLLISION_GROUNDBALL, COLLISION_GROUNDBALL, collisionGroundBallAndGroundBallBegan, 0);
    
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_FIREBALL, collisionAngryBallAndFireBallBegan, 0);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_WATERBALL, collisionAngryBallAndWaterBallBegan, 0);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_GROUNDBALL, collisionAngryBallAndGroundBallBegan, 0);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_BONFIRE, collisionAngryBallAndBonfireBegan, 0);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_ELECTRICFIELD, collisionAngryBallAndElectricFieldBegan, collisionAngryBallAndElectricFieldEnded);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_BUTTON, collisionPlayerAndButtonBegan, collisionPlayerAndButtonEnded);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_ANOMALYBALL, collisionAngryBallAndAnomalyBallBegan, 0);
    
    scSetCollisionHandler(COLLISION_HIGH_SNOW, COLLISION_FIREBALL, collisionHighSnowAndFireBallBegan, 0);
    scSetCollisionHandler(COLLISION_HIGH_SNOW, COLLISION_WATERBALL, collisionHighSnowAndWaterBallBegan, 0);
    scSetCollisionHandler(COLLISION_HIGH_SNOW, COLLISION_GROUNDBALL, collisionHighSnowAndGroundBallBegan, 0);
    //scSetCollisionHandler(COLLISION_HIGH_SNOW, COLLISION_ELECTRICFIELD, collisionHighSnowAndElectricBallBegan, 0);
    scSetCollisionHandler(COLLISION_HIGH_SNOW, COLLISION_ANOMALYBALL, collisionHighSnowAndAnomalyBallBegan, 0);
    
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_SNOWFLAKE, collisionPlayerAndSnowflakeBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_VIOLET_SNOWFLAKE, collisionPlayerAndVioletSnowflakeBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_GOLD_COIN, collisionPlayerAndGoldCoinBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_BONFIRE, collisionPlayerAndBonfireBegan, 0);
    
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_ROLLING_BLADES, collisionPlayerAndRollingBladesBegan, 0);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_ICE_BLOCK, collisionPlayerAndIceBlockBegan, collisionPlayerAndIceBlockEnded);
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_ELECTRICFIELD, collisionPlayerAndElectricFieldBegan, 0);
    scSetCollisionHandler(COLLISION_FIREBALL, COLLISION_ELECTRICFIELD, collisionFireBallAndElectricFieldBegan, collisionFireBallAndElectricFieldEnded);
    scSetCollisionHandler(COLLISION_GROUNDBALL, COLLISION_ELECTRICFIELD, collisionGroundBallAndElectricFieldBegan, collisionGroundBallAndElectricFieldEnded);
    scSetCollisionHandler(COLLISION_WATERBALL, COLLISION_ELECTRICFIELD, collisionWaterBallAndElectricFieldBegan, collisionWaterBallAndElectricFieldEnded);
    scSetCollisionHandler(COLLISION_ANOMALYBALL, COLLISION_ELECTRICFIELD, collisionAnomalyBallAndElectricFieldBegan, collisionAnomalyBallAndElectricFieldEnded);
    
    //Electric Ball
    scSetCollisionHandler(COLLISION_PLAYER, COLLISION_ELECTRICBALL, collisionPlayerAndElectricBallBegan, collisionPlayerAndElectricBallEnded);
    scSetCollisionHandler(COLLISION_FIREBALL, COLLISION_ELECTRICBALL, collisionFireBallAndElectricBallBegan, collisionFireBallAndElectricBallEnded);
    scSetCollisionHandler(COLLISION_WATERBALL, COLLISION_ELECTRICBALL, collisionWaterBallAndElectricBallBegan, collisionWaterBallAndElectricBallEnded);
    scSetCollisionHandler(COLLISION_GROUNDBALL, COLLISION_ELECTRICBALL, collisionGroundBallAndElectricBallBegan, collisionGroundBallAndElectricBallEnded);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_ELECTRICBALL, collisionAngryBallAndElectricBallBegan, collisionAngryBallAndElectricBallEnded);
    scSetCollisionHandler(COLLISION_ELECTRICBALL, COLLISION_ELECTRICBALL, collisionElectricBallAndElectricBallBegan, collisionElectricBallAndElectricBallEnded);
    
    //Anomaly Ball
    scSetCollisionHandler(COLLISION_FIREBALL, COLLISION_ANOMALYBALL, collisionFireBallAndAnomalyBallBegan, 0);
    scSetCollisionHandler(COLLISION_WATERBALL, COLLISION_ANOMALYBALL, collisionWaterBallAndAnomalyBallBegan, 0);
    scSetCollisionHandler(COLLISION_GROUNDBALL, COLLISION_ANOMALYBALL, collisionGroundBallAndAnomalyBallBegan, 0);
    scSetCollisionHandler(COLLISION_ELECTRICBALL, COLLISION_ANOMALYBALL, collisionElectricBallAndAnomalyBallBegan, 0);
    scSetCollisionHandler(COLLISION_ANOMALYBALL, COLLISION_ANOMALYBALL, collisionAnomalyBallAndAnomalyBallBegan, 0);
    
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_ANGRYBALL, collisionAngryBallChargedBegan, 0, collisionAngryBallChargedBegan);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_HIGH_SNOW, collisionAngryBallChargedBegan, 0);
    scSetCollisionHandler(COLLISION_ANGRYBALL, COLLISION_LOW_SNOW, collisionAngryBallChargedBegan, 0);
    
    loLoadEmptyMap(true);
    activeMap->loadMap();
    activateAccelerometer();
    
    LOSettings::init();
    
//    loJoystic.touch = player;
//    loJoystic.direction = SunnyVector2D(0,0);
}

void loClear()
{
    LOMagicBox::clearModels();
    deactivateAccelerometer();
    if (loLabelKilo)
        delete []loLabelKilo;
    if (loLabelOk)
        delete [] loLabelOk;
    if (loLabelLoading)
        delete [] loLabelLoading;
    if (loLabelLocked)
        delete []loLabelLocked;
    if (loLabelShare)
        delete []loLabelShare;
    if (loLabelNewRecord)
        delete []loLabelNewRecord;
    
    if (isVAOSupported)
    {
        if (gameGlobalVAO) glDeleteVertexArrays(1, &gameGlobalVAO);
        gameGlobalVAO = 0;
    }
    if (gameGlobalVBO) glDeleteBuffers(1, &gameGlobalVBO);
    gameGlobalVBO = 0;
    
    losClearAllSounds();
    LOExplosion::clearExplosions();
    
    if (activeMap) delete activeMap;
    if (player) delete player;
    SHTextureBase::releaseLink(objectsTexture);
    SHTextureBase::releaseLink(menuTexture);
    SHTextureBase::releaseLink(fontTexture);
    for (short i = 0;i<LOP_Count;i++)
        SHTextureBase::releaseLink(particleTextures[i]);

    if (textureForRender) glDeleteTextures(1, &textureForRender);
    
    if (groundModel) delete groundModel;
    
    if (!isVerySlowDevice)
        loClearAllObjects();
    
    LOFont::clearFont();
    
    delete snowBorders;
    delete snowBallObj;
    delete snowBallCrashObj;
    delete fireBallObj;
    delete groundBallObj;
    delete angryBallObj;
    delete goldCoinObj;
        
    player = 0;
    activeMap = 0;
    groundModel = 0;
    angryBallObj = 0;
    goldCoinObj = 0;
    if (textureVertsVBO)
    {
        glDeleteBuffers(1, &textureVertsVBO);
        if (isVAOSupported)
            glDeleteVertexArrays(1, &textureVertsVAO);
    }
        
    if (textureFramebuffer) {
        glDeleteFramebuffers(1, &textureFramebuffer);
        textureFramebuffer = 0;
    }
    
    if (depthRenderbuffer) {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}

void loLoadMap(short num, bool instantly)
{
    if (!instantly)
    {
        lastAction = LOLA_LoadMap;
        actionParametr = num;
        return;
    }
    
    num++;
    char name[50];
    strcpy(name, "Base/Maps/map");
    short len = strlen(name);
    name[len] = '0' + (num/10);
    name[len+1] = '0' + (num%10);
    name[len+2] = '\0';
    loLoadMap(getPath(name, "lomap"));
}

void loLoadPreview(short num)
{
    num++;
    char name[50];
    strcpy(name, "Base/Maps/map");
    short len = strlen(name);
    name[len] = '0' + (num/10);
    name[len+1] = '0' + (num%10);
    name[len+2] = '\0';
    activeMap->loadPreview(getPath(name, "lomap"));
}

void loLoadSurvivalMap(short num, bool instantly, bool doubler, bool magnet, bool time)
{
    if (!instantly)
    {
        lastAction = LOLA_LoadSurvMap;
        actionParametr = num;
        actionAbilities[0] = doubler;
        actionAbilities[1] = magnet;
        actionAbilities[2] = time;
        return;
    }
    num++;
    char name[50];
    strcpy(name, "Base/Maps/SurvivalMap");
    short len = strlen(name);
    name[len] = '0' + (num/10);
    name[len+1] = '0' + (num%10);
    name[len+2] = '\0';
    loLoadMap(getPath(name, "lomap"));
    
    activeMap->prepareSurvivalMap();
    
    if (actionAbilities[0])
        player->activateAbility(LA_Doubler);
    if (actionAbilities[1])
        player->activateAbility(LA_Magnet);
    if (actionAbilities[2])
        player->activateAbility(LA_Time);
}

void loLoadEmptyMap(bool instantly)
{
    if (!instantly)
    {
        lastAction = LOLA_LoadEmptyMap;
        return;
    }

    loLoadMap(getPath("Base/Maps/EmptyMap", "lomap"),true);
    //loLoadMap(getPath("Base/Maps/SurvivalMap01", "lomap"),true);
    menuMode = true;
    gamePaused = true;
}

void loLoadMap(const char* fileName, bool menu)
{
    levelIsLoading = true;
    if (!activeMap) activeMap = new LOMap;
    menuMode = menu;
    activeMap->load(fileName);
    
    gamePaused = false;
    isMultiPlayer = false;
    menuMode = false;
    
    if (otherPlayersCount)
    {
        for (short i = 0;i<otherPlayersCount;i++)
        {
            delete otherPlayers[i].player;
            delete otherPlayers[i].snowBallCrashObj;
        }
        delete otherPlayers;
    }
    otherPlayersCount = 0;
    
    levelIsLoading = false;
}

SunnyVector2D screenToWorldCoords(SunnyVector2D scr)
{
    SunnyVector2D v;
    v.x = scr.x*(mapSize.x+horizontalMapAdd*2) - horizontalMapAdd;
    v.y = scr.y*(mapSize.y+verticalMapAdd*2) - verticalMapAdd;
    return v;
}

void loMoveByTouches()
{
    if (controlType == LOC_Touches)
    {
        SunnyVector2D acc = (loJoystic.location - player->getPosition());
        float d = acc.length();
        if (d>1)
            acc.normalize();
    
        const float maxValue = 0.3;
        float len = acc.length();
        if (len>maxValue)
            acc *= maxValue/len;
        player->setAngle(acc.x, acc.y);
        if (controlType==LOC_Touches)
            player->applyJoystic();
        
//        if (mapWind.z)
//            loJoystic.location += SunnyVector2D(mapWind.x,mapWind.y)*deltaTime;
    } else
    //if (loJoystic.touch)
    {
//        if (loJoystic.location.x>0.5)
//        {
//            const float minValue = 0.001;
//            
//            SunnyVector2D v= loJoystic.direction;
//            float len = v.length();
//            if (len<minValue)
//                v = SunnyVector2D(0,0);
//            else
//                v.normalize();
//            v*=0.3;
//            player->setAngle(v.x, v.y);
//        } else
        {
            const float maxValue = 0.3;
            const float minValue = 0.01;
            
            SunnyVector2D v = loJoystic.direction*0.6;
            float len = v.length();
            if (len<minValue)
                v = SunnyVector2D(0,0);
            else
            {
                v *= (len-minValue)/len;
                len = v.length();
                if (len>maxValue)
                    v *= maxValue/len;
            }
            player->setAngle(v.x, v.y);
        }
    }
}

void checkButtonsSounds()
{
    short buttonSound = sunnyButtonsGetSoundTag();
    if (buttonSound)
        switch (buttonSound) {
            case LOBT_Custom:
                losPlayButtonClick();
                break;
            case LOBT_Ressurection:
                losPlayRessurection();
                break;
            case LOBT_SpellShield:
                losPlaySpellSound(0);
                break;
            case LOBT_SpellLife:
                losPlaySpellSound(2);
                break;
            case LOBT_SpellSpeed:
                losPlaySpellSound(4);
                break;
            case LOBT_SpellTeleport:
                losPlaySpellSound(6);
                break;
            case LOBT_DropSound:
                losPlayDropSound();
                break;
            default:
                break;
        }
}

void loUpdate(double delta)
{
    if (lastAction!=LOLA_None)
    {
        if (lastAction==LOLA_LoadMap)
        {
            loLoadMap(actionParametr,true);
        } else
            if (lastAction == LOLA_LoadSurvMap)
            {
                loLoadSurvivalMap(actionParametr,true);
            }else
                if (lastAction == LOLA_LoadEmptyMap)
                {
                    loLoadEmptyMap(true);
                }
        lastAction = LOLA_None;
    }
    
    checkButtonsSounds();
    
    if (levelIsLoading) return;
    if (blurPower>0)
        blurPower -= delta*3;
    
//    if (gamePaused) delta = 0;
    deltaTime = delta;
    if (!gamePaused && !player->crashed)
    {
        playingTime+=deltaTime;
        realPlayingTime+=deltaTime;
        if (player->isAbilityActive(LA_Time))
            playingTime += deltaTime;
    }
    
    if (physicsPaused!=gamePaused && gamePaused)
    {
        losSetRollingSnowballSpeed(0);
    }
    physicsPaused = gamePaused;
    
    if (controlType!=LOC_Accelerometer)
        loMoveByTouches();
    activeMap->update();
    if (isFastDevice)
        loRenderToFrameBuffer();
    //if (physicsPaused) return;
    losPreUpdateSounds();
    activeMap->updateInfoForSounds();
    losUpdateSounds();
}

void loRenderJoystic(void * touch, SunnyVector2D position, SunnyVector2D direction)
{
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_ColoredAlpha]);
    glDisable(GL_DEPTH_TEST);
    SHTextureBase::bindTexture(objectsTexture);
    
    //glUniform4fv(uniformA_A, 1, SunnyVector4D(1,1,1,1));
    if (touch!=NULL)
    {
        glUniform4fv(uniformCA_color, 1, SunnyVector4D(0, 0, 0, 0.3));
        SunnyMatrix m = sunnyIdentityMatrix;
        m.pos = SunnyVector4D(position.x,position.y,-2,1);
        
        SunnyMatrix m1 = getScaleMatrix(SunnyVector3D(2,2,1)) * m;
        sunnyUseShader(globalShaders[LOS_ColoredAlpha]);
        glUniformMatrix4fv(globalModelview[LOS_ColoredAlpha], 1, GL_FALSE, &(m1 .front.x));
        SunnyDrawArrays(LOJoystic_VAO);
        
        m.pos.x += direction.x;
        m.pos.y += direction.y;
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        glUniform1f(uniformTSA_A, 0.5);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOJoystic_VAO);
    } else
    {
        glUniform4fv(uniformCA_color, 1, SunnyVector4D(0, 0, 0, 0.1));
        //for (short i = 0;i<1;i++)
        short i = 0;
        if (LOSettings::isLeftHanded()) i = 1;
        
        {
            SunnyMatrix m = sunnyIdentityMatrix;
            m.pos = SunnyVector4D(joysticCustomPosition[i].x,joysticCustomPosition[i].y,-2,1);
            SunnyMatrix m1 = getScaleMatrix(SunnyVector3D(2,2,1)) * m;
            sunnyUseShader(globalShaders[LOS_ColoredAlpha]);
            glUniformMatrix4fv(globalModelview[LOS_ColoredAlpha], 1, GL_FALSE, &(m1 .front.x));
            SunnyDrawArrays(LOJoystic_VAO);
            
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
            glUniform1f(uniformTSA_A, 0.4);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOJoystic_VAO);
        }
    }
}

void renderHUD()
{
    glEnable(GL_BLEND);
    if (controlType==LOC_Joystic && !menuMode && !gamePaused && !player->crashed)
    {
#ifndef KEYBOARD_CONTROL
#ifndef DO_NOT_RENDER_HUD
        loRenderJoystic(loJoystic.touch,SunnyVector2D(loJoystic.location.x*(mapSize.x+2*horizontalMapAdd) - horizontalMapAdd,loJoystic.location.y*(mapSize.y+2*verticalMapAdd) - verticalMapAdd),loJoystic.direction);
#endif
#endif
    }

    sunnyRenderButtons(false);
    
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, 1);
    glDisable(GL_DEPTH_TEST);
    activeMap->mainMenu->renderLevelButtonBack();
    sunnyRenderCallbackButtons();
    activeMap->mainMenu->render();
}

void renderAll()
{
    activeMap->renderES2();
//    renderHUD();
}

void loRenderSlowDevice()
{
    glViewport(0, 0, screenSize.x, screenSize.y);
    glClearColor(globalColor.x, globalColor.y, globalColor.z, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderAll();
}

void loRenderFramebufferTexture()
{
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, screenSize.x, screenSize.y);
    glClearColor(globalColor.x, globalColor.y, globalColor.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
//    glEnable(GL_BLEND);
//    SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(15,17,-2));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Dodge the threat", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    m1 = getTranslateMatrix(SunnyVector3D(15,12,-2));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Roll through crazy maps", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    m1 = getTranslateMatrix(SunnyVector3D(15,7,-2));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Learn magical spells", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    m1 = getTranslateMatrix(SunnyVector3D(15,3,-2));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Don't be caught", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
    
    //LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Получай магические подарки", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Останься в живых", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    m1 = getTranslateMatrix(SunnyVector3D(15,16,-2));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Изучай заклинания, чтобы выжить", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    m1 = getTranslateMatrix(SunnyVector3D(15,4,-2));
//    LOFont::writeTextInRect(&m1, SunnyVector2D(30,10), LOTA_Center, false, "Исследуй загадочный мир", SunnyVector4D(0,0,0,1),SunnyVector4D(0,0,0,1));
//    return;

    const float onePixel = mapSize.x/screenSize.x;
    
    sunnyUseShader(globalShaders[LOS_Textured]);
    SunnyMatrix m = sunnyIdentityMatrix;
    m.pos.z = -1;
    
    glUniformMatrix4fv(globalModelview[LOS_Textured], 1, GL_FALSE, &(m .front.x));

    glDisable(GL_BLEND);
    SHTextureBase::resetActiveTexture();
    glBindTexture(GL_TEXTURE_2D, textureForRender);
    if (isVAOSupported)
        glBindVertexArray(textureVertsVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, textureVertsVBO);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureIndVBO);
    
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);

    if (blurPower>0 && isFastDevice)
    {
        sunnyUseShader(globalShaders[LOS_TexturedC]);
        glEnable(GL_BLEND);
        short count = 1;
        if (isFastDevice) count = 2;
        glUniform4fv(uniformA_A, 1, SunnyVector4D(1,1,1,1.0/(1.0+count)));
        float scale;
        SunnyMatrix m1;
        for (short i = 1;i<=count;i++)
        {
            scale = 1.0 + onePixel/count*i*blurPower;
            m1 = getTranslateMatrix(SunnyVector3D(blurPosition.x, blurPosition.y, 0)) * m;
            m1 = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m1;
            m1 = getTranslateMatrix(SunnyVector3D(-blurPosition.x, -blurPosition.y, 0)) * m1;
            glUniformMatrix4fv(globalModelview[LOS_TexturedC], 1, GL_FALSE, &(m1 .front.x));
            glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
        }
        glDisable(GL_BLEND);
    }
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (isFastDevice)
    {
        bindGameGlobal();
        glEnable(GL_BLEND);
        GLuint tex = SHTextureBase::getTextureId(objectsTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex);
        activeMap->renderOverlay();
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        
        LOMainWindow::getSharedWindow()->renderPlayButton();
    }
}

void loRenderToFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, textureFramebuffer);
    glViewport(0, 0, screenSize.x, screenSize.y);
    glClearColor(globalColor.x, globalColor.y, globalColor.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderAll();
}

void loRender()
{
    if (levelIsLoading) return;
    if (isFastDevice)
    {
        loRenderFramebufferTexture();
    } else
    {
        loRenderSlowDevice();
    }
    GLenum error = glGetError();
    if (error)
        printf("\nGL Error = %d",error);
}

void loBlurExplosion(SunnyVector2D pos, float power)
{
    if (blurPower < power)
    {
        blurPosition = pos;
        blurPower = power;
    }
}

void loCalibrate(bool accelerometer)
{
    float x = lastAcc.x;
    float y = lastAcc.y;
    float z = lastAcc.z;
    SunnyMatrix *mat;
    if (accelerometer) mat = &initialMatrix;
    else mat = &initialMatrixJoystic;
    
    mat->right = SunnyVector4D(-y,-x,-z,0);
    mat->pos.w = 1;
    if (mat->right.y>0.5)
    {
        if (z<0)
            mat->up = SunnyVector4D(0,0,-1,0);
        else
        {
            mat->up = SunnyVector4D(0,0,1,0);
            mat->pos.w = -1;
        }
    }
    else
        if (mat->right.y<-0.5)
        {
            if (z<0)
                mat->up = SunnyVector4D(0,0,1,0);
            else
            {
                mat->up = SunnyVector4D(0,0,-1,0);
                mat->pos.w = -1;
            }
        }
        else
            mat->up = SunnyVector4D(0,1,0,0);
    
    mat->front = mat->up ^ mat->right;
    mat->front.normalize();
    mat->up = mat->right ^ mat->front;
}

void loAccelerometer(float sx,float sy,float sz)
{
    lastAcc = SunnyVector3D(sx,sy,sz);
    
    SunnyMatrix *m;
    if (controlType==LOC_Accelerometer)
        m = &initialMatrix;
    else
        m = &initialMatrixJoystic;
    
    SunnyVector4D acc = *m*SunnyVector4D(-sy,-sx,-sz,0);
    if (m->right.z>=0)
        acc.y = -acc.y;
    
    float x = acc.x;
    float y = acc.y;
    
    activeMap->setRotationAngle(SunnyVector2D(x,y));
    
    if (controlType!=LOC_Accelerometer) return;
    
    const float maxValue = 0.3;
    const float minValue = 0.01;
    
    SunnyVector2D v= SunnyVector2D(x,y);
    float len = v.length();
    if (len<minValue)
        v = SunnyVector2D(0,0);
    else
    {
        v *= (len-minValue)/len;
        len = v.length();
        if (len>maxValue)
            v *= maxValue/len;
    }
    
   /* if (fabsf(x)<=minValue) x = 0;
    else
        if (x>0)
        {
            if (x>maxValue) x = maxValue;
            x -= minValue;
        } else
        {
            if (x<-maxValue) x = -maxValue;
            x += minValue;
        }
    
    if (fabsf(y)<=minValue) y = 0;
    else
        if (y>0)
        {
            if (y>maxValue) y = maxValue;
            y -= minValue;
        } else
        {
            if (y<-maxValue) y = -maxValue;
            y += minValue;
        }*/
    
    player->setAngle(v.x, v.y);
}

//void loMultiplayerSetPosition(SunnyVector2D pos, SunnyVector2D vel, float radius, bool crashed, float w, float s,SunnyVector2D alpha,const char* playerId)
void loMultiplayerSetPosition(MessageUpdatePosition * pos, const char* playerId)
{
    for (short i = 0;i<otherPlayersCount;i++)
        if (!strcmp(otherPlayers[i].playerId, playerId))
        {
            otherPlayers[i].multiPlayerPlayedTime = pos->multiPlayerPlayedTime;
            otherPlayers[i].player->crashed = pos->crashed;
            otherPlayers[i].player->checkMultiplayerPosition(pos->pos);
            otherPlayers[i].player->velocity = pos->vel;
            otherPlayers[i].player->updateRadius(pos->radius);
            otherPlayers[i].player->waterKoef = pos->waterKoef;
            otherPlayers[i].player->sandFriction = pos->sandKoef;
            otherPlayers[i].player->setAngle(pos->alpha.x, pos->alpha.y);
            break;
        }
}

void loMultiplayerSnowExploded(LOScriptEffectType type, short num, const char * playerId)
{
    LOOtherPlayer * pl = 0;
    for (short i = 0;i<otherPlayersCount;i++)
        if (!strcmp(otherPlayers[i].playerId, playerId))
        {
            pl = &otherPlayers[i];
            break;
        }
    
    if (!pl) return;
    
    switch (type) {
        case SET_None:
            activeMap->explodePlayer(pl->player, pl->snowBallCrashObj);
            break;
        case SET_FireBall:
            activeMap->explodeFireBall(num, pl->player, pl->snowBallCrashObj);
            break;
        case SET_WaterBall:
            activeMap->explodeWaterBall(num, pl->player, pl->snowBallCrashObj);
            break;
        case SET_GroundBall:
            activeMap->explodeGroundBall(num, pl->player, pl->snowBallCrashObj);
            break;    
        default:
            break;
    }
}

void loMultiplayerRespawn(LOScriptEffectType type,SunnyVector2D pos, SunnyVector2D vel)
{
    switch (type) {
        case SET_FireBall:
            activeMap->createFireBall(pos, vel);
            break;
        case SET_WaterBall:
            activeMap->createWaterBall(pos, vel);
            break;
        case SET_GroundBall:
            activeMap->createGroundBall(pos, vel);
            break;
        case SET_AngryBall:
            activeMap->createAngryBall(pos);
            break;
        case SET_ElectricBall:
            activeMap->createElectricBall(pos, vel);
            break;
        case SET_AnomalyBall:
            activeMap->createAnomalyBall(pos, vel);
            break;
        default:
            break;
    }
}

void showPauseMenu();
double tpTouchTime;
SunnyVector2D tpTouchLocation;

void loTouchBegan(void * touch, float x, float y)
{
#ifndef KEYBOARD_CONTROL
    if (controlType==LOC_Joystic && loJoystic.touch==NULL)
    {
        if (y<0.5)
        {
            if ((x<0.3 && !LOSettings::isLeftHanded()) || (x>0.7 && LOSettings::isLeftHanded()))
            {
                loJoystic.touch = touch;
                loJoystic.location = SunnyVector2D(x,y);
//                loJoystic.location = SunnyVector2D(joysticCustomPosition[0].x/mapSize.x,joysticCustomPosition[0].y/mapSize.y);
                loJoystic.direction = SunnyVector2D(0,0);
                loTouchMoved(touch, x, y);
            }
        }
    }
#endif
    
    activeMap->mainMenu->touchBegan(touch, x, y);
    
    if (sunnyButtonsTouchBeganEvent(touch,x,y))
        return;
    
    tpTouchLocation = SunnyVector2D(x,y);
    tpTouchTime = realPlayingTime;
 
    if (controlType==LOC_Touches)
    {
        loJoystic.touch = touch;
        loJoystic.location = screenToWorldCoords(SunnyVector2D(x,y));
    }
}

void loTouchMoved(void * touch, float x, float y)
{
    activeMap->mainMenu->touchMoved(touch, x, y);
    
    sunnyButtonsTouchMovedEvent(touch,x,y);
#ifndef KEYBOARD_CONTROL
    if (loJoystic.touch == touch)
    {
        if (controlType==LOC_Joystic)
        {
            SunnyVector2D acc = (SunnyVector2D(x,y)-loJoystic.location);
            float d = acc.length();
            if (d>maxJoysticDistance)
            {
                acc.normalize();
                loJoystic.location += acc*(d-maxJoysticDistance);
                if (loJoystic.location.x>0.3 && !LOSettings::isLeftHanded())
                    loJoystic.location.x = 0.3;
                
                if (loJoystic.location.x<1-0.3 && LOSettings::isLeftHanded())
                    loJoystic.location.x = 1-0.3;
                
                if (loJoystic.location.y>0.5)
                    loJoystic.location.y = 0.5;
            }
            else
                acc/=maxJoysticDistance;
            loJoystic.direction = acc;
        } else
            if (controlType==LOC_Touches)
            {
                loJoystic.location = screenToWorldCoords(SunnyVector2D(x,y));
            }
    }
#endif
}

void loTouchEnded(void * touch, float x, float y)
{
    activeMap->mainMenu->touchEnded(touch, x, y);
    
    sunnyButtonsTouchEndedEvent(touch,x,y);
    
#ifndef KEYBOARD_CONTROL
    if (loJoystic.touch == touch)
    {
        loJoystic.touch = NULL;
        loJoystic.direction = SunnyVector2D(0,0);
        loAccelerometer();
    }
#endif
    
    if (!menuMode && !gamePaused && !player->crashed)
    if (realPlayingTime-tpTouchTime < 0.3)
        if ((tpTouchLocation-SunnyVector2D(x,y)).length()<0.03)
        {
            player->teleportToLocation(screenToWorldCoords(tpTouchLocation));
        }
}

void loSetJoysticPosition(SunnyVector2D position)
{
    loJoystic.direction = position;
}

void loResetMap()
{
    blurPower = 0;
    activeMap->resetMap();
}