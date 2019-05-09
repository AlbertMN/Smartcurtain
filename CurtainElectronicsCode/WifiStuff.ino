void APSetup() {
  ClearEeprom();
  debugLog("- Setting up AP");
  if(setup_ap()) {
    debugLog("\nAP successfully set up. Ready.");
  } else {
    sendError("Failed to start AP! Really not supposed to happen");
    setColor(255,0,0);
    ESP.restart();
    delay(120000);
    prepareSetup();
  }
}

void prepareSetup() {
  Serial.println("Preparing wifi connect...");
  if (prepare_wifi_connect()) {
    //Has WiFi in Eeprom - WiFi connected

    if (isInitialSetup)
      setColor(0,0,255);
      
    sendWakeCall();

    if (isInitialSetup) {
      isInitialSetup = false;
      setColor(0,255,0, 2000);
      delay(200);
    } else {
    }
    
    //Serial.println("#request_sent:0,0,0,0,0#"); //Tell nano "I'm ready!" - not needed anymore
  } else {
    APSetup();
  }
}

bool setup_ap() {
  debugLog("Setting up AP...");
  if(WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_OFF);
    delay(200);
  }
  
  WiFi.softAPConfig(theIP, theIP, NMask);
  WiFi.mode(WIFI_AP_STA);
  bool result = WiFi.softAP(network_name, "", 1, false, 1);
  if (result == true) {
    APFade();
    isInitialSetup = true;
    debugLog("AP set up. IP: ", false);
    debugLog(WiFi.softAPIP().toString());

    server.on("/", []() {
      server.send(200, "text/html", print_website());
    });
    server.on("/submit", &on_submit);
    server.onNotFound(handleNotFound);
    server.begin();
    debugLog("Server listening...");
    wasConnected = false;
    isAP = true;
    
    return true;
  } else {
    debugLog("Failed to set up AP!?");
    return false;
  }
}

bool prepare_wifi_connect() {
  //read SSID
  connectionTries = 0;
  
  debugLog("\nGetting WiFi\n");
  saved_ssid = "";
  saved_pass = "";

  //Start at 4
  for (int i = 4; i < 36; ++i) {
    saved_ssid += char(EEPROM.read(i));
  }
  for (int i = 36; i < 100; ++i) {
    saved_pass += char(EEPROM.read(i));
  }
  if (saved_ssid != "") {
    Serial.println("Got WiFi ssid (" + String(saved_ssid) + "), beginning connection...");
    
    WiFi.hostname("Smart Curtain");
    WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());
    WiFi.disconnect();
    //WiFi fix: https://github.com/esp8266/Arduino/issues/2186
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);

    Serial.println("Got to phase 1...");
    
    WiFi.mode(WIFI_STA);
    WiFi.hostname("Smart Curtain");
    wifiMillis = millis();
    WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());

    Serial.println("Got to phase 2...");
    
    debugLog("Connecting to WiFi with:");
    debugLog("SSID: ", false);
    debugLog(saved_ssid.c_str());
    debugLog("pass: ", false);
    debugLog(saved_pass.c_str());

    tryWifiConnect(); //sometimes doesn't return true when the internet is connected, don't use its return
    bool hasInternetNow = WiFi.status() == WL_CONNECTED;
    
    if(hasInternetNow) {
      deviceIP = WiFi.localIP().toString();
      deviceMAC = WiFi.macAddress();
      debugLog("#Connected to WiFi with SSID: " + WiFi.SSID()  + " in " + wifiMillis + "ms\n#");
      wasConnected = true;
      lastActivityMillis = millis();
      if(isAP) {
        isAP = false;
      }
      return true;
    } else {
      if(wasConnected) {
        //Has been connected to this WiFi, don't setup AP just yet

        if (isInitialSetup)
          setColor(255,0,0);
        
        debugLog("Failed to connect to WiFi, trying again in 2 minutes#");
        connectionTries++;

        disconnectTime = millis();
        while(connectionTries <= 60) { //Try/wait for two hours
          delay(5);
          if(millis() - disconnectTime > 120000) { //if 2 minutes have passed
            disconnectTime = millis();
            if(tryWifiConnect()) {
              return true;
            }
            connectionTries++;
          }
        }
        //Failed too many times, going to AP mode
        debugLog("Tried too many times");
        wasConnected = false;
        return false;
      } else {
        debugLog("Was not connected#");
      }
      //sendError("Failed to connect to WiFi"); < why'd we do this...?
    }
  } else {
    debugLog("No SSID specified#");
  }
  if (isInitialSetup)
    setColor(255,0,0,9990);
  delay(10000);
  return false;
}

bool tryWifiConnect() {
  int c = 1
      ,max_tries = 100;
  debugLog("\nWaiting for Wifi (" + WiFi.SSID() + ") to connect... Trying " + String(max_tries) + " times#");

  /* 0 : WL_IDLE_STATUS
     1 : WL_NO_SSID_AVAIL
     3 : WL_CONNECTED
     4 : WL_CONNECT_FAILED
     6 : WL_DISCONNECTED */

  if (isInitialSetup)
    setColor(50,0,255);
  bool lightOn = false;
  while (c <= max_tries+1) {
    if (WiFi.status() == WL_CONNECTED) {
      wifiMillis = millis() - wifiMillis;
      isSleeping = false;
      return true;
    } else if (WiFi.status() == WL_NO_SSID_AVAIL) {
      debugLog("Network not found");
      return false;
    } else if (WiFi.status() == WL_CONNECT_FAILED) {
      debugLog("Incorrect password");
      return false;
    }

    if(!isSleeping) {
      if(millis() - wifiConLedMillis > 250) {
        wifiConLedMillis = millis();
        if(lightOn) {
          if (isInitialSetup)
            ledOff();
          lightOn = false;
        } else {
          lightOn = true;
          if (isInitialSetup)
            setColor(50,0,255);
        }
      }
    }
    delay(200);
    
    //debugLog(String(c) + "'th try. WiFi status: ", false); //Nano can't function while this happens
    //debugLog(String(WiFi.status()));
    c++;
  }
  
  debugLog("\nConnect timed out, opening AP");
  return false;
}
