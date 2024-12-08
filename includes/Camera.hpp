#pragma once

#include "Util.hpp"

class Camera {
public:
    Camera(unsigned int stageWidth, unsigned int stageHeight, unsigned int blockSize, unsigned int screenWidth);
    void SetPlayerPosition(Vector2d pos);
    Vector2d GetPosition() const;
    Vector2d CalcViewPosition(Vector2d pos);
    Vector2d CalcViewPosition(double x, double y);

private:
    Vector2d mPos;
    unsigned int mStageWidth;
    unsigned int mStageHeight;
    unsigned int mBlockSize;
    unsigned int mScreenWidth;
};
