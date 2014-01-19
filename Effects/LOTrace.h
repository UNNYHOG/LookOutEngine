//
//  LOTrace.h
//  LookOut
//
//  Created by Pavel Ignatov on 04.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOTrace_h
#define LookOut_LOTrace_h
#include "SunnyVector2D.h"
#include "SunnyVector3D.h"
#include "SunnyOpenGl.h"
#include "SunnyMatrix.h"
#include "LOGlobalVar.h"

const short maxTracesCount = 400;
const short maxPostsCount = 100;
const short maxPlayerTracesCount = 150*2;

struct LOOneTrace
{
    SunnyVector2D position;
    float alpha;
    short traceNum;
    float scale;
};

struct LOOnePostEffect
{
    SunnyMatrix position;
    float alpha;
    short num;
};

class LOTrace
{
//    GLuint verticesVAO;
//    GLuint verticesVBO;
    
    LOOneTrace traces[maxTracesCount];
    short tracesCount;
    
    LOOneTrace playerTraces[maxPlayerTracesCount];
    short playerTracesCount;
    
    LOOneTrace otherPlayersTraces[maxPlayerTracesCount*maxPlayersCount];
    short otherPlayersTracesCount;
    
    LOOnePostEffect posts[maxPostsCount];
    short postsCount;
    void init();
    void clear();
public:
    static LOTrace *sharedTrace();
    LOTrace();
    ~LOTrace();

    void renderPosts(SunnyMatrix *MV);
    void render(SunnyMatrix *MV);
    void update();
    void addIceTrace(SunnyVector2D pos, short direction);
    void addRandomTrace(SunnyVector2D pos, float scale=1, float alpha = 1);
    void addRandomPlayerTrace(SunnyVector2D pos, float scale=1);
    void addRandomOtherPlayerTrace(SunnyVector2D pos, float scale=1);
    void addRandomPartEffect(SunnyMatrix *pos);
    void addFireTrace(SunnyMatrix *pos);
    void addGroundTrace(SunnyMatrix *pos);
    void addGroundWaterTrace(SunnyMatrix *pos);
    void addElectricTrace(SunnyMatrix *pos);
    void addGroundPartTrace(SunnyVector2D pos);
    void addWaterTrace(SunnyMatrix *pos);
    void addAngryBallRespTrace(SunnyMatrix *pos);
    void addAngryBallTrace(SunnyMatrix *pos);
    void addSnowWallPostEffect(SunnyMatrix *pos);
    void addTeleportationPostEffect(SunnyMatrix *pos);
    void reset();
};

#endif