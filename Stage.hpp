#pragma once
#include "Object.hpp"

class Stage {
public:
    void setStage(int* stage, int width, int height, int blockSize);
    void setScreenSize(int width, int height);
    int getHitVertical(Object obj);
    int getHitHorizontal(Object obj);
    int checkHitBlock(Object* obj);
private:
    int* m_stage;
    int m_stageWidth;
    int m_stageHeight;
    int m_screenWidth;
    int m_screenHeight;
    int m_blockSize;
};
