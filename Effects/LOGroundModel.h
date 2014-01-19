//
//  LOGroundModel.h
//  LookOut
//
//  Created by Pavel Ignatov on 15.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOGroundModel_h
#define LookOut_LOGroundModel_h
#include "SunnyModelObj.h"
#include "LOMapInfo.h"
#include "LOGlobalVar.h"

class LOGroundModel : public SunnyModelObj
{
    GLuint lastFaceVBO;
public:
    short standartBlock;
    short roundSBlock[4];
    short roundSAngleBlock[4];
    
//    LOGroundModel();
//    ~LOGroundModel();
    
    void prepareBlocks();
    void beginRender();
    void renderObject(short num);
    void endRender();
};

#endif
