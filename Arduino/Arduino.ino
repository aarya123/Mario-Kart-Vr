#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

#define ADAFRUIT_CC3000_IRQ 2
#define ADAFRUIT_CC3000_VBAT A3
#define ADAFRUIT_CC3000_CS 8
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);

#define WLAN_SSID "Hackathon24"
#define WLAN_PASS "redmond2015"
#define WLAN_SECURITY WLAN_SEC_WPA2


void setup() {
  Serial.begin(115200);
  Serial.println(F("\nInitialising the CC3000 ..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    while (1);
  }
  /*Serial.println(F("\nDeleting old connection profiles"));
  if (!cc3000.deleteProfiles()) {
    Serial.println(F("Failed!"));
    while (1);
  }*/
  listSSIDResults();
  char *ssid = WLAN_SSID;
  Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while (1);
  }
  Serial.println(F("Connected!"));
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
  uint32_t ip = 0;
  Serial.print(F("www.adafruit.com -> "));
  while  (ip  ==  0)  {
    if  (!  cc3000.getHostByName("www.adafruit.com", &ip))  {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }
  cc3000.printIPdotsRev(ip);
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");
  uint8_t replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  if (replies)
    Serial.println(F("Ping successful!"));
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}

void listSSIDResults(void)
{
  uint32_t index;
  uint8_t valid, rssi, sec;
  char ssidname[33];
  if (!cc3000.startSSIDscan(&index)) {
    Serial.println(F("SSID scan failed!"));
    return;
  }
  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));
  while (index) {
    index--;
    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));
  cc3000.stopSSIDscan();
}
