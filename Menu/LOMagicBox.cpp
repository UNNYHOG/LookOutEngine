//
//  LOMagicBox.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 08.09.13.
//
//

#include "LOMagicBox.h"
#include "iPhone_Strings.h"
#include "LOPlayer.h"
#include "SunnyModelObj.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

LODrop * lastDrop = 0;

SunnyModelObj *mbIceModels[IceState_Count];

SunnyMatrix * crashedIceMatrices;

void addDrop(LODropType type,short type2, short count)
{
    lastDrop->drop[lastDrop->dropCount].dropType = type;
    lastDrop->drop[lastDrop->dropCount].additionalType = type2;
    lastDrop->drop[lastDrop->dropCount].ammount = count;
    lastDrop->dropCount++;
    
    switch (type) {
        case LODrop_VS:
            LOScore::addSnowflake(count);
            break;
        case LODrop_Spell:
            LOScore::addAbilityCharges((LevelAbility)type2, count);
            break;
            
        default:
            break;
    }
}

void LOMagicBox::prepareModels()
{
    mbIceModels[IceState_Solid] = new SunnyModelObj;
    mbIceModels[IceState_Solid]->loadFromSobjFile(getPath("Base/Objects/ice_solid", "sh3do"));
    mbIceModels[IceState_Solid]->makeVBO(true, isVAOSupported, false);
    
    mbIceModels[IceState_Cracked] = new SunnyModelObj;
    mbIceModels[IceState_Cracked]->loadFromSobjFile(getPath("Base/Objects/ice_crack_small", "sh3do"));
    mbIceModels[IceState_Cracked]->makeVBO(true, isVAOSupported, false);
    
    mbIceModels[IceState_CrackedBig] = new SunnyModelObj;
    mbIceModels[IceState_CrackedBig]->loadFromSobjFile(getPath("Base/Objects/ice_crack_big", "sh3do"));
    mbIceModels[IceState_CrackedBig]->makeVBO(true, isVAOSupported, false);
    
    mbIceModels[IceState_Destroyed] = new SunnyModelObj;
    mbIceModels[IceState_Destroyed]->loadFromSobjFile(getPath("Base/Objects/ice_crashedobj", "sh3do"));
    mbIceModels[IceState_Destroyed]->makeVBO(true, isVAOSupported, false);
    
    ObjectObj ** objects = mbIceModels[IceState_Destroyed]->getObjects();
    crashedIceMatrices = new SunnyMatrix[mbIceModels[IceState_Destroyed]->objectsCount];
    for (short i = 0;i<mbIceModels[IceState_Destroyed]->objectsCount;i++)
    {
        crashedIceMatrices[i] = sunnyIdentityMatrix;
        crashedIceMatrices[i].pos.x = (objects[i]->maxSize.x + objects[i]->minSize.x)/2;
        crashedIceMatrices[i].pos.y = (objects[i]->maxSize.y + objects[i]->minSize.y)/2;
        crashedIceMatrices[i].pos.z = (objects[i]->maxSize.z + objects[i]->minSize.z)/2;
    }
}

short LOMagicBox::getDestroyedObjectsCount()
{
    return mbIceModels[IceState_Destroyed]->objectsCount;
}

SunnyMatrix * LOMagicBox::getDestroyedMatrices()
{
    return crashedIceMatrices;
}

void LOMagicBox::clearModels()
{
    for (short i = 0;i<IceState_Count;i++)
        delete mbIceModels[i];
    delete []crashedIceMatrices;
}

void LOMagicBox::renderMagicBox(LOIceState state, SunnyMatrix * matrix)
{
    glEnable(GL_DEPTH_TEST);
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    glUniform4fv(uniform3D_A, 1, SunnyVector4D(1.0, 1.0, 1.0, 0.6));
    SHTextureBase::bindTexture(objectsTexture);
    glEnable(GL_BLEND);
    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(matrix->front.x));
    mbIceModels[state]->render(true, isVAOSupported);
}

void LOMagicBox::renderCrashedMagicBox(SunnyMatrix * matrices,SunnyMatrix * matrix)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    glUniform4fv(uniform3D_A, 1, SunnyVector4D(1.0, 1.0, 1.0, 0.6));
    SHTextureBase::bindTexture(objectsTexture);
    mbIceModels[IceState_Destroyed]->prepareForRender(isVAOSupported);
    SunnyMatrix m;
    for (short i = 0;i<mbIceModels[IceState_Destroyed]->objectsCount;i++)
    {
        m = crashedIceMatrices[i].InverseMatrix() * matrices[i] * *matrix;
        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
        mbIceModels[IceState_Destroyed]->renderObject(i);
    }
}

