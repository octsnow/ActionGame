#pragma once
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include "ImageManager.hpp"

class Game {
public:
    void init(int* argc, char** argv, int width, int height);
    void destroy();

    void update();

    void displayFunc(void (*func)(void));
    void reshapeFunc(void (*func)(int, int));
    void idleFunc(void (*func)(void));

    int loadImage(std::string filepath, bool isBmp=false);
    int loadImage(std::string filepath, float sx, float sy, bool isBmp=false);
    int loadRegionImage(std::string filepath, int width, int height, int n, bool isBmp=false);
    int loadRegionImage(std::string filepath, float sx, float sy, int width, int height, int n, bool isBmp=false);
    void drawBox(int x1, int y1, int x2, int y2, int color);
    void drawImage(int handle, int dX, int dY, bool transpose=false, bool isReverse=false);
    void text(int x, int y, const char* format, ...);
    void clearScreen();
    void screenSwap();

    bool isPressed(char key);
    bool isUp(char key);
    bool isDown(char key);
private:
    uint8_t* m_screen;
    int m_width;
    int m_height;
    HDC m_hDC;
    HFONT m_hFont;
    HBITMAP m_hBitmap;
    HGDIOBJ m_h01dHandle;
    ImageManager m_imgManager;
};
