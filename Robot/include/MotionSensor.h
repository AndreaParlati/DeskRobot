#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MotionSensor {
private:
  Adafruit_MPU6050 mpu;
  float shakeThreshold;
  bool initialized;

public:
  explicit MotionSensor(float threshold = 18.0f);
  
  bool begin();
  bool isShaken();
  float getPitch(); // Inclinazione avanti/dietro
  float getRoll();  // Inclinazione destra/sinistra
};

#endif