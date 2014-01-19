//
//  LOTrees.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 25.08.13.
//
//

#include "LOTrees.h"
#include "SunnyModelObj.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "SunnyShaders.h"

SunnyModelObj * treeModel = 0;
const SunnyVector4D shadowTreeColor = SunnyVector4D(50.,65.,84.,50.)/255.;

struct LOOneTree
{
    SunnyMatrix position;
    short number;
};
static unsigned short loTreesCount;
static LOOneTree * loTrees;

void LOTrees::prepareModel()
{
    return;
    if (treeModel) return;
    
    treeModel = new SunnyModelObj;
    //treeModel->loadFromSobjFile(getPath("Base/Objects/Trees","sh3do"));
    treeModel->loadFromObjFile(getPath("Base/Objects/Trees","obj"));
    treeModel->makeVBO(true, isVAOSupported, true);
    
    loTrees = 0;
    loTreesCount = 0;
}

void LOTrees::clearTrees()
{
    if (loTrees)
        free(loTrees);
    loTrees = 0;
    loTreesCount = 0;
}

void LOTrees::addTree(short number, SunnyMatrix mat)
{
    if (loTrees)
        loTrees = (LOOneTree*)realloc(loTrees, sizeof(LOOneTree)*(loTreesCount+1));
    else
        loTrees = (LOOneTree*)malloc(sizeof(LOOneTree));
    
    loTrees[loTreesCount].position = mat;
    loTrees[loTreesCount].number = number;
    loTreesCount++;
}

void LOTrees::render(SunnyMatrix *MV)
{
    return;
    if (!loTreesCount) return;
    
    SunnyMatrix m;
    sunnyUseShader(globalShaders[LOS_SolidObject]);
    treeModel->prepareForRender(isVAOSupported);
    glUniform4fv(uniformSO_Color, 1, shadowTreeColor);
    
    SunnyMatrix rot = getRotationZMatrix(sin(getCurrentTime()*0.2)*0.03);
    
    for (short i = 0;i<loTreesCount;i++)
    {
        m = rot*loTrees[i].position**MV;
        glUniformMatrix4fv(globalModelview[LOS_SolidObject], 1, GL_FALSE, &(m.front.x));
        treeModel->renderObject(loTrees[i].number);
    }
}

void LOTrees::makeTempTrees()
{
    SunnyMatrix mat = getScaleMatrix(SunnyVector3D(1.5,0.8,1)) * getRotationZMatrix(M_PI_2*1.4) * getTranslateMatrix(SunnyVector3D(35,22,0));
    addTree(0, mat);
    
    mat = getScaleMatrix(SunnyVector3D(0.3,0.3,0.3))* getRotationZMatrix(M_PI_2*1.6) * getTranslateMatrix(SunnyVector3D(15,24,0));
    addTree(1, mat);
}