void LOMagicBox::openMagicBox(LOMagicBoxType type)
{
    if (!lastDrop)
        lastDrop = new LODrop;
    lastDrop->dropCount = 0;
    switch (type) {
        case LOMB_Small:
        {
            //printf("\n ==> Small MagicBox");
            //int vsCount = 50 + 10*(sRandomi()%11);
            //addDrop(LODrop_VS,0,vsCount);
            //printf("\n 1) Drop %d VS",vsCount);
            bool spellTaken[4] = {false,false,false,false};
            short availableCount = 0;
            for (short i = 0;i<4;i++)
            {
                spellTaken[i] = !LOScore::isSpellEnabled((LevelAbility)i);
                if (!spellTaken[i])
                    availableCount++;
            }
            
            if (availableCount)
            {
                short spellType = (sRandomi()%availableCount);
                availableCount = 0;
                for (short j = 0;j<4;j++)
                {
                    if (!spellTaken[j])
                    {
                        if (availableCount==spellType)
                        {
                            spellType = j;
                            break;
                        } else
                            availableCount++;
                    }
                }
                
                short spellCount = 2 + sRandomi()%4;
                addDrop(LODrop_Spell,spellType,spellCount);
            }
            //printf("\n 2) Drop spell %d : %d",spellType,spellCount);
            
            
            bool abilityTaken[3] = {false,false,false};
            availableCount = 0;
            for (short i = 0;i<3;i++)
            {
                abilityTaken[i] = !LOScore::isSpellEnabled((LevelAbility)(LA_Doubler+i));
                if (!abilityTaken[i])
                    availableCount++;
            }

            if (availableCount!=0)
            {
                short abilityType = sRandomi()%availableCount;
                
                availableCount = 0;
                for (short j = 0;j<3;j++)
                {
                    if (!abilityTaken[j])
                    {
                        if (availableCount==abilityType)
                        {
                            abilityType = LA_Doubler + j;
                            break;
                        } else
                            availableCount++;
                    }
                }
                
                short abilityCount = 1;
                addDrop(LODrop_Spell, abilityType, abilityCount);
                //printf("\n 3) Drop ability %d : %d",abilityType,abilityCount);
            }
            break;
        }
        
        case LOMB_Medium:
        {
            //printf("\n ==> Medium MagicBox");
//            int vsCount = 300 + 50*(sRandomi()%5);
            //printf("\n 1) Drop %d VS",vsCount);
//            addDrop(LODrop_VS,0,vsCount);
            bool spellTaken[4] = {false,false,false,false};
            short spell = 1 + sRandomi()%2;
            for (short i = 0; i<spell; i++)
            {
                short spellType = (sRandomi()%(4-i));
                for (short j = 0;j<4;j++)
                {
                    if (spellTaken[j])
                        spellType++;
                    else
                    {
                        if (spellType == j)
                            break;
                    }
                }
                short spellCount = 5 + sRandomi()%6;
                if (spell!=1)
                    spellCount /= (spell - 0.5);
                //printf("\n 2) Drop spell %d : %d",spellType,spellCount);
                addDrop(LODrop_Spell,spellType,spellCount);
                spellTaken[spellType] = true;
            }
            
            short abilityType = (LA_Doubler + sRandomi()%3);
            short abilityCount = 2 + sRandomi()%2;
           // printf("\n 3) Drop ability %d : %d",abilityType,abilityCount);
            addDrop(LODrop_Spell, abilityType, abilityCount);
            if (sRandomf()<0.02)
            {
                //printf("\n 4) Drop Puzzle Part");
                addDrop(LODrop_Puzzle, 0, 1);
            }
            break;
        }
            
        case LOMB_Big:
        {
            //printf("\n ==> Big MagicBox");
//            int vsCount = 1500 + 100*(sRandomi()%11);
            //printf("\n 1) Drop %d VS",vsCount);
//            addDrop(LODrop_VS,0,vsCount);
            
            bool spellTaken[4] = {false,false,false,false};
            short spell = 2 + sRandomi()%3;
            for (short i = 0; i<spell; i++)
            {
                int spellType = (sRandomi()%(4-i));
                for (short j = 0;j<4;j++)
                {
                    if (spellTaken[j])
                        spellType++;
                    else
                    {
                        if (spellType == j)
                            break;
                    }
                }
                short spellCount = 30 + sRandomi()%21;
                if (spell!=1)
                    spellCount /= (spell - 0.5);
                //printf("\n 2) Drop spell %d : %d",spellType,spellCount);
                addDrop(LODrop_Spell,spellType,spellCount);
                spellTaken[spellType] = true;
            }
            
            bool abilityTaken[3] = {false,false,false};
            short ability = 2;
            for (short i = 0; i<ability; i++)
            {
                int abilityType = (sRandomi()%(3-i));
                for (short j = 0;j<3;j++)
                {
                    if (abilityTaken[j])
                        abilityType = (abilityType+1)%3;
                    else
                    {
                        if (abilityType == j)
                            break;
                    }
                }
                abilityTaken[abilityType] = true;
                
                short abilityCount = 4 + sRandomi()%4;
                abilityType += LA_Doubler;
                //printf("\n 3) Drop ability %d : %d",abilityType,abilityCount);
                addDrop(LODrop_Spell, abilityType, abilityCount);
            }
            
            if (sRandomf()<0.1)
            {
                //printf("\n 4) Drop Puzzle Part");
                addDrop(LODrop_Puzzle, 0, 1);
            }
            break;
        }
            
        default:
            break;
    }
}

LODrop* LOMagicBox::getLastDrop()
{
    return lastDrop;
}