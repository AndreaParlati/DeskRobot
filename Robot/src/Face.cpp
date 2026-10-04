#include "Face.h"

Face::Face(Adafruit_SSD1306 &disp) : display(disp), currentEmotion(NEUTRAL) {}

void Face::begin() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void Face::setEmotion(Emotion newEmotion) {
  currentEmotion = newEmotion;
}

void Face::update() {
  display.clearDisplay();
  switch (currentEmotion) {
    case HAPPY:   drawHappy(); break;
    case DIZZY:   drawDizzy(); break;
    case NEUTRAL: 
    default:      drawNeutral(); break;
  }
  display.display();
}

void Face::drawNeutral() {
  display.fillRoundRect(25, 20, 25, 30, 8, SSD1306_WHITE);
  display.fillRoundRect(78, 20, 25, 30, 8, SSD1306_WHITE);
}

void Face::drawHappy() {
  display.fillCircle(37, 35, 15, SSD1306_WHITE);
  display.fillCircle(37, 40, 15, SSD1306_BLACK);
  display.fillCircle(90, 35, 15, SSD1306_WHITE);
  display.fillCircle(90, 40, 15, SSD1306_BLACK);
}

void Face::drawDizzy() {
  display.drawLine(25, 20, 50, 45, SSD1306_WHITE);
  display.drawLine(50, 20, 25, 45, SSD1306_WHITE);
  display.drawLine(78, 20, 103, 45, SSD1306_WHITE);
  display.drawLine(103, 20, 78, 45, SSD1306_WHITE);
}