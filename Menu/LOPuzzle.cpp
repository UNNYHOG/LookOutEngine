//
//  LOPuzzle.cpp
//  LookOut
//
//  Created by Ignatov on 27.08.13.
//
//

#include "LOPuzzle.h"
#include "SHTextureBase.h"
#include "iPhone_Strings.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"

const float puzzlePartSizeX = 0.021625*puzzlePartsScale*2;
const float puzzlePartSizeY = 0.015221*puzzlePartsScale*2;
const float puzzleFallHeight = 2;

int puzzleTextureID = -1;
SunnyModelObj * puzzleObject = 0;
short objectsNumbers[puzzleHorizontalSize*puzzleVerticalSize];
SunnyVector3D partsTranslations[puzzleHorizontalSize*puzzleVerticalSize];

LOPuzzle * sharedPuzzle = 0;

void storeEmptyRender(SunnyButton * btn);

void puzzleButtonCallback(SunnyButton * btn)
{
    if (sharedPuzzle)
        sharedPuzzle->buttonPressed(btn);
}

void LOPuzzle::buttonPressed(SunnyButton * btn)
{
    if (btn == puzzleButtons[PuzzleButton_Back])
    {
        LOMenu::getSharedMenu()->showPuzzleMenu(false);
    } else
        if (btn == puzzleButtons[PuzzleButton_Break])
        {
            for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
            {
                
                if (puzzleParts[i].active)
                {
                    puzzleParts[i].position = SunnyVector2D(4+sRandomf()*22, 3 + sRandomf()*14);
                    puzzleParts[i].rotation = sRandomi()%4;
                }
            
                for (short j = 0;j<4;j++)
                    connection[i][j] = -1;
            }
        }
}

void LOPuzzle::loadTexture()
{
    if (puzzleTextureID>=0) return;
    
    SHTextureBase::setAlphaFix(false);
    puzzleTextureID = SHTextureBase::loadTexture("Base/GameTextures/PuzzleImage.png");
    SHTextureBase::setAlphaFix(true);
}

void LOPuzzle::prepareForRenderIfNecessary()
{
    if (puzzleObject!=0) return;
    
    puzzleObject = new SunnyModelObj;
    puzzleObject->loadFromSobjFile(getPath("Base/Objects/Puzzle","sh3do"));
    puzzleObject->makeVBO(true, isVAOSupported, false);
    
    for (short i = 1; i<=puzzleObject->objectsCount; i++)
    {
        //Part.001
        char name[4];
        name[0] = '0';
        name[1] = '0' + i/10;
        name[2] = '0' + i%10;
        name[3] = '\0';
        short num = puzzleObject->getObjectNumBySubName(name);
        objectsNumbers[i-1] = num;
    }
    ObjectObj ** objects = puzzleObject->getObjects();
    SunnyVector2D max(-99999,-99999),min(99999,99999);
    for (short i = 0; i<puzzleObject->objectsCount; i++)
    {
        if (max.x<objects[i]->maxSize.x) max.x = objects[i]->maxSize.x;
        if (max.y<objects[i]->maxSize.z) max.y = objects[i]->maxSize.z;

    
        if (min.x>objects[i]->minSize.x) min.x = objects[i]->minSize.x;
        if (min.y>objects[i]->minSize.z) min.y = objects[i]->minSize.z;
    }
    SunnyVector2D fullSize(max.x-min.x,max.y-min.y);
    fullSize.x/=puzzleHorizontalSize;
    fullSize.y/=puzzleVerticalSize;
    
    for (short i = 0; i<puzzleObject->objectsCount; i++)
    {
        SunnyVector2D pos = SunnyVector2D(min.x + fullSize.x*(i%5 + 0.5), min.y + fullSize.y*(i/5 + 0.5));
        partsTranslations[i] = SunnyVector3D(-pos.x,0,-pos.y);
    }
}

