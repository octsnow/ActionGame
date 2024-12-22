#pragma once

#include "Util.hpp"

class Camera {
public:
    Camera(unsigned int stageWidth, unsigned int stageHeight, unsigned int blockSize, unsigned int screenWidth);
    void SetPlayerPosition(Vector2D pos);
    Vector2D GetPosition() const;
    Vector2D CalcViewPosition(Vector2D pos);
    Vector2D CalcViewPosition(double x, double y);

private:
    Vector2D mPos;
    unsigned int mStageWidth;
    unsigned int mStageHeight;
    unsigned int mBlockSize;
    unsigned int mScreenWidth;
};
