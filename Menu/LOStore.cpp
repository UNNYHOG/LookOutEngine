//
//  LOStore.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 08.09.13.
//

#include "LOStore.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "LOFont.h"
#include "iPhone_Strings.h"
#include "UnnyNet.h"
#include "LOSounds.h"
#include "LOGlobalVar.h"

enum LOOnSaleType
{
    SaleType_None = 0,
    SaleType_30,
    SaleType_x3,
};

struct LOProductInfo
{
    char* productID;
    float price;
    char* priceLocale;
    LOOnSaleType saleType;
    short goldCount;
};

enum LOAllGoldPurchases
{
    LOAllGold_20 = 0,
    LOAllGold_60,
    LOAllGold_125,
    LOAllGold_260,
    LOAllGold_700,
    LOAllGold_125_30,
    LOAllGold_260_30,
    LOAllGold_700_30,
    LOAllGold_125_x3,
    LOAllGold_260_x3,
    LOAllGold_700_x3,
    LOAllGold_Count,
};

LOProductInfo allProductInfo[LOAllGold_Count];
LOProductInfo *productInfo[LOGold_Count];

LOStore * sharedStore = 0;
bool storeWasLoaded = false;

LOStore * LOStore::getSharedStore()
{
    return sharedStore;
}

void loRenderPriceLabel(SunnyMatrix *mat, LOProductInfo *info,float appearAlpha = 1)
{
    if (info->price <= 0.01)//not loaded
    {
        //LOFont::writeText(mat, LOTA_Center, true, loLabelLoading);
        return;
    }
    
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    bindGameGlobal();
    LOFont::writeNumber(mat, true, (int)(info->price),SunnyVector4D(255,177,27,255)/255,SunnyVector4D(241,241,241,255)/255);
}

void loRenderPriceLabel(SunnyMatrix *mat, LOPriceType priceType, int price, float appearAlpha, LOTextAlign align,float scaleCoin)
{
    float coinsLabelWidth = 0;
    float coinsNumberWidth;
    
    float scale = 1.0/mat->front.length();
    float origin = 0;
    
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    bindGameGlobal();
    
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(mat->front.x));
    //GoldCoin
    int num = price;
    
    {
        //calc size
        if (price>=10000)
        {
            num = price/1000;
            coinsNumberWidth = LOFont::getNumberSize(num);
            coinsLabelWidth += LOFont::getTextSize(loLabelKilo);
        } else
        {
            coinsNumberWidth = LOFont::getNumberSize(price);
        }
        coinsLabelWidth += coinsNumberWidth;
    }

    if (priceType!=LOPT_Free)
    {
        float typeWidth;
        if (priceType == LOPT_VioletSnowflakes)
            typeWidth = snowflakeImageWidth;
        else
        if (priceType == LOPT_GoldCoins)
            typeWidth = goldCoinImageWidth;
        else
            typeWidth = yellowStarImageWidth;
        coinsLabelWidth += typeWidth;
    
        glUniform1f(uniformTSA_A, appearAlpha);
        if (align==LOTA_LeftCenter)
            origin = -(-coinsLabelWidth/2 + typeWidth/2);
        else
            if (align == LOTA_RightCenter)
                origin -= coinsLabelWidth/2;
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(origin - coinsLabelWidth/2 + typeWidth/2,0,scale*scaleCoin,scale*scaleCoin));
        switch (priceType) {
            case LOPT_VioletSnowflakes:
                SunnyDrawArrays(LOVioletSnowflake_VAO);
                break;
            case LOPT_GoldCoins:
                SunnyDrawArrays(LOGoldCoin_VAO);
                break;
            case LOPT_Stars:
                SunnyDrawArrays(LOYellowStar_VAO);
                break;
            default:
                break;
        }
        
        origin = origin - coinsLabelWidth/2 + typeWidth;
    } else
    {
        origin = -coinsLabelWidth/2;
    }
    
    SunnyMatrix m;
    {
        m = getTranslateMatrix(SunnyVector3D(origin,0,-1))**mat;
        LOFont::writeNumber(&m, true, num,yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha));
    }
    
    //Coins k
    if (price>=10000)
    {
        m = getTranslateMatrix(SunnyVector3D(origin + coinsNumberWidth,0,-1))**mat;
        LOFont::writeText(&m, LOTA_LeftCenter, true, loLabelKilo, yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha));
    }
}

char * loCreateChar(const char * str)
{
    char * s = new char[strlen(str)+1];
    strcpy(s, str);
    return s;
}

void storeButtonPressed(SunnyButton * btn)
{
    if (sharedStore)
        sharedStore->buttonPressed(btn);
}


void storeFBLoggenId(const char * playerID)
{
    if (sharedStore)
        sharedStore->facebookLoginSucceded(playerID);
}

void storeEmptyRender(SunnyButton * btn)
{
    
}

void renderButton2(SunnyButton * btn)
{
    
}

void updateStorePage()
{
    sharedStore->changePage((LOStorePage)(sharedStore->scroll->getCurrentPage()));
}

void LOStore::applyNormalPrices()
{
    productInfo[LOGold_20] = &(allProductInfo[LOAllGold_20]);
    productInfo[LOGold_60] = &(allProductInfo[LOAllGold_60]);
    productInfo[LOGold_125] = &(allProductInfo[LOAllGold_125]);
    productInfo[LOGold_260] = &(allProductInfo[LOAllGold_260]);
    productInfo[LOGold_700] = &(allProductInfo[LOAllGold_700]);
}

