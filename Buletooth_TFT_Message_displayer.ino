/*
  TFT Display - Bluetooth Message Receiver
  Shows messages from Bluetooth (HC-05) on MCUFRIEND TFT
  Based on LCD example but adapted for color TFT
*/

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>

#define BT_SERIAL Serial1

MCUFRIEND_kbv tft;
uint16_t W, H;

char str[128];     // buffer for received text
int i = 0;
bool newMsg = false;

// Colors
#define BLACK   0x0000
#define BLUE    0x001F
#define GREEN   0x07E0
#define CYAN    0x07FF
#define WHITE   0xFFFF

void setup() {
  // Start Bluetooth at 9600 baud
  BT_SERIAL.begin(9600);
  
  // Initialize TFT
  uint16_t id = tft.readID();
  if (id == 0xD3D3) id = 0x9486;
  tft.begin(id);
  tft.setRotation(1);  // Landscape
  W = tft.width();
  H = tft.height();

  // Clear screen
  tft.fillScreen(BLUE);
  
  // Draw header
  tft.fillRect(0, 0, W, 30, GREEN);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 8);
  tft.print("BT Message Display");
  
  // Initial message
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 80);
  tft.print("Waiting for");
  tft.setCursor(20, 110);
  tft.print("message...");
}

void loop() {
  // Check for Bluetooth data
  if (BT_SERIAL.available()) {
    i = 0;
    while (BT_SERIAL.available()) {
      char inChar = BT_SERIAL.read();
      if (inChar == '\n' || inChar == '\r') {
        // End of message
        break;
      }
      if (i < 127) {
        str[i++] = inChar;
      }
      delay(5);
    }
    str[i] = '\0';  // Null terminate
    
    if (i > 0) {  // Only process if we got text
      newMsg = true;
      BT_SERIAL.print("RECEIVED: ");
      BT_SERIAL.println(str);  // Echo back
    }
  }
  
  // Display new message
  if (newMsg) {
    // Clear message area (keep header)
    tft.fillRect(0, 31, W, H - 31, BLUE);
    
    // Display message
    tft.setTextColor(CYAN);
    tft.setTextSize(3);
    tft.setCursor(10, 60);
    
    // Word wrap for long messages
    int charWidth = 18;  // Approximate width per char at size 3
    int maxCharsPerLine = (W - 20) / charWidth;
    int len = strlen(str);
    
    if (len <= maxCharsPerLine) {
      // Short message - display centered
      tft.print(str);
    } else {
      // Long message - word wrap
      int y = 60;
      int pos = 0;
      
      while (pos < len && y < H - 30) {
        tft.setCursor(10, y);
        
        // Print up to maxCharsPerLine characters
        int charsThisLine = min(maxCharsPerLine, len - pos);
        for (int j = 0; j < charsThisLine; j++) {
          tft.print(str[pos + j]);
        }
        
        pos += charsThisLine;
        y += 30;  // Move to next line
      }
    }
    
    newMsg = false;
  }
}