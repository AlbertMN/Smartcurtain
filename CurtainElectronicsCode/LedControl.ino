int lastLedMillis = 0, blinkWaitTime = 30, brightness, fadeAmount = 8;
unsigned long ledActiveFor, ledActivated;
bool ledTimeout, lightOn;

#define redLed D6
#define greenLed D7
#define blueLed D0

void setColor(int redValue, int greenValue = 0, int blueValue = 0, int timeout = 0) {
  if (timeout != 0) {
    ledActiveFor = timeout;
    ledTimeout = true;
    ledActivated = millis();
  } else {
    ledTimeout = false;
    ledActivated = 0;
    ledActiveFor = 0;
  }

  lightOn = true;
  digitalWrite(redLed, redValue);
  digitalWrite(greenLed, greenValue);
  digitalWrite(blueLed, blueValue);
}

void setColorRed() {
  //"setColor" is "not defined" in the main file due to it's arguments for some reason. This "fixes" it.
  setColor(255, 0,0,0);
}

void SetupLEDS() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
}

void APFade() {
  if(millis() - lastLedMillis > blinkWaitTime) {
    lastLedMillis = millis();
    if(colorFading != "") {
      if(brightness >= 0) {
        if(colorFading == "red") {
          digitalWrite(redLed, brightness);
        } else if(colorFading == "green") {
          digitalWrite(greenLed, brightness);
        } else if(colorFading == "blue") {
          digitalWrite(blueLed, brightness);
        }
      }
    
      brightness = brightness + fadeAmount;
    
      // reverse the direction of the fading at the ends of the fade:
      if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      //Serial.println(brightness);
      // wait for 30 milliseconds to see the dimming effect
      delay(30);
    }
  }
}

void ledOff() {
  setColor(0,0,0);
}

void CheckLedOff() {
  if (ledTimeout && millis() - ledActivated > ledActiveFor) {
    ledOff();
  }
}
