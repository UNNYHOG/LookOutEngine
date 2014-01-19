//
//  LOStore.h
//  LookOut
//
//  Created by Pavel Ignatov on 08.09.13.
//
//

#ifndef __LookOut__LOStore__
#define __LookOut__LOStore__

#include <iostream>
#include "LOMagicBox.h"
#include "SunnyButton.h"
#include "SunnyScrollView.h"
#include "LOScore.h"

enum LOStoreBack
{
    StoreBack_None = 0,
    StoreBack_MainMenu,
    StoreBackPause,
    StoreBackLevelComplete,
    StoreBackDeathWindow,
    StoreBack_Survival,
};

enum LOStorePage
{
    LOStore_MagicBox = 0,
    LOStore_GoldCoins,
    LOStore_FreeCoins,
    LOStore_Count,
};

enum LOGoldPurchases
{
    LOGold_20 = 0,
    LOGold_60,
    LOGold_125,
    LOGold_260,
    LOGold_700,
    LOGold_Count,
};

class LOStore
{
    char * labelStore[LOStore_Count];
    //SunnyButton *pageBtn[LOStore_Count];
    SunnyButton *magicBtn[LOMB_Count];
    SunnyButton *magicBtnIce[LOMB_Count];
    SunnyButton *goldBtn[LOGold_Count];
    SunnyButton *freeGoldBtn[LOFreeGold_Count];
    float magicBtnAnimationTime[LOMB_Count];
    
    SunnyButton * backButton;
    SunnyButton * lockButton;

    LOStorePage currentPage;
    float *offset;
    static void applyNormalPrices();
    static void applySale30();
    static void applySalex3();
    static void productsInfoRecieved(const char * info);
    static void updateProductsPrices();
    static void inAppSucceded(const char * inApp);
    static void inAppFailed(const char * inApp);
    float storeTime;
    
    char * freeLabel[LOFreeGold_Count];
    LOPriceType freePriceType[LOFreeGold_Count];
    unsigned short freePriceValue[LOFreeGold_Count];
    
    void prepareGoldAnimation(int goldValue);
    bool goldCoinsAnimated;
    float goldCoinsAnimationTime;
    bool goldSoundPlayed;
    
    bool oldMenuMode;
    SunnyButton *scrollButtons[2];
public:
    static void loadStore();
    static LOStore * getSharedStore();
    
    SunnyScrollView * scroll;
    LOStoreBack storeBack;
    //bool isVisible;
    void buttonPressed(SunnyButton * btn);
    void facebookLoginSucceded(const char * playerId);
    void hideWindow();
    
    void changePage(LOStorePage page);
//    void setVisible(bool visible);
    void render();
    void renderOverlay();
    void update(float deltaTime);
    LOStore();
    ~LOStore();
};

#endif /* defined(__LookOut__LOStore__) */
