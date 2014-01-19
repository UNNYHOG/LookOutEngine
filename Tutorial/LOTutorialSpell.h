//
//  LOTutorialSpell.h
//  LookOut
//
//  Created by Pavel Ignatov on 17.11.13.
//
//

#ifndef __LookOut__LOTutorialSpell__
#define __LookOut__LOTutorialSpell__

#include <iostream>
#include "LOTutorial.h"
#include "LOPlayer.h"

class LOTutorialSpell : public LOTutorial
{
    LevelAbility ability;
    char * description;
    char * spellName;
public:
    LOTutorialSpell(LevelAbility spell);
    ~LOTutorialSpell();
    
    void render();
    bool renderOverlay();
};

#endif /* defined(__LookOut__LOTutorialSpell__) */
