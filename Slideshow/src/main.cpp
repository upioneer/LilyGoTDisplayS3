#include <Arduino.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <TJpg_Decoder.h>

#define PIN_POWER_ON 15
#define PIN_LCD_BL   38
#define PIN_BUTTON_NEXT 14 
#define PIN_BUTTON_PLAY 0

TFT_eSPI tft = TFT_eSPI();
String imageFiles[50];
int totalImages = 0;
int currentIndex = 0;

bool isPaused = false; 
unsigned long lastSlideTime = 0;
const unsigned long slideInterval = 5000;
unsigned long nextBtnTimestamp = 0;
unsigned long playBtnTimestamp = 0;
bool playBtnProcessed = false;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return false;
    tft.pushImage(x, y, w, h, bitmap);
    return true;
}

void reportStorage() {
    Serial.println("\n===============================");
    Serial.println("   MANUAL STORAGE REPORT");
    Serial.println("===============================");
    size_t total = LittleFS.totalBytes();
    size_t used = LittleFS.usedBytes();
    Serial.printf("PARTITION SIZE: %d KB\n", total / 1024);
    Serial.printf("SPACE USED:     %d KB\n", used / 1024);
    Serial.printf("SPACE FREE:     %d KB\n", (total - used) / 1024);
    Serial.println("===============================\n");
}

void scanFilesystem() {
    totalImages = 0;
    if (LittleFS.exists("/.sys/qr.jpg")) {
        imageFiles[totalImages++] = "/.sys/qr.jpg";
    }
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file && totalImages < 50) {
        if (!file.isDirectory()) {
            String name = "/" + String(file.name());
            String lower = name; lower.toLowerCase();
            if ((lower.endsWith(".jpg") || lower.endsWith(".jpeg")) && 
                lower != "/qr.jpg" && lower.indexOf("/.sys/") == -1) {
                imageFiles[totalImages++] = name;
            }
        }
        file = root.openNextFile();
    }
}

void shuffleImages() {
    tft.setSwapBytes(true);
    TJpgDec.drawFsJpg(0, 0, "/.sys/shuffle.jpg", LittleFS);
    for (int i = totalImages - 1; i > 0; i--) {
        int j = random(0, i + 1);
        String temp = imageFiles[i];
        imageFiles[i] = imageFiles[j];
        imageFiles[j] = temp;
    }
    currentIndex = 0;
    delay(500); 
}

void renderCurrent() {
    if (totalImages == 0) return;
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    TJpgDec.drawFsJpg(0, 0, imageFiles[currentIndex].c_str(), LittleFS);
    lastSlideTime = millis();
}

void togglePlayPause() {
    isPaused = !isPaused;
    tft.setSwapBytes(true);
    if (isPaused) {
        TJpgDec.drawFsJpg(0, 0, "/.sys/pause.jpg", LittleFS);
    } else {
        TJpgDec.drawFsJpg(0, 0, "/.sys/play.jpg", LittleFS);
    }
    delay(500); 
    renderCurrent();
}

void setup() {
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, HIGH);
    pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_PLAY, INPUT_PULLUP);
    
    Serial.begin(115200);
    LittleFS.begin(true);
    
    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    
    TJpgDec.setCallback(tft_output);
    TJpgDec.setJpgScale(1);

    if (LittleFS.exists("/.sys/splash.jpg")) {
        TJpgDec.drawFsJpg(0, 0, "/.sys/splash.jpg", LittleFS);
        delay(5000); // Extended to 5s
    }

    randomSeed(analogRead(0));
    scanFilesystem();
    renderCurrent();
}

void loop() {
    static bool lastNextState = HIGH;
    static bool lastPlayState = HIGH;

    bool currentNext = digitalRead(PIN_BUTTON_NEXT);
    if (currentNext == LOW && lastNextState == HIGH) {
        nextBtnTimestamp = millis();
    }
    if (currentNext == HIGH && lastNextState == LOW) {
        unsigned long dur = millis() - nextBtnTimestamp;
        if (dur > 2000) reportStorage();
        else if (dur > 50) {
            currentIndex = (currentIndex + 1) % totalImages;
            renderCurrent();
        }
    }
    lastNextState = currentNext;

    bool currentPlay = digitalRead(PIN_BUTTON_PLAY);
    if (currentPlay == LOW) {
        if (lastPlayState == HIGH) {
            playBtnTimestamp = millis();
            playBtnProcessed = false;
        }
        if (!playBtnProcessed && (millis() - playBtnTimestamp > 2000)) {
            shuffleImages();
            renderCurrent();
            playBtnProcessed = true; 
        }
    }
    if (currentPlay == HIGH && lastPlayState == LOW) {
        if (!playBtnProcessed) {
            unsigned long dur = millis() - playBtnTimestamp;
            if (dur > 50) togglePlayPause();
        }
    }
    lastPlayState = currentPlay;

    if (!isPaused && (millis() - lastSlideTime > slideInterval)) {
        currentIndex = (currentIndex + 1) % totalImages;
        renderCurrent();
    }
}
