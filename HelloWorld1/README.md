# "Hello World" for LilyGo T-Display S3

## Cruicial Settings
USB CDC On Boot	`Enabled`

Flash Size `16MB (128Mb)`

Partition Scheme `16M Flash (3MB APP/9.9MB FATFS)`

PSRAM `OPI PSRAM`

Flash Mode `QIO 80MHz`

```c++
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
  pinMode(15, OUTPUT);     // Power for the backlight
  digitalWrite(15, HIGH);  // Turn backlight ON
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Hello World!", 50, 80, 4);
}
void loop() {}
```
