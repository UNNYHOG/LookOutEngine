//
//  LOMessageWindow.h
//  LookOut
//
//  Created by Pavel Ignatov on 13.10.13.
//
//

#ifndef __LookOut__LOMessageWindow__
#define __LookOut__LOMessageWindow__

#include <iostream>
#include "SunnyButton.h"

enum LOMessageType
{
    LOMessage_None = 0,
    LOMessage_NoGold,
};

struct ButtonCallBack
{
    void (*callbackFunction)(SunnyButton*);
    SunnyButton * callbackParameter;
};

class LOMessageWindow
{
    char * text;
    float appearAlpha;
    ButtonCallBack *callbacks;
    SunnyButton ** buttons;
    short buttonsCount;
    char ** buttonsText;
    
    bool gameWasPaused;
    LOMessageType messageType;
    
    SunnyVector2D frameSize;
    
public:
    bool isVisible;
    LOMessageWindow();
    ~LOMessageWindow();
    
    void buttonPressed(SunnyButton *btn);
    void setText(const char * string);
    void setVisible(bool visible,LOMessageType type = LOMessage_None);
    void render();
    void setCallback(void (*func)(SunnyButton*),SunnyButton * btn, short buttonNum = 0);
    void changeButtonText(const char* newText);
    void addButton(const char* text, void (*func)(SunnyButton*),SunnyButton * btn);
};

#endif /* defined(__LookOut__LOMessageWindow__) */
