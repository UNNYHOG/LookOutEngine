//
//  LOTutorial.h
//  LookOut
//
//  Created by Ignatov on 01.11.13.
//
//

#ifndef __LookOut__LOTutorial__
#define __LookOut__LOTutorial__

#include <iostream>
#include "SunnyButton.h"
#include "LOSounds.h"

const float tutorialScale = 1.5;
const float rotationSpeed = M_PI_2;

void emptyRenderFunction(SunnyButton * btn);
const SunnyVector2D okButtonSize = SunnyVector2D(170,170/2);

class LOTutorial
{
    SunnyButton *lockButton;
    char * nameString;
    float nameScale;
    
protected:
    SunnyVector2D namePosition;
    SunnyVector2D frameSize;
    SunnyVector2D windowCenter;

    float appearAlpha;
    bool lastGameState;
    float tutorialTime;
    SunnyButton * okButton;
    float okButtonAlpha;
    void setName(const char * name, float scale = 0.5);
public:
    bool isVisible;
    LOTutorial();
    
    static void renderBackForSize(SunnyVector2D frameSize, SunnyVector2D center, float alpha, SunnyVector3D color = SunnyVector3D(215./255, 224./255, 233./255));
    virtual ~LOTutorial();
    
    virtual bool blockRenderOverlays();
    virtual void setVisible(bool visible);
    virtual void update(float deltaTime);
    virtual void render();
    virtual bool renderOverlay();
    virtual void reset();
    void addStandartOkButtonToClose(void (*func)(SunnyButton*) = 0);
    virtual void buttonPressed(SunnyButton *btn);
};

#endif /* defined(__LookOut__LOTutorial__) */
