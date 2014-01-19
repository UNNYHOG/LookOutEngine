//
//  LOCrashModel.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 09.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOCrashModel.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOTrace.h"
#include "SunnyOpengl.h"

LOCrashModel::LOCrashModel()
{
    matrices = 0;
    curMatrices = 0;
    velocity = 0;
    scale = 0;
}

LOCrashModel::~LOCrashModel()
{
    if (matrices) delete [] matrices;
    if (curMatrices) delete [] curMatrices;
    if (velocity) delete [] velocity;
    if (scale) delete [] scale;
}

void LOCrashModel::prepareModels()
{
    if (matrices) delete [] matrices;
    if (curMatrices) delete [] curMatrices;
    if (velocity) delete [] velocity;
    if (scale) delete [] scale;
    
    matrices = new SunnyMatrix[objectsCount];
    curMatrices = new SunnyMatrix[objectsCount];
    velocity = new SunnyVector3D[objectsCount];
    scale = new float[objectsCount];
    
    SunnyVector3D cen;
    bool *tempVert = new bool[vertices_qty];
    for (short i = 0; i<objectsCount; i++)
    {
        cen = (objects[i]->maxSize + objects[i]->minSize)/2;
        matrices[i] = getIdentityMatrix();
        matrices[i].pos = SunnyVector4D(cen.x,cen.y,cen.z,1.0);
        
        memset(tempVert, 0, sizeof(bool)*vertices_qty);
        for (unsigned short j = 0 ;j<objects[i]->polygons_qty;j++)
        {
            tempVert[objects[i]->polygons[j].vertice[0]] = true;
            tempVert[objects[i]->polygons[j].vertice[1]] = true;
            tempVert[objects[i]->polygons[j].vertice[2]] = true;
        }
        
        if (renderType == 3)
        {
            for (short j = 0;j<vertices_qty;j++)
                if (tempVert[j])
                    verticesNT[j].vertice -= cen;
        }
        
        curMatrices[i] = matrices[i];
    }
    delete [] tempVert;
}

void LOCrashModel::prepareToCrashForMenu(SunnyMatrix mat)
{
    for (int i = 0 ;i<objectsCount; i++)
    {
        curMatrices[i] = matrices[i]*mat;
        SunnyVector4D radius = curMatrices[i].pos - mat.pos;
        velocity[i] = SunnyVector3D(radius.x, -1,radius.z);
        
        scale[i] = mat.pos.y - sqrtf(radius.x*radius.x + radius.z*radius.z);
    }
    bonfireCrash = false;

}

void LOCrashModel::prepareToCrash(SunnyMatrix mat, SunnyVector2D direction)
{
    float r = player->getRadius()*2;
    SunnyMatrix scaleMat = getIdentityMatrix();
    scaleMat.front.x = scaleMat.up.y = scaleMat.right.z = r;
    
    SunnyVector2D rightDir(direction.y,-direction.x);
    for (int i = 0 ;i<objectsCount; i++)
    {
        curMatrices[i] = matrices[i]*scaleMat*mat;
        r = 1 + sRandomf();
        velocity[i] = 2/(player->getRadius())*SunnyVector3D(direction.x*r + (1.5-r)*rightDir.x*5*player->getRadius(),direction.y*r + (1.5-r)*rightDir.y*5*player->getRadius(),0.5 - sRandomf());// + Sunny4Dto3D(curMatrices[i].pos)*0.2;
        scale[i] = 1.0;
    }
    bonfireCrash = false;
}

