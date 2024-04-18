#include "Stage.hpp"

namespace {
};

void Stage::setStage(const int* stage, int width, int height, int blockSize) {
    this->m_stage.resize(width * height);
    for(int i = 0; i < width * height; i++) {
        this->m_stage[i] = stage[i];
    }

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
    Collider* pObjCollider = obj->getCurrentCollider();
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

    for(auto hitBox : pObjCollider->getHitBoxes()) {
        if(!hitBox.isPhysics) continue;

        Vector2d rectWorld = {
            objPos.x + hitBox.pos.x,
            objPos.y + hitBox.pos.y
        };
        Vector2d rectNewWorld = {
            objNewPos.x + hitBox.pos.x,
            objNewPos.y + hitBox.pos.y
        };

        int const leftBlkX      = rectWorld.x / this->m_blockSize,
                  topBlkY       = rectWorld.y / this->m_blockSize,
                  bottomBlkY    = (rectWorld.y + hitBox.height - 1) / this->m_blockSize,
                  rightBlkX     = (rectWorld.x + hitBox.width - 1) / this->m_blockSize;

        if(isFall){
            checkBlkY = (rectNewWorld.y + hitBox.height) / this->m_blockSize;
        }else{
            checkBlkY = (rectNewWorld.y - 1) / this->m_blockSize;
        }

        if(isRight) {
            checkBlkX = (rectNewWorld.x + hitBox.width) / this->m_blockSize;
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
                    double tmp = (checkBlkX * this->m_blockSize) - (rectWorld.x + hitBox.width);
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

        if(rectNewWorld.y + hitBox.height >= this->m_screenHeight) {
            objNewVec.y = this->m_screenHeight - (rectWorld.y + hitBox.height);
            continue;
        }

        for(int x = leftBlkX; x <= rightBlkX; x++) {
            int i = offset + x;
            if(i < 0 || i >= this->m_stageWidth * this->m_stageHeight) {
                continue;
            }
            if(0 < this->m_stage[i] && this->m_stage[i] < 3) {
                if(isFall) {
                    double tmp = (checkBlkY * this->m_blockSize) - (rectWorld.y + hitBox.height);
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
    obj->setIsWall(abs(vec.x) > abs(newVec.x));

    return flag;
}

void Stage::draw(OctGame* game, Vector2d cameraPos) {
    for(int y = 0; y < this->m_screenHeight && y < this->m_screenHeight / this->m_blockSize + 1; y++){
        for(int x = 0; x < this->m_stageWidth && x < this->m_screenWidth / this->m_blockSize + 1; x++){
            int x1 = x * this->m_blockSize - static_cast<int>(cameraPos.x) % this->m_blockSize,
                y1 = y * this->m_blockSize,
                x2 = (x + 1) * this->m_blockSize - static_cast<int>(cameraPos.x) % this->m_blockSize,
                y2 = (y + 1) * this->m_blockSize;
            int idx = y * this->m_stageWidth + x + cameraPos.x / this->m_blockSize;

            if(idx < 0 || this->m_stageWidth * this->m_stageHeight <= idx) continue;
            switch(this->m_stage[idx]){
            case 1:
//                game->drawImage(gGlassBlock, x1, y1);
                game->drawBox(x1, y1, x2, y2, 0x00FFFF);
                break;
            case 2:
                game->drawBox(x1, y1, x2, y2, 0xFFFF00);
                break;
            }
        }
    }
}
