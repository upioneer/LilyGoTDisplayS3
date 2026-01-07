# ESP32-S3 Slideshow

An image viewer hard coded for the LilyGo T-Display S3

It auto discovers images from the internal filesystem (LittleFS)

Images rotate every few seconds automatically but can also be paused or manually advanced

## Quick Start
- **Install VS Code and the PlatformIO Extension**
- **Clone this repository**
- **Prepare Images:** Place your `.jpg`/`.jpeg` files in the `/data` folder
- **Platform > Build the Filesystem:** Run `Build Filesystem Image` in PlatformIO
- **Platform > Flash Filesystem:** Run `Upload Filesystem Image` in PlatformIO
- **PlatformIO: Build** (check icon in status bar)
- **PlatformIO: Upload** (arrow icon in status bar)

## Hardware Configuration
- **Device:** LilyGo T-Display S3 (non-touch)
- **Display Driver:** ST7789 (8-bit Parallel)
- **Flash Size:** 16MB (Uses `default_16MB.csv`)

## How to Use
- **Button 14 (Right):** Short press for the next image
- **Button 14 (Right):** Long press for a storage report in the serial terminal
- **Button 0 (Left):** Short press to toggle play/pause of the slideshow
- **Button 0 (Left):** Long press to shuffle the image playback order
- **Serial Monitor:** Set to `115200` baud to see file discovery and partition health

## Image Requirements & Caveats
- **Dimensions:** 170 x 320 pixels
- **Format:** Baseline jpeg (progressive jpegs may fail to decode)
- **Filenames:** Avoid special characters. Keep names under 31 characters for LittleFS compatibility
- **Placement:** Add your prepared jpgs to the `/data` folder
- **Filesystem:** Uses LittleFS. Total storage is expanded to ~10MB via the custom partition table
- **Limit:** Currently set to index up to 50 images in the root directory

## Libraries Used
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) - Display driver
- [TJpg_Decoder](https://github.com/Bodmer/TJpg_Decoder) - Efficient jpeg rendering
