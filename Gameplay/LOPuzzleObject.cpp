//
//  LOPuzzleObject.cpp
//  LookOut
//
//  Created by Ignatov on 14.11.13.
//
//

#include "LOPuzzleObject.h"
#include "iPhone_Strings.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

extern int puzzleTextureID;
extern SunnyModelObj * puzzleObject;
extern short objectsNumbers[puzzleHorizontalSize*puzzleVerticalSize];
extern SunnyVector3D partsTranslations[puzzleHorizontalSize*puzzleVerticalSize];
const SunnyVector3D puzzleDestinationPosition = SunnyVector3D(mapSize.x/2, mapSize.y*0.55, -5);

const float puzzleAnimationTime = 2;
const float puzzleAppearHeight = -20;

void LOPuzzleObject::collectPuzzle()
{
    
}

void LOPuzzleObject::render()
{
    bindGameGlobal();
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    float t = time/puzzleAnimationTime;
    if (t>1) t = 1;
    SunnyMatrix m = getScaleMatrix(t*4.5) * getTranslateMatrix(Sunny4Dto3D(position.pos));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform1f(uniformTSA_A, 1+0.2*sinf(time));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    
    SHTextureBase::bindTexture(puzzleTextureID);
    SunnyMatrix m1 = getScaleMatrix(puzzlePartsScale) *  getRotationXMatrix(M_PI_2);
    m = getTranslateMatrix(partsTranslations[partNumber]) * m1 * position;
    puzzleObject->prepareForRender(isVAOSupported);
    
    sunnyUseShader(globalShaders[LOS_Textured3D]);
    glUniform4fv(uniform3D_C, 1, SunnyVector4D(1,1,1,1));
    glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
    puzzleObject->renderObject(objectsNumbers[partNumber]);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void LOPuzzleObject::update(float deltaTime)
{
    time += deltaTime;
    
    SunnyVector3D iPos(initPosition.x,initPosition.y,puzzleAppearHeight);
    float t = time/puzzleAnimationTime;
    if (t>1) t = 1;
    
    iPos = iPos*(1-t) + puzzleDestinationPosition*t;
    const float startScale = 0.1;
    position = getScaleMatrix((t+startScale)/(1+startScale)) * getRotationYMatrix(M_PI*4*(1-t)) * directionMatrix * getTranslateMatrix(iPos);
}

void LOPuzzleObject::renderShadow()
{
    
}

void LOPuzzleObject::initWithPosition(SunnyVector2D pos, unsigned short number)
{
    partNumber = number;
    initPosition = pos;
    time = 0;
    
    SunnyVector2D direction = SunnyVector2D(puzzleDestinationPosition.x,puzzleDestinationPosition.y) - pos;
    if (direction.length()<1)
        direction = SunnyVector2D(1,0);
    else
        direction.normalize();
    directionMatrix = sunnyIdentityMatrix;
    directionMatrix.front = SunnyVector4D(direction.x,direction.y,0,0);
    directionMatrix.up = directionMatrix.right ^ directionMatrix.front;
    
    update(0);
}

LOPuzzleObject::LOPuzzleObject()
{
    LOPuzzle::prepareForRenderIfNecessary();
}