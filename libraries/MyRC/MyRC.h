/* -----------------------------------------------------------------------------
 * Name:        MyRc 
 * Purpose:     helper functions for RC receiver with 'Cortex Mo Feather' board
 *              and PCA9685-board
 * Author:      Bernd Hinze
 * github		https://github.com/monbera/M0Rx
 * Created:     25.02.2020
 * Copyright:   (c) Bernd Hinze 2020
 * Licence:     MIT see https://opensource.org/licenses/MIT
 * ----------------------------------------------------------------------------
*/

#ifndef MyRC_h
#define MyRC_h
#include "Arduino.h"

const int FREQ = 50;
const int MAXPCA_I = 4065;
const int CONTROL = 255;
const int TRIMM = 127;

enum mode {
	SERVO, DIO, L298,
	};

enum config {
	MODE, CENTER, RATE, REVERSE, ACCFILT, FAILSAFE, STEPW, CENTER_TR, CFGlen
	};
	
enum TEL {
	HDR, CHA, VAL,
	};

void CtrData_init(int *cfg);
short PcaVal (int chan, int telval, int *cfg);
void fail_safe(int *cfg);
void trim_Chan (int chan, int trimval, int *cfg);
void update(int *cb, int *fdat, int *cfg);
void appendIP (int *, char *);
void strtoarr (char *rb, int *cb);
void config_chan(int chan, int *cfg, int *channeldat);
void config_chan_all(int *chanlst, int* *cfg_p, int *cfg);
int soft_ctl(int inp, int chan, int *fdat , int *cfg);

#endif
