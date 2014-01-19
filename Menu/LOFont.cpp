//
//  LOFont.cpp
//  LookOut
//
//  Created by Ignatov on 12.09.13.
//
//

#include "LOFont.h"
#include "iPhone_Strings.h"
#include "SHTextureBase.h"
#include "SUnnyMatrix.h"
#include "LookOutEngine.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "stddef.h"
#ifdef __ANDROID__
#include <stdio.h>
#endif
int loFontTexture = -1;
short loFontSymbolsCount = 0;
GLuint fontVAO, fontVBO=0;
unsigned char loFontSmallValue;
unsigned char loFontBigValue;
float loFontHeight;
float *loFontLetterWidth = 0;

extern bool nearestMode;

const float colorKoeff = 255./244.;

void scalePointsSize(SunnyVector2D scale,short num,SunnyVector4D *&points);

void LOFont::applyColor(int shaderUniform, SunnyVector4D color)
{
    color.x *= colorKoeff;
    color.y *= colorKoeff;
    color.z *= colorKoeff;
    glUniform4fv(shaderUniform, 1, color);
}

void LOFont::prepareFont()
{
    char fileName[32];
    strcpy(fileName, getLocalizedString("FontFileName"));
    char filePath[256];
    strcpy(filePath, "Base/Fonts/");
    strcat(filePath, fileName);
    strcat(filePath, ".png");
    loFontTexture = SHTextureBase::loadTexture(filePath);
    
    strcpy(filePath, "Base/Fonts/");
    strcat(filePath, fileName);
    strcat(filePath, ".lofont");
    
    sunnyFILE * stream = sunnyfopen(getPath(filePath), "rb");
    if (stream)
    {
        short params[4];
        sunnyfread(&loFontSymbolsCount, sizeof(loFontSymbolsCount), 1, stream);
        sunnyfread(&loFontSmallValue, sizeof(loFontSmallValue), 1, stream);
        sunnyfread(&loFontBigValue, sizeof(loFontBigValue), 1, stream);
        
        SunnyVector4D * points = new SunnyVector4D[loFontSymbolsCount*2*4];
        loFontLetterWidth = new float [loFontSymbolsCount*2];
        float smallTr;
        sunnyfread(&smallTr, sizeof(smallTr), 1, stream);
        for (short i = 0;i<loFontSymbolsCount*2;i++)
        {
            sunnyfread(&params[0], sizeof(params[0]), 4, stream);
            //printf("\n (%d, %d, %d, %d) ",params[0],params[1],params[2],params[3]);
            addPointsToArray(SunnyVector4D(params[0],params[1],params[2],params[3])/2, i, points, false, SunnyVector2D(1024,1024));
            if (i<loFontSymbolsCount)
                translatePointsInArray(SunnyVector2D(params[2],smallTr)/2, i, points);
            else
                translatePointsInArray(SunnyVector2D(params[2],0)/2, i, points);
            scalePointsSize(SunnyVector2D(globalFontScale, globalFontScale), i, points);
            loFontLetterWidth[i] = fabsf(points[i*4+3].x - points[i*4].x);
        }
        
        loFontHeight = fabsf(points[1].y - points[0].y);
        
        if (isVAOSupported)
        {
            glGenVertexArrays(1, &fontVAO);
            glBindVertexArray(fontVAO);
        }
        glGenBuffers(1, &fontVBO);
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SunnyVector4D)*loFontSymbolsCount*2*4, points, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        
        if (isVAOSupported)
            glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        delete [] points;
        sunnyfclose(stream);
    }
}

void LOFont::clearFont()
{
    SHTextureBase::releaseLink(fontTexture);
    if (fontVBO)
    {
        if (isVAOSupported)
            glDeleteVertexArrays(1,&fontVAO);
        glDeleteBuffers(1, &fontVBO);
        fontVBO = fontVAO = 0;
    }
    
    if (loFontLetterWidth)
    {
        delete[] loFontLetterWidth;
        loFontLetterWidth = 0;
    }
}

