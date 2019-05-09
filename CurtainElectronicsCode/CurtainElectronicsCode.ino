#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "esp8266HttpRequest.h"
#include <ESP8266WebServer.h>
#include <EEPROM.h>

/* Motor */
#include <DRV8833.h>
DRV8833 driver = DRV8833();
int motorSpeed = 1500;
#define motorPin1 10
#define motorPin2 9
/* Motor end */


#define photoResPin A0

bool isRolling;

unsigned long disconnectTime, timeToSleep;

int connectionTries
  ,wifiMillis
  ,wifiConLedMillis
  ,lastActivityMillis

  ,mainWifiSSIDStart = 4 //32 chars
  ,mainWifiPassStart = 36 //64 chars
  ,backupWifiSSIDStart = 100 //32 chars
  ,backupWifiPassStart = 132 //64 chars
  ,apiKeyStart = 196

  ,lastKnownMotorPosEepromStart = 260
  ,maxMotorPosEepromStart = 270
  
  ,errorEepromStart = 280;

String deviceIP
       ,colorFading
       ,deviceMAC
       ,unsentErrorMsg

       ,apiKey = "493dc288-0b49-42f7-82f4-1770bff1fd36"

       ,saved_ssid = ""
       ,saved_pass = ""
       
       ,wifiSSID
       ,wifiPassword
       
       ,mainWifiSSID
       ,mainWifiPass
       
       ,backupWifiSSID
       ,backupWifiPass
       
       ,rollDirection
       ,lastKnownRollDirection;


/*Soft AP*/
ESP8266WebServer server(80);
WiFiClient client;
IPAddress theIP(1, 2, 3, 4);
IPAddress NMask(255, 255, 255, 0);

WebRequest WR; //Web request

const String software_version = "v0.0.1"
             ,thumbprint = ""
             ,api_key = "4d9a4cfb-3b8f-482e-9c88-f0d6d0923d8f";

const int courtain_id = 1;
const char* network_name = "Smart Curtain #1";

const bool debug = true
           ,doSSL = false;

bool isAP = false, hasUnsentError = false, hasStartedEeprom, isInitialSetup = true, wasConnected = false, isSleeping = false, apStart = false, hasObstacle, isTop;

void debugLog(String message, bool ln = true) {
  if(debug) {
    if(ln) {
      Serial.println(message);
    } else {
      Serial.print(message);
    }
  }
}

void setup() {
  Serial.begin(9600);

  debugLog("[Smart Gardin " + software_version + " initiated]#");
  
  if(!hasStartedEeprom) {
    hasStartedEeprom = true;
    EEPROM.begin(512);
    debugLog("Started EEPROM");
  }
  Serial.println("Hejsa");

  //pinMode(photoResPin, INPUT);
  
  driver.attachMotorA(motorPin1, motorPin2);  

  Serial.println("Hejsa 2");
  
  SetupLEDS();
  RotarySetup();

  Serial.println("Hejsa 3");

  first_setup();
  Serial.println("Hejsa 4");
  prepareSetup();
  Serial.println("Hejsa 5");
}

void loop() {
  CheckLedOff();
  RotaryLoop();
  
  if (isAP) {
    if(!apStart) {
      apStart = true;
      debugLog("AP loop initiated");

      APFade();
    }
    server.handleClient();
  } else {
    //Check for error
    if (WiFi.status() == WL_CONNECTED) {
      if (millis() - lastActivityMillis > 5000 && !isRolling) {
        getWebUpdates();
      }
      
      /*if(hasUnsentError) {
        if(unsentErrorMsg != "") {
          //Eeprom contains an error that has not yet been sent to the database, try again
          debugLog("Sent error in loop");
          sendError(unsentErrorMsg);
        } else {
          hasUnsentError = false;
          debugLog("Had error but error was empty");
        }
      }*/

      /*if(!isSleeping && (millis() - lastActivityMillis > timeToSleep)) {
        putToSleep();
      }*/
    } else {
      if(!isSleeping && !isAP) { // AP could've begun while in the loop
        debugLog("WiFi died. Go to setup. Status is;");
        debugLog(String(WiFi.status()));
        //debugLog("Not sleeping and not connected to internet (not supposed to happen)");
          //WiFi.disconnect();
        //setColor(255,0,0);
          //prepareSetup();
      }
    }

    //Allow courtain to work no matter the WiFi state
    DoRoll();
  }
}

void StartRolling() {
  if (!isRolling || rollDirection != lastKnownRollDirection) {
    if (!hasObstacle) {
      if (rollDirection == "down") {
        if (!IsBottom()) {
          isTop = false;
          isRolling = true;
          
          driver.motorAForward(motorSpeed);
        } else {
          debugLog("Is already at bottom");
        }
      } else if (rollDirection == "up") {
        if (!IsTop()) {
          isRolling = true;
          
          driver.motorAReverse(motorSpeed);
        } else {
          debugLog("Is already at top");
        }
      }

      if (isRolling) {
        debugLog("Start roll " + rollDirection);
        lastKnownRollDirection = rollDirection;
      }
    } else {
      ObstacleFound();
      debugLog("Could not start roll; obstacle detected");
    }
  }
}

void StopRolling() {
  if (isRolling) {
    driver.motorAStop();
    debugLog("Roll stopped");
    isRolling = false;
    
    lastActivityMillis = millis();
  }
}

void ObstacleFound() {
  hasObstacle = true;
  
  setColorRed();
  delay(50);
  ledOff();
  delay(50);
  setColorRed();
  delay(50);
  ledOff();
}

unsigned long lastAnalogRead;

void DoRoll() {
  /*if (lastAnalogRead < millis() - 200) {
    lastAnalogRead = millis();
    int photoResVal = analogRead(photoResPin);

    //Serial.println(photoResVal);

    if (photoResVal < 400) {
      if (!hasObstacle) {
        //ObstacleFound();

        debugLog("Obstacle detected!");
      }
      StopRolling();
    } else {
      hasObstacle = false;
      //StartRolling();
    }
  }*/
}
