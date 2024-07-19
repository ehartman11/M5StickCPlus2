#include <ArduinoJson.h>
#include <M5StickCPlus2.h>
#include <IOXhop_FirebaseESP32.h>
#include "secrets.h"
#include <WiFi.h>

int selected = 0;
String actions[] = {"Eat", "Sleep", "Cry", "Potty", "Awake"};
int entryNum = 0;
String entryYear, entryMonth, entryDay, entryHour, entryMin;
int lastBatteryPercentage = 100;
bool heartbeat = false;

void setup() {
  M5.begin();
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  delay(1000);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  StickCP2.Rtc.setDateTime( { { 2024, 5, 22 }, { 13, 37, 00 } } );
  StickCP2.Display.setBrightness(8);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  StickCP2.Display.setCursor(10, 10);
  StickCP2.Display.print("Menu:");
  printMenu();
  StickCP2.Display.setCursor(160, 10);
  StickCP2.Display.print("100%");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  updateHeartbeat();
  updateBatteryDisplay();
  handleUserInput();
  delay(500);  
}

void updateHeartbeat() {
  StickCP2.Display.fillRect(210, 10, 25, 16, BLACK); 
  StickCP2.Display.setCursor(210, 10);
  if (heartbeat) {
    StickCP2.Display.print("*");
    heartbeat = false;
  } else {
    StickCP2.Display.print(" ");
    heartbeat = true;
  }
}

void updateBatteryDisplay() {
  float voltage = M5.Power.getBatteryVoltage();
  int currentBatteryPercentage = mapVoltageToPercentage(voltage);
  
  if ((currentBatteryPercentage <= 15 && currentBatteryPercentage != lastBatteryPercentage) ||
      abs(currentBatteryPercentage - lastBatteryPercentage) >= 5) {
    StickCP2.Display.fillRect(160, 10, 94, 20, BLACK); 
    StickCP2.Display.setCursor(160, 10);
    StickCP2.Display.print(String(currentBatteryPercentage) + "%");
    lastBatteryPercentage = currentBatteryPercentage;
  }
}

int mapVoltageToPercentage(float voltage) {
  float maxVoltage = 4200;
  float minVoltage = 3300;
  if (voltage > maxVoltage) voltage = maxVoltage;
  if (voltage < minVoltage) voltage = minVoltage;
  return (int)((voltage - minVoltage) / (maxVoltage - minVoltage) * 100);
}

void handleUserInput() {
    auto t = time(nullptr);
  {
    auto dt = StickCP2.Rtc.getDateTime();
    entryHour = String(dt.time.hours);
    entryMin = String(dt.time.minutes);
    entryYear = String(dt.date.year);
    entryMonth = String(dt.date.month);
    entryDay = String(dt.date.date);
  }

  StickCP2.update();
  if (StickCP2.BtnB.wasPressed()) {
    if (selected < 4) {
      selected += 1;
    } else {
      selected = 0;
    }
    StickCP2.Display.fillRect(0, 30, 320, 90, BLACK);
    printMenu();
  }

  if (StickCP2.BtnA.wasPressed()) {
    StickCP2.Display.setCursor(70, 117);
    StickCP2.Display.print("Logging: " + actions[selected]);
    entryNum += 1;
    Firebase.setInt("Number_" + String(entryNum), entryNum);
    Firebase.setString("Year_" + String(entryNum), entryYear);
    Firebase.setString("Month_" + String(entryNum), entryMonth);
    Firebase.setString("Day_" + String(entryNum), entryDay);
    Firebase.setString("Hour_" + String(entryNum), entryHour);
    Firebase.setString("Min_" + String(entryNum), entryMin);
    Firebase.setString("Action_" + String(entryNum), actions[selected]);
    StickCP2.Display.fillRect(0, 117, 320, 18, BLACK);
    StickCP2.Display.setCursor(70, 117);
    StickCP2.Display.print(actions[selected] + " logged.");
    delay(3000);
    StickCP2.Display.fillRect(0, 117, 320, 18, BLACK);
  }
}

void printMenu() {
  int x = 10;
  int y = 30;
  for (int i = 0; i < 5; i++) {
    StickCP2.Display.setCursor(x, y);
    if (i == selected) {
      StickCP2.Display.print("> " + actions[i] + "\n");
    } else {
      StickCP2.Display.print(actions[i] + "\n");
    }
    y += 18;
  }
}