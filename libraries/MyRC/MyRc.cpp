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

#include "MyRC.h"
#include "PCA9685.h"

/* Initialisation of the configuration array for 16 channels
 * with default values 
 * Parameter for channel configuration 
 * (MODE, CENTER, RATE, REVERSE, ACCFILT, FAILSAFE, STEPW, CENTER_TR)
 * MODE = SERVO, DIO or L298
 * CENTER = center position of servo in us  -> 1500
 * RATE = max. diviation from center in us  -> 500
 * REVERSE = boolean reverse orientation  (0 = false, 1 = true)
 * ACCFILT = boolean, using a filter or not to degrease the rising rate 
   of control values
 * FAILSAFE = 0..254 
 * STEPW = max. steps af rate within of cycle rate of 20 ms used by the filter
 * CENTER_TR = variable to store the current center with trimming 
*/

void CtrData_init(int *cfg){
	// filling the config table with default values
		for (int i=0; i <16; i++ ){
			*(cfg+i*8) = SERVO;
			*((cfg+i*8) + CENTER) = 1500;
			*((cfg+i*8) + RATE) = 500;
			*((cfg+i*8) + REVERSE) = false;
			*((cfg+i*8) + ACCFILT) = false;
			*((cfg+i*8) + FAILSAFE) = 127;
			*((cfg+i*8) + STEPW) = 127;
			*((cfg+i*8) + CENTER_TR) = 1500;					
			}	
	}

// configuration of an special channel
void config_chan(int chan, int *cfg, int *channeldat){
	*(cfg+chan*8) = *(channeldat + MODE);
	*((cfg+chan*8) + CENTER) = *(channeldat + CENTER);
	*((cfg+chan*8) + RATE) = *(channeldat + RATE);
	*((cfg+chan*8) + REVERSE) = *(channeldat + REVERSE);
	*((cfg+chan*8) + ACCFILT) = *(channeldat + ACCFILT);
	*((cfg+chan*8) + FAILSAFE) = *(channeldat + FAILSAFE);
	*((cfg+chan*8) + STEPW) = *(channeldat + STEPW);
	*((cfg+chan*8) + CENTER_TR) = *(channeldat + CENTER);				
	}	
	
/* configuration of an special channels, defined in cfg.h	
 * used to write the configuration from cfg.h of an model to
 * the configuration array
 */
void config_chan_all(int *chanlst, int* *cfg_p, int *cfg){
	int n = sizeof(chanlst) / sizeof(chanlst[0]);
	for (int i = 0; i <= n ; i++){
		*(cfg+chanlst[i]*8) 				= *(cfg_p[i] + MODE);
		*((cfg+chanlst[i]*8) + CENTER) 		= *(cfg_p[i] + CENTER);
		*((cfg+chanlst[i]*8) + RATE) 		= *(cfg_p[i] + RATE);
		*((cfg+chanlst[i]*8) + REVERSE) 	= *(cfg_p[i] + REVERSE);
		*((cfg+chanlst[i]*8) + ACCFILT) 	= *(cfg_p[i] + ACCFILT);
		*((cfg+chanlst[i]*8) + FAILSAFE)	= *(cfg_p[i] + FAILSAFE);
		*((cfg+chanlst[i]*8) + STEPW) 		= *(cfg_p[i] + STEPW);
		*((cfg+chanlst[i]*8) + CENTER_TR) 	= *(cfg_p[i] + CENTER);				
		}			
}	

/*Calculates the pulse value (0..4065) for the pca9685 module
 *tval = message input 0..254
 *[RATE] = maximal diviation (0..0.5) from the servo center puls duration 
 *[CENTER_TR] = servo center puls duration with timm adjustment 
 */
short PcaVal (int chan, int telval, int *cfg){
	int ival;
	short pval; 
	int rate = *(cfg + (chan * CFGlen) + RATE);  
	int center = *(cfg + (chan * CFGlen) + CENTER_TR);
	ival = (2 * rate * telval) / 254 + center - rate;
	pval = ival * FREQ * MAXPCA_I / 1000000; 		
	return pval;
}

// Only for test		
void set_pwm_(int chan, int on, int off) {
	Serial.print(chan);
	Serial.print(on);
	Serial.print(off);
	Serial.println();
	}
	
/* limiter to hold the input n in the range from 
 * minn to maxn
 */	
int clamp(int n, int minn, int maxn){
	if (n < minn) {return minn;} 
	else {
		if (n > maxn) {
			return maxn;
			} 
		else { 
			return n;}
		}
	}

int first = true;

/* Filter function to prevent strong increasing control
 * values for acceleration by STEPW per cycle 
 * only for inputs which are > than centerpos
 */ 
