#include "Item.hpp"
#include <iostream>
#include <time.h>

#define ITEM_LIFE_TIME 500// the item living time

Item::Item() {
    this->mStartTime = clock();
}

void Item::Update() {
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
//    this->AppendCollider(collider);
    this->SetTag("Coin");
}

void Coin::Update() {

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