void LOCrashModel::prepareToCrashFromBonfire(SunnyMatrix mat, SunnyVector2D direction)
{
    float r = player->getRadius()*2;
    SunnyMatrix scaleMat = getIdentityMatrix();
    scaleMat.front.x = scaleMat.up.y = scaleMat.right.z = r;
    
    SunnyVector2D rightDir(direction.y,-direction.x);
    for (int i = 0 ;i<objectsCount; i++)
    {
        curMatrices[i] = matrices[i]*scaleMat*mat;
        velocity[i] = SunnyVector3D(0.5-sRandomf() + direction.x*0.1,0.5-sRandomf() + direction.y*0.1,0)*1.5;//2/(player->getRadius())*SunnyVector3D(direction.x*r + (1.5-r)*rightDir.x*5*player->getRadius(),direction.y*r + (1.5-r)*rightDir.y*5*player->getRadius(),0.5 - sRandomf());// + Sunny4Dto3D(curMatrices[i].pos)*0.2;
        scale[i] = 1.0;
    }
    bonfireCrash = true;
}

void LOCrashModel::update()
{
    for (short i = 0; i < objectsCount; i++)
    {
        if (scale[i]<=0.2) continue;
        
        curMatrices[i].pos.x += velocity[i].x*deltaTime;
        curMatrices[i].pos.y += velocity[i].y*deltaTime;
        curMatrices[i].pos.z += (velocity[i].z - 9./2*deltaTime)*deltaTime;
        velocity[i].z -= 9.*deltaTime;
        
        if (curMatrices[i].pos.z<=0)
        {
            curMatrices[i].pos.z = 0;
            if (bonfireCrash)
            {
                velocity[i].z = fabsf(velocity[i].z)/5;
                velocity[i].x *= 0.8;
                velocity[i].y *= 0.8;
            } else
            {
                velocity[i].z = fabsf(velocity[i].z)/2;
                velocity[i].x *= 0.9;
                velocity[i].y *= 0.9;
            }
            if ((fabsf(velocity[i].x) + fabsf(velocity[i].y) >= 0.1) && (rand()%5==0 || velocity[i].z>0.3))
            {
                LOTrace::sharedTrace()->addRandomPartEffect(&curMatrices[i]);
            }
        }
        
        SunnyVector3D rotVec = velocity[i] ^ SunnyVector3D(0,0,-1);
        rotVec = -rotVec*0.1*(1.0+sRandomf()*0.5);
        float sinTetaAngle = sin(rotVec.x);
        float cosTetaAngle = cos(rotVec.x);
        float sinPhiAngle = sin(-rotVec.y);
        float cosPhiAngle = cos(-rotVec.y);
        SunnyMatrix m = curMatrices[i] * SunnyMatrix(SunnyVector4D(1,0,0,0),SunnyVector4D(0,cosTetaAngle,-sinTetaAngle,0),SunnyVector4D(0,sinTetaAngle,cosTetaAngle,0),SunnyVector4D(0,0,0,1));
        m = m * SunnyMatrix(SunnyVector4D(cosPhiAngle,0,-sinPhiAngle,0),SunnyVector4D(0,1,0,0),SunnyVector4D(sinPhiAngle,0,cosPhiAngle,0),SunnyVector4D(0,0,0,1));
        m.pos = curMatrices[i].pos;
        curMatrices[i] = m;
        
        scale[i] -= deltaTime*0.2;
    }
}

void LOCrashModel::updateForMenu()
{
    for (short i = 0; i < objectsCount; i++)
    {
        if (scale[i]<=0.2 || curMatrices[i].pos.y<scale[i]) continue;
        
        curMatrices[i].pos.x += velocity[i].x*deltaTime;
        curMatrices[i].pos.z += velocity[i].z*deltaTime;
        curMatrices[i].pos.y += (velocity[i].y - 9./2*deltaTime)*deltaTime;
        velocity[i].y -= 9.*deltaTime;

        SunnyVector3D rotVec = velocity[i] ^ SunnyVector3D(0,0,-1);
        rotVec = -rotVec*0.03*(1.0+sRandomf()*0.5);
        float sinTetaAngle = sin(rotVec.x);
        float cosTetaAngle = cos(rotVec.x);
        float sinPhiAngle = sin(-rotVec.y);
        float cosPhiAngle = cos(-rotVec.y);
        SunnyMatrix m = curMatrices[i] * SunnyMatrix(SunnyVector4D(1,0,0,0),SunnyVector4D(0,cosTetaAngle,-sinTetaAngle,0),SunnyVector4D(0,sinTetaAngle,cosTetaAngle,0),SunnyVector4D(0,0,0,1));
        m = m * SunnyMatrix(SunnyVector4D(cosPhiAngle,0,-sinPhiAngle,0),SunnyVector4D(0,1,0,0),SunnyVector4D(sinPhiAngle,0,cosPhiAngle,0),SunnyVector4D(0,0,0,1));
        m.pos = curMatrices[i].pos;
        curMatrices[i] = m;
        
//        scale[i] -= deltaTime*0.2;
    }
}

