#include "MotionSensor.h"

// Registri standard MPU-9250 / MPU-6500
#define MPU9250_ADDR         0x68
#define MPU9250_WHO_AM_I     0x75
#define MPU9250_PWR_MGMT_1   0x6B
#define MPU9250_ACCEL_XOUT_H 0x3B
#define MPU9250_ACCEL_CONFIG 0x1C

MotionSensor::MotionSensor(uint8_t sda, uint8_t scl, float threshold, unsigned long cooldown)
    : sdaPin(sda), sclPin(scl), i2cAddress(MPU9250_ADDR), shakeThreshold(threshold),
      cooldownMs(cooldown), lastShakeTime(0), initialized(false) {}

bool MotionSensor::begin() {
    Wire.begin(sdaPin, sclPin);
    Wire.setClock(400000); // I2C a 400kHz

    // Verifica se il dispositivo risponde all'indirizzo 0x68
    Wire.beginTransmission(i2cAddress);
    if (Wire.endTransmission() != 0) {
        // Prova l'indirizzo alternativo 0x69 (se AD0 è collegato a VCC)
        i2cAddress = 0x69;
        Wire.beginTransmission(i2cAddress);
        if (Wire.endTransmission() != 0) {
            Serial.println("[MPU-9250] Errore: Sensore non trovato sul bus I2C!");
            return false;
        }
    }

    // Risveglia il sensore (esce dalla modalità Sleep)
    Wire.beginTransmission(i2cAddress);
    Wire.write(MPU9250_PWR_MGMT_1);
    Wire.write(0x00);
    Wire.endTransmission();
    delay(10);

    // Imposta il range dell'accelerometro a ±8g (sensibilità 4096 LSB/g)
    Wire.beginTransmission(i2cAddress);
    Wire.write(MPU9250_ACCEL_CONFIG);
    Wire.write(0x10); // ±8g
    Wire.endTransmission();

    initialized = true;
    Serial.printf("[MPU-9250] Inizializzato con successo all'indirizzo 0x%02X!\n", i2cAddress);
    return true;
}

int16_t MotionSensor::readRegister16(uint8_t reg) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(i2cAddress, (uint8_t)2);
    
    if (Wire.available() >= 2) {
        return (Wire.read() << 8) | Wire.read();
    }
    return 0;
}

bool MotionSensor::isShaken() {
    if (!initialized) return false;

    // Evita di scatenare l'evento di continuo durante uno scuotimento prolungato
    if (millis() - lastShakeTime < cooldownMs) {
        return false;
    }

    // Legge i dati grezzi dei 3 assi dell'accelerometro (X, Y, Z)
    int16_t rawX = readRegister16(MPU9250_ACCEL_XOUT_H);
    int16_t rawY = readRegister16(MPU9250_ACCEL_XOUT_H + 2);
    int16_t rawZ = readRegister16(MPU9250_ACCEL_XOUT_H + 4);

    // Conversione da dati grezzi a m/s^2 (scala ±8g = 4096 LSB/g, 1g = 9.81 m/s^2)
    float ax = (rawX / 4096.0f) * 9.81f;
    float ay = (rawY / 4096.0f) * 9.81f;
    float az = (rawZ / 4096.0f) * 9.81f;

    // Calcola il modulo dell'accelerazione totale sqrt(ax^2 + ay^2 + az^2)
    float accelMagnitude = sqrt(ax * ax + ay * ay + az * az);

    // Quando è fermo sul tavolo la magnitudo è ~9.81 m/s^2 (gravità terrestre)
    if (accelMagnitude > shakeThreshold) {
        lastShakeTime = millis();
        return true;
    }

    return false;
}