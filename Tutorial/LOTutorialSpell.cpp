//
//  LOTutorialSpell.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 17.11.13.
//
//

#include "LOTutorialSpell.h"
#include "LOGlobalVar.h"

const SunnyVector2D tutor_abilityPosition = SunnyVector2D(mapSize.x*0.29,mapSize.y*0.43);

LOTutorialSpell::LOTutorialSpell(LevelAbility spell)
{
    ability = spell;
    setName("NEW_SPELL_UNLOCKED",0.6);
    switch (ability) {
        case LA_Teleport:
        {
            spellName = getLocalizedLabel("Teleportation");
            description = getLocalizedLabel("Teleportation_description");
            break;
        }
        case LA_Speed:
        {
            spellName = getLocalizedLabel("Speed");
            description = getLocalizedLabel("Speed_description");
            break;
        }
        case LA_Life:
        {
            spellName = getLocalizedLabel("Life");
            description = getLocalizedLabel("Life_description");
            break;
        }
        case LA_Shield:
        {
            spellName = getLocalizedLabel("Shield");
            description = getLocalizedLabel("Shield_description");
            break;
        }
        case LA_Magnet:
        {
            spellName = getLocalizedLabel("Magnet");
            description = getLocalizedLabel("Magnet_description");
            break;
        }
        case LA_Time:
        {
            spellName = getLocalizedLabel("TimeShifter");
            description = getLocalizedLabel("TimeShifter_description");
            break;
        }
        case LA_Doubler:
        {
            spellName = getLocalizedLabel("Doubler");
            description = getLocalizedLabel("Doubler_description");
            break;
        }
            
        default:
            break;
    }
    
    setVisible(true);
    addStandartOkButtonToClose();
}

LOTutorialSpell::~LOTutorialSpell()
{
    delete []description;
    delete []spellName;
}

void LOTutorialSpell::render()
{
    LOTutorial::render();
    
    SunnyMatrix m;
    
    m = getScaleMatrix(0.5) * getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y*0.59,-1));
    LOFont::writeText(&m, LOTA_Center, true, spellName, yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha));
    
    m = getScaleMatrix(0.4) * getTranslateMatrix(SunnyVector3D(mapSize.x*0.57,tutor_abilityPosition.y,-1));
    LOFont::writeTextInRect(&m, SunnyVector2D(mapSize.x*0.4,0), LOTA_Center, true, description,whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    
    m = getTranslateMatrix(SunnyVector3D(tutor_abilityPosition.x,tutor_abilityPosition.y,-1));
    float scale = 1;
    LOMenu::getSharedMenu()->renderSpellButton(ability, &m, scale, scale, 0, false, appearAlpha, true);
}

bool LOTutorialSpell::renderOverlay()
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(tutor_abilityPosition.x,tutor_abilityPosition.y,-1));
    LOMenu::renderSpellOverlay(ability, &m, appearAlpha);
    return true;
}