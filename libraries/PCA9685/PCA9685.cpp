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

#include "PCA9685.h"

void write_byte_data(byte dev, byte reg, byte val) {
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

byte read_byte_data(byte dev, byte reg) {
  Wire.beginTransmission(dev); // transmit to device
  Wire.write(reg);      // sets register pointer
  Wire.endTransmission();      // stop transmitting
  Wire.requestFrom(dev, 1);    // request byte from slave device
  byte c;
  if (1 <= Wire.available()) {
    c = Wire.read(); // receive a byte as character
  }
}

void software_reset() {
  Wire.beginTransmission(0x00);
  Wire.write(0x06);
  Wire.endTransmission();
}

void pwm_init(){
  set_all_pwm (0,  307);
  write_byte_data(PCA9685_ADDR, MODE2, OUTDRV);
  write_byte_data(PCA9685_ADDR, MODE1, ALLCALL);
  delay(5);
  byte mode1 = read_byte_data(PCA9685_ADDR, MODE1);
  mode1 = mode1 & ~SLEEP;
  write_byte_data(PCA9685_ADDR, MODE1, mode1);
  delay(5);
}

void set_pwm_freq(short freq) {
  byte oldmode, newmode = 0x00;
  //Set the PWM frequency to the provided value in hertz.
  byte prescale = (25000000 / (4096 * freq) - 1);
  printf ("Prescale %d \n", prescale);
  oldmode = read_byte_data(PCA9685_ADDR, MODE1);
  newmode = (oldmode & 0x7F) | SLEEP;
  printf("%x \n", newmode);
  write_byte_data(PCA9685_ADDR, MODE1, newmode);
  write_byte_data(PCA9685_ADDR, PRESCALE, prescale);
  write_byte_data(PCA9685_ADDR, MODE1, oldmode);
  delay(5);
  write_byte_data(PCA9685_ADDR, MODE1, oldmode | RESTART);
}

void set_pwm(byte chnl, short on, short off) {
  write_byte_data(PCA9685_ADDR, LED0_ON_L + (4 * chnl), on & 0xFF);
  write_byte_data(PCA9685_ADDR, LED0_ON_H  + (4 * chnl), on >> 8);
  write_byte_data(PCA9685_ADDR, LED0_OFF_L + (4 * chnl), off & 0xFF);
  write_byte_data(PCA9685_ADDR, LED0_OFF_H + (4 * chnl), off >> 8);
}

void set_all_pwm(short on, short off) {
    write_byte_data(PCA9685_ADDR, ALL_LED_ON_L, on & 0xFF);
    write_byte_data(PCA9685_ADDR, ALL_LED_ON_H, on >> 8);
    write_byte_data(PCA9685_ADDR, ALL_LED_OFF_L, off & 0xFF);
    write_byte_data(PCA9685_ADDR, ALL_LED_OFF_H, off >> 8);
}

void set_dio(byte chnl, short state) {
  if (state == 0) {
    write_byte_data(PCA9685_ADDR, LED0_ON_L  + (4 * chnl), 0x00);
    write_byte_data(PCA9685_ADDR, LED0_ON_H  + (4 * chnl), 0x00);
    write_byte_data(PCA9685_ADDR, LED0_OFF_L  + (4 * chnl), 0x00);
    write_byte_data(PCA9685_ADDR, LED0_OFF_H  + (4 * chnl), 0x10);
  }
  else {
    write_byte_data(PCA9685_ADDR, LED0_ON_L  + (4 * chnl), 0x00);
    write_byte_data(PCA9685_ADDR, LED0_ON_H  + (4 * chnl), 0x10);
    write_byte_data(PCA9685_ADDR, LED0_OFF_L  + (4 * chnl), 0x00);
    write_byte_data(PCA9685_ADDR, LED0_OFF_H  + (4 * chnl), 0x00);
  }
}
