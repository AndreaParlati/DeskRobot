#include "MotionSensor.h"
#include <Arduino.h>
#include <cmath>

MotionSensor::MotionSensor(float threshold) 
  : shakeThreshold(threshold), initialized(false) {}

bool MotionSensor::begin() {
  if (!mpu.begin()) {
    initialized = false;
    return false;
  }
  
  // Configurazione dei range del sensore
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  initialized = true;
  return true;
}

bool MotionSensor::isShaken() {
  if (!initialized) return false;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calcolo del modulo del vettore accelerazione |a| = sqrt(ax^2 + ay^2 + az^2)
  float magnitude = std::sqrt(a.acceleration.x * a.acceleration.x +
                              a.acceleration.y * a.acceleration.y +
                              a.acceleration.z * a.acceleration.z);

  return (magnitude > shakeThreshold);
}

float MotionSensor::getPitch() {
  if (!initialized) return 0.0f;
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  return std::atan2(-a.acceleration.x, std::sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / M_PI;
}

float MotionSensor::getRoll() {
  if (!initialized) return 0.0f;
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  return std::atan2(a.acceleration.y, a.acceleration.z) * 180.0 / M_PI;
}