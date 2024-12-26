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

void Leaser::Init(OctGame *pOctGame) {
    this->SetImageHandle(0, {
            pOctGame->LoadImageFile("assets/images/leaser.bmp", true)});
    this->mLeaserWidth = 20;
    Collider collider;
    collider.AddHitBox(25 - this->mLeaserWidth / 2.0, 0, this->mLeaserWidth, 15, true);
    collider.AddHitBox(25 - this->mLeaserWidth / 2.0, 0, this->mLeaserWidth, this->mDestinationY - this->mPosition.y, false);
    this->SetCollider(collider);
    this->SetColliderSet({0, 1});
    this->SwitchCollider(0);
    this->SetTag("Leaser");
    this->SetWeight(10);
    this->mCurrentDestinationY = this->mDestinationY;
    this->mLastDestinationY = this->mDestinationY;
}

void Leaser::Draw(OctGame *pOctGame, Camera *pCamera) {
    Object::Draw(pOctGame, pCamera);
    HitBox h = this->GetCollider().GetHitBoxes()[0];
    Vector2D p1 = pCamera->CalcViewPosition(this->GetPosition().x + h.pos.x, this->GetPosition().y + h.pos.y + 20);
    Vector2D p2 = pCamera->CalcViewPosition(this->GetPosition().x + h.pos.x + h.width - 1, this->mCurrentDestinationY + 1);
    pOctGame->DrawBox(
            p1.x, p1.y,
            p2.x, p2.y,
            0xFFFFFF, true);

    this->mLastDestinationY = this->mCurrentDestinationY;
    this->mCurrentDestinationY = this->mDestinationY;
}

void Leaser::StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetHitbox->isPhysics) {
        double newDestinationY = pTargetObject->GetPosition().y + pTargetHitbox->pos.y - 1;
        if(this->mCurrentDestinationY > newDestinationY) {
            this->mCurrentDestinationY = newDestinationY;
        }
    }
}

void Leaser::SetLeaserHeight(double height) {
    if(height < 0) {
        return;
    }
    this->mDestinationY = this->GetPosition().y + height;
}

double Leaser::GetLowestY() const {
    return this->mLastDestinationY;
}
