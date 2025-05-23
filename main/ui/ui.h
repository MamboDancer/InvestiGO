// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 8.3.11
// Project name: InvestiGO

#ifndef _INVESTIGO_UI_H
#define _INVESTIGO_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern bool isInvest;
extern bool isDeposit;
extern bool isWithdraw;

extern TaskHandle_t nfcTask;

// SCREEN: ui_HomePage
void ui_HomePage_screen_init(void);
extern lv_obj_t * ui_HomePage;
void ui_event_HomePageBeginButton(lv_event_t * e);
extern lv_obj_t * ui_HomePageBeginButton;
extern lv_obj_t * ui_labelAutorise;
extern lv_obj_t * ui_Panel1;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_Label3;
extern lv_obj_t * ui_Label1;
// CUSTOM VARIABLES

// SCREEN: ui_MainScreen
void ui_MainScreen_screen_init(void);
extern lv_obj_t * ui_MainScreen;
extern lv_obj_t * ui_MainScreenHeaderLabel;
void ui_event_MainScreenInvestButton(lv_event_t * e);
extern lv_obj_t * ui_MainScreenInvestButton;
extern lv_obj_t * ui_MainScreenInvestButtonLabel;
extern lv_obj_t * ui_MainScreenBalanceLabel;
void ui_event_MainScreenDepositButton(lv_event_t * e);
extern lv_obj_t * ui_MainScreenDepositButton;
extern lv_obj_t * ui_MainScreenDepositButtonLabel;
void ui_event_MainScreenWithdrawButton(lv_event_t * e);
extern lv_obj_t * ui_MainScreenWithdrawButton;
extern lv_obj_t * ui_MainScreenWithdrawButtonLabel;
extern lv_obj_t * ui_MainScreenTabView;
extern lv_obj_t * ui_TabPage1;
extern lv_obj_t * table;
void ui_event_MainScreenLogoutButton(lv_event_t * e);
extern lv_obj_t * ui_MainScreenLogoutButton;
extern lv_obj_t * ui_MainScreenLogoutButtonLabel;
// CUSTOM VARIABLES

// SCREEN: ui_LogInChoose
void ui_LogInChoose_screen_init(void);
extern lv_obj_t * ui_LogInChoose;
extern lv_obj_t * ui_LoginChooseLabel;
void ui_event_LogInScreenCancelButton(lv_event_t * e);
extern lv_obj_t * ui_LogInScreenCancelButton;
extern lv_obj_t * ui_CancelButtonLabel2;
// CUSTOM VARIABLES

// SCREEN: ui_RegisterScreen
void ui_RegisterScreen_screen_init(void);
extern lv_obj_t * ui_RegisterScreen;
extern lv_obj_t * ui_RegisterScreenLabel;
extern lv_obj_t * ui_RegisterScreenNameInputArea;
void ui_event_RegisterScreenKeyboard(lv_event_t * e);
extern lv_obj_t * ui_RegisterScreenKeyboard;
void ui_event_RegisterScreenCancelButton(lv_event_t * e);
extern lv_obj_t * ui_RegisterScreenCancelButton;
extern lv_obj_t * ui_RegisterScreenCancelButtonLabel;
// CUSTOM VARIABLES

// SCREEN: ui_MoneyDelegateScreen
void ui_MoneyDelegateScreen_screen_init(void);
extern lv_obj_t * ui_MoneyDelegateScreen;
extern lv_obj_t * ui_MoneyDelegateScreenHeaderLabel;
extern lv_obj_t * ui_MoneyDelegateScreenTextArea;
extern lv_obj_t * ui_MoneyDelegateScreenKeyboard;
void ui_event_MoneyDelegateScreenCancelButton(lv_event_t * e);
extern lv_obj_t * ui_MoneyDelegateScreenCancelButton;
extern lv_obj_t * ui_MoneyDelegateScreenCancelButtonLabel;
// CUSTOM VARIABLES

// SCREEN: ui_SuccessScreen
void ui_SuccessScreen_screen_init(void);
extern lv_obj_t * ui_SuccessScreen;
extern lv_obj_t * ui_Label4;
void ui_event_SuccessScreenButton(lv_event_t * e);
extern lv_obj_t * ui_SuccessScreenButton;
extern lv_obj_t * ui_SuccessScreenButtonLabel;
// CUSTOM VARIABLES

// EVENTS

extern lv_obj_t * ui____initial_actions0;

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
