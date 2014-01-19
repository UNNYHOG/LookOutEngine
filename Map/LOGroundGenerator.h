//
//  LOGroundGenerator.h
//  LookOut
//
//  Created by Pavel Ignatov on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOGroundGenerator_h
#define LookOut_LOGroundGenerator_h
#include "LOMapInfo.h"
#include "SunnyModelObj.h"
//#include "LOGlobalVar.h"

struct TVBOPointNT;

const int partsCount = 4;

const short forceFieldSizeX = (mapSizeX*smallCount)*partsCount;
const short forceFieldSizeY = (mapSizeY*smallCount)*partsCount;


class LOGroundGenerator
{
    TVBOPointN *verticesNT;//[generatorVertices_qty];
    TVBOPointNTC *verticesNTC;//[generatorVertices_qty];
    SunnyVector3D **powerField;
    
    SunnyModelObj * groundBorders;

    int polygons_qty;
    int renderPolygons_qty;
	polygon_typeObj *polygons;
    unsigned short *snowFromPolygon;
#ifndef VAO_NOT_SUPPORTED
    GLuint verticesVAO;
#endif
	GLuint verticesVBO;
    GLuint facesVBO;
    
    GLuint borderVAO,borderVBO,borderFaces;
    
    bool updateBuffer;
    bool needToReset;
    
    void init();
    void clear();
    void makeNormalForPoly(int &polyNum);
    void makeNormalForPolyC(unsigned short &polyNum);
    void addStandartNormalForPoint(unsigned short num);
    void addStandartNormalForPointC(unsigned short num);
    
    void renderSnowShadows(SunnyMatrix *MV, float shadowScale);
public:
    LOGroundGenerator();
    ~LOGroundGenerator();
    
    static bool needShadowsOnSides();
    
    void render(SunnyMatrix *MV, float shadowScale);
    void reset();
    void createNewGroundModel(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount],unsigned char * heightMap);
    void createGroundModel(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount]);
    void createGroundModelC(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount]);
    void updatePointsAfterExplode();
    void explodePolygons(short x, short y);
    
    friend class LOMapInfo;
};

#endif
