#include <LTR390.h>
#include <Wire.h>

#define I2C_ADDRESS 0x53

LTR390 ltr390(I2C_ADDRESS);

const int RED_SSR = 12;
const int ON_SWITCH = 2;
const float LUX_THRESHOLD = 2;
// SSR flag false -> SSR off -> Motor on.
// SSR flag true -> SSR on -> Motor off.
bool ssr_flag = false;
bool on = false;
unsigned long start = 0;
unsigned long end = 0;
unsigned long countdown = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_SSR, OUTPUT);
  pinMode(ON_SWITCH, INPUT);
 // pinMode(13, OUTPUT);  
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  if(!ltr390.init()){
    Serial.println("LTR390 not connected!");
  }

  ltr390.setMode(LTR390_MODE_ALS);
  ltr390.setGain(LTR390_GAIN_18);
  
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
  if(on){
    if (ltr390.newDataAvailable()) {
        // check if iodine clock reaction has not yet run its course
        if (ltr390.getLux() <= LUX_THRESHOLD) {
          // if so, the motor should be stopped and there is no more need
          // to check the reaction's state.
          digitalWrite(RED_SSR, HIGH);
        } else {
          digitalWrite(RED_SSR, LOW);
        }
      }
  } else {
    digitalWrite(RED_SSR, HIGH);
  } 
  on = digitalRead(ON_SWITCH);
}
