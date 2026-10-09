#include <Arduino.h>
#include "BuzzerPlayer.h"

// Pin del buzzer passivo (GPIO 18)
#define BUZZER_PIN 18

BuzzerPlayer buzzer(BUZZER_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n==============================================");
  Serial.println("   ESP32-S3: TEST AUDIO BUZZER PASSIVO        ");
  Serial.println("==============================================\n");

  // Inizializza il pin del buzzer
  buzzer.begin();
}

void loop() {
  Serial.println(">>> 1. TEST EFFETTI SONORI PER EMOZIONI <<<");

  Serial.println(" -> Emozione: NEUTRAL");
  buzzer.playNeutralSound();
  delay(2000);

  Serial.println(" -> Emozione: HAPPY");
  buzzer.playHappyBeep();
  delay(2000);

  Serial.println(" -> Emozione: DIZZY");
  buzzer.playDizzySound();
  delay(2000);

  Serial.println(" -> Emozione: SLEEPY");
  buzzer.playSleepySound();
  delay(3000);

  Serial.println("\n>>> 2. TEST TEMI MUSICALI COMPLETI <<<");

  Serial.println(" -> Riproduzione: Super Mario Bros Theme...");
  buzzer.playMarioTheme();
  delay(3000);

  Serial.println(" -> Riproduzione: Star Wars Main Theme...");
  buzzer.playStarWarsTheme();
  delay(4000);

  Serial.println("\n----------------------------------------------");
  Serial.println(" Fine ciclo di test. Riavvio sequenza tra 5s ");
  Serial.println("----------------------------------------------\n");
  delay(5000);
}