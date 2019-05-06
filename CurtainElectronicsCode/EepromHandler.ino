void ClearTheEeprom(int from, int to) {
  Serial.println("\nClearing Eeprom from " + String(from) + " to " + String(to));
  for (int i = from; i < to; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  delay(500);
}


void SaveError(String error) {
  //No access to internet, save the error in Eeprom and send it when has internet access

  for (int i = errorEepromStart; i < error.length(); ++i) {
    EEPROM.write(i, error[i]);
  }
  EEPROM.commit();
  delay(2000);
  Serial.println("Saving error-message in Eeprom");
  if(error != "") {
    hasUnsentError = true;
    unsentErrorMsg = error;
  }
}

void FirstSetup() {
  //Function to run the very first time
  String mustBeMsg = "done"
         ,msg = "";
  for (int i = 0; i < mustBeMsg.length(); ++i) {
    msg += char(EEPROM.read(i));
  }
  if(mustBeMsg != msg) {
    //Is first time on this EEPROM storage. Make sure it's cleared, as it can contain a lot of shit by default
    Serial.println("- First time setup");
    ClearTheEeprom(0, EEPROM.length());
    for (int i = 0; i < mustBeMsg.length(); ++i) {
      EEPROM.write(i, mustBeMsg[i]);
    }
    EEPROM.commit();
    delay(500);
  }

  //unsentErrorMsg
}

String ReadEeprom(int from, int to) {
  String msg = "";
  for (int i = from; i < to; ++i) {
    char c = char(EEPROM.read(i));
    msg += c;
  }
  
  return msg;
}
String ReadEeprom(int from, int to, char stopAt) {
  String msg = "";
  for (int i = from; i < to; ++i) {
    char c = char(EEPROM.read(i));
    if (c == stopAt) {
      break; //Reached "stop at" character
    }
    msg += c;
  }
  
  return msg;
}

void SetEeprom(int from, int to, String message) {
  //Serial.println("Writing \"" + message + "\" from " + String(from ) + " to " + String(to));
  int reali = 0;
  for (int i = from; i < to; ++i) {
    if (reali < message.length()) {
      EEPROM.write(i, message[reali]);
    }
    reali++;
  }
  EEPROM.commit();
  delay(500);
}

void SetMainNetwork(String ssid, String passwd) {
  if (ssid != mainWifiSSID) {
    //Set new wifi ssid
    SetEeprom(mainWifiSSIDStart, mainWifiPassStart, ssid);
  }
  if (passwd != mainWifiPass) {
    //Set new wifi pass
    SetEeprom(mainWifiPassStart, backupWifiSSIDStart, passwd);
  }
}

void ClearEeprom() {
  debugLog("\nClearing Eeprom\n");
  for (int i = 4; i < errorEepromStart; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void first_setup() {
  //Function to run the very first time
  String mustBeMsg = "done"
         ,msg = "";
  for (int i = 0; i < mustBeMsg.length(); ++i) {
    msg += char(EEPROM.read(i));
  }
  if(mustBeMsg != msg) {
    debugLog("- First time setup");
    ClearEeprom();
    for (int i = 0; i < mustBeMsg.length(); ++i) {
      EEPROM.write(i, mustBeMsg[i]);
    }
    EEPROM.commit();
    delay(500);
  }
}
