//
//  LOMagicBox.h
//  LookOut
//
//  Created by Pavel Ignatov on 08.09.13.
//
//

#ifndef __LookOut__LOMagicBox__
#define __LookOut__LOMagicBox__

#include <iostream>
#include "SUnnyMatrix.h"
enum LOMagicBoxType
{
    LOMB_Small = 0,
    LOMB_Medium,
    LOMB_Big,
    LOMB_Count,
};

enum LOIceState
{
    IceState_Solid = 0,
    IceState_Cracked,
    IceState_CrackedBig,
    IceState_Destroyed,
    IceState_Count
};

enum LODropType
{
    LODrop_VS = 0,
    LODrop_Spell,
    LODrop_Puzzle,
};

struct LODropInfo
{
    LODropType dropType;
    short additionalType;
    short ammount;
};

const short maxDropCount = 10;

struct LODrop
{
    LODropInfo drop[maxDropCount];
    short dropCount;
};

class LOMagicBox
{
    
public:
    static void prepareModels();
    static void clearModels();
    static void renderMagicBox(LOIceState state, SunnyMatrix * matrix);
    static void renderCrashedMagicBox(SunnyMatrix * matrices,SunnyMatrix * matrix);
    static void openMagicBox(LOMagicBoxType type);
    
    static short getDestroyedObjectsCount();
    static SunnyMatrix * getDestroyedMatrices();
    static LODrop* getLastDrop();
};

#endif /* defined(__LookOut__LOMagicBox__) */
