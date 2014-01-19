//
//  LOMessages.h
//  LookOut
//
//  Created by Pavel Ignatov on 27.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOMessages_h
#define LookOut_LOMessages_h

typedef enum {
    kGameStateWaitingForMatch = 0,
    kGameStateWaitingForRandomNumber,
    kGameStateWaitingForStart,
    kGameStateActive,
    kGameStateDone
} GameState;

typedef enum {
    kEndReasonWin,
    kEndReasonLose,
    kEndReasonDisconnect
} EndReason;

typedef enum {
    kMessageTypeRandomNumber = 0,
    kMessageTypeGameBegin,
    kMessageTypeTextMessage,
    kMessageTypeNumberMessage,
    kMessageTypeUnpauseGame,
    kMessageTypeTryUnpauseGame,
    kMessageTypeTryRestartGame,
    kMessageTypePauseGame,
    kMessageTypeUpdatePosition,
    kMessageReadyCheck,
    kMessageRespawn,
    kMessageMinutePast,
    kMessageSnowExploded,
    kMessageTypeGameOver,
    kMessageTypeWind
} MessageType;

typedef struct {
    MessageType messageType;
} Message;

typedef struct {
    Message message;
    LOScriptEffectType ballType;
    short ballNum;
} MessageSnowExploded;

typedef struct {
    Message message;
    double playingTime;
} MessageMinutePast;

typedef struct {
    Message message;
    uint32_t randomNumber;
    short gameMode;
} MessageRandomNumber;

typedef struct {
    Message message;
    LOScriptEffectType ballType;
    SunnyVector2D pos;
    SunnyVector2D vel;
} MessageRespawn;

typedef struct {
    Message message;
} MessageGameBegin;

typedef struct {
    Message message;
    SunnyVector2D newMapWind;
    SunnyVector4D mapWind;
} MessageWind;

typedef struct {
    Message message;
    char text[256];
} TextMessage;

typedef struct {
    Message message;
    short number;
} NumberMessage;

typedef struct {
    Message message;
} ReadyCheckMessage;

typedef struct {
    Message message;
} UnpauseMessage;

typedef struct {
    Message message;
    bool player1Won;
} MessageGameOver;

typedef struct {
    Message message;
    SunnyVector2D pos;
    SunnyVector2D vel;
    float radius;
    bool crashed;
    float waterKoef;//playingTime if dead
    float sandKoef;
    double multiPlayerPlayedTime;
    SunnyVector2D alpha;
} MessageUpdatePosition;

#endif
