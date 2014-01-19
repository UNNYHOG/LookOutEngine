//
//  LOPuzzleObject.h
//  LookOut
//
//  Created by Ignatov on 14.11.13.
//
//

#ifndef __LookOut__LOPuzzleObject__
#define __LookOut__LOPuzzleObject__

#include <iostream>
#include "SunnyMatrix.h"
#include "SunnyCollisionDetector.h"
//#include "LOGoldCoin.h"

class LOPuzzleObject
{
    SunnyMatrix position;
    SunnyVector2D initPosition;
    float time;
    unsigned short partNumber;
    SunnyMatrix directionMatrix;
public:
    LOPuzzleObject();
    
    void initWithPosition(SunnyVector2D pos, unsigned short number);
    void renderShadow();
    void render();
    void update(float deltaTime);
    void collectPuzzle();
};

#endif /* defined(__LookOut__LOPuzzleObject__) */
