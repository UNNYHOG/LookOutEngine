//
//  LOGroundGenerator.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOGroundGenerator.h"
#include "LOGlobalVar.h"
#include "SunnyModel3dsFunctions.h"
#include "iPhone_Strings.h"
#include "SunnyShaders.h"

//float borderAddX = 0.5;
//float borderAddY = 0.5;
const short borderFacesCount = 8;
short borderAddXparts = 1;//borderAddX*partsCount;
short borderAddYparts = 1;//borderAddY*partsCount;
short generatorSizeX = (mapSizeX*smallCount)*partsCount + 2*borderAddXparts;
short generatorSizeY = (mapSizeY*smallCount)*partsCount + 2*borderAddYparts;
int generatorVertices_qty = (generatorSizeX+1)*(generatorSizeY+1);

SunnyVector3D **sharedPowerField = NULL;

LOGroundGenerator::LOGroundGenerator()
{
    powerField = new SunnyVector3D*[forceFieldSizeX+1];
    for (int i = 0;i<=forceFieldSizeX;i++) powerField[i] = new SunnyVector3D[forceFieldSizeY+1];
    sharedPowerField = powerField;
    
    TVBOPointN * borderVertices;
    init();
    
    
    const float step = mapSize.x/(mapSizeX*smallCount*partsCount)*borderAddXparts;
    float addX = (1-mapScale)*mapSize.x/2 + horizontalMapAdd;
    float addY = (1-mapScale)*mapSize.y/2 + verticalMapAdd;
    addX*=2;
    addY*=2;
    const float tempScale = 0.992;
    SunnyVector2D verticesB[] = {   SunnyVector2D(-addX,mapSize.y+addY),SunnyVector2D(-addX,-addY),
                                    SunnyVector2D(-step,mapSize.y+step),SunnyVector2D(-step,-step),
                                    SunnyVector2D(mapSize.x + step*tempScale,mapSize.y+step),SunnyVector2D(mapSize.x + step*tempScale,-step),
                                    SunnyVector2D(mapSize.x+addX,mapSize.y+addY),SunnyVector2D(mapSize.x+ addX,-addY)};
    
    int verticesCount = 8;
    borderVertices = new TVBOPointN[verticesCount];
    for (int i = 0; i<verticesCount; i++)
    {
        borderVertices[i].vertice.x = verticesB[i].x;
        borderVertices[i].vertice.y = verticesB[i].y;
        borderVertices[i].vertice.z = 200./255;
        borderVertices[i].verticeNormal = SunnyVector3D(0,0,1);
    }
    
    //normals - > color
    SunnyVector3D lightPosition = SunnyVector3D(1,1,1.5).normalize();
    for (int i = 0;i<verticesCount;i++)
    {
        borderVertices[i].verticeNormal.z = 0.3 + 0.7*fmaxf((lightPosition & borderVertices[i].verticeNormal.normalize()),0.0);
        if (borderVertices[i].vertice.x<=0)
            borderVertices[i].verticeNormal.x = (5. - 960*(borderVertices[i].vertice.x/mapSize.x))/2048;
        else
            if (borderVertices[i].vertice.x>=mapSize.x)
                borderVertices[i].verticeNormal.x = (965. + 960*(1 - borderVertices[i].vertice.x/mapSize.x))/2048;
            else
                borderVertices[i].verticeNormal.x = (5. + 960*(borderVertices[i].vertice.x/mapSize.x))/2048;
        
        if (borderVertices[i].vertice.y<=0)
            borderVertices[i].verticeNormal.y = (2048.-645. + 640*(1.0 + borderVertices[i].vertice.y/mapSize.y))/2048.;
        else
            if (borderVertices[i].vertice.y>=mapSize.y)
                borderVertices[i].verticeNormal.y = (2048.-645. + 640*(-1.0 + borderVertices[i].vertice.y/mapSize.y))/2048.;
            else
                borderVertices[i].verticeNormal.y = (2048.-645. + 640*(1.0 - borderVertices[i].vertice.y/mapSize.y))/2048.;
    }

    if (isVAOSupported)
    {
        glGenVertexArrays(1, &borderVAO);
        glBindVertexArray(borderVAO);
    }
    
 	glGenBuffers(1, &borderVBO);
	glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVBOPointN)*verticesCount, borderVertices, GL_DYNAMIC_DRAW);
    
    TVBOPointN * offset2 = NULL;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->vertice);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->verticeNormal);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    
    delete []borderVertices;
    
    unsigned short polygons2[] = {0,1,2,   2,1,3,   0,2,4,   0,4,6,   3,1,5,   5,1,7,   4,5,7,   4,7,6};
    glGenBuffers(1, &borderFaces);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderFaces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(polygon_typeObj)*borderFacesCount, polygons2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

LOGroundGenerator::~LOGroundGenerator()
{
    for (int i = 0;i<=forceFieldSizeX;i++) delete[] powerField[i];
    delete[] powerField;

    clear();
}

bool LOGroundGenerator::needShadowsOnSides()
{
    return (horizontalMapAdd>=0.01);
}

void LOGroundGenerator::init()
{
    polygons = 0;
    polygons_qty = 0;
    if (isVAOSupported)
        verticesVAO = 0;
    verticesVBO = 0;
    facesVBO = 0;
    
    snowFromPolygon = 0;
    verticesNTC = 0;
    verticesNT = 0;
}

void LOGroundGenerator::clear()
{
    if (polygons) free(polygons);
    if (snowFromPolygon) free(snowFromPolygon);
    if (verticesVBO) glDeleteBuffers(1, &verticesVBO);
    if (facesVBO) glDeleteBuffers(1, &facesVBO);
    if (verticesVAO) glDeleteVertexArrays(1, &verticesVAO);
    if (verticesNTC) delete []verticesNTC;
    if (verticesNT) delete []verticesNT;
    
    init();
}

