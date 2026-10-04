#ifndef BUZZER_PLAYER_H
#define BUZZER_PLAYER_H

#include <Arduino.h>
#include "pitches.h"

class BuzzerPlayer {
private:
  uint8_t pin;

public:
  explicit BuzzerPlayer(uint8_t buzzerPin);
  
  void begin();
  void beep(unsigned int frequency, unsigned long duration);
  void playHappyBeep();
  void playDizzySound();
  void playMelody(const int melody[], const int durations[], size_t noteCount, int tempo = 130);
  void stop();
};

#endif