void LOStore::productsInfoRecieved(const char * info)
{
    if (!info) return;

    char * products = new char[strlen(info)+1];
    strcpy(products, info);
    
    char * str = strtok(products, " ");
    short state = 0;
    LOProductInfo * productInfo = 0;
    while (str) {
        
        switch (state) {
            case 0:
            {
                bool found = false;
                for (short i = 0;i<LOAllGold_Count;i++)
                    if (!strcmp(allProductInfo[i].productID, str))
                    {
                        productInfo = &(allProductInfo[i]);
                        found = true;
                        break;
                    }
                if (found)
                    state++;
                break;
            }
            case 1:
            {
                try {
                    productInfo->price = atof(str);
                } catch (int e) {
                    productInfo->price = 0;
                }
                state++;
                break;
            }
                
            case 2:
            {
                if (productInfo->priceLocale)
                    delete []productInfo->priceLocale;
                productInfo->priceLocale = getLocalizedLabel(str);
                state = 0;
                break;
            }
                
            default:
                break;
        }
        
        str = strtok(NULL, " ");
    }
    
    delete [] products;
}

void LOStore::updateProductsPrices()
{
    char * str = new char[LOAllGold_Count*strlen(allProductInfo[LOAllGold_125_30].productID)];
    strcpy(str, allProductInfo[0].productID);
    for (short i = 1;i<LOAllGold_Count;i++)
    {
        strcat(str, " ");
        strcat(str, allProductInfo[i].productID);
    }
    UnnyNet::requestProductsInfo(str,LOStore::productsInfoRecieved);
    delete []str;
}

void LOStore::applySale30()
{
    productInfo[LOGold_125] = &(allProductInfo[LOAllGold_125_30]);
    productInfo[LOGold_260] = &(allProductInfo[LOAllGold_260_30]);
    productInfo[LOGold_700] = &(allProductInfo[LOAllGold_700_30]);
}

void LOStore::applySalex3()
{
    productInfo[LOGold_125] = &(allProductInfo[LOAllGold_125_x3]);
    productInfo[LOGold_260] = &(allProductInfo[LOAllGold_260_x3]);
    productInfo[LOGold_700] = &(allProductInfo[LOAllGold_700_x3]);
}

void LOStore::loadStore()
{
    UnnyNet::setInAppPurchaseCallback(LOStore::inAppSucceded, LOStore::inAppFailed);
    
    if (storeWasLoaded)
    {
        for (short i = LOAllGold_20;i<=LOAllGold_700_x3;i++)
        {
            delete []allProductInfo[i].productID;
            allProductInfo[i].productID = 0;
            if (allProductInfo[i].priceLocale)
            {
                delete []allProductInfo[i].priceLocale;
                allProductInfo[i].priceLocale = 0;
            }
        }
            
    } else
        storeWasLoaded = true;
    
    allProductInfo[LOAllGold_20].productID = loCreateChar("com.unnyhog.dodge.coins.20");
    allProductInfo[LOAllGold_60].productID = loCreateChar("com.unnyhog.dodge.coins.60");
    allProductInfo[LOAllGold_125].productID = loCreateChar("com.unnyhog.dodge.coins.125");
    allProductInfo[LOAllGold_260].productID = loCreateChar("com.unnyhog.dodge.coins.260");
    allProductInfo[LOAllGold_700].productID = loCreateChar("com.unnyhog.dodge.coins.700");
    allProductInfo[LOAllGold_125_30].productID = loCreateChar("com.unnyhog.dodge.coins.125.sale30");
    allProductInfo[LOAllGold_260_30].productID = loCreateChar("com.unnyhog.dodge.coins.260.sale30");
    allProductInfo[LOAllGold_700_30].productID = loCreateChar("com.unnyhog.dodge.coins.700.sale30");
    allProductInfo[LOAllGold_125_x3].productID = loCreateChar("com.unnyhog.dodge.coins.125.salex3");
    allProductInfo[LOAllGold_260_x3].productID = loCreateChar("com.unnyhog.dodge.coins.260.salex3");
    allProductInfo[LOAllGold_700_x3].productID = loCreateChar("com.unnyhog.dodge.coins.700.salex3");
    
    allProductInfo[LOAllGold_20].goldCount = 20;
    allProductInfo[LOAllGold_60].goldCount = 60;
    allProductInfo[LOAllGold_125].goldCount = 125;
    allProductInfo[LOAllGold_260].goldCount = 260;
    allProductInfo[LOAllGold_700].goldCount = 700;
    allProductInfo[LOAllGold_125_30].goldCount = 125;
    allProductInfo[LOAllGold_260_30].goldCount = 260;
    allProductInfo[LOAllGold_700_30].goldCount = 700;
    allProductInfo[LOAllGold_125_x3].goldCount = 125*3;
    allProductInfo[LOAllGold_260_x3].goldCount = 260*3;
    allProductInfo[LOAllGold_700_x3].goldCount = 700*3;
    
    for (short i = LOAllGold_20;i<=LOAllGold_700;i++)
        allProductInfo[i].saleType = SaleType_None;
    for (short i = LOAllGold_125_30;i<=LOAllGold_700_30;i++)
        allProductInfo[i].saleType = SaleType_30;
    for (short i = LOAllGold_125_x3;i<=LOAllGold_700_x3;i++)
        allProductInfo[i].saleType = SaleType_x3;
    for (short i = LOAllGold_20;i<=LOAllGold_700_x3;i++)
    {
        allProductInfo[i].price = 0;
        allProductInfo[i].priceLocale = 0;
    }
    
    applyNormalPrices();
    
    updateProductsPrices();
}

