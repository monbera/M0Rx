#include <PCA9685.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Wire.begin();
  Wire.setClock(400000);
  software_reset();
  pwm_init();
  set_pwm_freq(50);

}

void loop() {
  set_pwm(0, 0, 307);
  delay(1000);
  set_pwm(0, 0, 207);
  delay(1000);
  set_pwm(0, 0, 407);
  delay(1000);

}
