//
//  LOSurvivalMap.h
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#ifndef __LookOut__LOSurvivalMap__
#define __LookOut__LOSurvivalMap__

#include <iostream>

class LOSurvivalMap
{
public:
    LOSurvivalMap();
    ~LOSurvivalMap();
    
    virtual void reset();
    virtual void update();
    virtual void loadMap(FILE * stream);
    virtual void saveMap(FILE * stream);
};

#endif /* defined(__LookOut__LOSurvivalMap__) */
