//
//  LOFont.h
//  LookOut
//
//  Created by Ignatov on 12.09.13.
//
//

#ifndef __LookOut__LOFont__
#define __LookOut__LOFont__

#include <iostream>
#include "SunnyMatrix.h"

enum LOTextAlign
{
    LOTA_LeftTop = 0,
    LOTA_LeftBottom,
    LOTA_RightTop,
    LOTA_RoghtBottom,
    LOTA_LeftCenter,
    LOTA_RightCenter,
    LOTA_Center,
};

class LOFont
{
    static void applyColor(int shaderUniform, SunnyVector4D color);
public:
    static void prepareFont();
    static void clearFont();
    static void writePrice(SunnyMatrix *position, LOTextAlign align, bool bold,float price, char * string, SunnyVector4D innerColor = SunnyVector4D(1,1,1,1), SunnyVector4D outerColor = SunnyVector4D(1,1,1,1));
    static void writeText(SunnyMatrix *position, LOTextAlign align, bool bold, const char * string, SunnyVector4D innerColor = SunnyVector4D(1,1,1,1), SunnyVector4D outerColor = SunnyVector4D(1,1,1,1));
    static void writeTextInRect(SunnyMatrix *position, SunnyVector2D rect, LOTextAlign align, bool bold, char * string, SunnyVector4D innerColor = SunnyVector4D(1,1,1,1), SunnyVector4D outerColor = SunnyVector4D(1,1,1,1));
    static void writeTime(SunnyMatrix *position, bool bold, const char * string, SunnyVector4D innerColor = SunnyVector4D(1,1,1,1), SunnyVector4D outerColor = SunnyVector4D(1,1,1,1));
    static void writeTime(SunnyMatrix *position, bool bold, int time, SunnyVector4D innerColor = SunnyVector4D(1,1,1,1), SunnyVector4D outerColor = SunnyVector4D(1,1,1,1), bool cutByHour = true);
    static void writeNumber(SunnyMatrix *position, bool bold, int number, SunnyVector4D innerColor = SunnyVector4D(1,1,1,1), SunnyVector4D outerColor = SunnyVector4D(1,1,1,1));
    
    static void copySubString(char * dest, const char * source, int start, int length);
    static float getNumberSize(int number);
    static float getTimeSize(int time, bool cutByHour = true);
    static float getTimeSize(char * string);
    static float getTextSize(const char * string);
    static float getTextSize(const char * string, int start, int length);
};



#endif /* defined(__LookOut__LOFont__) */
