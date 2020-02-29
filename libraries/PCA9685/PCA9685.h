/* Copyright (c) 2016 Adafruit Industries
 * Author: Tony DiCola
 * Original source of python library has been revised for own purposes
 *-----------------------------------------------------------------------------
 * Name:        Device driver for Feather PCA9685 board 
 * Purpose:     RC receiver with Cortex Mo Feather board
 *             
 * Author:      Bernd Hinze
 * github		https://github.com/monbera/M0Rx
 * Created:     25.02.2020
 * Copyright:   (c) Bernd Hinze 2020
 * Licence:     MIT see https://opensource.org/licenses/MIT
 * ----------------------------------------------------------------------------
*/

#ifndef PCA9685_h
#define PCA9685_h
#include "Arduino.h"
#include <Wire.h>

#define PCA9685_ADDR      (byte)0x40

#define MODE1             (byte)0x00
#define MODE2             (byte)0x01
#define SUBADR1           (byte)0x02
#define SUBADR2           (byte)0x03
#define SUBADR3           (byte)0x04
#define PRESCALE          (byte)0xFE
#define LED0_ON_L         (byte)0x06
#define LED0_ON_H         (byte)0x07
#define LED0_OFF_L        (byte)0x08
#define LED0_OFF_H        (byte)0x09
#define ALL_LED_ON_L      (byte)0xFA
#define ALL_LED_ON_H      (byte)0xFB
#define ALL_LED_OFF_L     (byte)0xFC
#define ALL_LED_OFF_H     (byte)0xFD

// Bits:
#define RESTART           (byte)0x80
#define SLEEP             (byte)0x10
#define ALLCALL           (byte)0x01
#define INVRT             (byte)0x10
#define OUTDRV            (byte)0x04


void write_byte_data(byte dev, byte reg, byte val);
byte read_byte_data(byte dev, byte reg);
void software_reset();
void pwm_init();
void set_pwm_freq(short freq);
void set_pwm(byte chnl, short on, short off);
void set_all_pwm(short on, short off);
void set_dio(short chnl, short state);


#endif
