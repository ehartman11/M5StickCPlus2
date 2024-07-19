/**
  Ethan Hartman 
  ephartman1@gmail.com
  CSC230
  Assignment: Understanding the Basic M5StickC Plus2
  Professor Prater
 */

#if defined(ARDUINO)

#define WIFI_SSID     "GoneFishing417"
#define WIFI_PASSWORD "Sparks2Bells417!"
#define NTP_TIMEZONE  "UTC-4"
#define NTP_SERVER1   "0.north-america.pool.ntp.org"
#define NTP_SERVER2   "1.north-america.pool.ntp.org"
#define NTP_SERVER3   "2.north-america.pool.ntp.org"

#include <WiFi.h>

// Different versions of the framework have different SNTP header file names and
// availability.
#if __has_include(<esp_sntp.h>)
#include <esp_sntp.h>
#define SNTP_ENABLED 1
#elif __has_include(<sntp.h>)
#include <sntp.h>
#define SNTP_ENABLED 1
#endif

#endif

#ifndef SNTP_ENABLED
#define SNTP_ENABLED 0
#endif

#include <M5StickCPlus2.h>

void setup(void) {
    StickCP2.begin();

    /// setup RTC ( NTP auto setting )

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    configTzTime(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

#if SNTP_ENABLED
    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        delay(1000);
    }
#else
    delay(1600);
    struct tm timeInfo;
    while (!getLocalTime(&timeInfo, 1000)) {
        Serial.print('.');
    };
#endif

    time_t t = time(nullptr) + 1;  // Advance one second.
    StickCP2.Rtc.setDateTime( { { 2024, 05, 8 }, { 2, 44, 00 } } );

    M5.Lcd.setBrightness(1); // set the screen brightness
    M5.Lcd.fillScreen(BLACK); // Change background to white
    M5.Lcd.setTextSize(2); // Change text size
    M5.Lcd.setTextColor(GREEN); // Change text color to green
    M5.Lcd.setRotation(1); // Rotate text along the long side
    M5.Lcd.setCursor(40, 50); // Set cursor position
    M5.Lcd.print("Ethan Hartman"); // Print name
    M5.Lcd.setTextSize(1); // Change text size to fit the date/time label
}

void loop(void) {

    static constexpr const char* const wd[7] = {"Sun", "Mon", "Tue", "Wed",
                                                "Thr", "Fri", "Sat"};
    // This creates an array that may be indexed using wd[integer of desired position]

    delay(500); // wait .5 sec

    /// ESP32 internal timer
    auto t = time(nullptr);

    {
      StickCP2.Display.fillRect(0, 75, 320, 16, BLACK); // Clear the line where date/time will go             
      StickCP2.Display.setCursor(5, 75); // set the cursor before writing date/time
      // write date/time
      auto dt = StickCP2.Rtc.getDateTime();
      Serial.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n",
                    dt.date.year, dt.date.month, dt.date.date,
                    wd[dt.date.weekDay], dt.time.hours, dt.time.minutes,
                    dt.time.seconds);
      
      StickCP2.Display.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d",
                              dt.date.year, dt.date.month, dt.date.date,
                              wd[dt.date.weekDay], dt.time.hours, dt.time.minutes,
                              dt.time.seconds);
          // The syntax of the above is as follows ex:(%04d) 4 decimals, from tm->tm_year + 1900
          // %s (string, or letters) from wd[tm->tm_day - 1]
    }
}
