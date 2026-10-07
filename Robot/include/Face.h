#ifndef FACE_H
#define FACE_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LittleFS.h>
#include <AnimatedGIF.h>

enum Emotion {
    NEUTRAL,
    HAPPY,
    DIZZY,
    SLEEPY
};

class Face {
private:
    LGFX& display;
    AnimatedGIF gif;
    Emotion currentEmotion;
    Emotion nextEmotion;      // La nuova emozione in attesa che la corrente finisca
    Emotion activeGifEmotion; // L'emozione attualmente in riproduzione sullo schermo

    void openGifForEmotion(Emotion emo);

public:
    Face(LGFX& disp);
    void begin();
    void setEmotion(Emotion newEmotion);
    void update();
};

#endif // FACE_H