//Made for IR-controller integration - dropped, and no longer supported. Only for testing purposes

/*#include <IRrecv.h>
#include <IRremoteESP8266.h>

#define kRecvPin D5

unsigned long lastSignal;

const uint8_t kTimeout = 15;
const uint16_t kCaptureBufferSize = 1024;
const uint16_t kMinUnknownSize = 12;

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;
String lastHex, upHex = "ffe01f", downHex = "ffa857", allUp = "ff22dd", allDown = "ff2fd";

bool hasDoneIR, IRthisRound;

void IRSetup() {
  irrecv.enableIRIn();
}

void IRloop() {
  IRthisRound = false;
  
  if(irrecv.decode(&results)) {
    uint64_t number = results.value;
    unsigned long long1 = (unsigned long)((number & 0xFFFF0000) >> 16 );
    unsigned long long2 = (unsigned long)((number & 0x0000FFFF));
    
    String hex = String(long1, HEX) + String(long2, HEX);

    //Serial.println(hex);
    
    if ((hex == upHex || hex == downHex || hex == allUp || hex == allDown) || (hex == "ffffffff" && (lastHex == upHex || lastHex == downHex || lastHex == allDown || lastHex == allUp))) {
      HexRead(hex == "ffffffff" ? lastHex : hex);
      lastHex = hex == "ffffffff" ? lastHex : hex;
    } else {
      lastHex = "";
    }
    
    irrecv.resume();
  }

  if ((isRolling && hasDoneIR && !IRthisRound && lastSignal < millis() - 250) && (lastHex == upHex || lastHex == downHex)) {
    //Stop rolling after x ms if signal was just a bit up/down and not all the way
    StopRolling();
  }

  if (hasDoneIR && !IRthisRound && lastSignal < millis() - 4000 && !isRolling) {
    //more than 4 sec since it last received IR signal - send current pos to server
    //Send web request
    Serial.println("Web request; current courtain position");
    hasDoneIR = false;
  }

  if (IRthisRound) {
    lastSignal = millis();
  }
}

void HexRead(String hex) {
  if (hex == upHex) {
    Serial.println("UP BITCCH");
    
    rollDirection = "up";
    StartRolling();
  } else if (hex == downHex) {
    Serial.println("DOWN BITCCH");

    rollDirection = "down";
    StartRolling();
  } else if (hex == allUp) {
    Serial.println("All the way up (bitch)");

    rollDirection = "up";
    StartRolling();
  } else if (hex == allDown) {
    Serial.println("All the way down (bitch)");

    rollDirection = "down";
    StartRolling();
  } else {
    debugLog("Unknown IR command");
    return;
  }
  
  IRthisRound = true;
  hasDoneIR = true;
}*/
