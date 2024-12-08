#include "Stage.hpp"
#include <fstream>

#define BLOCK_STONE 1
#define BLOCK_SOIL 2

using namespace std;

namespace {
    int g_ihandle_block_soil;
    int g_ihandle_block_stone;
};

void Stage::LoadStage(OctGame* pOctGame, const string filepath, int blockSize) {
    g_ihandle_block_soil = pOctGame->LoadImageFile("assets/images/blocks/soil.bmp", true);
    g_ihandle_block_stone = pOctGame->LoadImageFile("assets/images/blocks/stone.bmp", true);

    ifstream ifs(filepath);

    if(!ifs) {
        cerr << "error: failed to load stage file" << filepath << endl;
        return;
    }

    uint8_t width, height;
    ifs.read((char*)&width, sizeof(uint8_t));
    ifs.read((char*)&height, sizeof(uint8_t));

    this->mStageWidth = static_cast<int>(width);
    this->mStageHeight = static_cast<int>(height);
    this->mBlockSize = blockSize;

    this->mStage.resize(this->mStageWidth * this->mStageHeight);

    uint8_t* stage = new uint8_t[this->mStageWidth * this->mStageHeight];
    ifs.read((char*)stage, sizeof(uint8_t) * this->mStageWidth * this->mStageHeight);
    for(int i = 0; i < width * height; i++) {
        this->mStage[i] = static_cast<int>(stage[i]);
    }

    delete[] stage;
}

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

int Stage::GetWidth() {
    return this->mStageWidth;
}

int Stage::GetHeight() {
    return this->mStageHeight;
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

        bool hitGround = vec.y > newVec.y;
        bool hitWall = (vec.x * newVec.x > 0 && abs(vec.x) > abs(newVec.x)) || vec.x * newVec.x < 0;

        //pObject->SetVector(newVec.x, newVec.y);
        pObject->Translate(newVec.x, newVec.y);
        pObject->SetVector(hitWall ? 0 : vec.x, hitGround ? 0 : vec.y);

        pObject->SetIsGround(hitGround);
        pObject->SetIsWall(hitWall);
    });
}

void Stage::Draw(OctGame* pOctGame, Camera* pCamera) {
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
            case BLOCK_SOIL:
                pOctGame->DrawImage(g_ihandle_block_soil, x1, y1);
//                pOctGame->DrawBox(x1, y1, x2, y2, 0x00FFFF, true);
                break;
            case BLOCK_STONE:
                pOctGame->DrawImage(g_ihandle_block_stone, x1, y1);
//                pOctGame->DrawBox(x1, y1, x2, y2, 0xFFFF00, true);
                break;
            }
        }
    }
}
