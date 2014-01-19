//
//  LOSurvivalMap.h
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#ifndef __LookOut__LOSurvivalMap02__
#define __LookOut__LOSurvivalMap02__

#include <iostream>
#include "LOSurvivalMap.h"

class LOSurvivalMap02 : public LOSurvivalMap
{
    float nextRespawnTime[4];
    float respawnPeriod[4];
    float enemySpeed;
public:
    LOSurvivalMap02();
    ~LOSurvivalMap02();
    
    void reset();
    void update();
    void loadMap(FILE * stream);
    void saveMap(FILE * stream);
};

#endif /* defined(__LookOut__LOSurvivalMap__) */