void LOGroundGenerator::renderSnowShadows(SunnyMatrix *MV, float shadowScale)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    sunnyUseShader(globalShaders[LOS_Textured3DSnowShadow]);
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D((-0.8 + rotationAngle.x/2)*shadowScale,(-0.8 + rotationAngle.y/2)*shadowScale,0.1)) * *MV;

    glDisableVertexAttribArray(1);
   
    glUniform4fv(uniform3DSS_C, 1, SunnyVector4D(50./255*globalColor.x, 65./255*globalColor.y, 84./255*globalColor.z, 1.3*globalColor.x));
    glUniform2fv(uniform3DSS_H, 1, SunnyVector2D(0,0));
    glUniformMatrix4fv(globalModelview[LOS_Textured3DSnowShadow], 1, GL_FALSE, &(m.front.x));
    
    if (facesVBO)
    {
        glDrawElements(GL_TRIANGLES, renderPolygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    } else
        glDrawElements(GL_TRIANGLES, renderPolygons_qty*3, GL_UNSIGNED_SHORT, polygons);
    
    m = getTranslateMatrix(SunnyVector3D(0,0,0.3)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_Textured3DSnowShadow], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniform3DSS_C, 1, SunnyVector4D(1.0*globalColor.x, 1.0*globalColor.y, 1*globalColor.z, 1));
    glUniform2fv(uniform3DSS_H, 1, SunnyVector2D(1,0));
    if (facesVBO)
    {
        glDrawElements(GL_TRIANGLES, renderPolygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    } else
        glDrawElements(GL_TRIANGLES, renderPolygons_qty*3, GL_UNSIGNED_SHORT, polygons);
    
    glEnableVertexAttribArray(1);
    
    //borders
    if (isVAOSupported)
        glBindVertexArray(borderVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
        TVBOPointN * offset2 = NULL;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->vertice);
    }
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderFaces);
    glDrawElements(GL_TRIANGLES, borderFacesCount*3, GL_UNSIGNED_SHORT, NULL);
    glEnableVertexAttribArray(1);
    
    //borders...
    
    glDisable(GL_DEPTH_TEST);
}

void LOGroundGenerator::render(SunnyMatrix *MV, float shadowScale)
{
    glDisable(GL_BLEND);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    sunnyUseShader(globalShaders[LOS_Textured3DSnowWalls]);
    glUniform4fv(uniform3DSW_C, 1, SunnyVector4D(1.0*globalColor.x, 1.01*globalColor.y, 1.1*globalColor.z, 1.0));
    SunnyMatrix m = *MV;
    //m.pos.z += 0.01;
    
    if (isVAOSupported)
        glBindVertexArray(borderVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
        TVBOPointN * offset2 = NULL;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->vertice);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->verticeNormal);
        glEnableVertexAttribArray(1);
    }
    glUniformMatrix4fv(globalModelview[LOS_Textured3DSnowWalls], 1, GL_FALSE, &(m.front.x));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderFaces);
    glDrawElements(GL_TRIANGLES, borderFacesCount*3, GL_UNSIGNED_SHORT, NULL);
    
    if (isVAOSupported)
        glBindVertexArray(verticesVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
        TVBOPointN * offset2 = NULL;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->vertice);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->verticeNormal);
        glEnableVertexAttribArray(1);
    }
    if (facesVBO)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesVBO);
        glDrawElements(GL_TRIANGLES, renderPolygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    } else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, renderPolygons_qty*3, GL_UNSIGNED_SHORT, polygons);
    }
    
    glEnable(GL_BLEND);

    renderSnowShadows(MV,shadowScale);
    
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
}

void LOGroundGenerator::makeNormalForPoly(int &polyNum)
{
    SunnyVector3D normal = giveNormal(verticesNT[polygons[polyNum].vertice[0]].vertice, verticesNT[polygons[polyNum].vertice[1]].vertice, verticesNT[polygons[polyNum].vertice[2]].vertice);
    verticesNT[polygons[polyNum].vertice[0]].verticeNormal += normal;
    verticesNT[polygons[polyNum].vertice[1]].verticeNormal += normal;
    verticesNT[polygons[polyNum].vertice[2]].verticeNormal += normal;
    polyNum++;
}

void LOGroundGenerator::makeNormalForPolyC(unsigned short &polyNum)
{
    SunnyVector3D normal = giveNormal(verticesNTC[polygons[polyNum].vertice[0]].vertice, verticesNTC[polygons[polyNum].vertice[1]].vertice, verticesNTC[polygons[polyNum].vertice[2]].vertice);
    verticesNTC[polygons[polyNum].vertice[0]].verticeNormal += normal;
    verticesNTC[polygons[polyNum].vertice[1]].verticeNormal += normal;
    verticesNTC[polygons[polyNum].vertice[2]].verticeNormal += normal;
    polyNum++;
}

void LOGroundGenerator::addStandartNormalForPoint(unsigned short num)
{
    verticesNT[num].verticeNormal += SunnyVector3D(0,0,1);
}

void LOGroundGenerator::addStandartNormalForPointC(unsigned short num)
{
    verticesNTC[num].verticeNormal += SunnyVector3D(0,0,1);
}

