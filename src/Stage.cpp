#include "Stage.hpp"
#include <fstream>

using namespace std;

#pragma comment(lib, "BlockID.lib")

#define BLOCK_CONVEYOR_SPEED 10

namespace {
    std::vector<string> gBlockImagePathes = {
        "assets/images/blocks/stone.bmp",
        "assets/images/blocks/soil.bmp",
        "assets/images/blocks/conveyor_left.bmp",
        "assets/images/blocks/conveyor_right.bmp",
    };
    std::vector<GHandle> gBlockGHandles;

    bool LineIsHitBlock(std::vector<BLOCK_ID> *stage, int sw, int sh, int blockSize, Vector2D pos, int length, bool isVertical) {
        if(isVertical) {
            double offset = pos.y - static_cast<int>(pos.y / blockSize) * blockSize;
            int e = (offset + length - 1) / blockSize;
            if((offset + length - 1) - (e * blockSize) > 0) {
                e++;
            }

            for(int i = 0; i < e; i++) {
                int col = static_cast<int>(pos.x / blockSize);
                int row = static_cast<int>(pos.y / blockSize) + i;

                if(col < 0 || col >= sw
                || row < 0 || row >= sh) {
                    continue;
                }

                int bi = row * sw + col;
                if(Stage::GetCollisionType((*stage)[bi]) == CollisionType::BLOCK) {
                    return true;
                }
            }
        } else {
            double offset = pos.x - static_cast<int>(pos.x / blockSize) * blockSize;
            int e = (offset + length - 1) / blockSize;
            if((offset + length - 1) - (e * blockSize) > 0) {
                e++;
            }

            for(int i = 0; i < e; i++) {
                int col = static_cast<int>(pos.x / blockSize) + i;
                int row = static_cast<int>(pos.y / blockSize);

                if(col < 0 || col >= sw
                || row < 0 || row >= sh) {
                    continue;
                }

                int bi = row * sw + col;

                if(Stage::GetCollisionType((*stage)[bi]) == CollisionType::BLOCK) {
                    return true;
                }
            }
        }

        return false;
    }

    int CheckLineHitBlock(std::vector<BLOCK_ID> *stage, int sw, int sh, int blockSize, Vector2D pos, int length, Vector2D vec, bool isVertical) {
        Vector2D newPos;
        int d = blockSize;
        if(isVertical) {
            if(vec.x == 0) {
                return -1;
            }

            double slope = vec.y / vec.x;
            newPos.x = (static_cast<int>(pos.x / blockSize) + (vec.x > 0 ? 1 : 0)) * blockSize + (vec.x > 0 ? 0 : -1);
            newPos.y = pos.y + (newPos.x - pos.x) * slope;

            if(vec.x < 0) {
                d = -d;
            }
            while(true) {
                if(LineIsHitBlock(stage, sw, sh, blockSize, newPos, length, isVertical)) {
                    return newPos.x / blockSize;
                }

                if((pos.x + vec.x - newPos.x) * vec.x < 0) {
                    break;
                }

                newPos.x += d;
                newPos.y += d * slope;
            }
        } else {
            if(vec.y == 0) {
                return -1;
            }

            double slope = vec.x / vec.y;
            newPos.y = (static_cast<int>(pos.y / blockSize) + (vec.y > 0 ? 1 : 0)) * blockSize + (vec.y > 0 ? 0 : -1);
            newPos.x = pos.x + (newPos.y - pos.y) * slope;

            if(vec.y < 0) {
                d = -d;
            }
            while(true) {
                if(LineIsHitBlock(stage, sw, sh, blockSize, newPos, length, isVertical)) {
                    return newPos.y / blockSize;
                }

                if((pos.y + vec.y - newPos.y) * vec.y < 0) {
                    break;
                }

                newPos.y += d;
                newPos.x += d * slope;
            }
        }

        return -1;
    }

