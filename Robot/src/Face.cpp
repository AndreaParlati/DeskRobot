#include "Face.h"

static LGFX* displayPtr = nullptr;

static void GIFDraw(GIFDRAW *pDraw) {
    if (pDraw->y >= 240 || displayPtr == nullptr) return;

    uint16_t *usPalette = pDraw->pPalette;
    uint16_t usTemp[240];
    uint8_t *p = pDraw->pPixels;
    int iWidth = pDraw->iWidth;

    if (iWidth > 240) iWidth = 240;

    for (int x = 0; x < iWidth; x++) {
        uint8_t c = p[x];
        if (c == pDraw->ucTransparent) {
            usTemp[x] = TFT_BLACK;
        } else {
            usTemp[x] = usPalette[c];
        }
    }

    displayPtr->pushImage(pDraw->iX, pDraw->y + pDraw->iY, iWidth, 1, usTemp);
}

static void * GIFOpenFile(const char *fname, int32_t *pSize) {
    fs::File f = LittleFS.open(fname, "r");
    if (f) {
        *pSize = f.size();
        return new fs::File(f);
    }
    return NULL;
}

static void GIFCloseFile(void *pHandle) {
    fs::File *f = (fs::File *)pHandle;
    if (f) {
        f->close();
        delete f;
    }
}

static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    fs::File *f = (fs::File *)pFile->fHandle;
    if (f) return f->read(pBuf, iLen);
    return 0;
}

static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition, int iWhence) {
    fs::File *f = (fs::File *)pFile->fHandle;
    if (f) {
        f->seek(iPosition, (SeekMode)iWhence);
        return f->position();
    }
    return 0;
}

// --- CLASSE FACE ---

Face::Face(LGFX &disp) 
    : display(disp), 
      currentEmotion(NEUTRAL), 
      nextEmotion(NEUTRAL), 
      activeGifEmotion((Emotion)-1) {
    displayPtr = &display;
}

void Face::begin() {
    display.init();
    display.setRotation(0);
    display.setBrightness(255);
    display.fillScreen(TFT_BLACK);

    if (!LittleFS.begin(true)) {
        Serial.println("Errore: impossibile montare LittleFS!");
    }

    gif.begin(LITTLE_ENDIAN_PIXELS);
    openGifForEmotion(currentEmotion);
}

void Face::setEmotion(Emotion newEmotion) {
    // Registra la richiesta, ma NON interrompe la GIF corrente
    nextEmotion = newEmotion;
}

void Face::openGifForEmotion(Emotion emo) {
    gif.close();
    const char* filename = "/animations/neutral.gif";

    switch (emo) {
        case HAPPY:  filename = "/animations/happy.gif"; break;
        case DIZZY:  filename = "/animations/dizzy.gif"; break;
        case SLEEPY: filename = "/animations/sleepy.gif"; break;
        case NEUTRAL:
        default:     filename = "/animations/neutral.gif"; break;
    }

    if (gif.open(filename, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        activeGifEmotion = emo;
        currentEmotion = emo;
    } else {
        Serial.printf("Errore nell'apertura della GIF: %s\n", filename);
    }
}

void Face::update() {
    // Riproduce un singolo frame della GIF corrente
    int hasMoreFrames = gif.playFrame(true, NULL);

    // Se la GIF ha raggiunto l'ultimo frame (hasMoreFrames == 0)
    if (!hasMoreFrames) {
        // Se c'è una nuova emozione in coda, la carica ora che quella vecchia è finita
        if (nextEmotion != currentEmotion) {
            openGifForEmotion(nextEmotion);
        } else {
            // Altrimenti riavvia la stessa GIF per mantenerla in loop fluido
            gif.reset();
        }
    }
}