LOPuzzle::LOPuzzle()
{
    sharedPuzzle = this;
    playDropSound = false;
    playConnectSound = false;
    
    selectedPartsCount = 0;
    selectedParts = 0;
    connection = new short*[puzzleHorizontalSize*puzzleVerticalSize];
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
    {
        connection[i] = new short[4];
        for (short j = 0;j<4;j++)
            connection[i][j] = -1;
    }
    
    float btnSize = 340/2*0.82;
    const float buttonsY = (1280-1130)/1280.*mapSize.y-verticalMapAdd;
    puzzleButtons[PuzzleButton_Back] = sunnyAddButton(SunnyVector2D(150/1920.*mapSize.x-horizontalMapAdd,buttonsY), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
//    puzzleButtons[PuzzleButton_Store] = sunnyAddButton(SunnyVector2D(mapSize.x/2,buttonsY), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    puzzleButtons[PuzzleButton_Break] = sunnyAddButton(SunnyVector2D(1770/1920.*mapSize.x+horizontalMapAdd,buttonsY), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    
    for (short i = 0;i<PuzzleButton_Count;i++)
    {
        sunnyButtonSetCallbackFunc(puzzleButtons[i], puzzleButtonCallback);
        sunnyButtonSetRenderFunc(puzzleButtons[i], storeEmptyRender);
        puzzleButtons[i]->matrix.pos.z = -2;
    }
}

void LOPuzzle::removeData()
{
    if (!puzzleObject || sharedPuzzle) return;
    
    delete puzzleObject;
    puzzleObject = 0;
}

LOPuzzle::~LOPuzzle()
{
    sharedPuzzle = 0;
    
    LOScore::saveScores();

    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
        delete []connection[i];
    delete []connection;
    
    for (short i = 0;i<PuzzleButton_Count;i++)
        sunnyRemoveButton(puzzleButtons[i]);
}

void LOPuzzle::applyPuzzleData(LOP_OnePart *parts)
{
    puzzleParts = parts;
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
    {
        renderOrder[puzzleParts[i].order] = i;
        rotatingParts[i].isRotating = false;
        if (puzzleParts[i].position.x<0)
        {
            puzzleParts[i].position = SunnyVector2D(4+sRandomf()*22, 3 + sRandomf()*14);
//            puzzleParts[i].position = SunnyVector2D(15,10);
            puzzleHeight[i] = puzzleFallHeight;
            puzzleParts[i].rotation = sRandomi()%4;
        } else
            puzzleHeight[i] = 1;
        //temp
//        parts[i].active = true;
//        parts[i].position = SunnyVector2D(4+sRandomf()*22, 3 + sRandomf()*14);
        //temp...
    }
    
    short count = 0;
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
        if (puzzleParts[i].active)
            count++;
    if (count<=1)
        puzzleButtons[PuzzleButton_Break]->hidden = true;
    
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
    {
        checkTheConnection(i);
    }
    
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
    if (puzzleHeight[i]>1.5)
    {
        char buf = renderOrder[i];
        for (int j = i;j>0;j--)
            renderOrder[j]=renderOrder[j-1];
        renderOrder[0] = buf;
    }
    
    LOScore::saveScores();
}

static const float ppRotationTime = 0.3;

void LOPuzzle::update()
{
    prepareForRenderIfNecessary();
    
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
        if (rotatingParts[i].isRotating)
        {
            rotatingParts[i].time += deltaTime;
            if (rotatingParts[i].time>=ppRotationTime)
                rotatingParts[i].isRotating = false;
        }
    
    if (playDropSound)
    {
        losPlayPuzzleDropSound();
        playDropSound = false;
    }
    if (playConnectSound)
    {
        losPlayPuzzleConnectedSound();
        playConnectSound = false;
    }
}

void LOPuzzle::render()
{
    if (!puzzleTextureID<0 || puzzleObject==0) return;
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    SHTextureBase::bindTexture(puzzleTextureID);
    SunnyMatrix m1 = getScaleMatrix(puzzlePartsScale) *  getRotationXMatrix(M_PI_2);
    SunnyMatrix m;
    puzzleObject->prepareForRender(isVAOSupported);
    
    sunnyUseShader(globalShaders[LOS_Textured3D]);
    glUniform4fv(uniform3D_C, 1, SunnyVector4D(1,1,1,1));

    float depth = 0;
    float shadowD = 0;
    for (short i = puzzleHorizontalSize*puzzleVerticalSize-1;i>=0;i--)
    if (puzzleParts[renderOrder[i]].active)
    {
        if (rotatingParts[renderOrder[i]].isRotating)
        {
            float angle = (- 1 + rotatingParts[renderOrder[i]].time/ppRotationTime)*M_PI_2;
            SunnyVector2D distance = puzzleParts[renderOrder[i]].position - rotatingParts[renderOrder[i]].origin;
            SunnyVector2D pos;
            pos.x = distance.x*cosf(angle) + distance.y*sinf(angle);
            pos.y =-distance.x*sinf(angle) + distance.y*cosf(angle);
            pos += rotatingParts[renderOrder[i]].origin;
            angle = (puzzleParts[renderOrder[i]].rotation - 1 + rotatingParts[renderOrder[i]].time/ppRotationTime)*M_PI_2;
            m = getRotationZMatrix(-angle) * getTranslateMatrix(SunnyVector3D(pos.x,pos.y,-5));
        } else
        {
            m = getRotationZMatrix(-puzzleParts[renderOrder[i]].rotation*M_PI_2) * getTranslateMatrix(SunnyVector3D(puzzleParts[renderOrder[i]].position.x,puzzleParts[renderOrder[i]].position.y,-200+depth));
        }
        if (puzzleHeight[renderOrder[i]]>1)
        {
            m =  getScaleMatrix(puzzleHeight[renderOrder[i]]) * getRotationXMatrix(-M_PI*4*(puzzleHeight[renderOrder[i]]-1))*  m;
            puzzleHeight[renderOrder[i]] -= deltaTime;
            if (puzzleHeight[renderOrder[i]]<1)
                puzzleHeight[renderOrder[i]] = 1;
        }
        m = getTranslateMatrix(partsTranslations[renderOrder[i]]) * m1*m;
        glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
        puzzleObject->renderObject(objectsNumbers[renderOrder[i]]);
        
        if (i+1 == selectedPartsCount)
            shadowD = depth;

        depth += 5;
    }
    
    if (selectedParts)
    {
        glEnable(GL_BLEND);
        SunnyVector3D shadowSize(-0.15,-0.15,-1);
        sunnyUseShader(globalShaders[LOS_Textured3DA]);
        glUniform4fv(uniform3D_A, 1, SunnyVector4D(0,0,0,0.3));
        for (short i = 0;i<selectedPartsCount;i++)
        {
            if (rotatingParts[selectedParts[i]].isRotating)
            {
                float angle = (- 1 + rotatingParts[selectedParts[i]].time/ppRotationTime)*M_PI_2;
                SunnyVector2D distance = puzzleParts[selectedParts[i]].position - rotatingParts[selectedParts[i]].origin;
                SunnyVector2D pos;
                pos.x = distance.x*cosf(angle) + distance.y*sinf(angle);
                pos.y =-distance.x*sinf(angle) + distance.y*cosf(angle);
                pos += rotatingParts[selectedParts[i]].origin;
                angle = (puzzleParts[selectedParts[i]].rotation - 1 + rotatingParts[selectedParts[i]].time/ppRotationTime)*M_PI_2;
                m = getRotationZMatrix(-angle) * getTranslateMatrix(SunnyVector3D(pos.x,pos.y,-5));
            } else
                m = getRotationZMatrix(-puzzleParts[selectedParts[i]].rotation*M_PI_2) * getTranslateMatrix(SunnyVector3D(puzzleParts[selectedParts[i]].position.x+shadowSize.x,puzzleParts[selectedParts[i]].position.y+shadowSize.y,-200+shadowSize.z+shadowD));
            m = getTranslateMatrix(partsTranslations[renderOrder[i]]) * m1*m;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            puzzleObject->renderObject(objectsNumbers[selectedParts[i]]);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    
    const float mapKoef = mapSize.x/1920;
    const float farDist = -20;
    //fade
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    m = sunnyIdentityMatrix;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform1f(uniformTSA_A, 1);
    
    //Buttons
    {
        
        m = getTranslateMatrix(SunnyVector3D(0,0,farDist));
        glUniform1f(uniformTSA_A, 1);
        float btnScale;
        SunnyMatrix rot;
        rot = m;
        //Back
        rot.pos = puzzleButtons[PuzzleButton_Back]->matrix.pos;
        puzzleButtons[PuzzleButton_Back]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        btnScale *= 0.82;
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, 6*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+6);
        
//        //Store
//        rot.pos = puzzleButtons[PuzzleButton_Store]->matrix.pos;
//        puzzleButtons[PuzzleButton_Store]->active?btnScale = defaultButtonScale:btnScale = 1;
//        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
//        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
//        SunnyDrawArrays(LOLCButtonsVAO);
//        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, -2*mapKoef, btnScale, btnScale));
//        SunnyDrawArrays(LOLCButtonsVAO+2);
        
        //break
        if (!puzzleButtons[PuzzleButton_Break]->hidden)
        {
            rot.pos = puzzleButtons[PuzzleButton_Break]->matrix.pos;
            puzzleButtons[PuzzleButton_Break]->active?btnScale = defaultButtonScale:btnScale = 1;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            btnScale *= 0.82;
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
            SunnyDrawArrays(LOLCButtonsVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, 4*mapKoef, btnScale, btnScale));
            SunnyDrawArrays(LOLCButtonsVAO+3);
        }
    }
}

void LOPuzzle::moveParts(SunnyVector2D delta, char * parts, char partsCount)
{
    for (short i = 0;i<partsCount;i++)
        puzzleParts[parts[i]].position += delta;
}

void LOPuzzle::checkTheConnection(char number)
{
    const float gresh = 0.1;
        
    char * cParts;
    char cPartsCount = getConnectedParts(number, cParts);
    
    //left side
    if (number%puzzleHorizontalSize!=0 && connection[number][0]<0)
    {
        char part = number-1;
        if (puzzleParts[part].active && puzzleParts[number].rotation == puzzleParts[part].rotation)
        {
            float dx = (puzzleParts[number].position.x - puzzleParts[part].position.x);
            float dy = (puzzleParts[number].position.y - puzzleParts[part].position.y);
        
            bool q = false;
            switch (puzzleParts[number].rotation) {
                case 0:
                    q = (fabsf(dx-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dy)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D(-(dx-puzzlePartSizeX),-dy), cParts, cPartsCount);
                    break;
                case 1:
                    q = (fabsf(-dy-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dx)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D(-dx,(-dy-puzzlePartSizeX)), cParts, cPartsCount);
                    break;
                case 2:
                    q = (fabsf(-dx-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dy)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D((-dx-puzzlePartSizeX),-dy), cParts, cPartsCount);
                    break;
                case 3:
                    q = (fabsf(dy-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dx)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D(-dx,-(dy-puzzlePartSizeX)), cParts, cPartsCount);
                    break;
                default:
                    break;
            }
            
            if (q)
            {
                connection[number][0] = part;
                connection[part][2] = number;
                playConnectSound = true;
            }
        }
    }
    
    //top side
    if (number>=puzzleHorizontalSize && connection[number][1]<0)
    {
        char part = number-puzzleHorizontalSize;
        if (puzzleParts[part].active && puzzleParts[number].rotation == puzzleParts[part].rotation)
        {
            float dx = (puzzleParts[number].position.x - puzzleParts[part].position.x);
            float dy = (puzzleParts[number].position.y - puzzleParts[part].position.y);
            
            bool q = false;
            switch (puzzleParts[number].rotation) {
                case 0:
                    q = (fabsf(-dy-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dx)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D(-dx,(-dy-puzzlePartSizeY)), cParts, cPartsCount);
                    break;
                case 1:
                    q = (fabsf(-dx-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dy)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D((-dx-puzzlePartSizeY),-dy), cParts, cPartsCount);
                    break;
                case 2:
                    q = (fabsf(dy-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dx)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D(-dx,-(dy-puzzlePartSizeY)), cParts, cPartsCount);
                    break;
                case 3:
                    q = (fabsf(dx-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dy)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D(-(dx-puzzlePartSizeY),-dy), cParts, cPartsCount);
                    break;
                default:
                    break;
            }
            
            if (q)
            {
                connection[number][1] = part;
                connection[part][3] = number;
                playConnectSound = true;
            }
        }
    }
    
    //right side
    if (number%puzzleHorizontalSize!=puzzleHorizontalSize-1 && connection[number][2]<0)
    {
        char part = number+1;
        if (puzzleParts[part].active && puzzleParts[number].rotation == puzzleParts[part].rotation)
        {
            float dx = (puzzleParts[number].position.x - puzzleParts[part].position.x);
            float dy = (puzzleParts[number].position.y - puzzleParts[part].position.y);
            
            bool q = false;
            switch (puzzleParts[number].rotation) {
                case 0:
                    q = (fabsf(-dx-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dy)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D((-dx-puzzlePartSizeX),-dy), cParts, cPartsCount);
                    break;
                case 1:
                    q = (fabsf(dy-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dx)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D(-dx,-(dy-puzzlePartSizeX)), cParts, cPartsCount);
                    break;
                case 2:
                    q = (fabsf(dx-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dy)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D(-(dx-puzzlePartSizeX),-dy), cParts, cPartsCount);
                    break;
                case 3:
                    q = (fabsf(-dy-puzzlePartSizeX)<gresh*puzzlePartSizeX && fabsf(dx)<gresh*puzzlePartSizeY);
                    if (q) moveParts(SunnyVector2D(-dx,(-dy-puzzlePartSizeX)), cParts, cPartsCount);
                    break;
                default:
                    break;
            }
            
            if (q)
            {
                connection[number][2] = part;
                connection[part][0] = number;
                playConnectSound = true;
            }
        }
    }
    
    //bottom side
    if (number/puzzleHorizontalSize+1 < puzzleVerticalSize && connection[number][3]<0)
    {
        char part = number+puzzleHorizontalSize;
        if (puzzleParts[part].active && puzzleParts[number].rotation == puzzleParts[part].rotation)
        {
            float dx = (puzzleParts[number].position.x - puzzleParts[part].position.x);
            float dy = (puzzleParts[number].position.y - puzzleParts[part].position.y);
            
            bool q = false;
            switch (puzzleParts[number].rotation) {
                case 0:
                    q = (fabsf(dy-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dx)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D(-dx,-(dy-puzzlePartSizeY)), cParts, cPartsCount);
                    break;
                case 1:
                    q = (fabsf(dx-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dy)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D(-(dx-puzzlePartSizeY),-dy), cParts, cPartsCount);
                    break;
                case 2:
                    q = (fabsf(-dy-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dx)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D(-dx,(-dy-puzzlePartSizeY)), cParts, cPartsCount);
                    break;
                case 3:
                    q = (fabsf(-dx-puzzlePartSizeY)<gresh*puzzlePartSizeY && fabsf(dy)<gresh*puzzlePartSizeX);
                    if (q) moveParts(SunnyVector2D((-dx-puzzlePartSizeY),-dy), cParts, cPartsCount);
                    break;
                default:
                    break;
            }
            
            if (q)
            {
                connection[number][3] = part;
                connection[part][1] = number;
                playConnectSound = true;
            }
        }
    }
    
    free(cParts);
}

char LOPuzzle::getConnectedParts(char number, char*&connections)
{
    char count = 1;
    connections = (char*)malloc(sizeof(char));
    connections[0] = number;
    
    bool activeParts[puzzleHorizontalSize*puzzleVerticalSize];
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
        activeParts[i] = 0;
    activeParts[number] = true;
    bool anyChanges = true;
    while (anyChanges)
    {
        anyChanges = false;
        for (short j = 0;j<puzzleHorizontalSize*puzzleVerticalSize;j++)
            if (activeParts[j])
            {
                for (short k = 0;k<4;k++)
                    if (connection[j][k] >= 0 && !activeParts[connection[j][k]])
                    {
                        anyChanges = true;
                        activeParts[connection[j][k]] = true;
                        
                        connections = (char*)realloc(connections, sizeof(char)*(count+1));
                        connections[count] = connection[j][k];
                        count++;
                    }
            }
    }
    return count;
}

void screenToButtonsPuzzle(float &x,float&y)
{
    x = x*(mapZoomBounds.y-mapZoomBounds.x) + mapZoomBounds.x;
    y = y*(mapZoomBounds.w-mapZoomBounds.z) + mapZoomBounds.z;
}

void LOPuzzle::touchBegan(void * touch, float x, float y)
{
    screenToButtonsPuzzle(x,y);
    
    for (short i = 0;i<puzzleHorizontalSize*puzzleVerticalSize;i++)
    {
        if (fabsf(x-puzzleParts[renderOrder[i]].position.x) < puzzlePartSizeX/2 && fabsf(y-puzzleParts[renderOrder[i]].position.y) < puzzlePartSizeY/2)
        {
            char buf = renderOrder[i];
            for (int j = i;j>0;j--)
                renderOrder[j]=renderOrder[j-1];
            renderOrder[0] = buf;
           
            selectedPartsCount = getConnectedParts(buf,selectedParts);
            
            for (short k = 1; k<selectedPartsCount; k++)
            {
                short m = 0;
                for (short l = 0;l<puzzleHorizontalSize*puzzleVerticalSize;l++)
                    if (selectedParts[k] == renderOrder[l])
                    {
                        m = l;
                        break;
                    }
                    
                buf = renderOrder[m];
                for (int j = m;j>k;j--)
                    renderOrder[j]=renderOrder[j-1];
                renderOrder[k] = buf;
            }
            
            lastTouchLocation = SunnyVector2D(x,y);
            startTouchTime = getCurrentTime();
            moveDelta = 0;
            break;
        }
    }
}

void LOPuzzle::touchMoved(void * touch, float x, float y)
{
    if (selectedPartsCount>0)
    {
        screenToButtonsPuzzle(x,y);
        SunnyVector2D v(x,y);
        SunnyVector2D delta = v - lastTouchLocation;
        for (short i = 0;i<selectedPartsCount;i++)
            puzzleParts[selectedParts[i]].position += delta;
        lastTouchLocation = v;
        
        moveDelta += fabsf(delta.x) + fabsf(delta.y);
    }
}

void LOPuzzle::rotateParts(char* parts, char partsCount)
{
    for (short i = 0;i<partsCount;i++)
    {
        puzzleParts[parts[i]].rotation = (puzzleParts[parts[i]].rotation+1)%4;
        rotatingParts[parts[i]].isRotating = true;
        rotatingParts[parts[i]].origin = puzzleParts[parts[0]].position;
        rotatingParts[parts[i]].time = 0;
        if (i!=0)
        {
            SunnyVector2D distance = puzzleParts[parts[i]].position - puzzleParts[parts[0]].position;
            puzzleParts[parts[i]].position = puzzleParts[parts[0]].position + SunnyVector2D(distance.y, - distance.x);
        }
    }
}

void LOPuzzle::touchEnded(void * touch, float x, float y)
{
    if (selectedPartsCount>0)
    {
        screenToButtonsPuzzle(x,y);
        if (getCurrentTime()-startTouchTime<0.3 && moveDelta<0.5)
        {
            rotateParts(selectedParts, selectedPartsCount);
        } 
        for (short i = 0;i<selectedPartsCount;i++)
            checkTheConnection(selectedParts[i]);
        
        if (!playConnectSound)
            playDropSound = true;
        
        selectedPartsCount = 0;
        free(selectedParts);
        selectedParts = 0;
    }
}
