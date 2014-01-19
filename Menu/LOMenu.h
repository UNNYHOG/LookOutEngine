//
//  LOMenu.h
//  LookOut
//
//  Created by Pavel Ignatov on 23.06.13.
//
//

#ifndef __LookOut__LOMenu__
#define __LookOut__LOMenu__

#include <iostream>
#include "SunnyButton.h"
#include "LOGlobalVar.h"
#include "LOPuzzle.h"
#include "LOStore.h"
#include "LOLevelComplete.h"
#include "LOPause.h"
#include "LODeathWindow.h"
#include "LOMessageWindow.h"
#include "LOTasksView.h"
#include "SunnyScrollView.h"
#include "LORewardView.h"
#include "LOTutorial.h"
#include "LOSurvivalComplete.h"
#include "LOMainWindow.h"

//enum LOMainMenu
//{
//    LOMM_Play = 0,
//    LOMM_PlaySurvival,
//    LOMM_Puzzle,
//    LOMM_GameCenter,
//    LOMM_Count,
//};

enum LOSpellButton
{
    LOSpellLife = 0,
    LOSpellTeleport,
    LOSpellSpeed,
    LOSpellShield,
    LOSpellCount,
};

enum LOSurvivalButton
{
    LOSB_Back = 0,
    LOSB_Play,
    LOSB_SpellDoubler,
    LOSB_SpellMagnet,
    LOSB_SpellTime,
    LOSB_Count,
};

enum LOSocialButton
{
    LOSocialButton_FB = 0,
    LOSocialButton_TW,
    LOSocialButton_VK,
    LOSocialButton_Count,
};

//struct LOTouch {
//    void * touch;
//    SunnyVector2D startTouchLocation;
//    SunnyVector2D touchLocation;
//    float lastVelocity;
//    double lastTime;
//    double startTime;
//};

//const short menuMaxTouchesCount = 1;

const short hintFlagsCount = 1;

class LOMenu
{
    bool hintFlags[hintFlagsCount];
    float hintTimer;
    LOPuzzle * puzzleWindow;
    LOStore * store;
    
//    SunnyButton *mainMenuButtons[LOMM_Count];
    SunnyButton *survivalButtons[LOSB_Count];
    SunnyButton *levelsButtons[loLevelsPerStep+1];
    SunnyButton *socialButtons[LOSocialButton_Count];
    
    //GamePlay buttons
    SunnyButton *spellButtons[LOSpellCount];
    SunnyButton *gamePauseButton;
    LOPriceType priceType[LOSpellCount];
    unsigned short price[LOSpellCount];
    
    float gamePauseAlphaTime;
    
    SunnyMatrix playerPositionInLevels;
    SunnyVector2D playerTargetPosition;
    float playerSelectedVelocity;
    float playOnBallAlpha;
    double menuShowedTime;
    
    void makeSoundCheckButtons();
    
    float levelButtonsAlpha;
    
    bool isGamePlay();
    bool isLevelSelectionShowed();
    bool isSurvivalSelectionShowed();
    
    LOLevelComplete * levelComplete;
    
    LOMainWindow * mainWindow;
    LODeathWindow * deathWindow;
    LOMessageWindow * messageWindow;
    LOTasksView * tasksView;
    LORewardView * rewardView;
    
    float nextCoinAnimationTime;
    short nextAnimatedCoin;
    
    char * episodeWName;
    short episodeWNumber;
    
    char * labelTimeRecord;
    float labelTimePosX;
    char * labelToBeContinued;
    char * labelStayTuned;
    
    SunnyButton *scrollButtons[2];
    SunnyButton *scrollButtonsSurv[2];
    void renderLevelSelection();
    void renderLevelSelectionSurvival();
    void renderLevelSelectionLastPage();
    
    SunnyScrollView * scrollView;
    LOTutorial * tutorial;
    
    bool isStarAnimated;
    float starAnimationTime;
    float starAnimationScale;
    
    bool puzzleMenuMode;
public:
    LOSurvivalComplete *survComplete;
    LOPause * pauseWindow;
    void showSurvivalCompleteWindow();
    void closeSurvivalCompleteAndUpdateTasks();
    bool isRewardViewVisible();
    
    bool isTutorial();
    void updateLevelSelection();
    SunnyButton ** getSurvivalButtons();
    static LOMenu *getSharedMenu();
    static void setCurrentPage(short page);
    void closeTutorial(bool forcePauseGame = false);
    bool levelCompleteShown;
//    short movingToStage;
    
    LOMenu();
    ~LOMenu();
    
    void animateStar();
    void backButtonPressed();
    
    void resetHintFlags();
    void dontShowTutorial();
    void showHelpMessage();
    void render();
    //void renderUnderSpells();
    void renderOverlay();
    void update();
    void updateSpells();
    void recalculateSurvivalPrices(SunnyButton * btn);
    void buttonPressed(SunnyButton* btn);
    void levelButtonPressed(SunnyButton* btn);
    void spellButtonPressed(SunnyButton* btn);
    void gamePlayButtonPressed(SunnyButton* btn);
    void pauseTheGame();
    void controlButtonPressed(SunnyButton* btn);
    void showGameOver();
    void showLevelComplete(float time, short coins,short time2, short time3);
    void renderPauseButton();
    
    void renderLevelButtonBack();
    void renderLevelButton(SunnyButton * btn);
    
    bool isLevelCompletedShown();
    void showSettingsWindow();
    
    void touchBegan(void * touch, float x, float y);
    void touchMoved(void * touch, float x, float y);
    void touchEnded(void * touch, float x, float y);
    
    void showPuzzleMenu(bool visible);
    void showStoreMenu(bool visible,LOStoreBack back);
    bool isSurvivalButtonsVisible();
    void showSurvivalButtons(bool visible, bool updatePrices = false);
    void showGameSpellButtons(bool visible);
    void showGamePlayButtons(bool visible);
    void showGameOverButtons(bool visible);
    void showLevelCompleteButtons(bool visible);
    void showMainMenu(bool visible);
    void showLevelsButtons(bool visible);
    void showSurvivalTutorial();
    void showSurvivalHelper();
    void showMagicDropInfo(LOMagicBoxType type);
    bool isAnyWindowOn();

    void showMessage(const char * notLocalizedString);
    void showSpellInactive();
    bool showSpellTutorial(LevelAbility spell);
    void showPuzzleTutorial(SunnyVector2D pos, unsigned short number, bool justClose);
    
    void showRessurectionHint();
    
    void showNotEnoughGoldHint(void (*func)(SunnyButton*) = 0,SunnyButton * btn = 0);
    void show5StarsHint();
    void showMagicBox(LOMagicBoxType type);
    
    void renderCallbackSpellButton(SunnyButton * btn);
    
    void showGoldPurchasedWindow(int goldCoins);
    
    LOLevelComplete * getLevelCompleteWindow();
    LODeathWindow * getDeathWindow();
    void leftHandedChanged();
    
    static void renderSpellOverlay(LevelAbility ability, SunnyMatrix *m, float alpha);
    static void renderSpellButton(LevelAbility ability, SunnyMatrix *m, float staticScale, float activeScale, float offset, bool drawPrice = true, float alpha = 1, bool enabled = true);
    static void renderPuzzleImage(SunnyMatrix *m, float alpha);
    
    bool canCast();
    
    friend class LODeathWindow;
};

#endif /* defined(__LookOut__LOMenu__) */
