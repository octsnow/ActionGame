#pragma once

#include "BlockId.hpp"
#include "OctGame.hpp"
#include "Object.hpp"

#include <string>
#include <vector>

enum class CollisionType {
	AIR,
    BLOCK
};

typedef struct stage_object_info_tag {
    BLOCK_ID blockId;
    Vector2D position;
} STAGEOBJECTINFO;

class Stage {
public:
    void Init(OctGame *pOctGame);
    void LoadStage(OctGame *pOctGame, const std::string filepath, int blockSize, std::vector<STAGEOBJECTINFO> *outStageObjects);
    void SetStage(const int *stage, int width, int height, int blockSize, std::vector<STAGEOBJECTINFO> *outStageObjects);
    void SetScreenSize(int width, int height);

    int GetWidth();
    int GetHeight();
	static CollisionType GetCollisionType(BLOCK_ID blockId);

    void Translate(Object *pObject, Vector2D vec);
    void TranslateVector(Object *pObject);
    void Draw(OctGame* game, Camera* pCamera);

    BLOCK_ID GetBlockID(int x, int y);

private:
    std::vector<BLOCK_ID> mStage;
    int mStageWidth;
    int mStageHeight;
    int mScreenWidth;
    int mScreenHeight;
    int mBlockSize;
};
