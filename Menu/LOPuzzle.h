//
//  LOPuzzle.h
//  LookOut
//
//  Created by Ignatov on 27.08.13.
//
//

#ifndef __LookOut__LOPuzzle__
#define __LookOut__LOPuzzle__

#include <iostream>

const char puzzleHorizontalSize = 5;
const char puzzleVerticalSize = 4;
const float puzzlePartsScale = 100;

#include "SUnnyMatrix.h"
#include "SunnyOpengl.h"
#include "SunnyModelObj.h"
#include "SunnyButton.h"

struct LOP_OnePart
{
    SunnyVector2D position;
    char rotation;//90*rotation
    bool active;
    char order;
};

struct PP_RotateStructure
{
    bool isRotating;
    float time;
    SunnyVector2D origin;
};

enum LOPuzzleButtons
{
    PuzzleButton_Back = 0,
    PuzzleButton_Break,
    PuzzleButton_Count,
};

class LOPuzzle
{
    SunnyButton * puzzleButtons[PuzzleButton_Count];
    
    short **connection;
    LOP_OnePart *puzzleParts;
    
    char *selectedParts;
    char selectedPartsCount;
    double startTouchTime;
    SunnyVector2D lastTouchLocation;
    float puzzleHeight[puzzleHorizontalSize*puzzleVerticalSize];
    char renderOrder[puzzleHorizontalSize*puzzleVerticalSize];
    PP_RotateStructure rotatingParts[puzzleHorizontalSize*puzzleVerticalSize];
    
    void checkTheConnection(char number);
    void rotateParts(char* parts, char partsCount);
    void moveParts(SunnyVector2D delta, char * parts, char partsCount);
    char getConnectedParts(char number, char*&connections);
    
    float moveDelta;
    
    bool playDropSound;
    bool playConnectSound;
public:
    LOPuzzle();
    ~LOPuzzle();
    
    void applyPuzzleData(LOP_OnePart *parts);
    void update();
    void render();
    
    static void loadTexture();
    static void prepareForRenderIfNecessary();
    static void removeData();
    
    void touchBegan(void * touch, float x, float y);
    void touchMoved(void * touch, float x, float y);
    void touchEnded(void * touch, float x, float y);
    void buttonPressed(SunnyButton * btn);
};

#endif /* defined(__LookOut__LOPuzzle__) */
