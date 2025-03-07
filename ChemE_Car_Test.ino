#include <LTR390.h>
#include <Wire.h>

#define I2C_ADDRESS 0x53

LTR390 ltr390(I2C_ADDRESS);

const int RED_SSR = 12;
const float LUX_THRESHOLD = 0.5;
// SSR flag false -> SSR off -> Motor on.
// SSR flag true -> SSR on -> Motor off.
bool ssr_flag = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_SSR, OUTPUT);
 // pinMode(13, OUTPUT);  
  Serial.begin(115200);
  Wire.begin();
  if(!ltr390.init()){
    Serial.println("LTR390 not connected!");
  }

  ltr390.setMode(LTR390_MODE_ALS);
  ltr390.setGain(LTR390_GAIN_3);
  
  // highest possible resolution to make sure we stop the cart
  // as close to the instant of the reaction running it's course
  // as possible. If this causes an unacceptable slowdown in the 
  // operation of the LTR, feel free to change back to default 
  // value of 18-bid
  ltr390.setResolution(LTR390_RESOLUTION_20BIT);

  // SSR's initial state should be off (governed by ssr_flag). 
  // SSR being off means that power can flow from battery to motor
  // and motor is on.
  digitalWrite(RED_SSR, LOW);
}

void loop() {
  // if the motor is supposed to be on
  if (!ssr_flag) { 
    if (ltr390.newDataAvailable()) {
      float lux = ltr390.getLux();
      Serial.print(""); 
      Serial.println(lux);
      // check if iodine clock reaction has not yet run its course
      if (lux <= LUX_THRESHOLD) {
        // if so, the motor should be stopped and there is no more need
        // to check the reaction's state.
        ssr_flag = true; 
        Serial.println("Threshold hit");
        digitalWrite(RED_SSR, HIGH);
      }           
    }
  }
}
