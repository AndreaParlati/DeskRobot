#include "BuzzerPlayer.h"

BuzzerPlayer::BuzzerPlayer(uint8_t buzzerPin) : pin(buzzerPin) {}

void BuzzerPlayer::begin() {
  pinMode(pin, OUTPUT);
  stop();
}

void BuzzerPlayer::beep(unsigned int frequency, unsigned long duration) {
  tone(pin, frequency, duration);
}

void BuzzerPlayer::playHappyBeep() {
  tone(pin, NOTE_E6, 80);
  delay(90);
  tone(pin, NOTE_A6, 120);
}

void BuzzerPlayer::playDizzySound() {
  for (int freq = 1000; freq > 300; freq -= 100) {
    tone(pin, freq, 30);
    delay(35);
  }
}

void BuzzerPlayer::playMelody(const int melody[], const int durations[], size_t noteCount, int tempo) {
  int wholenote = (60000 * 4) / tempo;

  for (size_t i = 0; i < noteCount; i++) {
    int divider = durations[i];
    int noteDuration = 0;

    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote / abs(divider)) * 1.5;
    }

    if (melody[i] != REST) {
      tone(pin, melody[i], noteDuration * 0.85);
    } else {
      noTone(pin);
    }

    delay(noteDuration);
    noTone(pin);
  }
}

void BuzzerPlayer::stop() {
  noTone(pin);
}