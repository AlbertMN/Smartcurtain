String encryptionTypeStr(uint8_t authmode) {
  switch (authmode) {
    case ENC_TYPE_NONE:
      return "OPEN";
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA_PSK";
    case ENC_TYPE_CCMP:
      return "WPA2_PSK";
    case ENC_TYPE_AUTO:
      return "WPA_WPA2_PSK";
    default:
      return "UNKOWN";
  }
}

bool updateSettings(JsonObject& root) {
  if(root.containsKey("settings")) {
    JsonObject& settings = root["settings"];
    /*reclickInterval = settings["reclickInterval"];
    Serial.println("set_reclick_interval:" + String(reclickInterval));
    bool resetToAp = settings["resetToAP"];
    timeToSleep = settings["timeToSleep"];
    debugLog("Synced settings");

    if(resetToAp) {
      debugLog("Web reset to AP");
      ClearEeprom();
      blinkWaitTime = 30;
      setup_ap();
      return true;
    }*/
  }
  return false;
}

//Get updates from website
void getWebUpdates() {
  String url = "http://gardin.albe.pw/api/curtain/get_pos.php?curtain_id=" + String(courtain_id) + "&api_key=" + String(apiKey);
  JsonObject& request = WR.SendRequest(url, doSSL, thumbprint, "", "");
  bool theStatus = request["status"];
  String msg = request["message"];

  if (theStatus) {
    debugLog("- Update web request success");

    int newPercentage = request["percentage"];
    int newSpeed = request["speed"];
    int newEncoderPos = request["encoder_pos"];
    int newMaxPos = request["max_pos"];

    if (newEncoderPos != -999) {
      debugLog("- Set encoder position to " + String(newEncoderPos));
      currentPos = newEncoderPos;
    }

    if (newMaxPos != maxPos) {
      debugLog("- Set new max position to " + String(newMaxPos) + ", pos is; " + String(GetPercentage()) + "%");
      maxPos = newMaxPos;
    }

    if (newSpeed != motorSpeed) {
      debugLog("- Set new motor speed to " + String(newSpeed));
      motorSpeed = newSpeed;
    }

    if (GetPercentage() != newPercentage && newPercentage != desiredPercentage) {
      if (!(GetPercentage() == newPercentage || (GetPercentage() >= newPercentage - 3 && GetPercentage() <= newPercentage + 3))) {
        debugLog("- New desired percentage; " + String(newPercentage) + ", current percentage is " + String(GetPercentage()) + "%");
        desiredPercentage = newPercentage;
      }
    }

    lastActivityMillis = millis();
    return;
  } else {
    debugLog(msg);
  }
}

//Send call to website saying "I'm alive!" after being turned off / off the WiFi
int wakeCallTries = -1;

void sendWakeCall() {
  wakeCallTries++;
  debugLog("Starting wake-call");

  if(wakeCallTries > 0) {
    delay(10000);
    wakeRequest();
  } else {
    wakeRequest();
  }
}

void wakeRequest() {  
  //String url = "http://gardin.albe.pw/api/curtain/wake_call.php?courtain_id=" + String(courtain_id) + "&wifi_ssid=" + saved_ssid.c_str() + "&ip=" + deviceIP + "&mac=" + deviceMAC + "&wifi_millis=" + String(wifiMillis);
  String url = "http://gardin.albe.pw/api/curtain/wake_call.php";
  
  JsonObject& request = WR.SendRequest(url, doSSL, thumbprint, "", "");
  bool theStatus = request["status"];
  String msg = request["message"];

  if (theStatus) {
    /*if(updateSettings(request)) {
      //Reset to AP
      return;
    }*/
    debugLog("- Has access to internet & the Smart Courtain API, beginning loop\n\n");
    lastActivityMillis = millis();

    return;
  } else {
    Serial.println(msg);
    //sendError("Failed to send wake call; " + msg);
  }
  sendWakeCall();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i < server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void on_submit(){
  debugLog("Submitting");
  String s;
  if (server.hasArg("wifi_ssid") && server.hasArg("wifi_pass")) {
    saved_ssid = server.arg("wifi_ssid");
    saved_pass = server.arg("wifi_pass");
  }
  
  if(saved_ssid.length() > 1) {
    ClearEeprom();
    delay(10);
    debugLog("Setting Eeprom values");
    for (int i = 0; i < saved_ssid.length(); ++i) {
      EEPROM.write(4+i, saved_ssid[i]);
    }
      
    for (int i = 0; i < saved_pass.length(); ++i) {
      EEPROM.write(36+i, saved_pass[i]);
    }
    
    s = "\r\n\r\n<!DOCTYPE HTML>\r\n<html><h1>Submitted</h1> "
    "<p>Password Saved... Reset to boot into new wifi</html>\r\n\r\n";
  } else {
    s = "Length is not > 1 :( (" + saved_ssid + ")";
  }
  server.send(200,"text/html", s);
  EEPROM.commit();
  setColor(0,255,0);
  //Wait 1 second, then restart
  delay(2000);
  Serial.println("#ap_done#");
  ledOff();
  prepareSetup();
}
