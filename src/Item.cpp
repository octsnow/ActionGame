#include "Item.hpp"
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
    this->SetTag("Item");
}

void Item::Update(OctGame* pOctGame) {
    Vector2d pos = this->GetOffsetPosition();
    time_t elapsedTime = clock() - this->mStartTime;
    pos.y = sin((elapsedTime % (ITEM_MOVE_CYCLE_TIME * 1000) / (double)(ITEM_MOVE_CYCLE_TIME * 1000)) * numbers::pi) * ITEM_MOVE_HEIGHT - ITEM_MOVE_HEIGHT;
    this->SetOffsetPosition(pos.x, pos.y);
}

void Item::StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pOctGame == NULL) {
        printf("pOctGame is nullptr\n");
        return;
    }
    if(pTargetObject == NULL) {
        printf("pTaretObject is nullptr\n");
        return;
    }
    if(pTargetHitbox == NULL) {
        printf("pTargetHitbox is nullptr\n");
        return;
    }
    MovableObject::StayObject(pOctGame, hitbox, pTargetObject, pTargetHitbox);
    if(pTargetObject->CompareTag("Player")) {
        if(pTargetHitbox->CompareTag("body") && this->CanPickup()) {
            this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
        }
    }
}

ITEM_ID Item::GetItemID() const {
    return this->mItemID;
}

bool Item::CanPickup() const {
    if(clock() - this->mStartTime >= ITEM_LIFE_TIME) {
        return true;
    }

    return false;
}

void Coin::Init(OctGame* pOctGame) {
    Collider collider;

    collider.AddHitBox(0, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
    this->SetTag("Coin");
}

void Coin::Update(OctGame* pOctGame) {

}

void Coin::EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Player")) {
        if(pTargetHitbox->CompareTag("body")) {
            this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
        }
    }
}

void SlimeHat::Init(OctGame* pOctGame) {
    Collider collider;
    collider.AddHitBox(0, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("assets/images/slimeHat.bmp", 0.5, 0.5, true)});
    this->SetSize(50, 50);
    this->mItemID = ITEM_ID::HAT_SLIME;
}

void SlimeWeapon::Init(OctGame* pOctGame) {
    Collider collider;
    collider.AddHitBox(0, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("assets/images/waterball.bmp", 0.5, 0.5, true)});
    this->SetSize(50, 50);
    this->mItemID = ITEM_ID::WEAPON_SLIME;
}

void FireWeapon::Init(OctGame* pOctGame) {
    Collider collider;
    collider.AddHitBox(0, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("assets/images/slimeHat.bmp", 0.5, 0.5, true)});
    this->SetSize(50, 50);
    this->mItemID = ITEM_ID::WEAPON_FIRE;
}
