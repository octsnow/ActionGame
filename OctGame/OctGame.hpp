#pragma once
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include "ImageManager.hpp"

using namespace std;

class Game {
public:
    void init(int* argc, char** argv, int width, int height);
    void destroy();

    void displayFunc(void (*func)(void));
    void reshapeFunc(void (*func)(int, int));
    void idleFunc(void (*func)(void));
    void keyboardFunc(void (*func)(unsigned char, int, int));
    void keyboardUpFunc(void (*func)(unsigned char, int, int));

    int loadImage(string filepath, bool isBmp=false);
    int loadImage(string filepath, float sx, float sy, bool isBmp=false);
    int loadRegionImage(string filepath, int width, int height, int n, bool isBmp=false);
    int loadRegionImage(string filepath, float sx, float sy, int width, int height, int n, bool isBmp=false);
    void drawBox(int x1, int y1, int x2, int y2, int color);
    void drawImage(int handle, int dX, int dY, bool transpose=false);
    void text(int x, int y, const char* format, ...);
    void clearScreen();
    void screenSwap();

private:
    uint8_t* m_screen;
    int m_width;
    int m_height;
    HDC m_hDC;
    HFONT m_hFont;
    HBITMAP m_hBitmap;
    HGDIOBJ m_h01dHandle;

    ImageManager imgManager;
};
