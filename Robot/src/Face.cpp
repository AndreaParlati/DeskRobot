#include "Face.h"

// Usiamo variabili statiche come nel main funzionante
static Face* faceInstance = nullptr;
static File gifFile;

// --- CALLBACK DI DECODIFICA GIF IDENTICA AL MAIN ---
static void GIFDraw(GIFDRAW *pDraw) {
    if (faceInstance == nullptr) return;
    
    LGFX_Sprite& canvas = faceInstance->getCanvas();
    uint8_t *s;
    uint16_t *palette;
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > 240)
        iWidth = 240 - pDraw->iX;

    pDraw->y += pDraw->iY;
    if (pDraw->y >= 240)
        return;

    palette = (uint16_t *)pDraw->pPalette;
    x = pDraw->iX;
    y = pDraw->y;
    s = pDraw->pPixels;

    if (pDraw->ucHasTransparency) {
        uint8_t ucTransparent = pDraw->ucTransparent;
        for (int i = 0; i < iWidth; i++) {
            if (s[i] != ucTransparent) {
                canvas.drawPixel(x + i, y, palette[s[i]]);
            }
        }
    } else {
        for (int i = 0; i < iWidth; i++) {
            canvas.drawPixel(x + i, y, palette[s[i]]);
        }
    }
}

// --- CALLBACK FILESYSTEM IDENTICHE AL MAIN ---
static void *GIFOpenFile(const char *fname, int32_t *pSize) {
    gifFile = LittleFS.open(fname, "r");
    if (!gifFile) return NULL;
    *pSize = gifFile.size();
    return (void *)&gifFile;
}

static void GIFCloseFile(void *pHandle) {
    File *f = (File *)pHandle;
    if (f) f->close();
}

static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    File *f = (File *)pFile->fHandle;
    int32_t iBytesRead = f->read(pBuf, iLen);
    pFile->iPos = f->position();
    return iBytesRead;
}

static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
    File *f = (File *)pFile->fHandle;
    f->seek(iPosition, SeekSet);
    pFile->iPos = f->position();
    return pFile->iPos;
}

// --- CLASSE FACE ---

Face::Face(LGFX &disp) 
    : display(disp), 
      canvas(&disp),
      currentEmotion(NEUTRAL), 
      nextEmotion(NEUTRAL), 
      activeGifEmotion((Emotion)-1) {
    faceInstance = this;
}

void Face::begin() {
    display.init();
    display.setRotation(0);

    // Inizializzazione Sprite e PSRAM come nel main
    canvas.setColorDepth(16);
    canvas.setPsram(true);
    canvas.createSprite(240, 240);
    canvas.fillScreen(TFT_BLACK);

    if (!LittleFS.begin(true)) {
        Serial.println("[LittleFS] Errore di montaggio del filesystem!");
        return;
    }

    // Palette per ST7789 come nel main
    gif.begin(GIF_PALETTE_RGB565_LE);
    openGifForEmotion(currentEmotion);
}

void Face::setEmotion(Emotion newEmotion) {
    nextEmotion = newEmotion;
}

void Face::openGifForEmotion(Emotion emo) {
    gif.close();
    const char* filename = "/animations/neutral.gif";

    switch (emo) {
        case HAPPY:  filename = "/animations/happy.gif"; break;
        case DIZZY:  filename = "/animations/dizzy.gif"; break;
        case SLEEPY:  filename = "/animations/sleepy.gif"; break;
        case NEUTRAL:
        default:     filename = "/animations/neutral.gif"; break;
    }

    if (gif.open(filename, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        activeGifEmotion = emo;
        currentEmotion = emo;
    } else {
        Serial.printf("[GIF] Errore nell'apertura della GIF: %s\n", filename);
    }
}

void Face::update() {
    int delayMs = 0;
    
    // Decodifica il frame e legge il ritardo nativo della GIF
    int hasMoreFrames = gif.playFrame(false, &delayMs);

    // Invia il canvas al display via DMA alla massima velocità
    display.startWrite();
    canvas.pushSprite(0, 0);
    display.endWrite();

    if (!hasMoreFrames) {
        if (nextEmotion != currentEmotion) {
            openGifForEmotion(nextEmotion);
        } else {
            gif.reset();
        }
    }

    // AUMENTO FPS: Moltiplica il delay per un fattore inferiore a 1.0.
    // Esempio: 0.35 = ~3x più veloce (FPS quasi triplicati)
    float speedFactor = 0.35; 
    int fastDelay = delayMs * speedFactor;

    if (fastDelay > 0) {
        delay(fastDelay);
    } else {
        vTaskDelay(1); // Micro-pausa per evitare il blocco del Watchdog
    }
}