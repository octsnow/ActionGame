#pragma once

#include "OctGame.hpp"
#include "Object.hpp"
#include <string>
#include <vector>

enum class CollisionType {
	AIR,
    BLOCK
};

class Stage {
public:
    void LoadStage(OctGame* pOctGame, const std::string filepath, int blockSize);
    void SetStage(const int* stage, int width, int height, int blockSize);
    void SetScreenSize(int width, int height);

    int GetWidth();
    int GetHeight();
	CollisionType GetColType(int blockNum);

	Vector2d AdjustVector(Object* obj);
    void CheckHitBlock(ObjectList& objList);
    void Draw(OctGame* game, Camera* pCamera);

private:
    std::vector<int> mStage;
    int mStageWidth;
    int mStageHeight;
    int mScreenWidth;
    int mScreenHeight;
    int mBlockSize;
};
