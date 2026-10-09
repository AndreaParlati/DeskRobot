#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class MotionSensor {
private:
    uint8_t sdaPin;
    uint8_t sclPin;
    uint8_t i2cAddress;
    float shakeThreshold;       // Soglia dell'accelerazione per rilevare lo scuotimento (m/s^2)
    unsigned long cooldownMs;   // Cooldown per evitare letture doppie
    unsigned long lastShakeTime;
    bool initialized;

    // Metodi interni per lettura I2C diretta dei dati dell'accelerometro
    int16_t readRegister16(uint8_t reg);

public:
    MotionSensor(uint8_t sda = 1, uint8_t scl = 2, float threshold = 18.0f, unsigned long cooldown = 2000);
    
    bool begin();
    bool isShaken();
    void setThreshold(float threshold) { shakeThreshold = threshold; }
};

#endif // MOTION_SENSOR_H