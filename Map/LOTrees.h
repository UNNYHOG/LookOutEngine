//
//  LOTrees.h
//  LookOut
//
//  Created by Pavel Ignatov on 25.08.13.
//
//

#ifndef __LookOut__LOTrees__
#define __LookOut__LOTrees__

#include <iostream>
#include "SUnnyMatrix.h"

class LOTrees
{
public:
    static void clearTrees();
    static void addTree(short number, SunnyMatrix mat);
    static void render(SunnyMatrix *MV);
    static void prepareModel();
    static void makeTempTrees();
};

#endif /* defined(__LookOut__LOTrees__) */
