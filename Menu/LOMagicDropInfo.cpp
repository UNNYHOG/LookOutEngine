//
//  LOMagicDropInfo.cpp
//  LookOut
//
//  Created by Ignatov on 19.11.13.
//
//

#include "LOMagicDropInfo.h"
#include "LOGlobalVar.h"

const SunnyVector2D dropPositionCenter = SunnyVector2D(mapSize.x/2,mapSize.y/2);

LOMagicDropInfo::LOMagicDropInfo(LOMagicBoxType type)
{
    const float scale = 0.6;
    switch (type) {
        case LOMB_Small:
//            dropChances[DropType_VioletSnowflake] = getLocalizedLabel("DROP_SOME");
            dropChances[DropType_Spell] = getLocalizedLabel("DROP_SOME");
            dropChances[DropType_Buff] = getLocalizedLabel("DROP_ONE");
            dropChances[DropType_Puzzle] = 0;
            setName("SMALL_GIFT",scale);
            break;
        case LOMB_Medium:
//            dropChances[DropType_VioletSnowflake] = getLocalizedLabel("DROP_SEVERAL");
            dropChances[DropType_Spell] = getLocalizedLabel("DROP_SEVERAL");
            dropChances[DropType_Buff] = getLocalizedLabel("DROP_COUPLE");
            dropChances[DropType_Puzzle] = getLocalizedLabel("DROP_VERY_RARE");
            setName("MEDIUM_GIFT",scale);
            break;
        case LOMB_Big:
//            dropChances[DropType_VioletSnowflake] = getLocalizedLabel("DROP_MANY");
            dropChances[DropType_Spell] = getLocalizedLabel("DROP_MANY");
            dropChances[DropType_Buff] = getLocalizedLabel("DROP_A_FEW");
            dropChances[DropType_Puzzle] = getLocalizedLabel("DROP_RARE");
            setName("BIG_GIFT",scale);
            break;
            
        default:
            break;
    }
    setVisible(true);
    addStandartOkButtonToClose();
}

LOMagicDropInfo::~LOMagicDropInfo()
{
    for (short i = 0;i<DropType_Count;i++)
        if (dropChances[i])
            delete [] dropChances[i];
}

const float leftDistance = 6;
const float rightDistance = 3;
const float verticalDistance = 1.1;

bool LOMagicDropInfo::renderOverlay()
{
    //Spells
    float YY = mapSize.y/2-0.3;
    if (dropChances[DropType_Puzzle])
        YY += verticalDistance;
    SunnyMatrix m = getScaleMatrix(0.5) * getTranslateMatrix(SunnyVector3D(mapSize.x/2-leftDistance,YY,-2));
    const float spellDistances = 0.85;
    SunnyMatrix m1;
    bindGameGlobal();
    m1 = m * getTranslateMatrix(SunnyVector3D(-spellDistances,spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Teleport, &m1, appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D( spellDistances,spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Speed, &m1, appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D(-spellDistances,-spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Life, &m1, appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D(spellDistances,-spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Shield, &m1, appearAlpha);
    
    //Ability
    bindGameGlobal();
    m = getScaleMatrix(0.5) * getTranslateMatrix(SunnyVector3D(mapSize.x/2+rightDistance,YY,-2));
    
    m1 = m * getTranslateMatrix(SunnyVector3D(-spellDistances,spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Magnet, &m1, appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D( spellDistances,spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Time, &m1, appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D(0,-spellDistances,0));
    LOMenu::renderSpellOverlay(LA_Doubler, &m1, appearAlpha);
    return true;
}

void LOMagicDropInfo::render()
{
    LOTutorial::render();

    //Spells
    float YY = mapSize.y/2-0.3;
    if (dropChances[DropType_Puzzle])
        YY += verticalDistance;
    SunnyMatrix m = getScaleMatrix(0.5) * getTranslateMatrix(SunnyVector3D(mapSize.x/2-leftDistance,YY,-2));
    const float spellDistances = 0.85;
    SunnyMatrix m1;
    bindGameGlobal();
    m1 = m * getTranslateMatrix(SunnyVector3D(-spellDistances,spellDistances,0));
    LOMenu::renderSpellButton(LA_Teleport, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D( spellDistances,spellDistances,0));
    LOMenu::renderSpellButton(LA_Speed, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D(-spellDistances,-spellDistances,0));
    LOMenu::renderSpellButton(LA_Life, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D(spellDistances,-spellDistances,0));
    LOMenu::renderSpellButton(LA_Shield, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = getScaleMatrix(0.7) * m * getTranslateMatrix(SunnyVector3D(spellDistances*2.5,0,0));
    LOFont::writeText(&m1, LOTA_LeftCenter, true, dropChances[DropType_Spell],whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    
    
    //Ability
    bindGameGlobal();
    m = getScaleMatrix(0.5) * getTranslateMatrix(SunnyVector3D(mapSize.x/2+rightDistance,YY,-2));
    
    m1 = m * getTranslateMatrix(SunnyVector3D(-spellDistances,spellDistances,0));
    LOMenu::renderSpellButton(LA_Magnet, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D( spellDistances,spellDistances,0));
    LOMenu::renderSpellButton(LA_Time, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = m * getTranslateMatrix(SunnyVector3D(0,-spellDistances,0));
    LOMenu::renderSpellButton(LA_Doubler, &m1, 1, 1, 0,false,appearAlpha);
    
    m1 = getScaleMatrix(0.7) * m * getTranslateMatrix(SunnyVector3D(spellDistances*2.5,0,0));
    LOFont::writeText(&m1, LOTA_LeftCenter, true, dropChances[DropType_Buff],whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    
    //Puzzle
    if (dropChances[DropType_Puzzle])
    {
        bindGameGlobal();
        YY -= 3*verticalDistance;
        m = getScaleMatrix(0.5) * getTranslateMatrix(SunnyVector3D(mapSize.x/2 -1,YY,-2));
        m1 = getScaleMatrix(2) * m * getTranslateMatrix(SunnyVector3D(-2,0,0));
        LOMenu::renderPuzzleImage(&m1, appearAlpha);
        LOFont::writeText(&m, LOTA_LeftCenter, true, dropChances[DropType_Puzzle],whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    }
}


