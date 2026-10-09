#ifndef FACE_H
#define FACE_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LittleFS.h>
#include <AnimatedGIF.h>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789  _panel_instance;
    lgfx::Bus_SPI       _bus_instance;

public:
    LGFX() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000;    // 80 MHz SPI (Come nel main)
            cfg.pin_sclk = 4;             // SCL
            cfg.pin_mosi = 5;             // SDA
            cfg.pin_miso = -1;
            cfg.pin_dc   = 7;             // DC
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = 15;    // CS
            cfg.pin_rst          = 6;     // RST
            cfg.pin_busy         = -1;
            cfg.panel_width      = 240;
            cfg.panel_height     = 240;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.invert           = true;
            cfg.rgb_order        = false;
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

enum Emotion { NEUTRAL, HAPPY, DIZZY, SLEEPY, SLEEP };

class Face {
private:
    LGFX& display;
    LGFX_Sprite canvas;
    AnimatedGIF gif;
    Emotion currentEmotion;
    Emotion nextEmotion;
    Emotion activeGifEmotion;

    void openGifForEmotion(Emotion emo);

public:
    Face(LGFX& disp);
    void begin();
    void setEmotion(Emotion newEmotion);
    void update();

    // Serve a GIFDraw per accedere allo sprite
    LGFX_Sprite& getCanvas() { return canvas; }
};

#endif // FACE_H