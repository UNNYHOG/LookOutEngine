//
//  LOCrashModel.h
//  LookOut
//
//  Created by Pavel Ignatov on 09.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOCrashModel_h
#define LookOut_LOCrashModel_h

#include "SunnyModelObj.h"

class LOCrashModel : public SunnyModelObj
{
    SunnyMatrix *matrices;
    SunnyMatrix * curMatrices;
    SunnyVector3D *velocity;
    float *scale;
    bool bonfireCrash;
public:
    LOCrashModel();
    ~LOCrashModel();
    
    void prepareModels();
    void renderForMenu(SunnyMatrix *MV);
    void render(SunnyMatrix *MV);
    void renderShadows();
    void update();
    void updateForMenu();
    void prepareToCrashForMenu(SunnyMatrix mat);
    void prepareToCrash(SunnyMatrix mat, SunnyVector2D direction);
    void prepareToCrashFromBonfire(SunnyMatrix mat, SunnyVector2D direction);
    void renderOriginal(SunnyMatrix *MV);
    void renderShadowsByMatrices(SunnyMatrix * matrices);
    void renderByMatrices(SunnyMatrix *MV, SunnyMatrix * matrices, float * angles);
    void getMatricesForCrash(SunnyMatrix matrices[]);
    void prepareForRender();
};

#endif