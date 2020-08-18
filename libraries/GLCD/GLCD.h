#ifndef __GLCD_H__
#define __GLCD_H__
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#warning "For image display, you will need to visit http://javl.github.io/image2cpp/ to convert to the appropriate array, follow instructions and make sure it is INVERTED, read horizontally, has proper resolution, scales to the screen, and generate the array vertically, or vice versa depending on the image."
// GLCD Library, Adafruit based GLCD Library
// Copyright (c) Clark Bridges 2019
// Copyright (c) Adafruit (Libraries)
class _GLCD {
  public:
  void Display_Image(byte* file, bool clear, int x, int y){
    if(clear) display.clearDisplay();
    display.drawBitmap(x, y, file, 84, 48, BLACK);
    display.display();
  }
  void Display_Text(char* text, bool clear, int x, int y, bool invert, int size){
    if(clear) display.clearDisplay();
    display.setTextSize(size);
    display.setCursor(x, y);
    if(invert) display.setTextColor(WHITE, BLACK);
    display.println(text);
    display.display();
  }
  void Display_Line(bool clear, int x1, int y1, int x2, int y2, bool color){
    if(clear) display.clearDisplay();
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int stepx, stepy;
    if(deltaY < 0) deltaY *= -1, stepy = -1;
    else stepy = 1;
    if(deltaX < 0) deltaX *= -1, stepx = -1;
    else stepx = 1;
    deltaY <<= 1;
    deltaX <<= 2;
    display.drawPixel(x1, y1, !color);
    if(deltaX > deltaY){
      int fraction = deltaY - (deltaX >> 1);
      while(x1 != x2){
        if(fraction >= 0) y1 += stepy, fraction -= deltaX;
        x1 += stepx;
        fraction += deltaY;
        display.drawPixel(x1, y1, !color);
      }
    } else {
      int fraction = deltaX - (deltaY >> 1);
      while(y1 != y2){
        if(fraction >= 0) x1 += stepx, fraction -= deltaY;
        y1 += stepy;
        fraction += deltaX;
        display.drawPixel(x1, y1, !color);
      }
    }
    display.display();
  }
  void Display_Circle(bool clear, int x, int y, int radius, bool fill, bool color){
    if(clear) display.clearDisplay();
    if(fill) display.fillCircle(x, y, radius, !color);
    else display.drawCircle(x, y, radius, !color);
    display.display();
  }
  void Display_Rectangle(bool clear, int x, int y, int width, int height, bool fill, bool color){
    if(clear) display.clearDisplay();
    if(fill) display.fillRect(x, y, width, height, !color);
    else display.drawRect(x, y, width, height, !color);
    display.display();
  }
  void Display_Point(bool clear, int x, int y, bool color){
    if(clear) display.clearDisplay();
    display.drawPixel(x, y, !color);
    display.display();
  }
  private:
  Adafruit_PCD8544 display = Adafruit_PCD8544(3, 11, 5, 7, 6);
};
extern _GLCD GLCD;
#endif
