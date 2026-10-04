#ifndef FACE_H
#define FACE_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

enum Emotion {
  NEUTRAL,
  HAPPY,
  DIZZY,
  SLEEPY
};

class Face {
private:
  Adafruit_SSD1306 &display;
  Emotion currentEmotion;

  void drawNeutral();
  void drawHappy();
  void drawDizzy();

public:
  explicit Face(Adafruit_SSD1306 &disp);
  void begin();
  void setEmotion(Emotion newEmotion);
  void update();
};

#endif