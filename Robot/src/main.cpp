#include <Arduino.h>
#include "Face.h"

// Istanza del display e dell'interfaccia Face
LGFX lcd;
Face face(lcd);

void setup() {
  Serial.begin(115200);

  // Inizializza il display, LittleFS e la GIF iniziale (NEUTRAL)
  face.begin();
}

void loop() {
  // Avanza di un frame nella riproduzione della GIF
  face.update();
}