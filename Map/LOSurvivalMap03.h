//
//  LOSurvivalMap.h
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#ifndef __LookOut__LOSurvivalMap03__
#define __LookOut__LOSurvivalMap03__

#include <iostream>
#include "LOSurvivalMap.h"

class LOSurvivalMap03 : public LOSurvivalMap
{
    float nextRespawnTime[5];//4 -electric
    float respawnPeriod[5];

    float nextEclipse;
    char eclipseNumber;
    char eclipseStartNumber;
    float eclipseTimer;
    bool systemsOn;
    void calcNextEclipse();
    float enemySpeed;
public:
    LOSurvivalMap03();
    ~LOSurvivalMap03();
    
    void reset();
    void update();
    void loadMap(FILE * stream);
    void saveMap(FILE * stream);
};

#endif /* defined(__LookOut__LOSurvivalMap__) */
