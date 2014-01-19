//
//  LOCoinsPurchased.cpp
//  LookOut
//
//  Created by Ignatov on 14.11.13.
//
//

#include "LOCoinsPurchased.h"
#include "LOGlobalVar.h"

LOCoinsPurchased::LOCoinsPurchased()
{
    setName("Congratulations",0.8);
    labelYouGot = getLocalizedLabel("You Got");
}

LOCoinsPurchased::~LOCoinsPurchased()
{
    delete []labelYouGot;
}

void LOCoinsPurchased::render()
{
    LOTutorial::render();
    
    float scale = 0.5;
    SunnyMatrix m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y*0.5,-2));
    LOFont::writeText(&m, LOTA_Center, true, labelYouGot,whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    
    m = m * getTranslateMatrix(SunnyVector3D(0,-0.1*mapSize.y,0));
    loRenderPriceLabel(&m, LOPT_GoldCoins, (int)goldCoins, appearAlpha, LOTA_Center, scale*1.2);
}

bool LOCoinsPurchased::renderOverlay()
{
    
    return true;
}

void LOCoinsPurchased::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
}

void LOCoinsPurchased::initValue(unsigned long coins)
{
    goldCoins = coins;
    setVisible(true);
    addStandartOkButtonToClose();
}