void LOGroundGenerator::createNewGroundModel(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount], unsigned char * heightMap)
{
    const float originalSnowHeight = 1.0;
    const float snowHeight = originalSnowHeight/255.;
    clear();
    
    bool q = false;
    
    if (!verticesNT)
        verticesNT = new TVBOPointN[generatorVertices_qty];
    
    const float step = mapSize.x/(mapSizeX*smallCount*partsCount);
    const short pointsInOneLine = generatorSizeX+1;
    
    int differenceX = borderAddXparts;
    int differenceY = borderAddYparts;
    
    int startC = 0;
    for (int j = 0;j<=generatorSizeY;j++)
        for (int i = 0;i<=generatorSizeX;i++)
        {
            verticesNT[startC].vertice.x = (i-borderAddXparts)*step;
            verticesNT[startC].vertice.y = mapSize.y - (j-borderAddYparts)*step;
            verticesNT[startC].verticeNormal = SunnyVector3D(0,0,0);
            verticesNT[startC].vertice.z = snowHeight*200;
            startC++;
        }
    
    int sizeX = (mapSizeX*smallCount*partsCount + 1 + 2*borderAddXparts*0);
    int sizeY = (mapSizeY*smallCount*partsCount + 1 + 2*borderAddYparts*0);
    int nowPoint = 0;
    int bufferedPolygonsCount = 0;
    polygons = (polygon_typeObj*)malloc(sizeof(polygon_typeObj));
    
    for (int j = 0;j<sizeY;j++)
    {
        startC = (j+differenceY)*pointsInOneLine + (differenceX);
        for (int i = 0;i<sizeX;i++)
        {
            verticesNT[startC].vertice.z = snowHeight*heightMap[nowPoint];
            startC++;
            nowPoint++;
        }
    }
    
//    for (int i = 0;i<generatorVertices_qty;i++)
//        verticesNT[i].vertice.z = snowHeight*heightMap[i];
    
    nowPoint = 0;
    for (int j = 0;j<sizeY-1;j++)
    {
        for (int i = 0;i<sizeX-1;i++)
        {
            if (heightMap[nowPoint] || heightMap[nowPoint+1] || heightMap[nowPoint+sizeX] || heightMap[nowPoint+sizeX+1])
            {
                if (polygons_qty>=bufferedPolygonsCount)
                {
                    bufferedPolygonsCount += 100;
                    polygons = (polygon_typeObj*)realloc(polygons, sizeof(polygon_typeObj)*bufferedPolygonsCount);
                }
                startC = (j+differenceY)*pointsInOneLine + (i+differenceX);
                polygons[polygons_qty].vertice[0] = startC;
                polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
                polygons[polygons_qty].vertice[2] = startC + 1;
                makeNormalForPoly(polygons_qty);
                polygons[polygons_qty].vertice[0] = startC + 1;
                polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
                polygons[polygons_qty].vertice[2] = startC + 1 + pointsInOneLine;
                makeNormalForPoly(polygons_qty);
            }
            nowPoint++;
        }
        nowPoint++;
    }
    
    //Left Border
    for (int i = 0;i<borderAddXparts;i++)
    for (int j = 0;j<generatorSizeY;j++)
    {
        if (polygons_qty>=bufferedPolygonsCount)
        {
            bufferedPolygonsCount += 100;
            polygons = (polygon_typeObj*)realloc(polygons, sizeof(polygon_typeObj)*bufferedPolygonsCount);
        }
        
        startC = j*pointsInOneLine + i;
        polygons[polygons_qty].vertice[0] = startC;
        polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
        polygons[polygons_qty].vertice[2] = startC + 1;
        makeNormalForPoly(polygons_qty);
        polygons[polygons_qty].vertice[0] = startC + 1;
        polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
        polygons[polygons_qty].vertice[2] = startC + 1 + pointsInOneLine;
        makeNormalForPoly(polygons_qty);
    }
    //right border
    for (int i = generatorSizeX-borderAddXparts;i<generatorSizeX;i++)
    for (int j = 0;j<generatorSizeY;j++)
    {
        if (polygons_qty>=bufferedPolygonsCount)
        {
            bufferedPolygonsCount += 100;
            polygons = (polygon_typeObj*)realloc(polygons, sizeof(polygon_typeObj)*bufferedPolygonsCount);
        }
        
        startC = j*pointsInOneLine + i;
        polygons[polygons_qty].vertice[0] = startC;
        polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
        polygons[polygons_qty].vertice[2] = startC + 1;
        makeNormalForPoly(polygons_qty);
        polygons[polygons_qty].vertice[0] = startC + 1;
        polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
        polygons[polygons_qty].vertice[2] = startC + 1 + pointsInOneLine;
        makeNormalForPoly(polygons_qty);
    }
    //Top Border
    for (int j = 0;j<borderAddYparts;j++)
    for (int i = borderAddXparts;i<generatorSizeX-borderAddXparts;i++)
    {
        if (polygons_qty>=bufferedPolygonsCount)
        {
            bufferedPolygonsCount += 100;
            polygons = (polygon_typeObj*)realloc(polygons, sizeof(polygon_typeObj)*bufferedPolygonsCount);
        }
        
        startC = j*pointsInOneLine + i;
        polygons[polygons_qty].vertice[0] = startC;
        polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
        polygons[polygons_qty].vertice[2] = startC + 1;
        makeNormalForPoly(polygons_qty);
        polygons[polygons_qty].vertice[0] = startC + 1;
        polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
        polygons[polygons_qty].vertice[2] = startC + 1 + pointsInOneLine;
        makeNormalForPoly(polygons_qty);
    }
    //Bottom Border
    for (int j = generatorSizeY-borderAddYparts;j<generatorSizeY;j++)
        for (int i = borderAddXparts;i<generatorSizeX-borderAddXparts;i++)
        {
            if (polygons_qty>=bufferedPolygonsCount)
            {
                bufferedPolygonsCount += 100;
                polygons = (polygon_typeObj*)realloc(polygons, sizeof(polygon_typeObj)*bufferedPolygonsCount);
            }
            
            startC = j*pointsInOneLine + i;
            polygons[polygons_qty].vertice[0] = startC;
            polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
            polygons[polygons_qty].vertice[2] = startC + 1;
            makeNormalForPoly(polygons_qty);
            polygons[polygons_qty].vertice[0] = startC + 1;
            polygons[polygons_qty].vertice[1] = startC + pointsInOneLine;
            polygons[polygons_qty].vertice[2] = startC + 1 + pointsInOneLine;
            makeNormalForPoly(polygons_qty);
        }

    
    if (isVAOSupported)
    {
        glGenVertexArrays(1, &verticesVAO);
        glBindVertexArray(verticesVAO);
    }
    
    //normals - > color
    SunnyVector3D lightPosition = SunnyVector3D(1,1,1.5).normalize();
    for (int i = 0;i<generatorVertices_qty;i++)
    {
        verticesNT[i].verticeNormal.z = 0.3 + 0.7*fmaxf((lightPosition & verticesNT[i].verticeNormal.normalize()),0.0);
        verticesNT[i].verticeNormal.x = (5. + 960*(verticesNT[i].vertice.x/mapSize.x))/2048;
        verticesNT[i].verticeNormal.y = (2048.-645. + 640*(1.0 - verticesNT[i].vertice.y/mapSize.y))/2048.;
    }
    
    float d;
    const short stepHeight = 1;
    for (short j = 0;j<forceFieldSizeY;j++)
    {
        startC = borderAddXparts + (j+borderAddYparts)*pointsInOneLine;
        for (short i = 0;i<forceFieldSizeX;i++)
        {
            powerField[i][j].x = 100*(- verticesNT[startC+stepHeight].vertice.z - verticesNT[startC+pointsInOneLine+stepHeight].vertice.z + verticesNT[startC-(stepHeight-1)].vertice.z + verticesNT[startC+pointsInOneLine-(stepHeight-1)].vertice.z);
            powerField[i][j].y = 100*(- verticesNT[startC-(stepHeight-1)].vertice.z - verticesNT[startC+stepHeight].vertice.z + verticesNT[startC+pointsInOneLine-(stepHeight-1)].vertice.z + verticesNT[startC+pointsInOneLine+stepHeight].vertice.z);
            d = sqrtf(powerField[i][j].x*powerField[i][j].x + powerField[i][j].y*powerField[i][j].y);
            if (d<0.001)
            {
                powerField[i][j].x = 1;
                powerField[i][j].y = 0;
            } else
            {
                powerField[i][j].x/=d;
                powerField[i][j].y/=d;
            }

            if (verticesNT[startC].vertice.z>originalSnowHeight*0.3)
            {
                powerField[i][j].z = 100;
            } else
            {
                powerField[i][j].z = 0;//0.00*d*(verticesNT[startC].vertice.z + verticesNT[startC+1].vertice.z + verticesNT[startC+pointsInOneLine].vertice.z + verticesNT[startC+pointsInOneLine+1].vertice.z);
            }
            startC++;
        }
    }
    
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVBOPointN)*generatorVertices_qty, verticesNT, GL_DYNAMIC_DRAW);
    
    TVBOPointN * offset2 = NULL;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->vertice);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointN), offset2->verticeNormal);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    
    renderPolygons_qty = polygons_qty;
    updateBuffer = false;
    needToReset = false;
    
    q = true;
    