int soft_ctl(int inp, int chan, int *fdat , int *cfg){
	int da = *((cfg + chan * 8) + STEPW);
	int centerpos = *((cfg + chan * 8) + FAILSAFE);	
	int an = *(fdat + chan);
	int cout = 0;
	int REV = *((cfg + chan * 8) + REVERSE);
	if (first == true) { 
		an = inp; 
		first = false;
		}		
	cout = inp;
	if ((inp > centerpos) & (inp > (an + da))){
		cout = an + da;
		}
	if ((inp < centerpos) & (inp < (an - da))){
		cout = an - da;
		}			
	an = clamp(cout, 0, 254);
	*(fdat + chan) = an;
	return an;
}


/* set all channel to the fail save position 
 */
void fail_safe(int *cfg){
	int fsave;
	for (byte i = 0; i < 16; i++){
		fsave = *((cfg+i*8) + FAILSAFE);
		set_pwm(i, 0, PcaVal(i, fsave, cfg));
		}	
	}		
/* change the CENTER_TR value of an channel 
 * trimval = 0..50 
 */
void trim_Chan (int chan, int trimval, int *cfg){
	if (*(cfg+(chan*CFGlen) + REVERSE) == true){
	    trimval = 50 - trimval;
	}	
	int center = *(cfg + (chan * CFGlen) + CENTER);
	int newtrimmval = center * (trimval -25)/254 + center;
	*(cfg + (chan * CFGlen) + CENTER_TR) = newtrimmval;
}	

/* fills the  the default broadcast bbutter 
 * with - STX, ID, IP, sensor dafault value, CR 
 */
void BCTel (char *ctab, int *ip, char *sb)
{	*sb = '\x02';
	*(sb + 1) = '0';
	*(sb + 2) ='1';   
	// appends IP 
	for (int i = 0; i < 4; i++) {
		*(sb + 2*i + 3) = *(ctab + *(ip+i)/16);
		*(sb + 2*i + 4) = *(ctab + *(ip+i)%16);
	}
	// appends sensor value 
	for (int i = 0; i<4 ; i++) {
		*(sb + 11 + i) = '0';
	}
	*(sb + 2*3 + 9)	= '\x0D';
	*(sb + 2*3 + 10)	= '\0';
}

/* replace the sensor value with the 
 * current measured value 
 * ctab - CodeTable
 * sb - buffer of the broadcast, livesign telegram
 */
void update_V(char *ctab, float val, char *sb)
{	int Z = 0, F = 0; 
	Z = (int)val;
	F = (val*1000 - Z*1000)/10;
	*(sb + 11 + 0) = *(ctab + Z/16);
	*(sb + 11 + 1) = *(ctab + Z%16);
	*(sb + 11 + 2) = *(ctab + F/16);
	*(sb + 11 + 3) = *(ctab + F%16);	
}

/* interface for MORx.ino 
 * cb - array of received values (hdr, cha, val) 
 * fdat - temporary array for the filter function
 * cfg - overall configuration
 */
void update(int *cb, int *fdat, int *cfg){
	int hdr, cha, val, ival, mod;	 
	for (int i = 1; i < *cb + 1; i = i+3){
		// servo control
		hdr = *(cb + i + HDR);
		cha = *(cb + i + CHA);
		val = *(cb + i + VAL);			
		mod = *(cfg + (cha * CFGlen) + MODE);
		
		ival = val;
		if (hdr == CONTROL){
			if (*(cfg + (cha * CFGlen) + REVERSE) == true){
				ival = 254 - val;
			}	
			if (*(cfg + (cha * CFGlen) + ACCFILT) == true){
				ival = soft_ctl(ival, cha, fdat , cfg);
			}
			if (*(cfg + (cha * CFGlen) + MODE) == SERVO){
				set_pwm(cha, 0, PcaVal(cha, ival, cfg)); 
			}
		}
		printf("%d %d %d %d " , hdr, cha, ival, val);
		printf("\n");	
		// trimm control
		if (hdr == TRIMM){
			trim_Chan(cha, val, cfg);
		}
	}
}

/* converts the received buffer in an arrray of integer values 
 * checks if the telegram ID is 2 
 * rb - received from transmitter
 * cb - integrer array (cnt, hdr, cha, val, hdr, cha, val...) 
*/
int teltoarr (char *rb, int *cb)
{   int number;	
	int cntctrbu = 0;
	*cb = 0;
	if (((*(rb + 1)-48)*16  +  (*(rb + 2)-48)) == 2) {
		for (int i = 3; *(rb + i) != 13; i=i+2) {
			if (i%2 == 1){ 
				number = (*(rb + i)-48)*16 +  (*(rb + i + 1)-48); 
				*(cb + cntctrbu + 1) = number;
				cntctrbu +=1;
				*cb +=1;	
			}			
		}
	} 
	return cntctrbu; 
}