    Vector2D AdjustBoxVector(std::vector<BLOCK_ID> *stage, int sw, int sh, int blockSize, Vector2D pos, int width, int height, Vector2D vec) {
        int blockRow, blockCol;
        double bx, by;
        double dx = 0, dy = 0;
        Vector2D newVec = vec;

        if(vec.x > 0) {
            Vector2D _pos = pos;
            _pos.x = pos.x + width - 1;
            blockCol = CheckLineHitBlock(stage, sw, sh, blockSize, _pos, height, vec, true);
        } else {
            blockCol = CheckLineHitBlock(stage, sw, sh, blockSize, pos, height, vec, true);
        }

        if(vec.y > 0) {
            Vector2D _pos = pos;
            _pos.y = pos.y + height - 1;
            blockRow = CheckLineHitBlock(stage, sw, sh, blockSize, _pos, width, vec, false);
        } else {
            blockRow = CheckLineHitBlock(stage, sw, sh, blockSize, pos, width, vec, false);
        }

        if(blockRow == -1 && blockCol == -1) {
            return vec;
        }

        if(vec.x > 0) {
            bx = blockCol * blockSize;
            dx = bx - (pos.x + width);
        } else if(vec.x < 0) {
            bx = (blockCol + 1) * blockSize;
            dx = bx - pos.x;
        }

        if(vec.y > 0) {
            by = blockRow * blockSize;
            dy = by - (pos.y + height);
        } else if(vec.y < 0) {
            by = (blockRow + 1) * blockSize;
            dy = by - pos.y;
        }

        double absDx = abs(dx);
        double absDy = abs(dy);

        if(blockCol == -1) {
            newVec.y = dy;
        } else  if(blockRow == -1) {
            newVec.x = dx;
        } else if(absDy < absDx) {// || vec.y * dy < 0) {
            newVec.y = dy;
        } else if(absDx < absDy) {// || vec.x * dx < 0) {
            newVec.x = dx;
        } else if(absDx == absDy) {
            newVec.x = dx;
            //newVec.y = dy;
        }

        return newVec;
    }

    Vector2D AdjustVector(Collider collider, Vector2D pos, Vector2D vec, int blockSize, int screenWidth, int screenHeight, std::vector<BLOCK_ID> *stage, int sw, int sh){
        if(vec.x == 0 && vec.y == 0) {
            return vec;
        }

        Vector2D newPos = pos + vec;
        Vector2D newVec = vec;
        bool isRight = vec.x > 0;
        bool isFall = vec.y > 0;
        int checkBlocksX, checkBlocksY;


        for(HitBox hitbox : collider.GetHitBoxes()) {
            if(!hitbox.isPhysics) continue;
            if(!hitbox.isActive) continue;

            Vector2D _newVec = AdjustBoxVector(
                    stage,
                    sw, sh,
                    blockSize,
                    pos + hitbox.pos,
                    hitbox.width,
                    hitbox.height,
                    vec);

            if(vec.x != _newVec.x) {
                Vector2D _pos = pos + hitbox.pos;
                _pos.x += _newVec.x;
                Vector2D _vec;
                _vec.y = _newVec.y;
                _newVec.y = AdjustBoxVector(
                        stage,
                        sw, sh,
                        blockSize,
                        _pos,
                        hitbox.width,
                        hitbox.height,
                        _vec).y;
            } else if(vec.y != _newVec.y) {
                Vector2D _pos = pos + hitbox.pos;
                _pos.y += _newVec.y;
                Vector2D _vec;
                _vec.x = _newVec.x;
                _newVec.x = AdjustBoxVector(
                        stage,
                        sw, sh,
                        blockSize,
                        _pos,
                        hitbox.width,
                        hitbox.height,
                        _vec).x;
            }

            if(abs(_newVec.x) < abs(newVec.x)) {
                newVec.x = _newVec.x;
            }
            if(abs(_newVec.y) < abs(newVec.y)) {
                newVec.y = _newVec.y;
            }
        }

        return newVec;
    }

