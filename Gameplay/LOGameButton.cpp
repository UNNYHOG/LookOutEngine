//
//  LOButton.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 12.09.12.
//
//

#include "LOGameButton.h"
#include "SunnyOpenGL.h"
#ifndef SKIP_PHYSICS
#include "LOGlobalVar.h"
#endif

LOGameButton::LOGameButton()
{
    body = scCreateCircle(cellSize/2);
    body->layers = LAYER_LOW;
    body->collisionType = COLLISION_BUTTON;
    scActivateCircle(body);
}

LOGameButton::~LOGameButton()
{
    scDeactivateCircle(body);
    delete body;
}

void LOGameButton::initWithParams(SunnyVector2D pos,short repetable)
{
    position = pos;
    repeat = repetable;
    reset();
    
    body->position = pos;
}

void LOGameButton::reset()
{
    buttonState = LOBS_WaitingUp;
}

void LOGameButton::render()
{
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(position.x, position.y,1,1));
    glUniform1f(uniformTSA_A, 1);
    glDrawArrays(GL_TRIANGLE_STRIP, (LOButton_VAO)*4, 4);
    if (buttonState == LOBS_Down || buttonState == LOBS_WaitingDown)
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(position.x, position.y,0.87,0.87));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOButton_VAO+1)*4, 4);
}