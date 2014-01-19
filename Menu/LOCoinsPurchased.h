//
//  LOCoinsPurchased.h
//  LookOut
//
//  Created by Ignatov on 14.11.13.
//
//

#ifndef __LookOut__LOCoinsPurchased__
#define __LookOut__LOCoinsPurchased__

#include <iostream>
#include "LOTutorial.h"

class LOCoinsPurchased : public LOTutorial
{
    unsigned long goldCoins;
    char * labelYouGot;
public:
    LOCoinsPurchased();
    ~LOCoinsPurchased();
    void render();
    bool renderOverlay();
    void update(float deltaTime);
    void initValue(unsigned long coins);
};

#endif /* defined(__LookOut__LOCoinsPurchased__) */
