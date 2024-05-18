#include "Camera.hpp"

Camera::Camera(unsigned int stageWidth, unsigned int stageHeight, unsigned int blockSize, unsigned int screenWidth)
: mPos({0, 0})
, mStageWidth(stageWidth)
, mStageHeight(stageHeight)
, mBlockSize(blockSize)
, mScreenWidth(screenWidth) {
}

void Camera::SetPlayerPosition(Vector2d pos) {
    Vector2d cameraPos;
    if(pos.x < static_cast<int>(this->mScreenWidth / 2)) {
        this->mPos.x = 0;
    } else if(pos.x > this->mStageWidth * this->mBlockSize - static_cast<int>(this->mScreenWidth / 2)) {
        this->mPos.x = this->mStageWidth * this->mBlockSize - this->mScreenWidth;
    } else {
        this->mPos.x = pos.x - static_cast<int>(this->mScreenWidth / 2);
    }
}

Vector2d Camera::GetPosition() const {
    return this->mPos;
}

Vector2d Camera::CalcViewPosition(Vector2d pos) {
    return this->CalcViewPosition(pos.x, pos.y);
}

Vector2d Camera::CalcViewPosition(double x, double y) {
    Vector2d pos;
    pos.x = x - this->mPos.x;
    pos.y = y - this->mPos.y;

    return pos;
}
