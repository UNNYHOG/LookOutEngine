//
//  LOVioletSnowflake.cpp
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#include "LOVioletSnowflake.h"
#include "LOGlobalVar.h"
#include "LOEvents.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"

const float violetSnowflakeScale = 0.3;

LOVioletSnowflake::LOVioletSnowflake()
{
    physicsCreated = false;
    physics = scCreateCircle(cellSize/2*(3./4.));
    physics->layers = LAYER_SNOWFLAKE;
    physics->collisionType = COLLISION_VIOLET_SNOWFLAKE;
    physics->data = this;
    startToDissapear = false;
}

LOVioletSnowflake::~LOVioletSnowflake()
{
    scDeactivateCircle(physics);
    delete physics;
}

SunnyVector2D LOVioletSnowflake::getPosition()
{
    return physics->position;
}

void LOVioletSnowflake::prepareToDissapear()
{
    scDeactivateCircle(physics);
    startToDissapear = true;
    dissapearFrame = 0;
}

void LOVioletSnowflake::initWithPosition(SunnyVector2D pos, SunnyVector2D velocity)
{
    physics->position = pos;
    physics->velocity = velocity;
    physicsCreated = true;
    scActivateCircle(physics);
    startToDissapear = false;
    
    haveVelocity = velocity.length()>0.1;
    
    lifeTime = 0;
}

void LOVioletSnowflake::collectSnowflake(bool grab)
{
    physicsCreated = false;
    scDeactivateCircle(physics);
    
    if (grab)
    {
        LOEvents::OnSnowflakeCollected();
        if (player->isAbilityActive(LA_Doubler))
            LOEvents::OnSnowflakeCollected();
    }
}

void LOVioletSnowflake::renderForMenu(SunnyVector2D pos, float scale, float alpha)
{
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(pos.x,pos.y,-1));
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, alpha);
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
    SunnyDrawArrays(LOVioletSnowflake_VAO);
}

void LOVioletSnowflake::render(SunnyMatrix *MV)
{
    float scale = 1;
    if (startToDissapear)
    {
        if (dissapearFrame>=2) return;
        scale = (2-dissapearFrame)*0.35;
        dissapearFrame++;
    }

    SunnyMatrix m1 = getScaleMatrix(violetSnowflakeScale*scale) * getTranslateMatrix(SunnyVector3D(physics->position.x,physics->position.y,0)) **MV;
    
    SunnyMatrix m = getScaleMatrix(backScale) * getRotationZMatrix(lifeTime/6*M_PI) *m1;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOVioletSnowflakeInGame_VAO);
    
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m1.front.x));
    SunnyDrawArrays(LOVioletSnowflakeInGame_VAO+1);
    
    if (startToDissapear)
        LOSnowflake::renderDissapear(SunnyVector2D(physics->position.x, physics->position.y), dissapearFrame, 1);
}

void LOVioletSnowflake::render(SunnyVector2D pos,float time, float scale, float alpha)
{
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    SHTextureBase::bindTexture(objectsTexture);
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(1,1,1,alpha));
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(pos.x, pos.y, -1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    
    float back = (0.6 + (1+sinf(time))*0.7/2)*scale;
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, back, back));
    SunnyDrawArrays(LOSnowflake_VAO+1);
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, scale, scale));
    m = getRotationZMatrix(-time/6*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOSnowflake_VAO);
}

void LOVioletSnowflake::renderOverlay(SunnyMatrix *MV)
{
    float scale = 1;
    if (startToDissapear)
    {
        if (dissapearFrame>=2) return;
        scale = (2-dissapearFrame)*0.35;
    }
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(physics->position.x,physics->position.y,scale,scale));
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
}

void LOVioletSnowflake::renderOverlay(SunnyVector2D pos, float scale, float alpha)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(pos.x, pos.y, -1));
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,alpha));
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
    
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
}

bool LOVioletSnowflake::update()
{
    if (startToDissapear && dissapearFrame>=2)
        return false;
    
    lifeTime += deltaTime;
    backScale = (0.9 + (1+sinf(lifeTime))*0.2/2);
    static const float distance = 10;
    if (player->isAbilityActive(LA_Magnet))
    {
        SunnyVector2D direction = player->getPosition() - physics->position;
        float d = direction.length();
        if (d>0.01 && d < distance)
        {
            physics->velocity/=1.5;
            direction/=d;
            float a = 100.0/(1.0+d/distance);
            physics->velocity += direction*a*deltaTime;
        } else
            if (!haveVelocity)
                physics->velocity /= 1.1;
    }
    
    static const float distanceForDissapear = 6;
    if (physics->position.x<-distanceForDissapear || physics->position.x>mapSize.x+distanceForDissapear ||
        physics->position.y<-distanceForDissapear || physics->position.y>mapSize.y+distanceForDissapear)
        {
            activeMap->destroyVioletSnowflake(this);
        }
    return true;
}

