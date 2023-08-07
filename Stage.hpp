#pragma once
#include "Object.hpp"

enum class CollisionType {
	AIR,
	BLOCK
};

class Stage {
public:
    void setStage(int* stage, int width, int height, int blockSize);
    void setScreenSize(int width, int height);
    double checkHitVertical(Object* obj);
    double checkHitHorizontal(Object* obj);
	CollisionType getColType(int blockNum);
    int checkHitBlock(Object* obj);
private:
    int* m_stage;
    int m_stageWidth;
    int m_stageHeight;
    int m_screenWidth;
    int m_screenHeight;
    int m_blockSize;
};
