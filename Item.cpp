#include "Item.hpp"
#include <iostream>
#include <cmath>
#include <numbers>
#include <time.h>

#define ITEM_LIFE_TIME 500 // the item living time
#define ITEM_MOVE_CYCLE_TIME 2
#define ITEM_MOVE_HEIGHT 10

using namespace std;

namespace {
}

Item::Item() {
    this->mStartTime = clock();
}

void Item::Update(OctGame* pOctGame) {
    Vector2d pos = this->GetOffsetPosition();
    time_t elapsedTime = clock() - this->mStartTime;
    pos.y = sin((elapsedTime % (ITEM_MOVE_CYCLE_TIME * 1000) / (double)(ITEM_MOVE_CYCLE_TIME * 1000)) * numbers::pi) * ITEM_MOVE_HEIGHT - ITEM_MOVE_HEIGHT;
    this->SetOffsetPosition(pos.x, pos.y);
}

bool Item::CanPicup() {
    if(clock() - this->mStartTime < ITEM_LIFE_TIME) {
        return false;
    }
    return true;
}

void Coin::Init(OctGame* pOctGame) {
    Collider collider;

    collider.AddHitBox(0, 0, 50, 50, true, false);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
    this->SetTag("Coin");
}

void Coin::Update(OctGame* pOctGame) {

}

void Coin::EnterObject(const Object* pObject, const HitBox* pHitbox) {
    if(pObject->CompareTag("Player")) {
        this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
    }
}

void Hat::Init(OctGame* pOctGame) {
    Collider collider;
    this->SetTag("SlimeHat");
    collider.AddHitBox(0, 0, 50, 50, true, false);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
//    this->AppendCollider(collider);
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/slimeHat.bmp", 0.5, 0.5, true)});
    this->SetSize(50, 50);
}

void Hat::EnterObject(const Object* pObject, const HitBox* pHitbox) {
    if(pObject->CompareTag("Player") && this->CanPicup()) {
        this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
    }
}
