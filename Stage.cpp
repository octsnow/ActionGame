#include "Stage.hpp"

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

double Stage::checkHitVertical(Object* obj){
    Collider* collider = obj->getCollider();
    Vector2d pos = obj->getPosition();
	Vector2d vec = obj->getVector();
	double newPosY = pos.y + vec.y;
	double newVecY = obj->getVector().y;
    bool isFall = vec.y > 0;
	
	obj->setPosition(pos.x, newPosY);

    for(auto r : collider->getRects()) {
	    int const left = pos.x + r.pos.x;
	    int const right = left + r.width - 1;
	    int leftBlkX, rightBlkX, checkBlkY, offset;
		double rectWorldY = newPosY + r.pos.y;

	    if(rectWorldY + r.height >= this->m_screenHeight){
			newVecY = this->m_screenHeight - (pos.y + r.pos.y + r.height);
			continue;
	    }

	    if(isFall){
			checkBlkY = (rectWorldY + r.height) / this->m_blockSize;
	    }else{
			checkBlkY = (rectWorldY - 1) / this->m_blockSize;
	    }

	    offset = checkBlkY * this->m_stageWidth;

	    leftBlkX = left / this->m_blockSize;
	    rightBlkX = right / this->m_blockSize;

	    for(int i = leftBlkX; i <= rightBlkX; i++) {
			int index = offset + i;
			if(index < 0 || index >= this->m_stageWidth * this->m_stageHeight) continue;
			if(0 < this->m_stage[index] && this->m_stage[index] < 3) {
				if(isFall) {
					double tmp = (checkBlkY * this->m_blockSize) - (pos.y + r.pos.y + r.height);
					if(tmp < newVecY) {
						newVecY = tmp;
					}
				} else {
					double tmp = ((checkBlkY + 1) * this->m_blockSize) - rectWorldY;
					if(tmp > newVecY) {
						newVecY = tmp;
					}
				}

				break;
			}
	    }
    }

	obj->setPosition(pos.x, pos.y);

    return newVecY;
}

double Stage::checkHitHorizontal(Object* obj) {
    Collider* collider = obj->getCollider();
    Vector2d pos = obj->getPosition();
	Vector2d vec = obj->getVector();
	double newPosX = pos.x + vec.x;
	double newVecX = vec.x;
	int topBlkY, bottomBlkY, checkBlkX;
    bool isRight = vec.x > 0;

	obj->setPosition(newPosX, pos.y);

    for(auto r : collider->getRects()) {
	    int const  top = pos.y + r.pos.y;
	    int const  bottom = top + r.height - 1;
		double rectWorldX = newPosX + r.pos.x;

		if(isRight) {
			checkBlkX = (rectWorldX + r.width) / this->m_blockSize;
		} else {
			checkBlkX = (rectWorldX - 1) / this->m_blockSize;
		}
		
	    topBlkY = top / this->m_blockSize;
	    bottomBlkY = bottom / this->m_blockSize;

	    for(int y = topBlkY; y <= bottomBlkY; y++) {
			int i = y * this->m_stageWidth + checkBlkX;
			if(i < 0 || i >= this->m_stageWidth * this->m_stageHeight) continue;
			if(this->getColType(this->m_stage[i]) == CollisionType::BLOCK) {
				if(isRight) {
					double tmp = (checkBlkX * this->m_blockSize) - (pos.x + r.pos.x + r.width);
					if(tmp < newVecX) {
						newVecX = tmp;
					}
				} else {
					double tmp = ((checkBlkX + 1) * this->m_blockSize) - (pos.x + r.pos.x);
					if(tmp > newVecX) {
						newVecX = tmp;
					}
				}
				break;
			}
	    }
    }

	obj->setPosition(pos.x, pos.y);

    return newVecX;
}

int Stage::checkHitBlock(Object* obj) {
    int flag = 0;
	Vector2d pos = obj->getPosition();
	Vector2d vec = obj->getVector();
	Vector2d newVec;

	newVec.x = this->checkHitHorizontal(obj);
	newVec.y = this->checkHitVertical(obj);

	obj->setVector(newVec.x, newVec.y);
	obj->setIsGround(vec.y > newVec.y);

    return flag;
}
