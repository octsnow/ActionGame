#include "Stage.hpp"

namespace {
};

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

CollisionType Stage::getColType(int blockNum) {
    if(0 < blockNum && blockNum < 3) {
        return CollisionType::BLOCK;
    }

    return CollisionType::AIR;
}

Vector2d Stage::adjustVector(Object* obj){
    Collider* pObjCollider = obj->getCollider();
    Vector2d objPos = obj->getPosition();
    Vector2d objVec = obj->getVector();
    Vector2d objNewPos = {
        objPos.x + objVec.x,
        objPos.y + objVec.y
    };
    Vector2d objNewVec = objVec;
    bool isFall = objVec.y > 0;
    bool isRight = objVec.x > 0;
    int topBlkY, bottomBlkY, leftBlkX, rightBlkX;
    int checkBlkX, checkBlkY;

    for(auto r : pObjCollider->getRects()) {
        Vector2d rectWorld = {
            objPos.x + r.pos.x,
            objPos.y + r.pos.y
        };
        Vector2d rectNewWorld = {
            objNewPos.x + r.pos.x,
            objNewPos.y + r.pos.y
        };

        int const leftBlkX      = rectWorld.x / this->m_blockSize,
                  topBlkY       = rectWorld.y / this->m_blockSize,
                  bottomBlkY    = (rectWorld.y + r.height - 1) / this->m_blockSize,
                  rightBlkX     = (rectWorld.x + r.width - 1) / this->m_blockSize;

        if(isFall){
            checkBlkY = (rectNewWorld.y + r.height) / this->m_blockSize;
        }else{
            checkBlkY = (rectNewWorld.y - 1) / this->m_blockSize;
        }

        if(isRight) {
            checkBlkX = (rectNewWorld.x + r.width) / this->m_blockSize;
        } else {
            checkBlkX = (rectNewWorld.x - 1) / this->m_blockSize;
        }
       for(int y = topBlkY; y <= bottomBlkY; y++) {
            int i = y * this->m_stageWidth + checkBlkX;
            if(i < 0 || i >= this->m_stageWidth * this->m_stageHeight) {
                continue;
            }
            if(this->getColType(this->m_stage[i]) == CollisionType::BLOCK) {
                if(isRight) {
                    double tmp = (checkBlkX * this->m_blockSize) - (rectWorld.x + r.width);
                    if(tmp < objNewVec.x) {
                        objNewVec.x = tmp;
                    }
                } else {
                    double tmp = ((checkBlkX + 1) * this->m_blockSize) - rectWorld.x;
                    if(tmp > objNewVec.x) {
                        objNewVec.x = tmp;
                    }
                }
                break;
            }
        }
        
        int offset = checkBlkY * this->m_stageWidth;

        if(rectNewWorld.y + r.height >= this->m_screenHeight) {
            objNewVec.y = this->m_screenHeight - (rectWorld.y + r.height);
            continue;
        }

        for(int x = leftBlkX; x <= rightBlkX; x++) {
            int i = offset + x;
            if(i < 0 || i >= this->m_stageWidth * this->m_stageHeight) {
                continue;
            }
            if(0 < this->m_stage[i] && this->m_stage[i] < 3) {
                if(isFall) {
                    double tmp = (checkBlkY * this->m_blockSize) - (rectWorld.y + r.height);
                    if(tmp < objNewVec.y) {
                        objNewVec.y = tmp;
                    }
                } else {
                    double tmp = ((checkBlkY + 1) * this->m_blockSize) - rectWorld.y;
                    if(tmp > objNewVec.y) {
                        objNewVec.y = tmp;
                    }
                }

                break;
            }
        }
    }

    return objNewVec;
}

int Stage::checkHitBlock(Object* obj) {
    int flag = 0;
    Vector2d pos = obj->getPosition();
    Vector2d vec = obj->getVector();
    Vector2d newVec;

    newVec = this->adjustVector(obj);

    obj->setVector(newVec.x, newVec.y);
    obj->setIsGround(vec.y > newVec.y);

    return flag;
}
