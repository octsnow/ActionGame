#include "Camera.hpp"

Camera::Camera(unsigned int stageWidth, unsigned int stageHeight, unsigned int blockSize, unsigned int screenWidth)
: mPos({0, 0})
, mStageWidth(stageWidth)
, mStageHeight(stageHeight)
, mBlockSize(blockSize)
, mScreenWidth(screenWidth) {
}

void Camera::SetPlayerPosition(Vector2D pos) {
    Vector2D cameraPos;
    if(pos.x < static_cast<int>(this->mScreenWidth / 2)) {
        this->mPos.x = 0;
    } else if(pos.x > this->mStageWidth * this->mBlockSize - static_cast<int>(this->mScreenWidth / 2)) {
        this->mPos.x = this->mStageWidth * this->mBlockSize - this->mScreenWidth;
    } else {
        this->mPos.x = pos.x - static_cast<int>(this->mScreenWidth / 2);
    }
}

Vector2D Camera::GetPosition() const {
    return this->mPos;
}

Vector2D Camera::CalcViewPosition(Vector2D pos) {
    return this->CalcViewPosition(pos.x, pos.y);
}

Vector2D Camera::CalcViewPosition(double x, double y) {
    Vector2D pos;
    pos.x = x - this->mPos.x;
    pos.y = y - this->mPos.y;

    return pos;
}
