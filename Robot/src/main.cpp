#include <Arduino.h>
#include <Wire.h>
#include "AppConnector.h"
#include "Face.h"
#include "MotionSensor.h"
#include "BuzzerPlayer.h"

#define TOUCH_PIN 4
#define BUZZER_PIN 18

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Face robotFace(display);
MotionSensor motion(18.0f);
BuzzerPlayer buzzer(BUZZER_PIN);
AppConnector appConnector;

Emotion mapStringToEmotion(const String& exp) {
  if (exp == "FOCUS") return NEUTRAL;
  if (exp == "SLEEP") return SLEEP;
  if (exp == "HAPPY") return HAPPY;
  return NEUTRAL;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(TOUCH_PIN, INPUT);

  robotFace.begin();
  buzzer.begin();
  motion.begin();

  // Gestisce automaticamente: connessione Wi-Fi salvata O avvio Access Point
  appConnector.begin();

  if (appConnector.isProvisioningMode()) {
    // Se è in modalità Access Point, mostra sul display che è in attesa del Wi-Fi
    robotFace.setEmotion(NEUTRAL);
    robotFace.update();
  } else {
    robotFace.setEmotion(HAPPY);
    robotFace.update();
  }
}

void loop() {
  appConnector.handle();

  bool isTouched = digitalRead(TOUCH_PIN);

  // Se si tiene premuto il sensore Touch (es. reset Wi-Fi se si cambia casa)
  static unsigned long touchStartTime = 0;
  if (isTouched) {
    if (touchStartTime == 0) touchStartTime = millis();
    if (millis() - touchStartTime > 5000) { // Premuto per 5 secondi
      buzzer.playDizzySound();
      appConnector.resetWifiCredentials(); // Cancella Wi-Fi e riavvia
    }
  } else {
    touchStartTime = 0;
  }

  // Se il robot è in configurazione AP, non aggiornare la faccina in base ai sensori
  if (appConnector.isProvisioningMode()) {
    delay(50);
    return;
  }

  // Normale logica del robot
  if (motion.isShaken()) {
    robotFace.setEmotion(DIZZY);
    robotFace.update();
    buzzer.playDizzySound();
  } else if (isTouched) {
    robotFace.setEmotion(HAPPY);
    robotFace.update();
    buzzer.playHappyBeep();
    delay(150);
  } else {
    String appExp = appConnector.getCurrentExpression();
    robotFace.setEmotion(mapStringToEmotion(appExp));
    robotFace.update();
  }

  delay(20);
}