void LOFont::writeTextInRect(SunnyMatrix *position, SunnyVector2D rect, LOTextAlign align, bool bold, char * string, SunnyVector4D innerColor, SunnyVector4D outerColor)
{
    float scale = position->front.length();
    unsigned short linesCount = 0;
    char ** lines = (char**)malloc(sizeof(char*));
    float size = 0;
    int lastSavePosition = 0;
    int lastSavePositionForLine = 0;
    for (int i = 0;i<strlen(string);i++)
    {
        short num = string[i];
        if (num==32 || i == strlen(string)-1)//space
        {
            size += getTextSize(string,lastSavePosition,i-lastSavePosition)*scale;
            if (size>rect.x && lastSavePosition!=0)
            {
                lines = (char**)realloc(lines,sizeof(char*)*(linesCount+1));
                lines[linesCount] = new char[lastSavePosition - lastSavePositionForLine+1];
                copySubString(lines[linesCount],string,lastSavePositionForLine,lastSavePosition-lastSavePositionForLine);
                lastSavePositionForLine = lastSavePosition+1;
                size = getTextSize(string,lastSavePositionForLine,i-lastSavePositionForLine)*scale;
                linesCount++;
            }
            lastSavePosition = i;
        }
    }
    
    lastSavePosition = (int)strlen(string);
    lines = (char**)realloc(lines,sizeof(char*)*(linesCount+1));
    lines[linesCount] = new char[lastSavePosition - lastSavePositionForLine+1];
    copySubString(lines[linesCount],string,lastSavePositionForLine,lastSavePosition-lastSavePositionForLine);
    linesCount++;
        
    SunnyMatrix m = *position;
    m.pos += m.up * loFontHeight/2*(linesCount-1);
    for (unsigned short i = 0;i<linesCount;i++)
    {
        writeText(&m, align, bold, lines[i],innerColor,outerColor);
        m.pos -= m.up * loFontHeight;
    }
    
    for (unsigned short i = 0;i<linesCount;i++)
        delete[]lines[i];
    free(lines);
}

void LOFont::writePrice(SunnyMatrix *position, LOTextAlign align, bool bold,float price, char * string, SunnyVector4D innerColor, SunnyVector4D outerColor)
{
    char priceString[64];
    int intPrice = price;
    
    //int
    char inverseStr[64];
    int len = 0;
    do
    {
        int k = intPrice%10;
        inverseStr[len++] = '0' + k;
        intPrice /= 10;
    }while (intPrice>0);
    
    for (short i = 0;i<len;i++)
        priceString[i] = inverseStr[len-i-1];
    priceString[len] = '\0';
    //float
    intPrice = price;
    int first = (price-intPrice)*10;
    int second = (price-intPrice)*100 - first*10+0.5;
    if (first!=0 || second!=0)
    {
        priceString[len] = '.';
        priceString[len+1] = '0'+first;
        priceString[len+2] = '0'+second;
        priceString[len+3] = ' ';
        priceString[len+4] = '\0';
    } else
    {
        priceString[len] = ' ';
        priceString[len+1] = '\0';
    }
    
    strcat(priceString, string);
    writeText(position, align, bold, priceString,innerColor,outerColor);
}