    void BlockGimmick(Stage *pStage, Object *pObject, int blockSize) {
        Vector2D mostLeftPos, mostRightPos, mostBottomPos;
        int mostLeftLength = 0, mostRightLength = 0, mostBottomLength = 0;

        Vector2D p = pObject->GetPosition();
        for(HitBox h : pObject->GetCollider().GetHitBoxes()) {
            if(!h.isActive || !h.isPhysics) {
                continue;
            }

            Vector2D leftPos(p.x + h.pos.x - 1, p.y + h.pos.y);
            Vector2D rightPos(p.x + h.pos.x + h.width, p.y + h.pos.y);
            Vector2D bottomPos(p.x + h.pos.x, p.y + h.pos.y + h.height);

            if(mostLeftLength == 0 || mostLeftPos.x > leftPos.x) {
                mostLeftPos.x = leftPos.x;
                mostLeftPos.y = leftPos.y;
                mostLeftLength = h.height;
            }
            if(mostRightLength == 0 || mostRightPos.x < rightPos.x) {
                mostRightPos.x = rightPos.x;
                mostRightPos.y = rightPos.y;
                mostRightLength = h.height;
            }
            if(mostBottomLength == 0 || mostBottomPos.y < bottomPos.y) {
                mostBottomPos.x = bottomPos.x;
                mostBottomPos.y = bottomPos.y;
                mostBottomLength = h.width;
            }
        }

        bool blockFlags[BLOCK_ID::BID_NUM];
        for(int i = 0; i < BLOCK_ID::BID_NUM; i++) {
            blockFlags[i] = false;
        }

        if(pObject->IsGround() && mostBottomLength != 0) {
            int s = (int)(mostBottomPos.x / blockSize);
            int e = (int)((mostBottomPos.x + mostBottomLength - 1) / blockSize);
            int y = mostBottomPos.y / blockSize;
            for(int x = s; x <= e; x++) {
                BLOCK_ID blockId = pStage->GetBlockID(x, y);
                if(blockFlags[blockId]) {
                    continue;
                }
                blockFlags[blockId] = true;

                switch(blockId) {
                    case BLOCK_ID::BID_CONVEYOR_LEFT: {
                        Vector2D vec = pObject->GetVector();
                        vec.x = -BLOCK_CONVEYOR_SPEED;
                        pObject->SetVector(vec);
                        break;
                    }
                    case BLOCK_ID::BID_CONVEYOR_RIGHT: {
                        Vector2D vec = pObject->GetVector();
                        vec.x = BLOCK_CONVEYOR_SPEED;
                        pObject->SetVector(vec);
                        break;
                   }
                    default:
                        break;
                }
            }
        }

        for(int i = 0; i < BLOCK_ID::BID_NUM; i++) {
            blockFlags[i] = false;
        }

        if(pObject->IsWall() && mostLeftLength != 0) {
            int s = (int)(mostLeftPos.y / blockSize);
            int e = (int)((mostLeftPos.y + mostLeftLength - 1) / blockSize);
            int x = mostLeftPos.x / blockSize;
            for(int y = s; y <= e; y++) {
                BLOCK_ID blockId = pStage->GetBlockID(x, y);
                if(blockFlags[blockId]) {
                    continue;
                }
                switch(blockId) {
                    default:
                        break;
                }
            }
        }

        for(int i = 0; i < BLOCK_ID::BID_NUM; i++) {
            blockFlags[i] = false;
        }

        if(pObject->IsWall() && mostRightLength != 0) {
            int s = (int)(mostRightPos.y / blockSize);
            int e = (int)((mostRightPos.y + mostRightLength - 1) / blockSize);
            int x = mostRightPos.x / blockSize;
            for(int y = s; y <= e; y++) {
                BLOCK_ID blockId = pStage->GetBlockID(x, y);
                if(blockFlags[blockId]) {
                    continue;
                }
                switch(blockId) {
                    default:
                        break;
                }
            }
        }
    }

    GHandle GetBlockGHandle(BLOCK_ID blockId) {
        int i = blockId - 1;
        if(i < 0 || i >= gBlockGHandles.size()) {
            return -1;
        }

        return gBlockGHandles[i];
    }
};

void Stage::Init(OctGame *pOctGame) {
    for(string path : gBlockImagePathes) {
        gBlockGHandles.push_back(pOctGame->LoadImageFile(path, true));
    }
}

