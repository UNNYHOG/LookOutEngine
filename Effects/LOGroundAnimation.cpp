//
//  LOGroundAnimation.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 09.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "LOGroundAnimation.h"
#include "LOGlobalVar.h"

LOGroundAnimation::LOGroundAnimation()
{
    positions = 0;
}

LOGroundAnimation::~LOGroundAnimation()
{
    if (positions) delete [] positions;
}

void LOGroundAnimation::prepareLOGroundAnimation()
{
    if (positions) delete [] positions;
    
    positions = new SunnyVector3D[objectsCount];
    SunnyVector3D cen;
    bool *tempVert = new bool[vertices_qty];
    for (short i = 1; i<objectsCount; i++)
    {
        cen = (objects[i]->maxSize + objects[i]->minSize)/2;
        positions[i] = cen;
        
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
    }
    delete [] tempVert;
}

void LOGroundAnimation::renderCenterBall()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[0]->faceVBO);
    glDrawElements(GL_TRIANGLES, objects[0]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
}

void LOGroundAnimation::prepareForRender()
{
    if (isVAOSupported)
        glBindVertexArray(verticesVAO);
    else
        bindVBO(true);
}

void LOGroundAnimation::getMatricesForCrash(SunnyMatrix matrices[])
{
    for (short i = 0;i<objectsCount-1;i++)
    {
        matrices[i] = getIdentityMatrix();
        matrices[i].pos.x = positions[i+1].x; 
        matrices[i].pos.y = positions[i+1].y;
        matrices[i].pos.z = positions[i+1].z;
    }
}

void LOGroundAnimation::renderShadowsByMatrices(SunnyMatrix * matrices)
{
    for (short i = 0;i<objectsCount-1;i++)
    {
        float z = matrices[i].pos.z;
        glUniform1f(uniformTSA_A, 1.0 + z/cellSize);
        if (z<0) z = 0;
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(matrices[i].pos.x - lightPosition.x*(z/lightPosition.z), matrices[i].pos.y - lightPosition.y*(z/lightPosition.z),1,1));
        
        glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+1)*4, 4);
    }
}

void LOGroundAnimation::renderByMatrices(SunnyMatrix *MV, SunnyMatrix * matrices, float * angles)
{
    SunnyMatrix m;
    for (short i = 0;i<objectsCount-1;i++)
    {
        m = matrices[i] **MV;
        m = getRotationXMatrix(angles[i]*M_PI/180) *m;
        glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i+1]->faceVBO);
        glDrawElements(GL_TRIANGLES, objects[i+1]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    }
}

void LOGroundAnimation::render(SunnyMatrix *MV, float centerAngle, SunnyVector2D * angles)
{
    SunnyMatrix mat = getIdentityMatrix();
    SunnyMatrix matPhi = getIdentityMatrix();
    SunnyMatrix matTeta = getIdentityMatrix();
    
//    glPushMatrix();
    mat.up.y = cosf(centerAngle); mat.up.z = sinf(centerAngle);
    mat.right.y = -mat.up.z;mat.right.z = mat.up.y;
    SunnyMatrix m = mat * *MV;
    glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[0]->faceVBO);
    glDrawElements(GL_TRIANGLES, objects[0]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
//    glPopMatrix();
    
    for (int i = 1 ;i<objectsCount; i++)
    {
        mat = getIdentityMatrix();
        mat.pos = SunnyVector4D(positions[i].x,positions[i].y,positions[i].z,1.0);
        matPhi.front.x = cosf(angles[i-1].x);matPhi.front.y = sinf(angles[i-1].x);
        matPhi.up.x = -matPhi.front.y; matPhi.up.y = matPhi.front.x;
        matTeta.up.y = cosf(angles[i-1].y); matTeta.up.z = sinf(angles[i-1].y);
        matTeta.right.y = -matTeta.up.z;matTeta.right.z = matTeta.up.y;
        mat = mat * matPhi * matTeta;
//        glPushMatrix();
//        glMultMatrixf(&mat[0][0]);
        m = mat ** MV;
        glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i]->faceVBO);
        glDrawElements(GL_TRIANGLES, objects[i]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
//        glPopMatrix();
    }
}