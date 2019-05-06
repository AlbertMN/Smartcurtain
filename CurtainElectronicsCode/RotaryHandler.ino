#include <Encoder.h>

Encoder myEnc(D2, D4);

long oldPosition = -999, currentPos = 0, maxPos = 300, desiredPercentage = 999;
int timesNotMoved, stillFor, originalLastSpeed;
bool movedThisTime, hasSavedLastPos;

void RotarySetup() {
  pinMode(D3, INPUT_PULLUP);

  if (maxPos == 0) {
    //Check if a max pos is set in EEPROM, otherwise; calibrate
    String lastKnownMaxPos = ReadEeprom(maxMotorPosEepromStart, errorEepromStart, '#');
    lastKnownMaxPos.replace("#", "");
    maxPos = lastKnownMaxPos.toInt();
  }

  String lastKnownPos = ReadEeprom(lastKnownMotorPosEepromStart, maxMotorPosEepromStart, '#');
  lastKnownPos.replace("#", "");
  currentPos = lastKnownPos.toInt();
  myEnc.write(currentPos);

  debugLog("Last known encoder position was; " + String(currentPos));
}

bool IsTop() {
  return isTop;
}

float GetPercentage() {
  return (float(currentPos) / float(maxPos) * float(100));
}

bool IsBottom() {
  return (currentPos == maxPos);
}

void TopReached() {
  myEnc.write(0);
  currentPos = 0;
  
  isTop = true;
  StopRolling();
}

void RotaryLoop() {  
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    movedThisTime = true;
    hasSavedLastPos = false;

    stillFor = 0;
    timesNotMoved = 0;
    
    oldPosition = newPosition;

    if (!IsBottom() && newPosition == maxPos) {
      //Max pos (bottom) reached!
      debugLog("Bottom (" + String(maxPos) + ") reached!");
      currentPos = maxPos;
      StopRolling();
      isTop = false;

      //Serial.println("Set 'isTop' to false! (" + String(isTop) + ")");
    }

    if (!IsTop() && newPosition == 0) {
      debugLog("Reached top");
      TopReached();
    } else {
      /*Serial.println();
      Serial.println(!IsTop());
      Serial.println(newPosition == 0);
      Serial.println();*/
    }

    if ((!IsBottom() || rollDirection == "up") && (!IsTop() || rollDirection == "down")) {
      /*if (GetPercentage() > 100 || GetPercentage() < 0) {
        StopRolling();
      } else {*/
        currentPos = newPosition;
        debugLog("Rotary pos: " + String(newPosition) + "(" + String(GetPercentage()) + "%)");
      //}
    } else {
      if (IsTop()) {
        currentPos = 0;
        myEnc.write(0);
      } else if (IsBottom()) {
        currentPos = maxPos;
        myEnc.write(maxPos);
      }
    }
  } else {
    stillFor++;

    if (stillFor >= 500 && !hasSavedLastPos) {
      //SAVE LAST KNOWN MOTOR NUMBER      
      SetEeprom(lastKnownMotorPosEepromStart, maxMotorPosEepromStart, (String(myEnc.read()) + "#"));
      hasSavedLastPos = true;

      debugLog("Saved last known encoder location");
    }
  }

  if (desiredPercentage != 999) {
    if (desiredPercentage >= 0 && desiredPercentage <= 100) {
      if (GetPercentage() > desiredPercentage) {
        //Has to roll up to get to new desired percentage
        rollDirection = "up";
        StartRolling();
      } else {
        //Has to roll down
        rollDirection = "down";
        StartRolling();
      }
    } else {
      debugLog("New desired percentage is not between 0-100, resetting");
      desiredPercentage = 999;
    }

    if (isRolling) {
      /*if (rollDirection == "up") { <<<<<< TODO
        if (GetPercentage() <= desiredPercentage + 10 && originalLastSpeed != 0) {
          //Slow down for the last few percents to get a more accurate run
          originalLastSpeed = motorSpeed;
          debugLog(" ---- Set motor speed to 500 ---- ###########");
          motorSpeed = 500;
        }
      } else {
        if (GetPercentage() >= desiredPercentage - 5 && originalLastSpeed != 0) {
          //Slow down for the last few percents to get a more accurate run
          originalLastSpeed = motorSpeed;
          debugLog(" ---- Set motor speed to 500 ---- ########### 2");
          motorSpeed = 500;
        }
      }*/
      
      if (GetPercentage() == desiredPercentage || (GetPercentage() >= desiredPercentage - 3 && GetPercentage() <= desiredPercentage + 3)) {
        debugLog("Reached desired percentage");
        desiredPercentage = 999;
        StopRolling();
        
        //motorSpeed = originalLastSpeed;
        //originalLastSpeed = 0;
      }
    }
  }

  if (isRolling && rollDirection == "up" && !movedThisTime) {
    //Is all the way up
    timesNotMoved++;
    Serial.println("SUPERSONIC " + String(timesNotMoved));
    
    if (timesNotMoved > 25) {
      //Motor still rolling, but rotary encoder has been still for a while - must be at the top
      debugLog("Top reached " + String(timesNotMoved) +"!");
      
      TopReached();
    }
  }

  movedThisTime = false;
}

void Calibrate() {
  
}
