#ifndef BUZZER_PLAYER_H
#define BUZZER_PLAYER_H

#include <Arduino.h>
#include "pitches.h"

// --- MELODIA 1: Super Mario Bros Theme ---
const int marioMelody[] = {
  NOTE_E5, NOTE_E5, REST, NOTE_E5, REST, NOTE_C5, NOTE_E5, REST,
  NOTE_G5, REST, REST, REST, NOTE_G4, REST, REST, REST
};
const int marioDurations[] = {
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8
};
const size_t marioNoteCount = sizeof(marioMelody) / sizeof(marioMelody[0]);
const int marioTempo = 140;

// --- MELODIA 2: Star Wars Theme ---
const int starWarsMelody[] = {
  NOTE_AS4, NOTE_AS4, NOTE_AS4,
  NOTE_F5, NOTE_C6,
  NOTE_A5, NOTE_G5, NOTE_F5, NOTE_D6, NOTE_C6,
  NOTE_A5, NOTE_G5, NOTE_F5, NOTE_D6, NOTE_C6,
  NOTE_A5, NOTE_G5, NOTE_A5, NOTE_G5
};
const int starWarsDurations[] = {
  8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 4,
  8, 8, 8, 2
};
const size_t starWarsNoteCount = sizeof(starWarsMelody) / sizeof(starWarsMelody[0]);
const int starWarsTempo = 108;

class BuzzerPlayer {
private:
  uint8_t pin;

public:
  explicit BuzzerPlayer(uint8_t buzzerPin);
  
  void begin();
  void beep(unsigned int frequency, unsigned long duration);

  // Sound Effects dedicati alle 4 Emozioni del robot
  void playNeutralSound();
  void playHappyBeep();
  void playDizzySound();
  void playSleepySound();

  // Riproduzione di melodie
  void playMelody(const int melody[], const int durations[], size_t noteCount, int tempo = 130);
  
  // Temi predefiniti
  void playMarioTheme();
  void playStarWarsTheme();

  void stop();
};

#endif