void LOFont::writeText(SunnyMatrix *position, LOTextAlign align, bool bold, const char * string, SunnyVector4D innerColor, SunnyVector4D outerColor)
{
    if (SHTextureBase::bindTexture(loFontTexture))
    {
        if (isVAOSupported)
        {
            glBindVertexArray(fontVAO);
        } else
        {
            glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    struct BufferedNumbers
    {
        char number[32];
        SunnyMatrix pos;
    };
    BufferedNumbers *bufferedNumbers = (BufferedNumbers*)malloc(sizeof(BufferedNumbers));
    short bufCount = 0;
    
    SunnyVector4D tsv(0,0,1,1);
    if (align == LOTA_Center)
    {
        float len = 0;
        short order = 0;
        for (short i = 0;i<strlen(string);i++)
        {
            unsigned char num = string[i];
            
            if (num == 208) continue;//RU
            if (num == 209){ order=64; continue;}
            num+=order;
            order = 0;
            
            if (num==32)//space
            {
                len += loFontLetterWidth[0]/2;
                continue;
            }
            if (num>=loFontSmallValue && num<loFontSmallValue+loFontSymbolsCount)
            {
                num -= loFontSmallValue;
            } else
            if (num>=loFontBigValue && num<loFontBigValue + loFontSymbolsCount)
            {
                num -= loFontBigValue;
                num +=loFontSymbolsCount;
            } else
            //number
            {
                if (num>='0' && num<='9')
                {
                    char str[2];
                    str[0] = num;
                    str[1] = '\0';
                    len += getTimeSize(str);
                }
                for (short k = 0;k<numbersSymbolsCount;k++)
                    if (fontSymbols[k] == num)
                    {
                        char str[2];
                        str[0] = num;
                        str[1] = '\0';
                        len += getTimeSize(str);
                        break;
                    }
                continue;
            }
            
            if (num<0 || num>=2*loFontSymbolsCount) continue;
            
            len += loFontLetterWidth[num];
        }
        
        tsv.x -= len/2;
    }
    
    GLuint tsu;
    if (bold)
    {
        sunnyUseShader(globalShaders[LOS_FontBold]);
        glUniformMatrix4fv(globalModelview[LOS_FontBold], 1, GL_FALSE, position->front);
        applyColor(uniform3D_FB_Color, innerColor);
        applyColor(uniform3D_FB_BackColor, outerColor);
        tsu = uniform3D_FB_TS;
    }
    else
    {
        sunnyUseShader(globalShaders[LOS_FontRegular]);
        glUniformMatrix4fv(globalModelview[LOS_FontRegular], 1, GL_FALSE, position->front);
        applyColor(uniform3D_FR_Color, innerColor);
        tsu = uniform3D_FR_TS;
    }
    
    glUniform4fv(tsu, 1, tsv);
    bool previousWasNumber = false;

    short order = 0;
    for (short i = 0;i<strlen(string);i++)
    {
        unsigned char num = string[i];
        
        if (num == 208) continue;//RU
        if (num == 209){ order=64; continue;}
        num+=order;
        order = 0;
        
        if (num==32)//space
        {
            tsv.x += loFontLetterWidth[loFontSymbolsCount]/2;
            glUniform4fv(tsu, 1, tsv);
            continue;
        }
        if (num>=loFontSmallValue && num<loFontSmallValue+loFontSymbolsCount)
        {
            num -= loFontSmallValue;
        } else
        if (num>=loFontBigValue && num<loFontBigValue + loFontSymbolsCount)
        {
            num -= loFontBigValue;
            num +=loFontSymbolsCount;
        } else
        {
            bool q = false;
            if (num>='0' && num<='9')
                q = true;
            else
            {
                for (short k = 0;k<numbersSymbolsCount;k++)
                    if (fontSymbols[k] == num)
                    {
                        q = true;
                        break;
                    }
            }
            if (q)
            {
                if (!previousWasNumber)
                {
                    bufferedNumbers = (BufferedNumbers*)realloc(bufferedNumbers, sizeof(BufferedNumbers)*(bufCount+1));
                    bufferedNumbers[bufCount].number[0] = string[i];
                    bufferedNumbers[bufCount].number[1] = '\0';
                    bufferedNumbers[bufCount].pos = *position;
                    bufferedNumbers[bufCount].pos.pos += bufferedNumbers[bufCount].pos.front*tsv.x;
                    bufCount++;
                } else
                {
                    short len = strlen(bufferedNumbers[bufCount-1].number);
                    bufferedNumbers[bufCount-1].number[len] = string[i];
                    bufferedNumbers[bufCount-1].number[len+1] = '\0';
                }
                previousWasNumber = true;
            }

            continue;
        }
        
        if (previousWasNumber)
        {
            tsv.x += getTimeSize(bufferedNumbers[bufCount-1].number);
            glUniform4fv(tsu, 1, tsv);
            previousWasNumber = false;
            //continue;
        }
        
        SunnyDrawArrays(num);
        
        tsv.x += loFontLetterWidth[num];
        glUniform4fv(tsu, 1, tsv);
    }
    
    for (short i = 0;i<bufCount;i++)
    {
        bindGameGlobal();
        writeTime(&bufferedNumbers[i].pos, bold, bufferedNumbers[i].number, innerColor, outerColor);
    }
    
    free(bufferedNumbers);
}

void LOFont::writeTime(SunnyMatrix *position, bool bold, int time, SunnyVector4D innerColor, SunnyVector4D outerColor, bool cutByHour)
{
    if (cutByHour || time<3600)
    {
        if (time>=3600) time = 3600;
        char str[8];
        int min = time/60;
        int sec = time%60;
        short len = 0;
        if (min>=10)
            str[len++] = '0' + min/10;
        str[len++] = '0' + min%10;
        str[len++] = ':';
        str[len++] = '0' + sec/10;
        str[len++] = '0' + sec%10;
        str[len] = '\0';
        
        writeTime(position, bold, str, innerColor, outerColor);
    } else
    {
        char str[64];
        int hours = time/3600;
        time -= hours*3600;
        int min = time/60;
        int sec = time%60;
        
        char inverseStr[64];
        int len = 0;
        do
        {
            int k = hours%10;
            inverseStr[len++] = '0' + k;
            hours /= 10;
        }while (hours>0);
        
        for (short i = 0;i<len;i++)
            str[i] = inverseStr[len-i-1];
        str[len++] = ':';
        str[len++] = '0' + min/10;
        str[len++] = '0' + min%10;
        str[len++] = ':';
        str[len++] = '0' + sec/10;
        str[len++] = '0' + sec%10;
        str[len] = '\0';
        writeTime(position, bold, str, innerColor, outerColor);
    }
}

void LOFont::writeNumber(SunnyMatrix *position, bool bold, int number, SunnyVector4D innerColor, SunnyVector4D outerColor)
{
    char inverseStr[64];
    char str[64];
    int len = 0;
    do
    {
        int k = number%10;
        inverseStr[len++] = '0'+k;
        number /= 10;
    }while (number>0);
    
    for (short i = 0;i<len;i++)
        str[i] = inverseStr[len-i-1];
    str[len] = '\0';
    
    writeTime(position, bold, str, innerColor, outerColor);
}

float LOFont::getTimeSize(int time, bool cutByHour)
{
    if (cutByHour || time<3600)
    {
        if (time>=3600) time = 3600;
        char str[8];
        int min = time/60;
        int sec = time%60;
        short len = 0;
        if (min>=10)
            str[len++] = '0' + min/10;
        str[len++] = '0' + min%10;
        str[len++] = ':';
        str[len++] = '0' + sec/10;
        str[len++] = '0' + sec%10;
        str[len] = '\0';

        return getTimeSize(str);
    } else
    {
        char str[64];
        int hours = time/3600;
        time -= hours*3600;
        int min = time/60;
        int sec = time%60;
        
        char inverseStr[64];
        int len = 0;
        do
        {
            int k = hours%10;
            inverseStr[len++] = '0' + k;
            hours /= 10;
        }while (hours>0);
        
        for (short i = 0;i<len;i++)
            str[i] = inverseStr[len-i-1];
        str[len++] = ':';
        str[len++] = '0' + min/10;
        str[len++] = '0' + min%10;
        str[len++] = ':';
        str[len++] = '0' + sec/10;
        str[len++] = '0' + sec%10;
        str[len] = '\0';
        
        return getTimeSize(str);
    }
}

void LOFont::copySubString(char * dest, const char * source, int start, int length)
{
    if (start + length > strlen(source))
    {
        printf("\n Error : copySubString : our of source string bounds");
        return;
    }
    
    for (short i = 0;i<length;i++)
        dest[i] = source[start+i];
    dest[length] = '\0';
}

float LOFont::getNumberSize(int number)
{
    float move = 0;
    while (number>0)
    {
        int k = number%10;
        move += numbersSizes[k]*pixelToWorld;
        number /= 10;
    }
    
    return  move;
}

void LOFont::writeTime(SunnyMatrix *position, bool bold, const char * string, SunnyVector4D innerColor, SunnyVector4D outerColor)
{
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(position->front.x));
    
    int k;
    float move;
    if (bold)
    {
        applyColor(uniformTSC_C, outerColor);
        move = 0;
        for (int i = 0;i<strlen(string);i++)
        {
            if (string[i]>='0' && string[i]<='9')
            {
                k = string[i] - '0';
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(move + numbersSizes[k]/2*pixelToWorld, 0, 1, 1));
                SunnyDrawArrays(LONumbers_VAO + k);
                move+= numbersSizes[k]*pixelToWorld;
                continue;
            }
            else
            {
                if (string[i]==32)
                {
                    move += numbersSizes[0]*pixelToWorld/2;
                    continue;
                }
            }
            
            for (short k = 0;k<numbersSymbolsCount;k++)
                if (fontSymbols[k] == string[i])
                {
                    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(move + numbersSymbolsSizes[k]/4*pixelToWorld, 0, 1, 1));
                    SunnyDrawArrays(LONumbersSymbols_VAO + k);
                    move += numbersSymbolsSizes[k]*pixelToWorld/2;
                    break;
                }
        }
    }
    
    applyColor(uniformTSC_C, innerColor);
    move = 0;
    for (int i = 0;i<strlen(string);i++)
    {
        if (string[i]>='0' && string[i]<='9')
        {
            k = string[i] - '0';
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(move + numbersSizes[k]/2*pixelToWorld, 0, 1, 1));
            SunnyDrawArrays(LONumbers_VAO + numbersCount + k);
            move+= numbersSizes[k]*pixelToWorld;
            continue;
        }
        else
        {
            if (string[i]==32)
            {
                move += numbersSizes[0]*pixelToWorld/2;
                continue;
            }
        }
        
        for (short k = 0;k<numbersSymbolsCount;k++)
            if (fontSymbols[k] == string[i])
            {
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(move + numbersSymbolsSizes[k]/4*pixelToWorld, 0, 1, 1));
                SunnyDrawArrays(LONumbersSymbols_VAO + numbersSymbolsCount + k);
                move += numbersSymbolsSizes[k]*pixelToWorld/2;
                break;
            }
    }
}