void Stage::LoadStage(OctGame* pOctGame, const string filepath, int blockSize, std::vector<STAGEOBJECTINFO> *outStageObjects) {
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
        if((BLOCK_ID)stage[i] == BLOCK_ID::BID_NONE || IsBlock((BLOCK_ID)stage[i])) {
            this->mStage[i] = (BLOCK_ID)(stage[i]);
        } else {
            STAGEOBJECTINFO soi;
            soi.blockId = (BLOCK_ID)stage[i];
            soi.position.x = i % width * blockSize;
            soi.position.y = (int)(i / width) * blockSize;
            outStageObjects->push_back(soi);
        }
    }

    delete[] stage;
}

void Stage::SetStage(const int* stage, int width, int height, int blockSize, std::vector<STAGEOBJECTINFO> *outStageObjects) {
    this->mStage.resize(width * height);
    for(int i = 0; i < width * height; i++) {
        if(IsBlock((BLOCK_ID)stage[i])) {
            this->mStage[i] = (BLOCK_ID)stage[i];
        } else {
            STAGEOBJECTINFO soi;
            soi.blockId = (BLOCK_ID)stage[i];
            soi.position.x = i % width * blockSize;
            soi.position.y = (int)(i / width) * blockSize;
            outStageObjects->push_back(soi);
        }
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

CollisionType Stage::GetCollisionType(BLOCK_ID blockId) {
    if(IsBlock(blockId)) {
        return CollisionType::BLOCK;
    }

    return CollisionType::AIR;
}

void Stage::Translate(Object *pObject, Vector2D vec) {
    Vector2D pos = pObject->GetPosition();
    Vector2D newVec;

    newVec = AdjustVector(
            pObject->GetCollider(),
            pos,
            vec,
            this->mBlockSize,
            this->mScreenWidth,
            this->mScreenHeight,
            &this->mStage,
            this->mStageWidth,
            this->mStageHeight);

    bool hitGround = vec.y > 0 && vec.y != newVec.y;
    bool hitWall = vec.x != newVec.x;

    pObject->Translate(newVec.x, newVec.y);

    pObject->SetIsGround(hitGround);
    pObject->SetIsWall(hitWall);

    BlockGimmick(this, pObject, this->mBlockSize);
}

void Stage::TranslateVector(Object *pObject) {
    Vector2D vec = pObject->GetVector();
    Vector2D pos = pObject->GetPosition();
    Vector2D newVec;

    newVec = AdjustVector(
            pObject->GetCollider(),
            pos,
            vec,
            this->mBlockSize,
            this->mScreenWidth,
            this->mScreenHeight,
            &this->mStage,
            this->mStageWidth,
            this->mStageHeight);

    pObject->SetVector(newVec);
    pObject->UpdatePosition();

    bool hitGround = vec.y > 0 && vec.y != newVec.y;
    bool hitWall = vec.x != newVec.x;

    pObject->SetIsGround(hitGround);
    pObject->SetIsWall(hitWall);

    BlockGimmick(this, pObject, this->mBlockSize);
}

void Stage::Draw(OctGame* pOctGame, Camera* pCamera) {
    for(int y = 0; y < this->mScreenHeight && y < this->mScreenHeight / this->mBlockSize + 1; y++){
        for(int x = 0; x < this->mStageWidth && x < this->mScreenWidth / this->mBlockSize + 1; x++){
            Vector2D cameraPos = pCamera->GetPosition();
            int x1 = x * this->mBlockSize - static_cast<int>(cameraPos.x) % this->mBlockSize,
                y1 = y * this->mBlockSize,
                x2 = (x + 1) * this->mBlockSize - static_cast<int>(cameraPos.x) % this->mBlockSize,
                y2 = (y + 1) * this->mBlockSize;
            int idx = y * this->mStageWidth + x + cameraPos.x / this->mBlockSize;

            if(idx < 0 || this->mStageWidth * this->mStageHeight <= idx) continue;

            GHandle gh = GetBlockGHandle(this->mStage[idx]);

            if(gh >= 0) {
                pOctGame->DrawImage(gh, x1, y1);
            }
        }
    }
}

BLOCK_ID Stage::GetBlockID(int x, int y) {
    if(0 <= x && x < this->mStageWidth
    && 0 <= y && y < this->mStageHeight) {
        return this->mStage[y * this->mStageWidth + x];
    }

    return BLOCK_ID::BID_NONE;
}
