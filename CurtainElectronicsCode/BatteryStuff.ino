void putToSleep() {
  debugLog("Is being put to sleep...");
  lastActivityMillis = 0;
  isSleeping = true;
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(50);
  WiFi.forceSleepBegin(0);
  delay(50);
  debugLog("Sleeping\n");
}

void sleepWake() {
  WiFi.forceSleepWake();
  delay(100);
}
