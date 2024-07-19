/*
 Ethan Hartman 
 4/21/24
 CSC230 - Using Thingspeak API to collect and visualize data
 ephartman1@gmail.com
*/


#include "ThingSpeak.h"
#include "secrets.h"
#include "M5StickCPlus2.h"
#include "WiFi.h"

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
int aCount = 0;
unsigned long duration = 30000;
unsigned long startTime = 0;

WiFiClient  client;

void setup() {
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  auto cfg = M5.config();
  StickCP2.begin(cfg);
  StickCP2.Display.setRotation(1);
  StickCP2.Display.setTextColor(GREEN);
  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.setFont(&fonts::FreeSansBold9pt7b);
  StickCP2.Display.setTextSize(1);
}

void loop() {
  StickCP2.update();
  StickCP2.Display.clear();
  StickCP2.Display.setCursor(0, 10);

  StickCP2.Display.clear();
  StickCP2.Display.setCursor(0, 10);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass); 
    delay(5000);
  }

  StickCP2.Display.setCursor(0, 70);

  if (StickCP2.BtnA.wasPressed()) {
    if (startTime == 0) {
      startTime = millis(); // Start counting time when first button press is detected
    }

    // Increment button counts
    if (StickCP2.BtnA.wasPressed()) {
      StickCP2.Display.clear();
      StickCP2.Display.setCursor(0, 70);
      StickCP2.Display.printf("Button-A pressed\n");
      delay(2000);
      aCount++;
    }

    // Check if duration has elapsed
    if (millis() - startTime >= duration) {
      // Send button counts to ThingSpeak
      recordPress(1, aCount);

      // Reset counts and start time for next duration
      aCount = 0;
      startTime = 0;
    }
  }
}

void recordPress(int field, int numPresses) {
  int httpCode = 0;
  while (httpCode != 200) {
    StickCP2.Display.setCursor(0, 70);
    StickCP2.Display.clear();
    StickCP2.Display.printf("error");
    ThingSpeak.writeField(myChannelNumber, field, numPresses, myWriteAPIKey);
    delay(5000);
  } 
    StickCP2.Display.printf("Channel write successful.\n");
    delay(5000);
}
