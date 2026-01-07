# ESP32-S3 Photo Rotator (LilyGo T-Display S3)

An image viewer for the LilyGo T-Display S3

It auto-discovers images from the internal filesystem (LittleFS)

Rotating thru the images is done thru a button press

## Quick Start
- **Install VS Code and the PlatformIO Extension**
- **Clone this repository**
- **Prepare Images:** Place your `.jpg`/`.jpeg` files in the `/data` folder
- **Platform > Build the Filesystem:** Run `Build Filesystem Image` in PlatformIO
- **Platform > Flash Filesystem:** Run `Upload Filesystem Image` in PlatformIO
- **PlatformIO: Build** (✅ check icon in status bar)
- **PlatformIO: Upload** (➡️ arrow iconc in status bar)

## Hardware Configuration
- **Device:** LilyGo T-Display S3 (non-touch)
- **Display Driver:** ST7789 (8-bit Parallel)
- **Flash Size:** 16MB (Uses `default_16MB.csv`)

## How to Use
- **Button 14 (Right):** Short press for the next image
- **Button 14 (Right):** Long press for a storage report in the serial terminal
- **Serial Monitor:** Set to `115200` baud to see file discovery and partition health

## Image Requirements & Caveats
- **Dimensions:** 170 x 320 pixels
- **Format:** Baseline JPEG (progressive jpegs may fail to decode)
- **Filesystem:** Uses LittleFS. Total storage is expanded to ~10MB via the custom partition table
- **Limit:** Currently set to index up to 50 images

## Libraries Used
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) - Display driver
- [TJpg_Decoder](https://github.com/Bodmer/TJpg_Decoder) - Efficient jpeg rendering
