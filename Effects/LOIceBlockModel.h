//
//  LOIceBlockModel.h
//  LookOut
//
//  Created by Pavel Ignatov on 25.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOIceBlockModel_h
#define LookOut_LOIceBlockModel_h
#include "SunnyModelObj.h"

class LOIceBlockModel: public SunnyModelObj
{
    
public:
    void prepareForIceBlock();
    void renderWithMatrices(SunnyMatrix *MV, SunnyMatrix * insideMat);
};

#endif
