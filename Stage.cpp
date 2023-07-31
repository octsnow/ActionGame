#include "Stage.hpp"

void Stage::setStage(int* stage, int width, int height, int blockSize) {
    this->m_stage = stage;
    this->m_stageWidth = width;
    this->m_stageHeight = height;
    this->m_blockSize = blockSize;
}

void Stage::setScreenSize(int width, int height) {
    this->m_screenWidth = width;
    this->m_screenHeight = height;
}

int Stage::getHitVertical(Object obj){
    bool isFall = obj.getVector().y > 0;
    const int width = obj.getWidth();
    const int height = obj.getHeight();
    Vector2d pos = obj.getPosition();
    int const  left = pos.x;
    int const  right = pos.x + width - 1;
    int groundY = -1;
    int leftBlkX, rightBlkX, checkBlkY, offset;

    if(pos.y + height >= this->m_screenHeight){
        return this->m_screenHeight - height;
    }

    if(isFall){
        checkBlkY = (pos.y + height) / this->m_blockSize;
    }else{
        checkBlkY = (pos.y - 1 ) / this->m_blockSize;
    }

    offset = checkBlkY * this->m_stageWidth;

    leftBlkX = left / this->m_blockSize;
    rightBlkX = right / this->m_blockSize;

    for(int i = leftBlkX; i <= rightBlkX; i++) {
        int index = offset + i;
        if(index < 0 || index >= this->m_stageWidth * this->m_stageHeight) continue;
        if(0 < this->m_stage[index] && this->m_stage[index] < 3) {
            if(isFall) {
                groundY = checkBlkY * this->m_blockSize - height;
            } else {
                groundY = checkBlkY * this->m_blockSize + this->m_blockSize;
            }
            break;
        }
    }

    return groundY;
}

int Stage::getHitHorizontal(Object obj){
    bool isRight = obj.getVector().x > 0;
    const int width = obj.getWidth();
    const int height = obj.getHeight();
    const Vector2d pos = obj.getPosition();
    int const  top = pos.y;
    int const  bottom = pos.y + height - 1;
    int WallX = -1;
    int topBlkY, bottomBlkY, checkBlkX;
    if(isRight) {
        checkBlkX = (pos.x + width) / this->m_blockSize;
    } else {
        checkBlkX = (pos.x - 1) / this->m_blockSize;
    }

    topBlkY = top / this->m_blockSize;
    bottomBlkY = bottom / this->m_blockSize;

    for(int i = topBlkY; i <= bottomBlkY; i++) {
        int index = i * this->m_stageWidth + checkBlkX;
        if(index < 0 || index >= this->m_stageWidth * this->m_stageHeight) continue;
        if(0 < this->m_stage[index] && this->m_stage[index] < 3){
            if(isRight) {
                WallX = checkBlkX * this->m_blockSize - width; 
            } else {
                WallX = (checkBlkX + 1) * this->m_blockSize;
            }
        }
    }

    return WallX;
}

int Stage::checkHitBlock(Object* obj) {
    bool isGround = false;
    int groundY = -1, wallX = -1;
    int flag = 0;

    if(obj->getVector().y != 0) {
        Vector2d tmpPos = obj->getPosition();
        Vector2d pVec = obj->getVector();
        obj->translate(0, pVec.y);
        groundY = this->getHitVertical(*obj);
        obj->setPosition(obj->getPosition().x, tmpPos.y);
    }

    if(obj->getVector().x != 0) {
        Vector2d tmpPos = obj->getPosition();
        Vector2d pVec = obj->getVector();
        obj->translate(pVec.x, 0);
        wallX = this->getHitHorizontal(*obj);
        obj->setPosition(tmpPos.x, obj->getPosition().y);
    }

    if(groundY >= 0) {
        isGround = obj->getVector().y > 0;
        obj->setVector(obj->getVector().x, 0);
        obj->setPosition(obj->getPosition().x, groundY);
        flag += 1; // flag 01
    }

    if(wallX >= 0) {
        obj->setVector(0, obj->getVector().y);
        obj->setPosition(wallX, obj->getPosition().y);
        flag += 2; // flag 10
    }

    obj->setIsGround(isGround);

    return flag;
}