//    if (!q)
    {
//        delete [] verticesNT;
//        verticesNT = 0;
        if (facesVBO) glDeleteBuffers(1, &facesVBO);
        glGenBuffers(1, &facesVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(polygon_typeObj)*polygons_qty, polygons, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        free(polygons);
        polygons = 0;
    }
}

void LOGroundGenerator::createGroundModel(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount])
{
    /*
    const float snowHeight = 0.9;
    clear();
    
    if (!verticesNT)
        verticesNT = new TVBOPointNT[generatorVertices_qty];
    
    unsigned short c = 0;
    for (short i = 0;i<=generatorSizeX;i++)
        for (short j = 0;j<=generatorSizeY;j++)
            verticesNT[c++].verticeNormal.z = 0;
        
    const float step = mapSize.x/(mapSizeX*smallCount*partsCount);
    const short pointsInOneLine = generatorSizeX+1;
    
    bool q = false;
    unsigned short startC;
    float height;
    for (short i = 0;i<mapSizeX*smallCount;i++)
        for (short j = 0;j<mapSizeY*smallCount;j++)
            if (smallSnow[i][j].snowType)
            {
                if (smallSnow[i][j].snowType == ST_Low)
                    height = snowHeight;
                else
                {
                    height = snowHeight + 0.9;
                    q = true;
                }
                startC = ((j+borderAddY)*pointsInOneLine + (i+borderAddX))*partsCount;
                
                c = startC;
                for (short k = 0;k<=partsCount;k++)
                {
                    for (short l = 0;l<=partsCount;l++)
                        verticesNT[c++].verticeNormal.z = height;
                    c += pointsInOneLine - (partsCount+1);
                }
                
                smallSnow[i][j].polygonNum = 0;
                
                //top border
                c = startC;
                if (j != 0 && !smallSnow[i][j-1].snowType)
                {
                for (short k = 0;k<=partsCount;k++)
                    {
                        verticesNT[c+pointsInOneLine].verticeNormal.z = 
                        verticesNT[c].verticeNormal.z = 0;//(mapHeight[i][j]*(partsCount-k) + k*mapHeight[i+1][j])/partsCount*snowHeight;
                        c++;
                    }
                    smallSnow[i][j].polygonNum |= 1;
                }
                
                //bottom border
                c = startC + partsCount*pointsInOneLine;
                if (j != mapSizeY*smallCount-1 && !smallSnow[i][j+1].snowType)
                {
                    for (short k = 0;k<=partsCount;k++)
                    {
                        verticesNT[c-pointsInOneLine].verticeNormal.z = 
                        verticesNT[c].verticeNormal.z = 0;//(mapHeight[i][j+1]*(partsCount-k) + k*mapHeight[i+1][j+1])/partsCount*snowHeight;
                        c++;
                    }
                    smallSnow[i][j].polygonNum |= 1<<1;
                }
                
                //left border
                c = startC;
                if (i != 0 && !smallSnow[i-1][j].snowType)
                {
                for (short k = 0;k<=partsCount;k++)
                    {
                        verticesNT[c+1].verticeNormal.z = 
                        verticesNT[c].verticeNormal.z = 0;//(mapHeight[i][j]*(partsCount-k) + k*mapHeight[i][j+1])/partsCount*snowHeight;
                        c+=pointsInOneLine;
                    }
                    smallSnow[i][j].polygonNum |= 1<<2;
                }
                
                //right border
                c = startC + partsCount;
                if (i!=mapSizeX*smallCount-1 && !smallSnow[i+1][j].snowType)
                {
                    for (short k = 0;k<=partsCount;k++)
                    {
                        verticesNT[c-1].verticeNormal.z = 
                        verticesNT[c].verticeNormal.z = 0;//(mapHeight[i+1][j]*(partsCount-k) + k*mapHeight[i+1][j+1])/partsCount*snowHeight;
                        c+=pointsInOneLine;
                    }
                    smallSnow[i][j].polygonNum |= 1<<3;
                }
            }
    
    c = 0;
    //top
    for (short i = 0;i<=generatorSizeX;i++)
        for (short j = 0;j<borderAddYparts;j++)
        {
            verticesNT[c].verticeNormal.z = snowHeight;
            c++;
        }
    //left
    for (short j = borderAddYparts;j<=generatorSizeY-borderAddYparts;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {
            verticesNT[c].verticeNormal.z = snowHeight;
            c++;
        }   
        c+=pointsInOneLine - borderAddXparts;
    }
    //right
    c = pointsInOneLine*(borderAddYparts+1)-borderAddXparts;//-1;
    for (short j = borderAddYparts;j<=generatorSizeY-borderAddYparts;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {
            verticesNT[c].verticeNormal.z = snowHeight;
            c++;
        }   
        c+=pointsInOneLine - borderAddXparts;
    }
    //bottom
    c = pointsInOneLine*(generatorSizeY-borderAddYparts+1);
    for (short i = 0;i<=generatorSizeX;i++)
        for (short j = generatorSizeY-borderAddYparts+1;j<=generatorSizeY;j++)
        {
            verticesNT[c].verticeNormal.z = snowHeight;
            c++;
        }
    
    c = 0;
    float koeff = 0.7;
    for (short j = 0;j<=generatorSizeY;j++)
        for (short i = 0;i<=generatorSizeX;i++)
        {
//            verticesNT[c].verticeNormal.x = (i-borderAddXparts - 0.4 + sRandomf() * 0.8)*step;
//            verticesNT[c].verticeNormal.y = mapSize.y - (j-borderAddYparts - 0.4 + sRandomf() * 0.8)*step;
            verticesNT[c].verticeNormal.x = (i-borderAddXparts)*step;
            verticesNT[c].verticeNormal.y = mapSize.y - (j-borderAddYparts)*step;
            verticesNT[c].verticeNormal.z *= koeff + (1-koeff)*2*sRandomf();;//0.65 + sRandomf() * 0.7;
            c++;
        }
    
    c = 0;
    for (short i = 0;i<=generatorSizeX;i++)
        verticesNT[c++].vertice = SunnyVector3D((i-borderAddXparts)*step,mapSize.y + borderAddYparts*step,snowHeight);
    
    for (short j = 1;j<generatorSizeY;j++)
    {
        verticesNT[c++].vertice = SunnyVector3D(-borderAddXparts*step,mapSize.y - (j-borderAddYparts)*step,snowHeight);
        for (short i = 1;i<generatorSizeX;i++)
        {
            verticesNT[c].vertice = ( verticesNT[c-1].verticeNormal + verticesNT[c+1].verticeNormal + verticesNT[c].verticeNormal
                                    + verticesNT[c-1-pointsInOneLine].verticeNormal + verticesNT[c-pointsInOneLine].verticeNormal + verticesNT[c+1-pointsInOneLine].verticeNormal
                                    + verticesNT[c-1+pointsInOneLine].verticeNormal + verticesNT[c+pointsInOneLine].verticeNormal + verticesNT[c+1+pointsInOneLine].verticeNormal)/9;
            c++;
        }
        verticesNT[c++].vertice = SunnyVector3D((generatorSizeX-borderAddXparts)*step,mapSize.y - (j-borderAddYparts)*step,snowHeight);
    }
    
    for (short i = 0;i<=generatorSizeX;i++)
        verticesNT[c++].vertice = SunnyVector3D((i-borderAddXparts)*step,-borderAddYparts*step,snowHeight);
    
    c = 0;
    for (short j = 0;j<=generatorSizeY;j++)
        for (short i = 0;i<=generatorSizeX;i++)
            verticesNT[c++].verticeNormal = SunnyVector3D(0,0,0);
    
    polygons_qty = partsCount*partsCount*((mapSizeX*smallCount+2*borderAddX)*borderAddY + borderAddX*mapSizeY*smallCount)*4;
    polygons = (polygon_typeObj*)malloc(sizeof(polygon_typeObj)*polygons_qty);
    unsigned short lastSnowFromPolyCount = polygons_qty/(partsCount*partsCount*2);
    snowFromPolygon = (unsigned short*)malloc(sizeof(unsigned short)*lastSnowFromPolyCount);
    c = 0;
    unsigned short c1 = 0;
    //top
    for (short j = 0;j<borderAddYparts;j++)
    {
        for (short i = 0;i<generatorSizeX;i++)
        {
            polygons[c].vertice[0] = c1;
            polygons[c].vertice[1] = polygons[c].vertice[0] + (pointsInOneLine);
            polygons[c].vertice[2] = polygons[c].vertice[0] + 1;
            makeNormalForPoly(c);
            
            polygons[c].vertice[0] = polygons[c-1].vertice[2];
            polygons[c].vertice[1] = polygons[c-1].vertice[1];
            polygons[c].vertice[2] = polygons[c].vertice[1] + 1;
            makeNormalForPoly(c);
            c1++;
        }
        c1++;
    }
    //left
    for (short j = 0;j<mapSizeY*smallCount*partsCount;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {
            polygons[c].vertice[0] = c1;
            polygons[c].vertice[1] = polygons[c].vertice[0] + (pointsInOneLine);
            polygons[c].vertice[2] = polygons[c].vertice[0] + 1;
            makeNormalForPoly(c);
            
            polygons[c].vertice[0] = polygons[c-1].vertice[2];
            polygons[c].vertice[1] = polygons[c-1].vertice[1];
            polygons[c].vertice[2] = polygons[c].vertice[1] + 1;
            makeNormalForPoly(c);
            c1++;
        }   
        c1+=pointsInOneLine - borderAddXparts;
    }
    //right
    c1 = pointsInOneLine*(borderAddYparts+1)-borderAddXparts-1;//-2;
    for (short j = 0;j<mapSizeY*smallCount*partsCount;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {
            polygons[c].vertice[0] = c1;
            polygons[c].vertice[1] = polygons[c].vertice[0] + (pointsInOneLine);
            polygons[c].vertice[2] = polygons[c].vertice[0] + 1;
            makeNormalForPoly(c);
            
            polygons[c].vertice[0] = polygons[c-1].vertice[2];
            polygons[c].vertice[1] = polygons[c-1].vertice[1];
            polygons[c].vertice[2] = polygons[c].vertice[1] + 1;
            makeNormalForPoly(c);
            c1++;
        }   
        c1+=pointsInOneLine - borderAddXparts;
    }
    //bottom
    c1 = (generatorSizeY-borderAddYparts)*pointsInOneLine;
    for (short j = 0;j<borderAddYparts;j++)
    {
        for (short i = 0;i<generatorSizeX;i++)
        {
            polygons[c].vertice[0] = c1;
            polygons[c].vertice[1] = polygons[c].vertice[0] + (pointsInOneLine);
            polygons[c].vertice[2] = polygons[c].vertice[0] + 1;
            makeNormalForPoly(c);
            
            polygons[c].vertice[0] = polygons[c-1].vertice[2];
            polygons[c].vertice[1] = polygons[c-1].vertice[1];
            polygons[c].vertice[2] = polygons[c].vertice[1] + 1;
            makeNormalForPoly(c);
            c1++;
        }
        c1++;
    }
    
    SunnyVector3D normal;
    for (short i = 0;i<mapSizeX*smallCount;i++)
        for (short j = 0;j<mapSizeY*smallCount;j++)
        {
            if (!smallSnow[i][j].snowType) continue;
            short dim = 0;
            bool tl=true,tr=true,bl=true,br=true;
            
            if (smallSnow[i][j].snowType == ST_High)
            {
                if (smallSnow[i][j].polygonNum & 1)
                {
                    if (smallSnow[i][j].polygonNum & 1<<2) {tl = false;dim++;}
                    if (smallSnow[i][j].polygonNum & 1<<3) {tr = false;dim++;}
                }
                if (smallSnow[i][j].polygonNum & 1<<1)
                {
                    if (smallSnow[i][j].polygonNum & 1<<2) {bl = false;dim++;}
                    if (smallSnow[i][j].polygonNum & 1<<3) {br = false;dim++;}
                }
            }
            
            polygons = (polygon_typeObj*)realloc(polygons, sizeof(polygon_typeObj)*(polygons_qty+partsCount*partsCount*2));
            lastSnowFromPolyCount++;
            snowFromPolygon = (unsigned short*)realloc(snowFromPolygon, sizeof(unsigned short)*lastSnowFromPolyCount);
            snowFromPolygon[lastSnowFromPolyCount-1] = i + j*mapSizeX*smallCount;
            unsigned short count = polygons_qty;
                        
            if (dim==0)
            {
                if (smallSnow[i][j].polygonNum & 1)
                    for (short k = 0;k<=partsCount;k++)
                        addStandartNormalForPoint(polygons[count].vertice[0] = (i+borderAddX)*partsCount+k + ((j+borderAddY)*partsCount)*(pointsInOneLine));
                if (smallSnow[i][j].polygonNum & 1<<3)
                    for (short l = 0;l<=partsCount;l++)
                        addStandartNormalForPoint(polygons[count].vertice[0] = (i+borderAddX+1)*partsCount + ((j+borderAddY)*partsCount+l)*(pointsInOneLine));
                if (smallSnow[i][j].snowType == ST_Low)
                {
                    if (smallSnow[i][j].polygonNum & 1<<1)
                        for (short k = 0;k<=partsCount;k++)
                            addStandartNormalForPoint(polygons[count].vertice[0] = (i+borderAddX)*partsCount+k + ((j+borderAddY+1)*partsCount)*(pointsInOneLine));
                    if (smallSnow[i][j].polygonNum & 1<<2)
                        for (short l = 0;l<=partsCount;l++)
                            addStandartNormalForPoint(polygons[count].vertice[0] = (i+borderAddX)*partsCount + ((j+borderAddY)*partsCount+l)*(pointsInOneLine));
                }

                
                smallSnow[i][j].polygonNum = count;
                
                for (short k = 0;k<partsCount;k++)
                    for (short l = 0;l<partsCount;l++)
                    {
                        polygons[count].vertice[0] = (i+borderAddX)*partsCount+k + ((j+borderAddY)*partsCount+l)*(pointsInOneLine);
                        polygons[count].vertice[1] = polygons[count].vertice[0] + (pointsInOneLine);
                        polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                        makeNormalForPoly(count);
                        
                        polygons[count].vertice[0] = polygons[count-1].vertice[2];
                        polygons[count].vertice[1] = polygons[count-1].vertice[1];
                        polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                        makeNormalForPoly(count);
                    }
            } else
            {
                smallSnow[i][j].polygonNum = count;
                for (short k = 1;k<partsCount-1;k++)
                    for (short l = 1;l<partsCount-1;l++)
                    {
                        polygons[count].vertice[0] = (i+borderAddX)*partsCount+k + ((j+borderAddY)*partsCount+l)*(pointsInOneLine);
                        polygons[count].vertice[1] = polygons[count].vertice[0] + (pointsInOneLine);
                        polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                        makeNormalForPoly(count);
                        
                        polygons[count].vertice[0] = polygons[count-1].vertice[2];
                        polygons[count].vertice[1] = polygons[count-1].vertice[1];
                        polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                        makeNormalForPoly(count);
                    }
                
                unsigned short s,f;
                if (tl) s = 0;
                else
                {
                    s = 1;
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + ((j+borderAddY)*partsCount+1)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + 1;
                    polygons[count].vertice[2] = polygons[count].vertice[1] - pointsInOneLine;
                    makeNormalForPoly(count);
                    
                    polygons[count] = polygons[count-1];
                    count++;
                }
                
                if (tr) f = partsCount;
                else
                {
                    f = partsCount-1;
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + f + ((j+borderAddY)*partsCount)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + pointsInOneLine;
                    polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count] = polygons[count-1];
                    count++;
                }
                //top
                for (short k = s;k<f;k++)
                {
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount+k + ((j+borderAddY)*partsCount)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + (pointsInOneLine);
                    polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count].vertice[0] = polygons[count-1].vertice[2];
                    polygons[count].vertice[1] = polygons[count-1].vertice[1];
                    polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                    makeNormalForPoly(count);
                }
                
                s = 1;
                if (bl) f = partsCount;
                else
                {
                    f = partsCount-1;
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + ((j+borderAddY)*partsCount+f)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + pointsInOneLine + 1;
                    polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count] = polygons[count-1];
                    count++;
                }
                //left
                for (short l = s;l<f;l++)
                {
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + ((j+borderAddY)*partsCount+l)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + (pointsInOneLine);
                    polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count].vertice[0] = polygons[count-1].vertice[2];
                    polygons[count].vertice[1] = polygons[count-1].vertice[1];
                    polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                    makeNormalForPoly(count);
                }
                
                s = 1;
                if (br) f = partsCount;
                else
                {
                    f = partsCount - 1;
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + f + ((j+borderAddY)*partsCount+f)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + pointsInOneLine;
                    polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count] = polygons[count-1];
                    count++;
                }
                
                //bottom
                for (short k = s;k<f;k++)
                {
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + k + ((j+borderAddY)*partsCount+partsCount-1)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + (pointsInOneLine);
                    polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count].vertice[0] = polygons[count-1].vertice[2];
                    polygons[count].vertice[1] = polygons[count-1].vertice[1];
                    polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                    makeNormalForPoly(count);
                }
                
                //right
                for (short l = 1;l<partsCount-1;l++)
                {
                    polygons[count].vertice[0] = (i+borderAddX)*partsCount + partsCount-1 + ((j+borderAddY)*partsCount+l)*(pointsInOneLine);
                    polygons[count].vertice[1] = polygons[count].vertice[0] + (pointsInOneLine);
                    polygons[count].vertice[2] = polygons[count].vertice[0] + 1;
                    makeNormalForPoly(count);
                    
                    polygons[count].vertice[0] = polygons[count-1].vertice[2];
                    polygons[count].vertice[1] = polygons[count-1].vertice[1];
                    polygons[count].vertice[2] = polygons[count].vertice[1] + 1;
                    makeNormalForPoly(count);
                }
            }
            polygons_qty = count;
        }
    
    c = 0;
    float len;
    for (short j = 0;j<=generatorSizeY;j++)
        for (short i = 0;i<=generatorSizeX;i++)
        {
            len = verticesNT[c].verticeNormal.length();
            if (len)
                verticesNT[c].verticeNormal/=len;
            
//            verticesNT[c].mapCoords = SunnyVector2D((970./2048*(1+verticesNT[c].vertice.x/mapSize.x)),(2048.-650.*(verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
    
    //left
    c = 0;
    for (short j = 0;j < borderAddYparts;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {            
            verticesNT[c].mapCoords = SunnyVector2D((5. - 960*(verticesNT[c].vertice.x/mapSize.x))/2048,(2048.-645. - 640*(1.0-verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += pointsInOneLine-borderAddXparts;
    }
    for (short j = borderAddYparts;j<generatorSizeY - borderAddYparts;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {
            verticesNT[c].mapCoords = SunnyVector2D((5. - 960*(verticesNT[c].vertice.x/mapSize.x))/2048,(2048.-645. + 640*(1.0 - verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += pointsInOneLine-borderAddXparts;
    }
    for (short j = generatorSizeY - borderAddYparts;j<=generatorSizeY;j++)
    {
        for (short i = 0;i<borderAddXparts;i++)
        {
            verticesNT[c].mapCoords = SunnyVector2D((5. - 960*(verticesNT[c].vertice.x/mapSize.x))/2048,(2048. + 640*(verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += pointsInOneLine-borderAddXparts;
    }
    
    //center
    c = borderAddXparts;
    for (short j = 0;j < borderAddYparts;j++)
    {
        for (short i = borderAddXparts;i<generatorSizeX-borderAddXparts;i++)
        {            
            verticesNT[c].mapCoords = SunnyVector2D((5. + 960*(verticesNT[c].vertice.x/mapSize.x))/2048,(2048.-645. - 640*(1.0-verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += 2*borderAddXparts+1;
    }
    for (short j = borderAddYparts;j<=generatorSizeY - borderAddYparts;j++)
    {
        for (short i = borderAddXparts;i<generatorSizeX-borderAddXparts;i++)
        {
            verticesNT[c].mapCoords = SunnyVector2D((5. + 960*(verticesNT[c].vertice.x/mapSize.x))/2048,(2048.-645. + 640*(1.0 - verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += 2*borderAddXparts+1;
    }
    for (short j = generatorSizeY - borderAddYparts + 1;j<=generatorSizeY;j++)
    {
        for (short i = borderAddXparts;i<generatorSizeX-borderAddXparts;i++)
        {
            verticesNT[c].mapCoords = SunnyVector2D((5. + 960*(verticesNT[c].vertice.x/mapSize.x))/2048,(2048. + 640*(verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += 2*borderAddXparts+1;
    }
    
    //right
    c = generatorSizeX-borderAddXparts;
    for (short j = 0;j < borderAddYparts;j++)
    {
        for (short i = generatorSizeX-borderAddXparts;i<=generatorSizeX;i++)
        {            
            verticesNT[c].mapCoords = SunnyVector2D((5. + 960 + 960*(1.0 - verticesNT[c].vertice.x/mapSize.x))/2048,(2048.-645. - 640*(1.0-verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += pointsInOneLine-(borderAddXparts+1);
    }
    for (short j = borderAddYparts;j<=generatorSizeY - borderAddYparts;j++)
    {
        for (short i = generatorSizeX-borderAddXparts;i<=generatorSizeX;i++)
        {
            verticesNT[c].mapCoords = SunnyVector2D((5. + 960 + 960*(1.0 - verticesNT[c].vertice.x/mapSize.x))/2048,(2048.-645. + 640*(1.0 - verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += pointsInOneLine-(borderAddXparts+1);
    }
    for (short j = generatorSizeY - borderAddYparts + 1;j<=generatorSizeY;j++)
    {
        for (short i = generatorSizeX-borderAddXparts;i<=generatorSizeX;i++)
        {
            verticesNT[c].mapCoords = SunnyVector2D((5. + 960 + 960*(1.0 - verticesNT[c].vertice.x/mapSize.x))/2048,(2048. + 640*(verticesNT[c].vertice.y/mapSize.y))/2048.);
            c++;
        }
        c += pointsInOneLine-(borderAddXparts+1);
    }

    if (isVAOSupported)
    {
        glGenVertexArrays(1, &verticesVAO);
        glBindVertexArray(verticesVAO);
    }
    
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVBOPointNT)*generatorVertices_qty, &verticesNT[0], GL_DYNAMIC_DRAW);
    
    TVBOPointNT * offset2 = NULL;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointNT), offset2->vertice);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TVBOPointNT), offset2->verticeNormal);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TVBOPointNT), offset2->mapCoords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    if (isVAOSupported)
        glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    renderPolygons_qty = polygons_qty;
    updateBuffer = false;
    needToReset = false;
    
//    printf("\n\n renderPolygons_qty = %d\n\n",renderPolygons_qty);
    if (!q)
    {
        delete [] verticesNT;
        verticesNT = 0;
        if (facesVBO) glDeleteBuffers(1, &facesVBO);
        glGenBuffers(1, &facesVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(polygon_typeObj)*polygons_qty, polygons, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        free(polygons);
        polygons = 0;
    }*/
}

