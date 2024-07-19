/**
  Ethan Hartman 
  4/11/24
  M5StickC IMU Sensors
  CSC230
  ephartman1@gmail.com
 */

#include "M5StickCPlus2.h"
#define LED_BUILTIN 19

float maxX = 0.0;
float maxY = 0.0;
float maxZ = 0.0;
float maxAccel = 0.0;

void setup() {
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    StickCP2.Display.setRotation(1);
    StickCP2.Display.setTextColor(GREEN);
    StickCP2.Display.setTextDatum(middle_center);
    StickCP2.Display.setFont(&fonts::FreeSansBold9pt7b);
    StickCP2.Display.setTextSize(1);
}

void loop(void) {
    auto imu_update = StickCP2.Imu.update();
    if (imu_update) {
        StickCP2.Display.setCursor(0, 40);
        StickCP2.Display.clear(); 

        auto data = StickCP2.Imu.getImuData();
        float accX = data.accel.x;
        float accY = data.accel.y;
        float accZ = data.accel.z;
        float accelMag = sqrt((accX * accX) + (accY * accY) + (accZ * accZ));

          // Update max values
        maxX = max(maxX, abs(accX));
        maxY = max(maxY, abs(accY));
        maxZ = max(maxZ, abs(accZ));
        maxAccel = max(maxAccel, accelMag);

        StickCP2.Display.printf("%0.2f %0.2f %0.2f\r\n", accX,
                                accY, accZ);
        StickCP2.Display.printf("%0.4f \r\n", accelMag);


        StickCP2.Display.printf("%0.2f %0.2f %0.2f\r\n", maxX,
                                maxY, maxZ);
        StickCP2.Display.printf("%0.4f \r\n", maxAccel);

        if (maxAccel >= 1.3) {
          // fall detected
          digitalWrite(LED_BUILTIN, HIGH); // Turn on red LED
          StickCP2.Display.setCursor(0, 40);
          StickCP2.Display.clear();
          StickCP2.Display.printf("Fall Detected"); 
          delay(5000); // Blink rate
          maxAccel = 0.0;
          digitalWrite(LED_BUILTIN, LOW); // Turn off red LED
          delay(100); // Blink rate
        }
        delay(100);
    }
}