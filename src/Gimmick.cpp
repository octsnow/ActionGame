#include "Gimmick.hpp"

void Flooting::Init(OctGame* pOctGame) {
    this->SetTag("Gimmick");
    this->SetWeight(2);

    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("assets/images/floating.bmp", true)});
    this->SetSize(100, 30);

    Collider collider;
    collider.AddHitBox(0, 0, 100, 30, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);
    this->SetGravity(0.1);
}

void Flooting::Draw(OctGame *pOctGame, Camera *pCamera) {
    Object::Draw(pOctGame, pCamera);
}

void Goal::Init(OctGame *pOctGame) {
    this->SetTag("Goal");
    this->SetWeight(1);

    this->SetImageHandle(200, {
            pOctGame->LoadImageFile("assets/images/goal0.bmp", true),
            pOctGame->LoadImageFile("assets/images/goal1.bmp", true),
            pOctGame->LoadImageFile("assets/images/goal2.bmp", true)});
    Collider collider;
    collider.AddHitBox(0, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);

    this->mIsGoal = false;
}

void Goal::Draw(OctGame *pOctGame, Camera *pCamera) {
    Object::Draw(pOctGame, pCamera);

    if(this->mIsGoal) {
        pOctGame->DrawText(800 / 2, 600 / 2, 0xFFFFFF, "G O A L!!!!!!!!");
    }
}

void Goal::EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Player") && pTargetHitbox->CompareTag("body")) {
        this->mIsGoal = true;
    }
}

bool Goal::IsGoal() const {
    return this->mIsGoal;
}
