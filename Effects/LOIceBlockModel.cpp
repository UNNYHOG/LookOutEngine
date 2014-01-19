//
//  LOIceBlockModel.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 25.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOIceBlockModel.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"

void LOIceBlockModel::prepareForIceBlock()
{
    if (objectsCount==2)
    {
        if (strcmp(objects[0]->objectName, "Inside"))
        {
            ObjectObj * obj = objects[0];
            objects[0] = objects[1];
            objects[1] = obj;
        }
    }
}

void LOIceBlockModel::renderWithMatrices(SunnyMatrix *MV,SunnyMatrix * insideMat)
{
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x, globalColor.y, globalColor.z, 0.9));

    static const float scale = cellSize*1.5;
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[1]->faceVBO);
    SunnyMatrix m = *MV;
    m.front *= scale; m.up *= scale; m.right *= scale;
    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
    glDrawElements(GL_TRIANGLES, objects[1]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    m = *insideMat * *MV;
    m.front *= scale; m.up *= scale; m.right *= scale;
    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[0]->faceVBO);
    glDrawElements(GL_TRIANGLES, objects[0]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

