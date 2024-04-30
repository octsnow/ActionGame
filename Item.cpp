#include "Item.hpp"

void Coin::Init(OctGame* pOctGame) {
    Collider collider;

    collider.AddHitBox(0, 0, 50, 50, true, false);
    this->AppendCollider(collider);
    this->SetTag("Coin");
}

void Coin::Update() {

}

void Coin::HitObject(const Object* pObject, const HitBox* pHitbox) {
    if(pObject->CompareTag("Player")) {
        this->PushMessage(ObjectMessage::OBJECTMESSAGE_DESTROY);
    }
}

void Hat::Init(OctGame* pOctGame) {
    /*
    Collider collider;
    this->SetTag("Hat");
    collider.AddHitBox(0, 0, 50, 50, false, false);
    this->AppendCollider(collider);*/
//    this->SetImageHandle(0, {
//        pOctGame->LoadImageFile("images/slimeHat.bmp", 0.5, 0.5, true)});
//    this->SetSize(50, 50);
}
