//
//  LOGroundModel.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 15.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOGroundModel.h"

void LOGroundModel::prepareBlocks()
{
    for (short i = 0;i<objectsCount && i<10;i++)
    {
        if (strstr(objects[i]->objectName, "Standart_Block"))
        {
            standartBlock = i;
        } else
            if (strstr(objects[i]->objectName, "Round_S") && strlen(objects[i]->objectName)<=strlen("Round_S") + 4)
            {
                char str[5];
                short n = 0;
                for (short j = strlen("Round_S")+1; j<strlen(objects[i]->objectName) && n<4; j++)
                {
                    str[n++] = objects[i]->objectName[j];
                }
                str[n] = '\0';
                n = atoi(str);
                if (n<=4)
                    roundSBlock[n-1] = i;
                else
                    printf("\n ERROR : wrong file name %s",objects[i]->objectName);
            } else
                if (strstr(objects[i]->objectName, "Round_S_Angle") && strlen(objects[i]->objectName)<=strlen("Round_S_Angle") + 4)
                {
                    char str[5];
                    short n = 0;
                    for (short j = strlen("Round_S_Angle")+1; j<strlen(objects[i]->objectName) && n<4; j++)
                    {
                        str[n++] = objects[i]->objectName[j];
                    }
                    str[n] = '\0';
                    n = atoi(str);
                    if (n<=4)
                        roundSAngleBlock[n-1] = i;
                    else
                        printf("\n ERROR : wrong file name %s",objects[i]->objectName);
                }
    }
}

void LOGroundModel::beginRender()
{
    if (isVAOSupported)
        glBindVertexArray(verticesVAO);
    else
        bindVBO(true);
    lastFaceVBO = 0;
}

void LOGroundModel::renderObject(short num)
{
    if (lastFaceVBO != objects[num]->faceVBO)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[num]->faceVBO);
        lastFaceVBO = objects[num]->faceVBO;
    }
    glDrawElements(GL_TRIANGLES, objects[num]->polygons_qty*3, GL_UNSIGNED_SHORT, NULL);
}

void LOGroundModel::endRender()
{
#ifndef VAO_NOT_SUPPORTED
    glBindVertexArray(0);
#else
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}