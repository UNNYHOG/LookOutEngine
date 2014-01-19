//
//  LOSurvivalMap.h
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#ifndef __LookOut__LOSurvivalMap04__
#define __LookOut__LOSurvivalMap04__

#include <iostream>
#include "LOSurvivalMap.h"

class LOSurvivalMap04 : public LOSurvivalMap
{
    float nextRespawnTime[6];
    float respawnPeriod[6];
    float enemySpeed;
public:
    LOSurvivalMap04();
    ~LOSurvivalMap04();
    
    void reset();
    void update();
    void loadMap(FILE * stream);
    void saveMap(FILE * stream);
};

#endif /* defined(__LookOut__LOSurvivalMap__) */