float LOFont::getTimeSize(char * string)
{
    float move = 0;
    for (int i = 0;i<strlen(string);i++)
    {
        if (string[i]>='0' && string[i]<='9')
        {
            move += numbersSizes[string[i] - '0']*pixelToWorld;
        }
        else
        if (string[i]==32) move += numbersSizes[0]*pixelToWorld/2;
        else
            for (short k = 0;k<numbersSymbolsCount;k++)
                if (fontSymbols[k] == string[i])
                {
                    move += numbersSymbolsSizes[k]/2*pixelToWorld;
                    break;
                }
    }
    
    return move;
}

float LOFont::getTextSize(const char * string, int start, int length)
{
    float move = 0;
    short order = 0;
    for (short i = 0;i<length;i++)
    {
        unsigned char num = string[start+i];
        
        if (num == 208) continue;//RU
        if (num == 209){ order=64; continue;}
        num+=order;
        order = 0;
        
        if (num==32)//space
        {
            move += loFontLetterWidth[0]/2;
            continue;
        }
        if (num>=loFontSmallValue && num<loFontSmallValue+loFontSymbolsCount)
        {
            num -= loFontSmallValue;
        } else
        if (num>=loFontBigValue && num<loFontBigValue + loFontSymbolsCount)
        {
            num -= loFontBigValue;
            num +=loFontSymbolsCount;
        } else
        {
            if (num>='0' && num<='9')
            {
                move += numbersSizes[num - '0']*pixelToWorld;
            } else
                for (short k = 0;k<numbersSymbolsCount;k++)
                    if (fontSymbols[k] == num)
                    {
                        move += numbersSymbolsSizes[k]/2*pixelToWorld;
                        break;
                    }
            continue;
        }
        
        if (num<0 || num>=2*loFontSymbolsCount) continue;
        
        move += loFontLetterWidth[num];
    }
    
    return move;
}

float LOFont::getTextSize(const char * string)
{
    return getTextSize(string,0,(int)strlen(string));
}




