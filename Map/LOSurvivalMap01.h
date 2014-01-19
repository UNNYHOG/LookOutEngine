//
//  LOSurvivalMap.h
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#ifndef __LookOut__LOSurvivalMap01__
#define __LookOut__LOSurvivalMap01__

#include <iostream>

#import "LOSurvivalMap.h"

class LOSurvivalMap01 : public LOSurvivalMap
{
    float nextRespawnTime[3];
    float respawnPeriod[3];
    float snowflakePeriod;
    float massSnowflakesChance;
    float enemySpeed;
public:
    LOSurvivalMap01();
    ~LOSurvivalMap01();
    
    void reset();
    void update();
    void loadMap(FILE * stream);
    void saveMap(FILE * stream);
};

#endif /* defined(__LookOut__LOSurvivalMap__) */
