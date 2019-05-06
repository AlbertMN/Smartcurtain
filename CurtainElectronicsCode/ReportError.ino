//int errorEepromStart = 125;
String entireError;

void checkSendError() {
  //Check if Eeprom has an error stored to be sent
  String error = "";
  for (int i = errorEepromStart; i < EEPROM.length(); ++i) {
    error += char(EEPROM.read(i));
  }
  entireError = error;
  
  if(error != "") {
    sendError(error);
  } else {
    hasUnsentError = false;
  }
}

void saveError(String error) {
  //No access to internet, save the error in Eeprom and send it when has internet access
  if(entireError != "")
    error += "#"; //Seperate errors by #
  if(error.length() <= EEPROM.length()) {
    for (int i = (errorEepromStart + entireError.length()); i < error.length(); ++i) {
      EEPROM.write(i, error[i]);
    }
    entireError += error;
    EEPROM.commit();
    delay(2000);
    debugLog("\nSaving error-message in Eeprom");
    if(error != "") {
      hasUnsentError = true;
      unsentErrorMsg = entireError;
    }
  } else {
    debugLog("Error string longer than EEPROM length");
  }
}

void clearEepromError() {
  //light_up("blue");
  debugLog("\nClearing error saved in Eeprom\n");
  for (int i = errorEepromStart; i < EEPROM.length(); ++i) {
    EEPROM.write(i, 0);
  }
  hasUnsentError = false;
  unsentErrorMsg = "";
  entireError = "";
  EEPROM.commit();
  delay(500);
}

void sendError(String errorMessage) {
  setColor(255,0,0);
  debugLog("Sending error: \"" + errorMessage + "\"\n\n");

  if (WiFi.status() == WL_CONNECTED) {
    String url = "http://gardin.albe.pw/error_report.php?board_id=" + String(courtain_id) + "&error_message=" + errorMessage;
    url.replace(" ", "%20");
    url.replace("\n", "");

    debugLog("Reporting error\n\n");

    JsonObject& request = WR.SendRequest(url, doSSL, thumbprint, "X-API-KEY", api_key);
    bool theStatus = request["status"];
    String msg = request["message"];
  
    if (theStatus) {
      if(updateSettings(request)) {
        //Reset to AP
        return;
      }
      clearEepromError();
      hasUnsentError = false;
      debugLog("Successfully sent error sent to database");
      //ledOff();
      return;
    } else {
      Serial.println(msg);
      sendError("Error report request failed; " + msg);
    }
  }
  ledOff();

  debugLog("Failed to send error");
  //clearEepromError(); < why'd we clear here?
}
