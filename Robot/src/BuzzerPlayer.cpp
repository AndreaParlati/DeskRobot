#include "BuzzerPlayer.h"

BuzzerPlayer::BuzzerPlayer(uint8_t buzzerPin) : pin(buzzerPin) {}

void BuzzerPlayer::begin() {
  pinMode(pin, OUTPUT);
  stop();
}

void BuzzerPlayer::beep(unsigned int frequency, unsigned long duration) {
  tone(pin, frequency, duration);
}

// --- SOUND EFFECTS PER EMOZIONI ---

void BuzzerPlayer::playNeutralSound() {
  // Bip di conferma discreto e sobrio
  tone(pin, NOTE_A4, 80);
  delay(90);
  noTone(pin);
}

void BuzzerPlayer::playHappyBeep() {
  // Arpeggio vivace e brillante
  tone(pin, NOTE_C5, 60);
  delay(70);
  tone(pin, NOTE_E5, 60);
  delay(70);
  tone(pin, NOTE_G5, 60);
  delay(70);
  tone(pin, NOTE_C6, 150);
  delay(160);
  noTone(pin);
}

void BuzzerPlayer::playDizzySound() {
  // Toni discendenti e ascendenti veloci
  for (int freq = 1200; freq > 300; freq -= 80) {
    tone(pin, freq, 25);
    delay(30);
  }
  for (int freq = 400; freq < 1000; freq += 100) {
    tone(pin, freq, 20);
    delay(25);
  }
  noTone(pin);
}

void BuzzerPlayer::playSleepySound() {
  // Sequenza di toni lenti e calanti ("sbadiglio")
  tone(pin, NOTE_E4, 250);
  delay(270);
  tone(pin, NOTE_D4, 300);
  delay(320);
  tone(pin, NOTE_C4, 450);
  delay(470);
  noTone(pin);
}

// --- GESTIONE MELODIE ---

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

void BuzzerPlayer::playMarioTheme() {
  playMelody(marioMelody, marioDurations, marioNoteCount, marioTempo);
}

void BuzzerPlayer::playStarWarsTheme() {
  playMelody(starWarsMelody, starWarsDurations, starWarsNoteCount, starWarsTempo);
}

void BuzzerPlayer::stop() {
  noTone(pin);
}