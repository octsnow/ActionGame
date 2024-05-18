#pragma once

#include "OctGame/OctGame.hpp"
#include "Object.hpp"
#include <vector>

enum class CollisionType {
	AIR,
	BLOCK
};

class Stage {
public:
    void SetStage(const int* stage, int width, int height, int blockSize);
    void SetScreenSize(int width, int height);
	Vector2d AdjustVector(Object* obj);
	CollisionType GetColType(int blockNum);
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
