#include <Arduino.h>
#include "MotionSensor.h"

// Definizione dei pin I2C dell'ESP32-S3
#define SDA_PIN 1
#define SCL_PIN 2

// Istanza del sensore:
// - SDA: GPIO 1
// - SCL: GPIO 2
// - Soglia di scuotimento: 18.0 m/s^2 (circa 1.8g, rilevabile quando agiti la scheda)
// - Cooldown: 2000 ms tra un rilevamento e l'altro
MotionSensor motion(SDA_PIN, SCL_PIN, 18.0f, 2000);

void setup() {
  Serial.begin(115200);
    // Attende la connessione del Monitor Seriale per un massimo di 3 secondi
  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000)) {
    delay(10);
  }

  delay(500); // Piccola pausa di stabilizzazione

  Serial.println("\n===============================================");
  Serial.println("  ESP32-S3: TEST RILEVAMENTO MPU-9250 / 6500   ");
  Serial.println("===============================================\n");

  // Inizializza la comunicazione I2C con il sensore
  if (!motion.begin()) {
    Serial.println("[ERRORE] Sensore non trovato!");
    Serial.println("Verifica i collegamenti:");
    Serial.println(" - VCC -> 3.3V (o 5V)");
    Serial.println(" - GND -> GND");
    Serial.println(" - SDA -> GPIO 1");
    Serial.println(" - SCL -> GPIO 2");
    
    // Blocca l'esecuzione in caso di errore di collegamento
    while (1) {
      delay(500);
    }
  }

  Serial.println(">>> Sensore pronto! Muovi o agita il sensore per testare...");
}

void loop() {
  // Controlla se il sensore viene agitato
  if (motion.isShaken()) {
    Serial.println("⚡ [EVENTO DETECTED] ROBOT AGITATO! -> Set Emozione: DIZZY");
  }

  delay(50); // Piccola pausa per non saturare la CPU
}