void LOStore::prepareGoldAnimation(int goldValue)
{
    goldCoinsAnimated = true;
    goldCoinsAnimationTime = 0;
    goldSoundPlayed = false;
    LOMenu::getSharedMenu()->showGoldPurchasedWindow(goldValue);
}

void LOStore::inAppSucceded(const char * inApp)
{
    for (short i = 0;i<LOAllGold_Count;i++)
    {
        if (!strcmp(inApp, allProductInfo[i].productID))
        {
            LOSendFlurryMessageString("InAppPurchased","ProductID",inApp,NULL);
            LOScore::addBoughtGoldCoin(allProductInfo[i].goldCount);
            if (sharedStore)
                sharedStore->prepareGoldAnimation(allProductInfo[i].goldCount);
            break;
        }
    }
}

void LOStore::inAppFailed(const char * inApp)
{
    LOMenu::getSharedMenu()->showMessage("ERROR_UNKNOWN");
}

LOStore::LOStore()
{
    oldMenuMode = menuMode;
    menuMode = true;
    currentPage = LOStore_MagicBox;
    sharedStore = this;
    storeBack = StoreBack_None;
    goldCoinsAnimated = false;
    
    backButton = 0;
    
    lockButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(100, 100));
    lockButton->soundTag = 0;
    sunnyButtonSetRenderFunc(lockButton, storeEmptyRender);
    
    //Magic box
    SunnyVector2D size(2,1);
    for (short i = 0;i<LOMB_Count;i++)
    {
        magicBtn[i] = sunnyAddButton(SunnyVector2D(7+8*i,6) , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
        sunnyButtonSetCallbackFunc(magicBtn[i],storeButtonPressed);
        sunnyButtonSetRenderFunc(magicBtn[i], storeEmptyRender);
        magicBtn[i]->matrix.pos.z = -15;
        
        magicBtnIce[i] = sunnyAddButton(SunnyVector2D(7+8*i,12) , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,SunnyVector2D(3,3));
        sunnyButtonSetCallbackFunc(magicBtnIce[i],storeButtonPressed);
        sunnyButtonSetRenderFunc(magicBtnIce[i], storeEmptyRender);
        magicBtnIce[i]->matrix.pos.z = -15;
        
        magicBtnAnimationTime[i] = -100;
    }
    
    backButton = sunnyAddButton(SunnyVector2D(2-horizontalMapAdd,2-verticalMapAdd), menuTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(1.5,1.5));
    sunnyButtonSetActiveState(backButton, SunnyVector4D(0,0.5,0.25,0.25));
    sunnyButtonSetCallbackFunc(backButton,storeButtonPressed);
    sunnyButtonSetRenderFunc(backButton, storeEmptyRender);
    
    //Gold
    const float mapKoef = mapSize.x/1920;
    size = SunnyVector2D(442,498)*mapKoef/2;
    goldBtn[LOGold_20]      = sunnyAddButton(SunnyVector2D(494,1280-390)*mapKoef , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
    goldBtn[LOGold_60]      = sunnyAddButton(SunnyVector2D(960,1280-390)*mapKoef , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
    goldBtn[LOGold_125]     = sunnyAddButton(SunnyVector2D(1428,1280-390)*mapKoef , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
    goldBtn[LOGold_260]     = sunnyAddButton(SunnyVector2D(712,1280-910)*mapKoef , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
    goldBtn[LOGold_700]     = sunnyAddButton(SunnyVector2D(1208,1280-910)*mapKoef , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
    for (short i = 0;i<LOGold_Count;i++)
    {
        sunnyButtonSetCallbackFunc(goldBtn[i],storeButtonPressed);
        goldBtn[i]->hidden = true;
        sunnyButtonSetRenderFunc(goldBtn[i], storeEmptyRender);
        goldBtn[i]->tag = i;
    }
    
    //Free Coins
    size = SunnyVector2D(746,214)*mapKoef/2;
    for (short i = 0;i<LOFreeGold_Count;i++)
    {
        freeGoldBtn[i] = sunnyAddButton(SunnyVector2D((576+(i%2)*766)*mapKoef,(1280-374-(i/2)*246)*mapKoef) , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Normal,size);
        freeGoldBtn[i]->hidden = true;
        sunnyButtonSetCallbackFunc(freeGoldBtn[i], storeButtonPressed);
        sunnyButtonSetRenderFunc(freeGoldBtn[i], storeEmptyRender);
    }
    
    freeLabel[LOFreeGoldUnnyNet] = getLocalizedLabel("UnnyNet Connect");
    freeLabel[LOFreeGoldRate] = getLocalizedLabel("Rate the Game");
    freeLabel[LOFreeGoldLikeUnnyhog] = getLocalizedLabel("Like Unnyhog");
    freeLabel[LOFreeGoldLikeDodge] = getLocalizedLabel("Like Dodge and Roll");
    freeLabel[LOFreeGoldTwitter] = getLocalizedLabel("Dodge Twitter");
    freeLabel[LOFreeGoldYouTube] = getLocalizedLabel("Unnyhog Youtube");
    
    freePriceType[LOFreeGoldUnnyNet] = LOPT_GoldCoins;
    freePriceType[LOFreeGoldRate] = LOPT_GoldCoins;
    freePriceType[LOFreeGoldLikeUnnyhog] = LOPT_GoldCoins;
    freePriceType[LOFreeGoldLikeDodge] = LOPT_GoldCoins;
    freePriceType[LOFreeGoldTwitter] = LOPT_GoldCoins;
    freePriceType[LOFreeGoldYouTube] = LOPT_GoldCoins;
    
    freePriceValue[LOFreeGoldUnnyNet]       = 5;
    freePriceValue[LOFreeGoldRate]          = 3;
    freePriceValue[LOFreeGoldLikeUnnyhog]   = 2;
    freePriceValue[LOFreeGoldLikeDodge]     = 2;
    freePriceValue[LOFreeGoldTwitter]       = 1;
    freePriceValue[LOFreeGoldYouTube]       = 1;
    
    //Common
    currentPage = LOStore_MagicBox;
    
    scroll = new SunnyScrollView;
    scroll->setFrame(SunnyVector2D(mapZoomBounds.y - mapZoomBounds.x,0));
    scroll->setPagesCount(LOStore_Count);
    scroll->setScrollStoppedCallback(updateStorePage);
    offset = scroll->getOffsetXPointer();
    
    storeTime = 0;
    
    labelStore[LOStore_MagicBox] = getLocalizedLabel("Magic Gifts");
    labelStore[LOStore_GoldCoins] = getLocalizedLabel("Gold Coins");
    labelStore[LOStore_FreeCoins] = getLocalizedLabel("Free Stuff");
    
    float scale = 2;
    scrollButtons[0] = sunnyAddButton(SunnyVector2D(124*mapKoef-horizontalMapAdd,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    scrollButtons[1] = sunnyAddButton(SunnyVector2D(1796*mapKoef+horizontalMapAdd,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    
    for (short i = 0;i<2;i++)
    {
        scrollButtons[i]->matrix.pos.z = -10;
        sunnyButtonSetCallbackFunc(scrollButtons[i], storeButtonPressed);
        sunnyButtonSetRenderFunc(scrollButtons[i], storeEmptyRender);
    }
    
    LOStore::loadStore();
}

void LOStore::changePage(LOStorePage page)
{
    if (currentPage == page) return;
    
    //Hide
    switch (currentPage) {
        case LOStore_MagicBox:
        {
            for (short i = 0;i<LOMB_Count;i++)
            {
                magicBtn[i]->hidden = true;
                magicBtnIce[i]->hidden = true;
            }
            break;
        }
        
        case LOStore_GoldCoins:
        {
            for (short i = 0;i<LOGold_Count;i++)
                goldBtn[i]->hidden = true;
            break;
        }
        case LOStore_FreeCoins:
        {
            for (short i = 0;i<LOFreeGold_Count;i++)
                freeGoldBtn[i]->hidden = true;
            break;
        }
            
        default:
            break;
    }
    currentPage = page;
    //Show
    switch (currentPage) {
        case LOStore_MagicBox:
        {
            for (short i = 0;i<LOMB_Count;i++)
            {
                magicBtn[i]->hidden = false;
                magicBtnIce[i]->hidden = false;
            }
            break;
        }
        case LOStore_GoldCoins:
        {
            for (short i = 0;i<LOGold_Count;i++)
                goldBtn[i]->hidden = false;
            break;
        }
        case LOStore_FreeCoins:
        {
            for (short i = 0;i<LOFreeGold_Count;i++)
                freeGoldBtn[i]->hidden = false;
            break;
        }
        default:
            break;
    }
}

//void LOStore::setVisible(bool visible)
//{
//    if (isVisible == visible) return;
//    
//    isVisible = visible;
//    
//    if (isVisible)
//    {
//        changePage(currentPage);
//    } else
//    {
//        changePage(LOStore_None);
//    }
//}

void LOStore::renderOverlay()
{
    SunnyMatrix m = sunnyIdentityMatrix;
    SunnyVector4D color;
    float scale = 2;
    float alpha = 255*(0.7+0.3*sinf(storeTime));
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
    for (short i = 0;i<LOMB_Count;i++)
    {
        m.pos = magicBtn[i]->matrix.pos;
        m.pos.y = mapSize.y/2+2;
        m.pos.x -= *offset;
        m.pos.z = -1;
        if (i==0) color = SunnyVector4D(255,239,53,alpha)/255;
        else if (i==1) color = SunnyVector4D(57,232,255,alpha)/255;
        else color = SunnyVector4D(128,255,91,alpha)/255;
        //color.w = 0.2;
        glUniform4fv(uniformOverlayTSA_C, 1, color);
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
}

void LOStore::render()
{
    const float mapKoef = mapSize.x/1920;
    
    SunnyMatrix m;
    glDisable(GL_DEPTH_TEST);
    //Buttons
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    float moveA = 1;
    float scale = 1;
    scrollButtons[0]->active?scale = defaultButtonScale:scale = 1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
    if (!scroll->isStopped()) moveA = 0.5;
    //Arrows
    if (scroll->getOffsetX()>=(mapZoomBounds.y-mapZoomBounds.x))
        glUniform1f(uniformTSA_A, moveA);
    else
        glUniform1f(uniformTSA_A, scroll->getOffsetX()/(scroll->getFrame().x)*moveA);
    m = getTranslateMatrix(SunnyVector3D(124*mapKoef-horizontalMapAdd,(1280-640)*mapKoef,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOLSArrowVAO);

    scrollButtons[1]->active?scale = defaultButtonScale:scale = 1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
    if (scroll->getOffsetX()<=(scroll->getPagesCount()-2)*scroll->getFrame().x)
        glUniform1f(uniformTSA_A, moveA);
    else
        glUniform1f(uniformTSA_A, ((scroll->getPagesCount()-1)*scroll->getFrame().x - scroll->getOffsetX())/(scroll->getFrame().x)*moveA);
    m = getScaleMatrix(SunnyVector3D(-1,1,1)) * getTranslateMatrix(SunnyVector3D(1796*mapKoef+horizontalMapAdd,mapSize.y/2,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOLSArrowVAO);
    //Arrows...
    
    glUniform1f(uniformTSA_A, 1);
    m = sunnyIdentityMatrix;
    float btnScale;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    for (short i = 0;i<LOMB_Count;i++)
    {
        m.pos = magicBtnIce[i]->matrix.pos;
        m.pos.x -= *offset;
        m.pos.z = -20;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOMagicBoxMagic_VAO+i);
    }
    
    m = getScaleMatrix(6)*getRotationXMatrix(M_PI_4/4);
    SunnyMatrix m1;
    for (short i = 0;i<LOMB_Count;i++)
    {
        m.pos = magicBtnIce[i]->matrix.pos;
        m.pos.x -= *offset;
        m1 = getRotationYMatrix(-(i-1)*M_PI_4/4)*m;
        LOMagicBox::renderMagicBox(IceState_Solid, &m1);
    }
    
    glDisable(GL_DEPTH_TEST);
    //Buttons
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    m = getRotationZMatrix(M_PI_2);
    for (short i = 0;i<LOMB_Count;i++)
    {
        m.pos = magicBtn[i]->matrix.pos;
        m.pos.x -= *offset;
        magicBtn[i]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        
        if (magicBtnAnimationTime[i]>=0)
        {
            float time = storeTime - magicBtnAnimationTime[i];
            if (time*7>=2*M_PI)
                magicBtnAnimationTime[i] = -100;
            else
                btnScale += (1 - cosf(time*7))/2*0.1;
        }
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOIcePriceButton_VAO);
    }
    
    //BackButton
    
    btnScale = 1;
    if (backButton->active)
        btnScale *= 0.9;
    glUniform1f(uniformTSA_A, 1);
    m = sunnyIdentityMatrix;
    m.pos = backButton->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, 4*mapKoef, btnScale, btnScale));
    SunnyDrawArrays(LOLCButtonsVAO+6);
    
    //Prices
    scale = 0.79;
    m = getScaleMatrix(scale);
    for (short i = LOMB_Medium;i<=LOMB_Big;i++)
    {
        m.pos = magicBtn[i]->matrix.pos;
        m.pos.x -= *offset;
        if (LOScore::getMagicBoxCount((LOMagicBoxType)i))
            loRenderPriceLabel(&m, LOPT_Free, LOScore::getMagicBoxCount((LOMagicBoxType)i));
        else
            loRenderPriceLabel(&m, LOPT_GoldCoins, LOScore::getMagicBoxPrice((LOMagicBoxType)i));
    }
    
    m.pos = magicBtn[LOMB_Small]->matrix.pos;
    m.pos.x -= *offset;
    if (LOScore::getMagicBoxCount(LOMB_Small))
        loRenderPriceLabel(&m, LOPT_Free, LOScore::getMagicBoxCount(LOMB_Small));
    else
        loRenderPriceLabel(&m, LOPT_Stars, LOScore::getMagicBoxPrice(LOMB_Small));
    //Prices...
    
    
    //GoldCoins
    float goldOffset = - *offset + scroll->getFrame().x;
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    glUniform1f(uniformTSA_A, 1);
    m = sunnyIdentityMatrix;
    for (short i = 0;i<LOGold_Count;i++)
    {
        m.pos = goldBtn[i]->matrix.pos;
        m.pos.x += goldOffset;
        goldBtn[i]->active?btnScale = 0.95:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOStoreButton_VAO);
        
        if (i==LOGold_20)
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-6*mapKoef, -26*mapKoef, btnScale, btnScale));
        else
            if (i==LOGold_60)
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, -44*mapKoef, btnScale, btnScale));
            else
                if (i==LOGold_125)
                    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-10*mapKoef, 2*mapKoef, btnScale, btnScale));
                else
                    if (i==LOGold_260)
                        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, -2*mapKoef, btnScale, btnScale));
                    else
                        if (i==LOGold_700)
                            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, -14*mapKoef, btnScale, btnScale));

        SunnyDrawArrays(LOStoreButton_VAO + i + 1);
    }
    
    for (short i = 0;i<LOGold_Count;i++)
    {
        m = getScaleMatrix(0.79);
        m.pos = goldBtn[i]->matrix.pos;
        m.pos.y -= 2;
        m.pos.x += goldOffset;
        //loRenderPriceLabel(&m,productInfo[i]);
    }
    
    if (productInfo[0]->price<0.01)
    {
        float scale = 0.8;
        m = getScaleMatrix(scale)* getTranslateMatrix(SunnyVector3D(mapSize.x/2 + goldOffset,mapSize.y/2,-1));
        LOFont::writeText(&m, LOTA_Center, true, loLabelLoading, yellowInnerColor(1),yellowOuterColor(1));
        m.pos.x += LOFont::getTextSize(loLabelLoading)*scale/2;
        bindGameGlobal();
        int time = storeTime*3;
        time = time%4;
        if (time==1)
            LOFont::writeTime(&m, true, ".", yellowInnerColor(1),yellowOuterColor(1));
        else
            if (time==2)
                LOFont::writeTime(&m, true, "..", yellowInnerColor(1),yellowOuterColor(1));
        else
            if (time==3)
                LOFont::writeTime(&m, true, "...", yellowInnerColor(1),yellowOuterColor(1));
    } else
    {
        const float distUp = 3.1;
        const float distDown = -2.96;
        float scaleUp = 0.45;
        float scaleDown = 0.30;
        for (short i = 0;i<LOGold_Count;i++)
        {
            m = getScaleMatrix(scaleUp);
            m.pos = goldBtn[i]->matrix.pos;
            m.pos.x += goldOffset;
            m.pos.y += distUp;
            loRenderPriceLabel(&m, LOPT_GoldCoins, productInfo[i]->goldCount,1,LOTA_Center,scaleUp*1.2);
            
            m = getScaleMatrix(scaleDown);
            m.pos = goldBtn[i]->matrix.pos;
            m.pos.x += goldOffset;
            m.pos.y += distDown;
            LOFont::writePrice(&m, LOTA_Center, true, productInfo[i]->price, productInfo[i]->priceLocale,whiteInnerColor(1),whiteOuterColor(1));
        }
    }
    //GoldCoins...
    
    //Free coins
    goldOffset = - *offset + scroll->getFrame().x*2;
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, 1);
    m = sunnyIdentityMatrix;
    for (short i = 0;i<LOFreeGold_Count;i++)
    {
        m.pos = freeGoldBtn[i]->matrix.pos;
        m.pos.x += goldOffset;
        freeGoldBtn[i]->active?btnScale = 0.95:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOFreeStuff_VAO);
        
        switch (i) {
            case LOFreeGoldUnnyNet:
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-242*mapKoef, -18*mapKoef, btnScale*0.62, btnScale*0.62));
                SunnyDrawArrays(LOSocialButtonFB_VAO);
                break;
            case LOFreeGoldLikeUnnyhog:
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-260*mapKoef, 0*mapKoef, btnScale, btnScale));
                SunnyDrawArrays(LOFreeStuff_VAO+1);
                break;
            case LOFreeGoldTwitter:
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-260*mapKoef, -2*mapKoef, btnScale, btnScale));
                SunnyDrawArrays(LOFreeStuff_VAO+2);
                break;
            case LOFreeGoldRate:
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-262*mapKoef, 0*mapKoef, btnScale, btnScale));
                SunnyDrawArrays(LOFreeStuff_VAO+3);
                break;
            case LOFreeGoldLikeDodge:
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-260*mapKoef, -2*mapKoef, btnScale, btnScale));
                SunnyDrawArrays(LOFreeStuff_VAO+4);
                break;
            case LOFreeGoldYouTube:
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-262*mapKoef, -2*mapKoef, btnScale, btnScale));
                SunnyDrawArrays(LOFreeStuff_VAO+5);
                break;
                
            default:
                break;
        }
    }
    
    scale = 0.3;
    m = getScaleMatrix(scale);
    for (short i = 0;i<LOFreeGold_Count;i++)
    {
        m.pos = freeGoldBtn[i]->matrix.pos;
        m.pos.x += 76*mapKoef;
        m.pos.y += 44*mapKoef;
        m.pos.x += goldOffset;
        LOFont::writeText(&m, LOTA_Center, true, freeLabel[i], SunnyVector4D(206,234,255,255)/255, SunnyVector4D(142,172,183,255)/255);
    }
    
    scale = 0.45;
    m = getScaleMatrix(scale);
    for (short i = 0;i<LOFreeGold_Count;i++)
        if (!LOScore::isFreeGoldRecieved((LOFreeGold)i))
        {
            m.pos = freeGoldBtn[i]->matrix.pos;
            m.pos.x += 78*mapKoef;
            m.pos.y -= 36*mapKoef;
            m.pos.x += goldOffset;
            
            loRenderPriceLabel(&m, freePriceType[i], freePriceValue[i],1,LOTA_LeftCenter,scale*1.2);
            
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-158*mapKoef, 0, 1/scale, 1/scale));
            SunnyDrawArrays(LOFreeStuff_VAO+6);
        } else
        {
            m.pos = freeGoldBtn[i]->matrix.pos;
            m.pos.x += 78*mapKoef;
            m.pos.y -= 36*mapKoef;
            m.pos.x += goldOffset;
            
            bindGameGlobal();
            SHTextureBase::bindTexture(menuTexture);
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1/scale, 1/scale));
            SunnyDrawArrays(LOFreeStuff_VAO+7);
        }
    
    //Free coins...
    
    //Dots
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    for (short i=0;i<LOStore_Count;i++)
    {
        m = getRotationZMatrix(78./180*M_PI*i)* getTranslateMatrix(SunnyVector3D((863.+122*i)/1920*mapSize.x,(1280-1188)/1280.*mapSize.y,-1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (i==currentPage)
            glUniform1f(uniformTSA_A, 1);
        else
            glUniform1f(uniformTSA_A, 0.5);
        SunnyDrawArrays(LOLSDotVAO);
    }
    //Dots...
    
    m = getScaleMatrix(0.7) * getTranslateMatrix(SunnyVector3D(mapSize.x/2,(1280-68)*mapKoef + verticalMapAdd,-1));
    
    for (short i = 0; i<LOStore_Count; i++)
    {
        float off = i*scroll->getFrame().x - *offset;
        m1 = m * getTranslateMatrix(SunnyVector3D(off,0,0));
        if (i!=LOStore_GoldCoins)
            m1 = m1* getTranslateMatrix(SunnyVector3D(0,-1,0));
        LOFont::writeText(&m1, LOTA_Center, true, labelStore[i],SunnyVector4D(255,172,42,255)/255,SunnyVector4D(241,241,241,255)/255);
    }
    
    //My Gold
    scale = 0.8;
    float newScale = scale;
    if (goldCoinsAnimated)
    {
        newScale += (1 - cosf(goldCoinsAnimationTime*7))/2*0.1;
        if (goldCoinsAnimationTime*7>=2*M_PI)
            goldCoinsAnimated = false;
    }
    m = getScaleMatrix(newScale) * getTranslateMatrix(SunnyVector3D(mapZoomBounds.y-2,2-verticalMapAdd,-1));
    loRenderPriceLabel(&m, LOPT_GoldCoins, (int)LOScore::getTotalCoins(),1,LOTA_RightCenter,newScale/scale);
}

void LOStore::update(float deltaTime)
{
    scroll->update(deltaTime);
    storeTime+=deltaTime;
    
    if (scroll->getTotalMoveByTouch()>0.4)
    {
        for (short i = 0;i<LOMB_Count;i++)
        {
            magicBtn[i]->active = false;
            magicBtnIce[i]->active = false;
        }
        for (short i = 0;i<LOGold_Count;i++)
            goldBtn[i]->active = false;
        for (short i = 0;i<LOFreeGold_Count;i++)
            freeGoldBtn[i]->active = false;
    }
    
    if (goldCoinsAnimated)
    {
        if (!goldSoundPlayed)
        {
            losPlayGoldCoinSound();
            goldSoundPlayed = true;
        }
        goldCoinsAnimationTime += deltaTime;
    }
}

void LOStore::facebookLoginSucceded(const char * playerId)
{
    LOScore::giveFreeGold(LOFreeGoldUnnyNet, freePriceType[LOFreeGoldUnnyNet], freePriceValue[LOFreeGoldUnnyNet]);
}

void LOStore::hideWindow()
{
    buttonPressed(backButton);
}

void LOStore::buttonPressed(SunnyButton * btn)
{
    if (scrollButtons[0] == btn)
    {
        scroll->moveToTheLeft();
        return;
    }
    if (scrollButtons[1] == btn)
    {
        scroll->moveToTheRight();
        return;
    }
    
    if (backButton == btn)
    {
        sharedStore = 0;
        switch (storeBack) {
            case StoreBack_MainMenu:
                LOMenu::getSharedMenu()->showMainMenu(true);
                break;
            case StoreBackPause:
                LOMenu::getSharedMenu()->showGamePlayButtons(true);
                break;
            case StoreBackLevelComplete:
                LOMenu::getSharedMenu()->getLevelCompleteWindow()->isVisible = true;
                break;
            case StoreBack_Survival:
                LOMenu::getSharedMenu()->showSurvivalButtons(true);
                break;
            case StoreBackDeathWindow:
                {
                    LOMenu::getSharedMenu()->getDeathWindow()->isVisible = true;
                    if (isSurvival)
                        LOTasksView::getSharedView()->isVisible = true;
                }
                break;
                
            default:
                break;
        }
        LOMenu::getSharedMenu()->showStoreMenu(false,StoreBack_None);
        return;
    } else
        ///ICE
        if (magicBtnIce[LOMB_Small] == btn)
        {
            if (LOScore::getMagicBoxCount(LOMB_Small))
                LOMenu::getSharedMenu()->showMagicBox(LOMB_Small);
            else
            {
                magicBtnAnimationTime[LOMB_Small] = storeTime;
                LOMenu::getSharedMenu()->showMagicDropInfo(LOMB_Small);
            }
        } else
            if (magicBtnIce[LOMB_Medium] == btn)
            {
                if (LOScore::getMagicBoxCount(LOMB_Medium))
                    LOMenu::getSharedMenu()->showMagicBox(LOMB_Medium);
                else
                {
                    magicBtnAnimationTime[LOMB_Medium] = storeTime;
                    LOMenu::getSharedMenu()->showMagicDropInfo(LOMB_Medium);
                }
            } else
                if (magicBtnIce[LOMB_Big] == btn)
                {
                    if (LOScore::getMagicBoxCount(LOMB_Big))
                        LOMenu::getSharedMenu()->showMagicBox(LOMB_Big);
                    else
                    {
                        magicBtnAnimationTime[LOMB_Big] = storeTime;
                        LOMenu::getSharedMenu()->showMagicDropInfo(LOMB_Big);
                    }
                } else
        //ICE...
        if (magicBtn[LOMB_Small] == btn)
        {
            //LOScore::addMagicBox(LOMB_Small);
            if (LOScore::getMagicBoxCount(LOMB_Small))
            {
                LOMenu::getSharedMenu()->showMagicBox(LOMB_Small);
            } else
            {
                LOMenu::getSharedMenu()->show5StarsHint();
            }
        } else
        if (magicBtn[LOMB_Medium] == btn)
        {
            if (LOScore::getMagicBoxCount(LOMB_Medium))
            {
                LOMenu::getSharedMenu()->showMagicBox(LOMB_Medium);
            } else
            {
                if (LOScore::buyMagicBox(LOMB_Medium))
                {
                    LOMenu::getSharedMenu()->showMagicBox(LOMB_Medium);
                } else
                {
                    LOMenu::getSharedMenu()->showNotEnoughGoldHint();
                }
            }
        } else
            if (magicBtn[LOMB_Big] == btn)
            {
                //LOScore::addMagicBox(LOMB_Big);
                if (LOScore::getMagicBoxCount(LOMB_Big))
                {
                    LOMenu::getSharedMenu()->showMagicBox(LOMB_Big);
                } else
                {
                    if (LOScore::buyMagicBox(LOMB_Big))
                    {
                        LOMenu::getSharedMenu()->showMagicBox(LOMB_Big);
                    } else
                    {
                        LOMenu::getSharedMenu()->showNotEnoughGoldHint();
                    }
                }
            } else
            {
                for (short i = 0;i<LOGold_Count;i++)
                    if (goldBtn[i] == btn)
                    {
                        if (productInfo[goldBtn[i]->tag]->price>=0.01)
                            UnnyNet::buyInapp(productInfo[goldBtn[i]->tag]->productID);
                        else
                        {
                            //Not loaded
                        }
                        return;
                    }
                
                if (btn == freeGoldBtn[LOFreeGoldUnnyNet])
                {
                    loMakeFBConnection(storeFBLoggenId);
                } else
                if (btn == freeGoldBtn[LOFreeGoldRate])
                {
                    loRateTheGame();
                    LOScore::giveFreeGold(LOFreeGoldRate, freePriceType[LOFreeGoldRate], freePriceValue[LOFreeGoldRate]);
                } else
                    if (btn == freeGoldBtn[LOFreeGoldTwitter])
                    {
                        loOpenTWFunPage(social_dodge_tw,social_dodge_tw_id);
                        LOScore::giveFreeGold(LOFreeGoldTwitter, freePriceType[LOFreeGoldTwitter], freePriceValue[LOFreeGoldTwitter]);
                    } else
                        if (btn == freeGoldBtn[LOFreeGoldYouTube])
                        {
                            loOpenUTFunPage(social_unnyhog_youtube,social_unnyhog_youtube_id);
                            LOScore::giveFreeGold(LOFreeGoldYouTube, freePriceType[LOFreeGoldYouTube], freePriceValue[LOFreeGoldYouTube]);
                        } else
                            if (btn == freeGoldBtn[LOFreeGoldLikeDodge])
                            {
                                loOpenFBFunPage(social_dodge_fb,social_dodge_fb_id);
                                LOScore::giveFreeGold(LOFreeGoldLikeDodge, freePriceType[LOFreeGoldLikeDodge], freePriceValue[LOFreeGoldLikeDodge]);
                            } else
                                if (btn == freeGoldBtn[LOFreeGoldLikeUnnyhog])
                                {
                                    loOpenFBFunPage(social_unnyhog_fb,social_unnyhog_fb_id);
                                    LOScore::giveFreeGold(LOFreeGoldLikeUnnyhog, freePriceType[LOFreeGoldLikeUnnyhog], freePriceValue[LOFreeGoldLikeUnnyhog]);
                                }
            }
}

LOStore::~LOStore()
{
    menuMode = oldMenuMode;
    sunnyRemoveButton(lockButton);
    sunnyRemoveButton(backButton);
    for (short i = 0;i<LOMB_Count;i++)
    {
        sunnyRemoveButton(magicBtn[i]);
        sunnyRemoveButton(magicBtnIce[i]);
    }
    for (short i = 0;i<LOGold_Count;i++)
        sunnyRemoveButton(goldBtn[i]);
    
    delete scroll;
    
    for (short i = 0;i<LOFreeGold_Count;i++)
    {
        sunnyRemoveButton(freeGoldBtn[i]);
        delete [] freeLabel[i];
    }
    
    for (short i = 0;i<LOStore_Count;i++)
        delete []labelStore[i];
    
    for (short i = 0;i<2;i++)
        sunnyRemoveButton(scrollButtons[i]);
}