void LOCrashModel::renderShadows()
{
    for (int i = 0 ;i<objectsCount; i++)
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(curMatrices[i].pos.x - lightPosition.x*(curMatrices[i].pos.z/lightPosition.z), curMatrices[i].pos.y - lightPosition.y*(curMatrices[i].pos.z/lightPosition.z),1,1));
        glUniform1f(uniformTSA_A, scale[i]*3);
        glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+1)*4, 4);
    }
}

void LOCrashModel::renderForMenu(SunnyMatrix *MV)
{
    prepareForRender();
    SunnyMatrix m;
    SunnyVector4D v = SunnyVector4D(1,1,20,0).normalize()*1.3;
    glUniform3fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "lightPosition"), 1, v);
    for (int i = 0 ;i<objectsCount; i++)
    {
        m = curMatrices[i]**MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured3DSnow], 1, GL_FALSE, &(m.front.x));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i]->faceVBO);
        glDrawElements(GL_TRIANGLES, objects[i]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    }
    glUniform3fv(glGetUniformLocation(globalShaders[LOS_Textured3DSnow], "lightPosition"), 1, customLightPositionGraphic);
}

void LOCrashModel::render(SunnyMatrix *MV)
{
    prepareForRender();
    SunnyMatrix m;
    for (int i = 0 ;i<objectsCount; i++)
    {
        m = getScaleMatrix(SunnyVector3D(scale[i], scale[i], scale[i]))* curMatrices[i]**MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured3DSnow], 1, GL_FALSE, &(m.front.x));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i]->faceVBO);
        glDrawElements(GL_TRIANGLES, objects[i]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    }
}

void LOCrashModel::prepareForRender()
{
    if (isVAOSupported)
        glBindVertexArray(verticesVAO);
    else
        bindVBO(true);
}

void LOCrashModel::renderOriginal(SunnyMatrix *MV)
{
    SunnyMatrix m;
    for (int i = 0 ;i<objectsCount; i++)
    {
        //glPushMatrix();
        //glMultMatrixf(&curMatrices[i][0][0]);
        m = curMatrices[i] **MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i]->faceVBO);
        glDrawElements(GL_TRIANGLES, objects[i]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
//        glPopMatrix();
    }
}

void LOCrashModel::getMatricesForCrash(SunnyMatrix matrices[])
{
    for (short i = 0;i<objectsCount;i++)
    {
        matrices[i] = curMatrices[i]; 
    }
}

void LOCrashModel::renderShadowsByMatrices(SunnyMatrix * matrices)
{
    for (int i = 0 ;i<objectsCount; i++)
    {
        float z = matrices[i].pos.z;
        glUniform1f(uniformTSA_A, 1.0 + z/cellSize);
        if (z<0) z = 0;
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(matrices[i].pos.x - lightPosition.x*(z/lightPosition.z), matrices[i].pos.y - lightPosition.y*(z/lightPosition.z),1,1));

        glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+1)*4, 4);
    }
}

void LOCrashModel::renderByMatrices(SunnyMatrix *MV, SunnyMatrix * matrices, float * angles)
{
    SunnyMatrix m;
    for (short i = 0;i<objectsCount;i++)
    {
        m = matrices[i] **MV;
        m = getRotationXMatrix(angles[i]*M_PI/180) *m;
        glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i]->faceVBO);
        glDrawElements(GL_TRIANGLES, objects[i]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    }
}

