//
//  LOExternMethods.h
//  LookOut
//
//  Created by Pavel Ignatov on 21.12.13.
//
//

#ifndef LookOut_LOExternMethods_h
#define LookOut_LOExternMethods_h

void loOpenURLIOS(const char * url);
void loCallBackVibrateIOS();
void loOpenUTFunPageIOS(const char * url, const char * pageID);
void loOpenFBFunPageIOS(const char * url, const char * pageID);
void loShareFacebookLCIOS(const int levelNumber, const char* episodeName, const float levelTime, const char* messageName, const char * messageCaption, const char * messageDesc);
void loShareFacebookSurvivalIOS(const char* episodeName, const float levelTime, const char* messageName, const char * messageCaption, const char * messageDesc);
void loMakeFBConnectionIOS(void (*success)(const char*));
void showGameCenterIOS();
void loRateTheGameIOS();
void loCallbackSetMusicIOS(const char * fileName, unsigned short startPosition);
void loCallbackSetMusicPositionIOS(unsigned short startPosition);
void scheduleLocalNotificationIOS(float delaySec, const char * textBody, const char * alertAction, const char * soundName);
void cancelAllLocalNotificationsIOS();

#endif
