//
//  LOMagicDropInfo.h
//  LookOut
//
//  Created by Ignatov on 19.11.13.
//
//

#ifndef __LookOut__LOMagicDropInfo__
#define __LookOut__LOMagicDropInfo__

#include <iostream>
#include "LOTutorial.h"
#include "LOMagicBox.h"

enum LOMD_Type
{
//    DropType_VioletSnowflake,
    DropType_Spell = 0,
    DropType_Buff,
    DropType_Puzzle,
    DropType_Count,
};

class LOMagicDropInfo : public LOTutorial
{
    char * dropChances[DropType_Count];
    LOMagicBoxType boxType;
public:
    LOMagicDropInfo(LOMagicBoxType type);
    ~LOMagicDropInfo();
    
    void render();
    bool renderOverlay();
};

#endif /* defined(__LookOut__LOMagicDropInfo__) */
