#include "Stage.hpp"

namespace {
};

void Stage::SetStage(const int* stage, int width, int height, int blockSize) {
    this->mStage.resize(width * height);
    for(int i = 0; i < width * height; i++) {
        this->mStage[i] = stage[i];
    }

    this->mStageWidth = width;
    this->mStageHeight = height;
    this->mBlockSize = blockSize;
}

void Stage::SetScreenSize(int width, int height) {
    this->mScreenWidth = width;
    this->mScreenHeight = height;
}

CollisionType Stage::GetColType(int blockNum) {
    if(0 < blockNum && blockNum < 3) {
        return CollisionType::BLOCK;
    }

    return CollisionType::AIR;
}

Vector2d Stage::AdjustVector(Object* obj){
//    assert(obj->HasColliders());
    Collider collider = obj->GetCollider();
//    Collider collider = obj->GetCurrentCollider();
    Vector2d pos = obj->GetPosition();
    Vector2d vec = obj->GetVector();
    Vector2d newPos = {
        pos.x + vec.x,
        pos.y + vec.y
    };
    Vector2d objNewVec = vec;
    bool isFall = vec.y > 0;
    bool isRight = vec.x > 0;
    int topBlkY, bottomBlkY, leftBlkX, rightBlkX;
    int checkBlkX, checkBlkY;

    for(HitBox hitBox : collider.GetHitBoxes()) {
        if(!hitBox.isPhysics) continue;
        if(!hitBox.isActive) continue;

        Vector2d rectWorld = {
            pos.x + hitBox.pos.x,
            pos.y + hitBox.pos.y
        };
        Vector2d rectNewWorld = {
            newPos.x + hitBox.pos.x,
            newPos.y + hitBox.pos.y
        };

        int const leftBlkX      = rectWorld.x / this->mBlockSize,
                  topBlkY       = rectWorld.y / this->mBlockSize,
                  bottomBlkY    = (rectWorld.y + hitBox.height - 1) / this->mBlockSize,
                  rightBlkX     = (rectWorld.x + hitBox.width - 1) / this->mBlockSize;

        if(isFall){
            checkBlkY = (rectNewWorld.y + hitBox.height) / this->mBlockSize;
        }else{
            checkBlkY = (rectNewWorld.y - 1) / this->mBlockSize;
        }

        if(isRight) {
            checkBlkX = (rectNewWorld.x + hitBox.width) / this->mBlockSize;
        } else {
            checkBlkX = (rectNewWorld.x - 1) / this->mBlockSize;
        }
       for(int y = topBlkY; y <= bottomBlkY; y++) {
            int i = y * this->mStageWidth + checkBlkX;
            if(i < 0 || i >= this->mStageWidth * this->mStageHeight) {
                continue;
            }
            if(this->GetColType(this->mStage[i]) == CollisionType::BLOCK) {
                if(isRight) {
                    double tmp = (checkBlkX * this->mBlockSize) - (rectWorld.x + hitBox.width);
                    if(tmp < objNewVec.x) {
                        objNewVec.x = tmp;
                    }
                } else {
                    double tmp = ((checkBlkX + 1) * this->mBlockSize) - rectWorld.x;
                    if(tmp > objNewVec.x) {
                        objNewVec.x = tmp;
                    }
                }
                break;
            }
        }
        
        int offSet = checkBlkY * this->mStageWidth;

        if(rectNewWorld.y + hitBox.height >= this->mScreenHeight) {
            objNewVec.y = this->mScreenHeight - (rectWorld.y + hitBox.height);
            continue;
        }

        for(int x = leftBlkX; x <= rightBlkX; x++) {
            int i = offSet + x;
            if(i < 0 || i >= this->mStageWidth * this->mStageHeight) {
                continue;
            }
            if(0 < this->mStage[i] && this->mStage[i] < 3) {
                if(isFall) {
                    double tmp = (checkBlkY * this->mBlockSize) - (rectWorld.y + hitBox.height);
                    if(tmp < objNewVec.y) {
                        objNewVec.y = tmp;
                    }
                } else {
                    double tmp = ((checkBlkY + 1) * this->mBlockSize) - rectWorld.y;
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

void Stage::CheckHitBlock(ObjectList& objectList) {
    objectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        Vector2d pos = pObject->GetPosition();
        Vector2d vec = pObject->GetVector();
        Vector2d newVec;

        newVec = this->AdjustVector(pObject);

        pObject->SetVector(newVec.x, newVec.y);
        pObject->SetIsGround(vec.y > newVec.y);
        pObject->SetIsWall(abs(vec.x) > abs(newVec.x));
    });
}

void Stage::Draw(OctGame* game, Camera* pCamera) {
    for(int y = 0; y < this->mScreenHeight && y < this->mScreenHeight / this->mBlockSize + 1; y++){
        for(int x = 0; x < this->mStageWidth && x < this->mScreenWidth / this->mBlockSize + 1; x++){
            Vector2d cameraPos = pCamera->GetPosition();
            int x1 = x * this->mBlockSize - static_cast<int>(cameraPos.x) % this->mBlockSize,
                y1 = y * this->mBlockSize,
                x2 = (x + 1) * this->mBlockSize - static_cast<int>(cameraPos.x) % this->mBlockSize,
                y2 = (y + 1) * this->mBlockSize;
            int idx = y * this->mStageWidth + x + cameraPos.x / this->mBlockSize;

            if(idx < 0 || this->mStageWidth * this->mStageHeight <= idx) continue;
            switch(this->mStage[idx]){
            case 1:
//                game->drawImage(gGlassBlock, x1, y1);
                game->DrawBox(x1, y1, x2, y2, 0x00FFFF, true);
                break;
            case 2:
                game->DrawBox(x1, y1, x2, y2, 0xFFFF00, true);
                break;
            }
        }
    }
}
