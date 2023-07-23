#include "OctGame.hpp"
#include <stdarg.h>

void Game::init(int* argc, char** argv, int width, int height) {
    this->m_width = width;
    this->m_height = height;

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow(argv[0]);
      
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC tmpDC = GetDC(GetDesktopWindow());
    this->m_hBitmap = CreateDIBSection(
          tmpDC, &bmi, DIB_RGB_COLORS, (void**)&this->m_screen, 0, 0);

    if(this->m_hBitmap == NULL) {
        cout << "hBitmap is NULL" << endl;
        return;
    }

    this->m_hDC = CreateCompatibleDC(tmpDC);
    this->m_h01dHandle = SelectObject(this->m_hDC, this->m_hBitmap);
    SetBkMode(this->m_hDC, TRANSPARENT);

    ReleaseDC(GetDesktopWindow(), tmpDC);

    this->m_hFont = CreateFont(
        30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_ROMAN, NULL
    );
    SetTextColor(this->m_hDC, RGB(0xFF, 0xFF, 0xFF));
    SelectObject(this->m_hDC, this->m_hFont);
}

void Game::displayFunc(void (*func)(void)) {
    glutDisplayFunc(func);
}

void Game::reshapeFunc(void (*func)(int, int)) {
    glutReshapeFunc(func);
}

void Game::idleFunc(void (*func)(void)) {
    glutIdleFunc(func);
}

void Game::keyboardFunc(void (*func)(unsigned char, int, int)) {
    glutKeyboardFunc(func);
}

void Game::keyboardUpFunc(void (*func)(unsigned char, int, int)) {
    glutKeyboardUpFunc(func);
}

void Game::destroy() {
    SelectObject(this->m_hDC, GetStockObject(SYSTEM_FONT));
    DeleteObject(this->m_hFont);

    SelectObject(this->m_hDC, this->m_h01dHandle);
    DeleteDC(this->m_hDC);
    DeleteObject(this->m_hBitmap);
}

int Game::loadImage(string filepath, bool isBmp) {
    return this->imgManager.loadImage(filepath, isBmp);
}

int Game::loadImage(string filepath, float sx, float sy, bool isBmp) {
    return this->imgManager.loadImage(filepath, sx, sy, isBmp);
}

int Game::loadRegionImage(string filepath, int width, int height, int n, bool isBmp) {
    return this->imgManager.loadRegionImage(filepath, width, height, n, isBmp);
}

int Game::loadRegionImage(string filepath, float sx, float sy, int width, int height, int n, bool isBmp) {
    return this->imgManager.loadRegionImage(filepath, sx, sy, width, height, n, isBmp);
}

void Game::drawBox(int x1, int y1, int x2, int y2, int color){
    int minX = x1 < x2 ? x1 : x2;
    int minY = this->m_height - (y1 > y2 ? y1 : y2) - 1;
    int maxX = x1 > x2 ? x1 : x2;
    int maxY = this->m_height - (y1 < y2 ? y1 : y2) - 1;

    for(int y = minY; y < maxY; y++){
        for(int x = minX; x < maxX; x++){
            for(int c = 0; c < 3; c++){
                if(y < 0 || this->m_height <= y) continue;
                if(x < 0 || this->m_width <= x) continue;
                this->m_screen[(y * this->m_width + x) * 3 + c] = (char)((color >> ((2 - c) * 8)) & 0xFF);
            }
        }
    }
}

void Game::drawImage(int handle, int dx, int dy, bool transpose, bool isReverse){
    cv::Mat* img = this->imgManager.getImage(handle);
    uchar* data = img->data;

    if(img == nullptr) {
        return;
    }

    int imgC = img->channels();
    int imgW = img->cols, imgH = img->rows;
    float tp = 1;

    if(transpose && imgC <= 3) transpose = false;

    for(int y = 0; y < imgH && y + dy < this->m_height; y++){
        for(int x = 0; x < imgW && x + dx < this->m_width; x++){
            int ix = x;
            int iy = y;
            if(isReverse) {
                ix = imgW - x - 1;
            }

            if(transpose) {
                if(data[(iy * imgW + ix) * imgC + 3] == 0) continue;
                tp = img->data[(iy * imgW + ix) * imgC + 3] / 255.0f;
            }

            for(int c = 0; c < 3; c++){
                int scIdx = ((this->m_height - (y + dy) - 1) * this->m_width + (x + dx)) * 3 + c;
                int dtIdx = (iy * imgW + ix) * imgC + c;
                if(iy + dy < 0 || this->m_height <= iy + dy) continue;
                if(ix + dx < 0 || this->m_width <= ix + dx) continue;
                this->m_screen[scIdx] = this->m_screen[scIdx] * (1 - tp) + img->data[dtIdx] * tp;
            }
        }
    }
}

void Game::text(int x, int y, const char* format, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buf, 256, format, ap);
    va_end(ap);

    TextOut(this->m_hDC, 0, 0, buf, lstrlen(buf));
}

void Game::clearScreen(){
    memset(this->m_screen, 0, this->m_height * this->m_width * 3);
}

void Game::screenSwap() {
    glDrawPixels(this->m_width, this->m_height, GL_RGB, GL_UNSIGNED_BYTE, this->m_screen );
    glutSwapBuffers();
}
