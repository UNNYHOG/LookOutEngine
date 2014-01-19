//
//  LOTutorialMessage.h
//  LookOut
//
//  Created by Pavel Ignatov on 17.11.13.
//
//

#ifndef __LookOut__LOTutorialMessage__
#define __LookOut__LOTutorialMessage__

#include <iostream>
#include "LOTutorial.h"

class LOTutorialMessage : public LOTutorial
{
    char * description;
    SunnyButton * showMeButton;
    SunnyButton * laterButton;
    char * showMeText;
    char * laterText;
    char * secondHeader;
public:
    LOTutorialMessage(const char * header, const char * text, const char * underHeader);
    ~LOTutorialMessage();
    
    void render();
    void buttonPressed(SunnyButton *btn);
};

#endif /* defined(__LookOut__LOTutorialMessage__) */
