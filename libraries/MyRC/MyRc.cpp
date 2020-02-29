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

/*Calculates the puls value (0..4065) for the pca9685 module
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
	if (first == true) { 
		an = inp; 
		first = false;
		}
	if ((inp > centerpos) & (inp > (an + da))){
		cout = an + da;
		}
	else {cout = inp;}
	an = clamp(cout, 0, 254);
	*(fdat + chan) = an;
	return an;
}


// set all channel to the fail save position 
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
	int center = *(cfg + (chan * CFGlen) + CENTER);
	int newtrimmval = center * (trimval -25)/254 + center;
	*(cfg + (chan * CFGlen) + CENTER_TR) = newtrimmval;
}	
		
/*  append the numbers of the IP array to an char buffer */
void appendIP (int *ip, char *sb){
  	char cip[5];
  	int eofsb = 0;
  	eofsb = strlen(sb);
  	for (int i = 0; i < 4; i++) {
    	sprintf(cip, "%d", *(ip + i));
    for (int ixp = 0; * (cip + ixp) != '\0'; ixp++) {
      	*(sb + eofsb) = *(cip + ixp);
      	eofsb += 1;
    }
    *(sb + eofsb) = '.';
    eofsb += 1;
  	}
  	*(sb + eofsb - 1) = '\0';
}

/* interface for the MORx.ino 
 * cb - array of received values (hdr, cha, val) 
 * fdat - temporary array for the filter function
 * cfg - overall configuration
 */
void update(int *cb, int *fdat, int *cfg){
	int hdr, cha, val, mod;	 
	for (int i = 1; i < *cb + 1; i = i+3){
		// servo control
		hdr = *(cb + i + HDR);
		cha = *(cb + i + CHA);
		val = *(cb + i + VAL);			
		mod = *(cfg + (cha * CFGlen) + MODE);
		//printf("%d %d %d %d " , hdr, cha, val, mod);
		//printf("\n");	
		if (hdr == CONTROL){
			if (*(cfg + (cha * CFGlen) + REVERSE) == true){
				val = 254 - val;
			}	
			if (*(cfg + (cha * CFGlen) + ACCFILT) == true){
				val = soft_ctl(val, cha, fdat , cfg);
			}
			if (*(cfg + (cha * CFGlen) + MODE) == SERVO){
				set_pwm(cha, 0, PcaVal(cha, val, cfg)); 
			}
		}
		// trimm control
		if (hdr == TRIMM){
			trim_Chan(cha, val, cfg);
		}
	}
}

/* converts the incoming comma seperated char array to a int array
 * it must end with an comma
 * rb - comma separated caracters, received ftom transmitter
 * cb - integrer array (cnt, hdr, cha, val, hdr, cha, val...) 
 * cnt = (length of cb) / 3
 */

void strtoarr (char *rb, int *cb) {
	  int previ = 0;
	  int lenc = 0;
	  int number;
	  int factor;
	  int cntctrbu = 0;
	  *cb = 0;
	  for (int i = 0; *(rb + i) != '\0'; i++) {
		if (*(rb + i) == ',') {
		  number = 0;
		  factor  = 1;
		  lenc = i - previ;
		  factor  = 1;
		  for (int j = 1; j <= lenc; j++) {
		    factor  = (factor * 10);
		    number += (*(rb + i - j) - 48) * factor / 10;
		  }
		  *(cb + cntctrbu + 1) = number;
		  cntctrbu += 1;
		  previ = i + 1;
		  *cb = *cb + 1; 
		}
  }
}
