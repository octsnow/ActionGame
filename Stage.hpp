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
    void setStage(const int* stage, int width, int height, int blockSize);
    void setScreenSize(int width, int height);
	Vector2d adjustVector(Object* obj);
	CollisionType getColType(int blockNum);
    int checkHitBlock(Object* obj);
    void draw(OctGame* game, Vector2d cameraPos);

private:
    std::vector<int> m_stage;
    int m_stageWidth;
    int m_stageHeight;
    int m_screenWidth;
    int m_screenHeight;
    int m_blockSize;
};
