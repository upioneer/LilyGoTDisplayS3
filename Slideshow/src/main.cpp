#include <Arduino.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <TJpg_Decoder.h>

// Hardware Pin Definitions for LilyGo T-Display S3
#define PIN_POWER_ON 15
#define PIN_LCD_BL   38
#define PIN_BUTTON_1 14 

TFT_eSPI tft = TFT_eSPI();

// Playlist configuration
String imageFiles[50];
int totalImages = 0;
int currentIndex = 0;

// Variables for Long-Press detection
unsigned long buttonPressTimestamp = 0;
bool isButtonPressed = false;

// Render callback for TJpg_Decoder
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return false;
    tft.pushImage(x, y, w, h, bitmap);
    return true;
}

// Reports storage stats to Serial Monitor
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

// Scans LittleFS for image files
void scanFilesystem() {
    Serial.println("Scanning filesystem for images...");
    File root = LittleFS.open("/");
    if (!root) {
        Serial.println("CRITICAL ERROR: Could not open root directory.");
        return;
    }

    File file = root.openNextFile();
    while (file && totalImages < 50) {
        String name = "/" + String(file.name());
        String lowerName = name;
        lowerName.toLowerCase();
        
        if (lowerName.endsWith(".jpg") || lowerName.endsWith(".jpeg")) {
            imageFiles[totalImages] = name;
            Serial.printf("FOUND: %s (%d bytes)\n", name.c_str(), file.size());
            totalImages++;
        }
        file = root.openNextFile();
    }
}

void displayNextImage() {
    if (totalImages == 0) {
        tft.fillScreen(TFT_ORANGE);
        tft.drawString("DISK EMPTY", tft.width()/2, tft.height()/2);
        return;
    }
    
    String target = imageFiles[currentIndex];
    Serial.printf("ACTION: Drawing %s\n", target.c_str());
    
    tft.fillScreen(TFT_BLACK);
    TJpgDec.drawFsJpg(0, 0, target.c_str(), LittleFS);
    
    currentIndex = (currentIndex + 1) % totalImages;
}

void setup() {
    // Hardware Power
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, HIGH);
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);

    Serial.begin(115200);

    // Mount Filesystem (true = auto-format if partition table changed)
    if (!LittleFS.begin(true)) {
        Serial.println("CRITICAL: LittleFS Mount Failed");
        return;
    }

    // Initialize Display
    tft.begin();
    tft.setSwapBytes(true);
    tft.setRotation(0); 
    tft.fillScreen(TFT_BLUE); 
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE);
    delay(300);

    // Build image list
    scanFilesystem();

    // Decoder Setup
    TJpgDec.setCallback(tft_output);
    TJpgDec.setJpgScale(1);

    // Initial Image
    displayNextImage();
    
    Serial.println("Setup Complete. Short press for next image, Long press for storage info.");
}

void loop() {
    static bool lastState = HIGH;
    bool currentState = digitalRead(PIN_BUTTON_1);

    // Button Pressed Down
    if (currentState == LOW && lastState == HIGH) {
        buttonPressTimestamp = millis();
        isButtonPressed = true;
        delay(50); // Small debounce
    }

    // Button Released
    if (currentState == HIGH && lastState == LOW) {
        unsigned long pressDuration = millis() - buttonPressTimestamp;
        
        if (pressDuration > 2000) {
            // It was a long press (2+ seconds)
            reportStorage();
        } else if (pressDuration > 50) {
            // It was a normal click
            displayNextImage();
        }
        isButtonPressed = false;
    }

    lastState = currentState;
}