void LOGroundGenerator::createGroundModelC(SmallSnow smallSnow[mapSizeX*smallCount][mapSizeY*smallCount])
{
    createGroundModel(smallSnow);
}

void LOGroundGenerator::reset()
{
    if (needToReset)
    {
        updateBuffer = true;
//        if (!(isVerySlowDevice && isPadDevice))
//        {
//            for (int i = 0;i<generatorVertices_qty;i++)
//                if (verticesNTC[i].vertice.z<0) verticesNTC[i].vertice.z += 2;
//        } else
        for (int i = 0;i<generatorVertices_qty;i++)
            if (verticesNT[i].vertice.z<0) verticesNT[i].vertice.z += 3;
        updatePointsAfterExplode();
    }
    
    renderPolygons_qty = polygons_qty;
}

void LOGroundGenerator::updatePointsAfterExplode()
{
    if (updateBuffer)
    {
//        double t1 = getCurrentTime();
        if (isVAOSupported)
            glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TVBOPointN)*generatorVertices_qty, &verticesNT[0]);
        updateBuffer = false;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void LOGroundGenerator::explodePolygons(short x, short y)
{
    const short pointsInOneLine = generatorSizeX+1;
    int startC = (y*pointsInOneLine + x)*partsCount + borderAddXparts + pointsInOneLine*borderAddYparts;
    
//    printf("\n (%d, %d)",x,y);
    int c = startC;
    
    x *= partsCount;
    y *= partsCount;
    for (short j = 0 ;j<=partsCount;j++)
    {
        c = startC +j*pointsInOneLine;
        for (short i = 0 ;i<=partsCount;i++)
        {
            if (verticesNT[c].vertice.z>=0)
                verticesNT[c].vertice.z -= 3;
            powerField[x+i][y+j].z = 0;
            c++;
        }
    }
    updateBuffer = true;
    needToReset = true;
}