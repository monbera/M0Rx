/* -----------------------------------------------------------------------------
 * Name:        M0Rx 
 * Purpose:     RC receiver with 'Cortex M0 Feather' board, WiFi 
 *              and PCA9685
 * Author:      Bernd Hinze
 * github		https://github.com/monbera/M0Rx
 * Created:     25.02.2020
 * Copyright:   (c) Bernd Hinze 2020
 * Licence:     MIT see https://opensource.org/licenses/MIT
 * ----------------------------------------------------------------------------
 */
#ifndef cfg_h
#define cfg_h
#include "Arduino.h"
#include <MyRC.h>

// define your local wifi configuration 
#define SECRET_SSID "LG_0815"  
#define SECRET_PASS "hnz3pzw51"

// Debug with serial (USB) interface
#define NODEBUG
// ID for the model
#define Testbed

/*
 * Parameter for channel configuration 
 * (MODE, CENTER, RATE, REVERSE, ACCFILT, FAILSAFE, STEPW)
 * MODE = SERVO, DIO or L298
 * CENTER = center position of servo in us  -> 1500
 * RATE = max. diviation from center in us  -> 500
 * REVERSE = boolean reverse orientation  (0 = false, 1 = true)
 * ACCFILT = boolean, using a filter or not to degrease the rising rate 
   of control values
 * FAILSAFE = 0..254 
 * STEPW = max. steps af rate within of cycle rate of 20 ms used by the filter
*/

#ifdef MyCar
//MODE, CENTER, RATE, REVERSE, ACCFILT, FAILSAFE, STEPW,
int Cdat0[7] = { SERVO, 1500, 500, false, true, 127, 10};  // ESC Controller TBS02 
int Cdat3[7] = { SERVO, 1500, 200, false, false, 127, 127}; // SERVO
int CHcfg[2] = {0, 3};  // used channels
int * CHcfg_P[2] = {Cdat0, Cdat3};  // list of pointer to the configuration array
#endif

#ifdef Boat
int Cdat0[7] = { SERVO, 1400, 400, false, true, 127, 10};   // ESC Controller PULSTEC
int Cdat3[7] = { SERVO, 1500, 500, false, false, 127, 127}; // SERVO
int CHcfg[2] = {0, 3};
int * CHcfg_P[2] = {Cdat0, Cdat3};
#endif

#ifdef Testbed
int Cdat0[7] = { SERVO, 1400, 400, false, true, 127, 10};  // ESC Controller PULSTEC
int Cdat2[7] = { DIO, 1500, 500, false, false, 0, 127};   // LED
int Cdat3[7] = { SERVO, 1500, 500, false, false, 127, 127}; // SERVO
int CHcfg[3] = {0, 2, 3 };
int * CHcfg_P[3] = {Cdat0, Cdat2, Cdat3};
#endif 


#endif
